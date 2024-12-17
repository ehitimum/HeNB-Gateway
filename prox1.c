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
#include "Global-ENB-ID.h"
#include "ProtocolIE-Field.h"
#include "S1SetupRequest.h"


#define CLIENT_PORT 36412      // Port for clients to connect
#define MME_PORT 36412         // Port to connect to MME
#define MME_ADDRESS "192.168.220.9"
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

int mme_fd;  // Global MME connection (shared by threads, can be thread-safe with mutex)
pthread_mutex_t mme_mutex = PTHREAD_MUTEX_INITIALIZER;

const char NEW_ENB_ID[] = {0x00, 0x12, 0x34};

typedef struct
{
    int client_fd;
    char s1ap_info_buffer[BUFFER_SIZE];
}client_data_t;


int decode_s1ap_message(const char *input, int input_size, char *output_buffer, int output_size){
    S1AP_PDU_t *pdu = NULL;
    asn_dec_rval_t rval;

    //Decode S1AP PDU
    rval = asn_decode(NULL, ATS_ALIGNED_BASIC_PER, &asn_DEF_S1AP_PDU, (void **)&pdu, input, input_size);
    if (rval.code != RC_OK)
    {
        snprintf(output_buffer, BUFFER_SIZE, "Failed to decode S1AP message.");\
        return;
    }

    // printf("Decoded S1AP Message:\n");
    // // Print the decoded structure in XML format for better understanding
    // asn_fprint(stdout, &asn_DEF_S1AP_PDU, pdu);

     // Check if the message is an InitiatingMessage
    if (pdu->present == S1AP_PDU_PR_initiatingMessage) {
        InitiatingMessage_t *initMsg = pdu->choice.initiatingMessage;
        if (initMsg->value.present == InitiatingMessage__value_PR_S1SetupRequest) {
            S1SetupRequest_t *setupRequest = &initMsg->value.choice.S1SetupRequest;

            // Call the separate function to replace eNB-ID
            replace_enb_id(setupRequest, output_buffer, output_size);
        }
    }

    asn_enc_rval_t enc_rval;
    enc_rval = asn_encode_to_buffer(NULL, ATS_ALIGNED_BASIC_PER, &asn_DEF_S1AP_PDU, pdu, output_buffer, output_size);
    if (enc_rval.encoded < 0) {
        snprintf(output_buffer, output_size, "Failed to encode modified S1AP message.");
        return -1;
    }

    // if (pdu->present == S1AP_PDU_PR_initiatingMessage)
    // {
    //     snprintf(output_buffer, BUFFER_SIZE, "Decoded S1AP Initiating Message, size: %d bytes", length);
    // } else {
    //     snprintf(output_buffer, BUFFER_SIZE, "Unsupported S1AP message type.");
    // }

    ASN_STRUCT_FREE(asn_DEF_S1AP_PDU, pdu);
    return enc_rval.encoded;
    
    
}

void replace_enb_id(S1SetupRequest_t *setupRequest, char *output_buffer, int output_size) {
    // Traverse the protocolIEs to find Global-ENB-ID
    for (int i = 0; i < setupRequest->protocolIEs.list.count; i++) {
        S1SetupRequestIEs_t *ie = setupRequest->protocolIEs.list.array[i];

        // Check if the current IE is Global-ENB-ID
        if (ie->id == ProtocolIE_ID_id_Global_ENB_ID) {
            Global_ENB_ID_t *enbID = &ie->value.choice.Global_ENB_ID;

            // Modify eNB-ID value
            if (enbID->eNB_ID.present == ENB_ID_PR_macroENB_ID) {
                memcpy(enbID->eNB_ID.choice.macroENB_ID.buf, NEW_ENB_ID, 3);
                snprintf(output_buffer, output_size, "Replaced eNB-ID with constant value: %02X %02X %02X",
                         NEW_ENB_ID[0], NEW_ENB_ID[1], NEW_ENB_ID[2]);
                printf("%s\n", output_buffer); // Optional: print confirmation to console
            }
        }
    }
}


void *handle_client(void *arg) {
    client_data_t *client_data = (client_data_t *)arg;
    int client_fd = client_data->client_fd;
    char buffer[BUFFER_SIZE];
    char output_buffer[BUFFER_SIZE];
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
        int encoded_size = decode_s1ap_message(buffer, bytes_received, output_buffer, BUFFER_SIZE);
        if (encoded_size > 0)
        {
            printf("Modified S1AP message successfully.\n");
            //Forward modified data to MME
            pthread_mutex_lock(&mme_mutex);
            sctp_sendmsg(mme_fd, output_buffer, encoded_size, NULL, 0, 0, 0, sndrcvinfo.sinfo_stream, 0, 0);
            pthread_mutex_unlock(&mme_mutex);
        }

        // Receive response from MME
        int bytes_from_mme = sctp_recvmsg(mme_fd, buffer, BUFFER_SIZE, NULL, 0, &sndrcvinfo, &flags);
        if (bytes_from_mme <= 0) {
            printf("MME disconnected or error occurred. Closing connection.\n");
            break;
        }

        buffer[bytes_from_mme] = '\0';
        printf("Received from MME on stream %d: %s\n", sndrcvinfo.sinfo_stream, buffer);

        // Send the response back to the client on the same stream
        sctp_sendmsg(client_fd, buffer, bytes_from_mme, NULL, 0, 0, 0, sndrcvinfo.sinfo_stream, 0, 0);
        
    }

    close(client_fd); // Close client connection
    return NULL;
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr, mme_addr;
    socklen_t addr_len = sizeof(client_addr);
    pthread_t thread_id;

    // Connect to the MME server
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

    // Set up SCTP server for client connections
    server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
    if (server_fd < 0) {
        perror("Client socket creation failed");
        close(mme_fd);
        exit(EXIT_FAILURE);
    }

    struct sctp_initmsg initmsg = {0};
    initmsg.sinit_num_ostreams = 10;
    initmsg.sinit_max_instreams = 10;
    initmsg.sinit_max_attempts = 10;

    setsockopt(server_fd, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(initmsg));

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

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        printf("New client connected from %s:%d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        int *client_fd_ptr = malloc(sizeof(int));
        *client_fd_ptr = client_fd;

        if (pthread_create(&thread_id, NULL, handle_client, client_fd_ptr) != 0) {
            perror("Thread creation failed");
            close(client_fd);
        } else {
            pthread_detach(thread_id); // Detach thread for automatic cleanup
        }
    }

    close(server_fd);
    close(mme_fd);
    return 0;
}
