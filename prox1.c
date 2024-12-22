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
#include "hashmap.h"


#define CLIENT_PORT 36412      // Port for clients to connect
#define MME_PORT 36412         // Port to connect to MME
#define MME_ADDRESS "192.168.220.9"
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

int mme_fd;  // Global MME connection (shared by threads, can be thread-safe with mutex)
pthread_mutex_t mme_mutex = PTHREAD_MUTEX_INITIALIZER;

HashMap *map = NULL;

//const char NEW_ENB_ID[10]; //= {0x00, 0x12, 0x34};
int decimal = 107021;
char NEW_ENB_ID[10];
char NEW_CELL_ID[4];



void decimalToHex(int decimal, char *hexStr) {
    // Use sprintf to convert decimal to hexadecimal
    sprintf(hexStr, "0x%X", decimal);
}



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
        return -1;
    }

    if (pdu->present == S1AP_PDU_PR_initiatingMessage)
    {
        InitiatingMessage_t *initMsg = pdu->choice.initiatingMessage;
        switch (initMsg->value.present)
        {
        case InitiatingMessage__value_PR_S1SetupRequest:
            S1SetupRequest_t *setupRequest = &initMsg->value.choice.S1SetupRequest;
            // Call the separate function to replace eNB-ID
            replace_enb_id(setupRequest, output_buffer, output_size);
            break;
        case InitiatingMessage__value_PR_InitialUEMessage:
            InitialUEMessage_t *initialUEMsg = &initMsg->value.choice.InitialUEMessage;
            replace_cell_id(initialUEMsg, output_buffer, output_size);
        default:
            break;
        }
    }
    
    asn_enc_rval_t enc_rval;
    enc_rval = asn_encode_to_buffer(NULL, ATS_ALIGNED_BASIC_PER, &asn_DEF_S1AP_PDU, pdu, output_buffer, output_size);
    if (enc_rval.encoded < 0) {
        snprintf(output_buffer, output_size, "Failed to encode modified S1AP message.");
        return -1;
    }

    ASN_STRUCT_FREE(asn_DEF_S1AP_PDU, pdu);
    return enc_rval.encoded;
    
    
}

void replace_enb_id(S1SetupRequest_t *setupRequest, char *output_buffer, int output_size) {
    printf("OK\n");
    decimalToHex(decimal, NEW_ENB_ID);

    // Traverse the protocolIEs to find Global-ENB-ID
    for (int i = 0; i < setupRequest->protocolIEs.list.count; i++) {
        S1SetupRequestIEs_t *ie = setupRequest->protocolIEs.list.array[i];

        // Check if the current IE is Global-ENB-ID
        if (ie->id == ProtocolIE_ID_id_Global_ENB_ID) {
            Global_ENB_ID_t *enbID = &ie->value.choice.Global_ENB_ID;

            // Modify eNB-ID value
            if (enbID->eNB_ID.present == ENB_ID_PR_macroENB_ID) {
                printf("Inserting value: 0x%X\n", *(uint32_t *)(enbID->eNB_ID.choice.macroENB_ID.buf));
                insert(map, NEW_ENB_ID, *(uint32_t *) enbID->eNB_ID.choice.macroENB_ID.buf);
                memcpy(enbID->eNB_ID.choice.macroENB_ID.buf, NEW_ENB_ID, 3);
                
                snprintf(output_buffer, output_size, "Replaced eNB-ID with constant value: %02X %02X %02X",
                         NEW_ENB_ID[0], NEW_ENB_ID[1], NEW_ENB_ID[2]);
                
                printf("%s\n", output_buffer); // Optional: print confirmation to console
                uint32_t value = get(map, NEW_ENB_ID);
                printf("Old EnbID: 0x%X\n", value);

            }
        }
    }
}

void cellIdentityCal(uint8_t *cellIdentity) {
    printf("ok\n");
    uint8_t randomCellID = rand() % 256;  // Generate random Cell ID
    printf("ok\n");
    int cellid = decimal * 256 + randomCellID;  // Calculate new Cell ID
    printf("Calculated Cell ID: 0x%X\n", cellid);

    // Copy the 4-byte integer into the buffer
    cellIdentity[0] = (cellid >> 24) & 0xFF;
    cellIdentity[1] = (cellid >> 16) & 0xFF;
    cellIdentity[2] = (cellid >> 8) & 0xFF;
    cellIdentity[3] = cellid & 0xFF;
}


void replace_cell_id(InitialUEMessage_t *initialUEMsg, char *output_buffer, int output_size) {
    printf("Starting Cell ID replacement.\n");
    for (int i = 0; i < initialUEMsg->protocolIEs.list.count; i++) {
        InitialUEMessage_IEs_t *ie = initialUEMsg->protocolIEs.list.array[i];
        if (ie->id == ProtocolIE_ID_id_EUTRAN_CGI) {
            EUTRAN_CGI_t *cellID = &ie->value.choice.EUTRAN_CGI;
            if (cellID->cell_ID.buf != NULL && cellID->cell_ID.size == 4) {
                // Print the old Cell ID
                printf("Old Cell ID: %02X %02X %02X %02X\n",
                       cellID->cell_ID.buf[0], cellID->cell_ID.buf[1],
                       cellID->cell_ID.buf[2], cellID->cell_ID.buf[3]);

                // Generate the new Cell ID
                cellIdentityCal(NEW_CELL_ID);

                // Store the old Cell ID in the map
                insert(map, NEW_CELL_ID, *(uint32_t *)cellID->cell_ID.buf);

                // Replace the Cell ID in the buffer
                memcpy(cellID->cell_ID.buf, NEW_CELL_ID, 4);

                // Format the output message
                snprintf(output_buffer, output_size, "Replaced Cell ID with constant value: %02X %02X %02X %02X",
                         NEW_CELL_ID[0], NEW_CELL_ID[1], NEW_CELL_ID[2], NEW_CELL_ID[3]);

                // Print the confirmation message
                printf("%s\n", output_buffer);

                // Optionally, retrieve the old value from the map for debugging
                uint32_t oldValue = get(map, NEW_CELL_ID);
                printf("Old Cell ID stored in map: 0x%X\n", oldValue);
            } else {
                printf("Cell ID field is not properly initialized or has an unexpected size.\n");
            }

            // Stop processing further as we've found the desired IE
            break;
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
            uint32_t value = get(map, NEW_ENB_ID);
            printf("Old EnbID: %X\n", value);

            uint32_t oldValue = get(map, NEW_CELL_ID);
            printf("Old Cell ID stored in map: 0x%X\n", oldValue);

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
    map = createHashMap(10);


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
