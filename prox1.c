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
u_int32_t NEW_ENB_S1AP_ID = 0;




void decimalToHex(int decimal, char *hexStr) {
    // Use sprintf to convert decimal to hexadecimal
    sprintf(hexStr, "0x%X", decimal);
}



typedef struct
{
    int client_fd;
    char s1ap_info_buffer[BUFFER_SIZE];
}client_data_t;


int decode_s1ap_message(const char *input, int input_size, char *output_buffer, int output_size, char *NEW_CELL_ID) {
    S1AP_PDU_t *pdu = NULL;
    asn_dec_rval_t rval;

    // Decode S1AP PDU
    rval = asn_decode(NULL, ATS_ALIGNED_BASIC_PER, &asn_DEF_S1AP_PDU, (void **)&pdu, input, input_size);
    if (rval.code != RC_OK) {
        snprintf(output_buffer, output_size, "Failed to decode S1AP message.");
        return -1;
    }

    // Check if the PDU is an initiating message
    if (pdu->present != S1AP_PDU_PR_initiatingMessage) {
        ASN_STRUCT_FREE(asn_DEF_S1AP_PDU, pdu);
        return 0; // No modifications
    }

    // Process the initiating message
    InitiatingMessage_t *initMsg = pdu->choice.initiatingMessage;
    switch (initMsg->value.present) {
        case InitiatingMessage__value_PR_S1SetupRequest: {
            S1SetupRequest_t *setupRequest = &initMsg->value.choice.S1SetupRequest;
            replace_enb_id(setupRequest, output_buffer, output_size);
            break;
        }
        case InitiatingMessage__value_PR_InitialUEMessage: {
            printf("----------------------InitialUEMsg------------------\n");
            InitialUEMessage_t *initialUEMsg = &initMsg->value.choice.InitialUEMessage;
            replace_cell_id(initialUEMsg, output_buffer, output_size, NEW_CELL_ID);
            replace_enb_ue_id(initialUEMsg, output_buffer, output_size);
            break;
        }
        case InitiatingMessage__value_PR_UplinkNASTransport: {
            printf("----------------------UplinkNASTransport------------------\n");
            UplinkNASTransport_t *uplink = &initMsg->value.choice.UplinkNASTransport;
            replace_uplink_cell_id(uplink, output_buffer, output_size, NEW_CELL_ID);
            replace_uplink_enb_ue_id(uplink, output_buffer, output_size);
            break;
        }
        case InitiatingMessage__value_PR_UEContextReleaseRequest: {
            printf("----------------------UEContextReleaseRequest------------------\n");
            UEContextReleaseRequest_t *releaseReq = &initMsg->value.choice.UEContextReleaseRequest;
            check_gw_context_release_indication(releaseReq, output_buffer, output_size);
            break;
        }
        case InitiatingMessage__value_PR_DownlinkNASTransport: {
            printf("----------------------DownlinkNASTransport------------------\n");
            DownlinkNASTransport_t *downLink = &initMsg->value.choice.DownlinkNASTransport;
            replace_downlink_ue_id(downLink, output_buffer, output_size);
            break;
        }
        case InitiatingMessage__value_PR_InitialContextSetupRequest:{
            printf("----------------------Initial Context Setup------------------\n");
            InitialContextSetupRequest_t *context = &initMsg->value.choice.InitialContextSetupRequest;
            replace_InitialContextSetup_ue_id(context,  output_buffer, output_size);
            break;
        }
        case InitiatingMessage__value_PR_UECapabilityInfoIndication:{
            printf("----------------------UECapabilityInfoIndication------------------\n");
            UECapabilityInfoIndication_t *ueCap = &initMsg->value.choice.UECapabilityInfoIndication;
            replace_UECap_enb_ue_id(ueCap, output_buffer, output_size);
            break;
        }
        default:
            printf("----------------------Some other Protocol------------------\n");
            break;
    }

    // Encode the modified message back to buffer
    asn_enc_rval_t enc_rval;
    enc_rval = asn_encode_to_buffer(NULL, ATS_ALIGNED_BASIC_PER, &asn_DEF_S1AP_PDU, pdu, output_buffer, output_size);
    if (enc_rval.encoded < 0) {
        snprintf(output_buffer, output_size, "Failed to encode modified S1AP message.");
        ASN_STRUCT_FREE(asn_DEF_S1AP_PDU, pdu);
        return -1;
    }

    ASN_STRUCT_FREE(asn_DEF_S1AP_PDU, pdu);
    return enc_rval.encoded;
}

void check_gw_context_release_indication(UEContextReleaseRequest_t *releaseRequest, char *output_buffer, int output_size) {
    printf("Checking GW Context Release Indication...\n");

    // Traverse the protocolIEs to find the GWContextReleaseIndication
    for (int i = 0; i < releaseRequest->protocolIEs.list.count; i++) {
        UEContextReleaseRequest_IEs_t *ie = releaseRequest->protocolIEs.list.array[i];

        // Check if the current IE is GWContextReleaseIndication
        if (ie->id == ProtocolIE_ID_id_GWContextReleaseIndication) {
            // Extract the value of GWContextReleaseIndication (which should be a boolean)
            GWContextReleaseIndication_t *gwContextIndication = &ie->value.choice.GWContextReleaseIndication;

            // Check if the GWContextReleaseIndication is set (true or false)
            if (*gwContextIndication == 1) {  // Assuming 1 means it's set (true)
                snprintf(output_buffer, output_size, "GWContextReleaseIndication is set: TRUE");
                printf("%s\n", output_buffer);  // Optional: print confirmation to console
            } else {
                snprintf(output_buffer, output_size, "GWContextReleaseIndication is not set: FALSE");
                printf("%s\n", output_buffer);  // Optional: print confirmation to console
            }
        }
    }
}

void replace_downlink_ue_id(DownlinkNASTransport_t *downLink, char *output_buffer, int output_size){
            DownlinkNASTransport_IEs_t *ie = downLink->protocolIEs.list.array[1];
            if (ie->id == ProtocolIE_ID_id_eNB_UE_S1AP_ID) {
                ENB_UE_S1AP_ID_t *ueID = &ie->value.choice.ENB_UE_S1AP_ID;
                
                if (ueID) {
                    printf("Current ENB UE ID: %u\n", *ueID);
                    u_int32_t value = get(map, *ueID);
                    printf("Old ENB UE ID: %u\n", value);
                    *ueID = value;
                    snprintf(output_buffer, output_size, "Replaced ENB UE ID with old value: %u", value);
                    printf("UEID IS NOW: %u\n", *ueID);
                    printf("%s\n", output_buffer);
                } else {
                    printf("ENB UE ID field is not properly initialized.\n");
                }
            }
}

void replace_InitialContextSetup_ue_id(InitialContextSetupRequest_t *context,  char *output_buffer, int output_size){
    InitialContextSetupRequestIEs_t *ie = context->protocolIEs.list.array[1];
    if (ie->id == ProtocolIE_ID_id_eNB_UE_S1AP_ID) {
                ENB_UE_S1AP_ID_t *ueID = &ie->value.choice.ENB_UE_S1AP_ID;
                
                if (ueID) {
                    printf("Current ENB UE ID: %u\n", *ueID);
                    u_int32_t value = get(map, *ueID);
                    printf("Old ENB UE ID: %u\n", value);
                    *ueID = value;
                    snprintf(output_buffer, output_size, "Replaced ENB UE ID with old value: %u", value);
                    printf("UEID IS NOW: %u\n", *ueID);
                    printf("%s\n", output_buffer);
                } else {
                    printf("ENB UE ID field is not properly initialized.\n");
                }
            }
}

void replace_UECap_enb_ue_id(UECapabilityInfoIndication_t *cap, char *output_buffer, int output_size){
    //UECapabilityInfoIndicationIEs_t *ie = cap->protocolIEs.list.array[1];
    for (int i = 0; i < cap->protocolIEs.list.count; i++) {
        UECapabilityInfoIndicationIEs_t *ie = cap->protocolIEs.list.array[i];
        if (ie->id == ProtocolIE_ID_id_eNB_UE_S1AP_ID) {
            ENB_UE_S1AP_ID_t *ueID = &ie->value.choice.ENB_UE_S1AP_ID;
            
            if (ueID) {
                // Print the old ENB UE ID
                printf("------------------Old ENB UE ID of UE CAP: %u\n", *ueID);
                // Replace the ENB UE ID with the new value
                u_int32_t value = getHashKey(map, *ueID);
                *ueID = value;
                // Format the output message
                snprintf(output_buffer, output_size, "-----------------Replaced ENB UE ID with new value: %u", value);
                
                // Print the confirmation message
                printf("%s\n", output_buffer);
            } else {
                printf("----------------ENB UE ID field is not properly initialized.\n");
            }

            // Stop processing further as we've found the desired IE
            break;
        }
    }
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


void replace_cell_id(InitialUEMessage_t *initialUEMsg, char *output_buffer, int output_size, char *NEW_CELL_ID) {
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

void replace_uplink_cell_id(UplinkNASTransport_t *uplink, char *output_buffer, int output_size, char *NEW_CELL_ID) {
    printf("Starting Cell ID replacement.\n");
    for (int i = 0; i < uplink->protocolIEs.list.count; i++) {
        UplinkNASTransport_IEs_t *ie = uplink->protocolIEs.list.array[i];
        if (ie->id == ProtocolIE_ID_id_EUTRAN_CGI) {
            EUTRAN_CGI_t *cellID = &ie->value.choice.EUTRAN_CGI;
            if (cellID->cell_ID.buf != NULL && cellID->cell_ID.size == 4) {
                // Print the old Cell ID
                printf("Old Cell ID: %02X %02X %02X %02X\n",
                       cellID->cell_ID.buf[0], cellID->cell_ID.buf[1],
                       cellID->cell_ID.buf[2], cellID->cell_ID.buf[3]);
                memcpy(cellID->cell_ID.buf, NEW_CELL_ID, 4);
                snprintf(output_buffer, output_size, "Replaced Cell ID with constant value: %02X %02X %02X %02X",
                         NEW_CELL_ID[0], NEW_CELL_ID[1], NEW_CELL_ID[2], NEW_CELL_ID[3]);
                printf("%s\n", output_buffer);
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

u_int32_t increment_enb_s1ap_id() {
    return ++NEW_ENB_S1AP_ID;
}

void replace_enb_ue_id(InitialUEMessage_t *initialUEMsg, char *output_buffer, int output_size) {
    printf("Starting ENB UE ID Replacement.\n");
    
    for (int i = 0; i < initialUEMsg->protocolIEs.list.count; i++) {
        InitialUEMessage_IEs_t *ie = initialUEMsg->protocolIEs.list.array[i];
        if (ie->id == ProtocolIE_ID_id_eNB_UE_S1AP_ID) {
            ENB_UE_S1AP_ID_t *ueID = &ie->value.choice.ENB_UE_S1AP_ID;
            
            if (ueID) {
                // Print the old ENB UE ID
                printf("Old ENB UE ID: %u\n", *ueID);
                // Replace the ENB UE ID with the new value
                increment_enb_s1ap_id();
                insert(map, NEW_ENB_S1AP_ID, *ueID);
                *ueID = NEW_ENB_S1AP_ID;
                // Format the output message
                snprintf(output_buffer, output_size, "Replaced ENB UE ID with new value: %u", NEW_ENB_S1AP_ID);
                
                // Print the confirmation message
                printf("%s\n", output_buffer);
            } else {
                printf("ENB UE ID field is not properly initialized.\n");
            }

            // Stop processing further as we've found the desired IE
            break;
        }
    }

}

void replace_uplink_enb_ue_id(UplinkNASTransport_t *uplink, char *output_buffer, int output_size) {
    printf("Starting ENB UE ID Replacement.\n");
    
    for (int i = 0; i < uplink->protocolIEs.list.count; i++) {
        UplinkNASTransport_IEs_t *ie = uplink->protocolIEs.list.array[i];
        if (ie->id == ProtocolIE_ID_id_eNB_UE_S1AP_ID) {
            ENB_UE_S1AP_ID_t *ueID = &ie->value.choice.ENB_UE_S1AP_ID;
            
            if (ueID) {
                // Print the old ENB UE ID
                printf("Old ENB UE ID: %u\n", *ueID);
                // Replace the ENB UE ID with the new value
                u_int32_t value = getHashKey(map, *ueID);
                *ueID = value;
                // Format the output message
                snprintf(output_buffer, output_size, "Replaced ENB UE ID with new value: %u", NEW_ENB_S1AP_ID);
                
                // Print the confirmation message
                printf("%s\n", output_buffer);
            } else {
                printf("ENB UE ID field is not properly initialized.\n");
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
    char NEW_CELL_ID[4];
    //uint32_t NEW_ENB_UE_ID;

    free(arg); // Free memory allocated for client_data

    while (1) {
        // Receive data from the client, including SCTP metadata
        int bytes_received = sctp_recvmsg(client_fd, buffer, BUFFER_SIZE, NULL, 0, &sndrcvinfo, &flags);
        if (bytes_received <= 0) {
            printf("Client disconnected or error occurred. Closing connection.\n");
            break;
        }
        buffer[bytes_received] = '\0';
        printf("Received from client on stream %d: %s\n", sndrcvinfo.sinfo_stream, buffer);

        // Decode and optionally modify the S1AP message
        int encoded_size = decode_s1ap_message(buffer, bytes_received, output_buffer, BUFFER_SIZE, NEW_CELL_ID);

        // Forward the packet to the MME
        pthread_mutex_lock(&mme_mutex);
        if (encoded_size > 0) {
            // Send the modified packet
            printf("Modified S1AP message successfully.\n");
            uint32_t value = get(map, NEW_ENB_ID);
            printf("Old EnbID: %X\n", value);

            uint32_t oldValue = get(map, NEW_CELL_ID);
            printf("Old Cell ID stored in map: 0x%X\n", oldValue);

            uint32_t ueValue = get(map, NEW_ENB_S1AP_ID);
            printf("Old ENB UE ID: %u\n", ueValue);
            sctp_sendmsg(mme_fd, output_buffer, encoded_size, NULL, 0, 0, 0, sndrcvinfo.sinfo_stream, 0, 0);
        } else {
            // Send the unmodified packet
            printf("No modifications made. Forwarding the original packet to MME.\n");
            sctp_sendmsg(mme_fd, buffer, bytes_received, NULL, 0, 0, 0, sndrcvinfo.sinfo_stream, 0, 0);
        }
        pthread_mutex_unlock(&mme_mutex);

        // Receive response from the MME
        int bytes_from_mme = sctp_recvmsg(mme_fd, buffer, BUFFER_SIZE, NULL, 0, &sndrcvinfo, &flags);
        if (bytes_from_mme <= 0) {
            printf("MME disconnected or error occurred. Closing connection.\n");
            break;
        }

        buffer[bytes_from_mme] = '\0';
        printf("Received from MME on stream %d: %s\n", sndrcvinfo.sinfo_stream, buffer);

        int re_encoded_size = decode_s1ap_message(buffer, bytes_from_mme, output_buffer, BUFFER_SIZE, NEW_CELL_ID);

        //pthread_mutex_lock(&mme_mutex);
        if (re_encoded_size > 0) {
            // Send the modified packet
            printf("Modified incoming MME S1AP message successfully.\n");
            sctp_sendmsg(client_fd, output_buffer, re_encoded_size, NULL, 0, 0, 0, sndrcvinfo.sinfo_stream, 0, 0);
        } else {
            // Send the unmodified packet
            printf("No modifications made. Forwarding the original packet to Client.\n");
            sctp_sendmsg(client_fd, buffer, bytes_from_mme, NULL, 0, 0, 0, sndrcvinfo.sinfo_stream, 0, 0);
        }
        //pthread_mutex_unlock(&mme_mutex);
        
    }

    close(client_fd); // Close client connection
    return NULL;
}


void capture_and_save_initial_ue_message(InitialUEMessage_t *initialUEMsg, const char *filename) {
    // Open the file for saving
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to open file for saving InitialUEMessage");
        return;
    }

    // Allocate a buffer for encoding
    unsigned char buffer[1024]; // Adjust size based on expected message size
    asn_enc_rval_t encode_result = asn_encode_to_buffer(
        0,                  // No constraints checking
        ATS_BER,            // BER encoding
        &asn_DEF_InitialUEMessage, // ASN.1 type descriptor
        initialUEMsg,       // The structure to encode
        buffer,             // The buffer to encode into
        sizeof(buffer)      // Size of the buffer
    );

    // Check if encoding was successful
    if (encode_result.encoded > 0 && (size_t)encode_result.encoded <= sizeof(buffer)) {
        // Write the encoded message to the file
        fwrite(buffer, 1, encode_result.encoded, file);
        printf("InitialUEMessage successfully saved to %s\n", filename);
    } else {
        printf("Failed to encode InitialUEMessage. Error code: %ld\n", encode_result.encoded);
    }

    fclose(file);
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
    initmsg.sinit_num_ostreams = 100;
    initmsg.sinit_max_instreams = 100;
    initmsg.sinit_max_attempts = 100;

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
    free(map->buckets);
    free(map);
    return 0;
}
