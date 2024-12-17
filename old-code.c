#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/sctp.h>

#include "S1AP-PDU.h"
#include "InitiatingMessage.h"
#include "asn_application.h"
#include "asn_codecs.h"


#define CLIENT_PORT 36412      // Port for clients to connect
#define MME_PORT 36412         // Port to connect to MME
#define MME_ADDRESS "192.168.220.9"
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

int mme_fd;  // Global MME connection (shared by threads, can be thread-safe with mutex)
pthread_mutex_t mme_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct
{
    int client_fd;
    char s1ap_info_buffer[BUFFER_SIZE];
}client_data_t;


void decode_s1ap_message(const char *buffer, int length, char *output_buffer){
    S1AP_PDU_t *pdu = NULL;
    asn_dec_rval_t rval;

    //Decode S1AP PDU
    rval = asn_decode(NULL, ATS_ALIGNED_BASIC_PER, &asn_DEF_S1AP_PDU, (void **)&pdu, buffer, length);
    if (rval.code != RC_OK)
    {
        snprintf(output_buffer, BUFFER_SIZE, "Failed to decode S1AP message.");\
        return;
    }

    printf("Decoded S1AP Message:\n");
    // Print the decoded structure in XML format for better understanding
    asn_fprint(stdout, &asn_DEF_S1AP_PDU, pdu);

    if (pdu->present == S1AP_PDU_PR_initiatingMessage)
    {
        snprintf(output_buffer, BUFFER_SIZE, "Decoded S1AP Initiating Message, size: %d bytes", length);
    } else {
        snprintf(output_buffer, BUFFER_SIZE, "Unsupported S1AP message type.");
    }

    ASN_STRUCT_FREE(asn_DEF_S1AP_PDU, pdu);
    
    
}



void *handle_client(void *arg) {
    client_data_t *client_data = (client_data_t *)arg;
    int client_fd = client_data->client_fd;
    char buffer[BUFFER_SIZE];
    struct sctp_sndrcvinfo sndrcvinfo;
    int flags;

    
    free(arg); // Free memory allocated for client_fd

    while(1){
        // Receive data from the client, including SCTP metadata
        int bytes_received = sctp_recvmsg(client_fd, buffer, BUFFER_SIZE, NULL, 0, &sndrcvinfo, &flags);
        if(bytes_received<=0){
            printf("Client disconnected or error occurred. Closing connection.\n");
            break;
        }
        buffer[bytes_received] = '\0';
        printf("Received from client on stream %d: %s\n", sndrcvinfo.sinfo_stream, buffer);

        // Decode S1AP message and store locally in the client-specific buffer
        decode_s1ap_message(buffer, bytes_received, client_data->s1ap_info_buffer);
        printf("Decoded S1AP Info: %s\n", client_data->s1ap_info_buffer);

        // Forward data to MME on the same stream
        pthread_mutex_lock(&mme_mutex);
        sctp_sendmsg(mme_fd, buffer, BUFFER_SIZE, NULL, 0, 0, 0, sndrcvinfo.sinfo_stream, 0, 0);
        // Receive response from the MME
        int bytes_from_mme = sctp_recvmsg(mme_fd, buffer, BUFFER_SIZE, NULL, 0, &sndrcvinfo, &flags);
        pthread_mutex_unlock(&mme_mutex);

        if(bytes_from_mme<=0){
            printf("MME disconnected or error occurred. Closing connection.\n");
            break;
        }
        buffer[bytes_from_mme] = '\0';
        printf("Received from MME on stream %d: %s\n", sndrcvinfo.sinfo_stream, buffer);
        // Send the response back to the client on the same stream
        sctp_sendmsg(client_fd, buffer, BUFFER_SIZE, NULL, 0, 0, 0, sndrcvinfo.sinfo_stream, 0, 0);
    }

    close(client_fd); // Close client connection
    return NULL;
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr, mme_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);
    pthread_t thread_id;

    // Step 1: Connect to the MME server
    mme_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
    if (mme_fd < 0) {
        perror("MME socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&mme_addr, 0, sizeof(mme_addr));
    mme_addr.sin_family = AF_INET;
    mme_addr.sin_port = htons(MME_PORT);
    if (inet_pton(AF_INET, MME_ADDRESS, &mme_addr.sin_addr) <= 0) {
        perror("Invalid MME address");
        close(mme_fd);
        exit(EXIT_FAILURE);
    }

    if (connect(mme_fd, (struct sockaddr *)&mme_addr, sizeof(mme_addr)) < 0) {
        perror("Failed to connect to MME");
        close(mme_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to MME at %s:%d\n", MME_ADDRESS, MME_PORT);

    // Step 2: Set up the SCTP server for client connections
    server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
    if (server_fd < 0) {
        perror("Client socket creation failed");
        close(mme_fd);
        exit(EXIT_FAILURE);
    }

    // Configure SCTP server to support multiple streams
    struct  sctp_initmsg initmsg = {0};
    initmsg.sinit_num_ostreams = 10; // Outbound streams
    initmsg.sinit_max_instreams = 10; // Inbound streams
    initmsg.sinit_max_attempts = 10; //Maximum attempts

    if (setsockopt(server_fd, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(initmsg)) < 0)
    {
        perror("SCTP INITMSG failed");
        close(server_fd);
        close(mme_fd);
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(CLIENT_PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        close(mme_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        close(server_fd);
        close(mme_fd);
        exit(EXIT_FAILURE);
    }

    printf("Listening for client connections on port %d...\n", CLIENT_PORT);

    // Step 3: Accept and handle client connections
    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        printf("New client connected from %s:%d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Create a thread for the new client connection
        // int *client_fd_ptr = malloc(sizeof(int));
        // *client_fd_ptr = client_fd;
        client_data_t *client_data = malloc(sizeof(client_data_t));
        client_data->client_fd = client_fd;

        if (pthread_create(&thread_id, NULL, handle_client, client_data) != 0) {
            perror("Thread creation failed");
            close(client_fd);
            free(client_data);
        } else {
            pthread_detach(thread_id);  // Detach thread to clean up resources when done
        }
    }

    close(server_fd);
    close(mme_fd);
    return 0;
}
