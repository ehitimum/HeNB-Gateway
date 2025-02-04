#include "henb-gw.h"

#define CLIENT_PORT 36412 // Port for clients to connect
#define MME_PORT 36412    // Port to connect to MME
#define MME_ADDRESS "192.168.220.9"
#define BUFFER_SIZE 9999
#define MAX_CLIENTS 10
#define MCC_MNC_BUF "\x00\xF1\x10" // MCC=001, MNC=01
#define MCC_MNC_LEN 3
#define TAC_BUF "\x00\x01"
#define TAC_LEN 2
#define ENB_NAME "Henb-Proxy"

#define MESSAGE_TYPE_INITIATING (1 << 8) // High byte for category
#define MESSAGE_TYPE_SUCCESSFUL (2 << 8)



int mme_fd; // Global MME connection (shared by threads, can be thread-safe with mutex)
pthread_mutex_t mme_mutex = PTHREAD_MUTEX_INITIALIZER;

HashMap *map = NULL;
int decimal = 107021;
char NEW_ENB_ID[10];
u_int32_t NEW_ENB_S1AP_ID = 0;

pthread_mutex_t pdu_mutex = PTHREAD_MUTEX_INITIALIZER;
size_t pdu_length = 0;
uint8_t *pdu_buffer = NULL;
int check = 0;
HashMap *packet_store;
HashMap *enbMap;


void decimalToHex(int decimal, char *hexStr)
{
    // Use sprintf to convert decimal to hexadecimal
    sprintf(hexStr, "0x%X", decimal);
}







int decode_s1ap_message(const char *input, int input_size, char *output_buffer, int output_size, char *NEW_CELL_ID, int client_fd, int *client_id)
{
    S1AP_PDU_t *pdu = NULL;
    asn_dec_rval_t rval;

    // Decode S1AP PDU
    rval = asn_decode(NULL, ATS_ALIGNED_BASIC_PER, &asn_DEF_S1AP_PDU, (void **)&pdu, input, input_size);
    if (rval.code != RC_OK)
    {
        snprintf(output_buffer, output_size, "Failed to decode S1AP message.");
        return -1;
    }
    if (pdu->present == S1AP_PDU_PR_initiatingMessage) {
    printf("Processing InitiatingMessage\n");
    int client = 0;


    printf("Before assignment, *client_id: %d\n", client);
    process_initating_msg(pdu->choice.initiatingMessage, output_buffer, output_size, NEW_CELL_ID, client_fd, &client);
    printf("After assignment, *client_id: %d\n", client);
    *client_id = client;
    } else if (pdu->present == S1AP_PDU_PR_successfulOutcome) {
        printf("Processing SuccessfulOutcome\n");
        process_sucessfullMsg(pdu->choice.successfulOutcome, output_buffer, output_size, NEW_CELL_ID);
    } else {
        snprintf(output_buffer, output_size, "Unexpected PDU type: %d", pdu->present);
        ASN_STRUCT_FREE(asn_DEF_S1AP_PDU, pdu);
        return -1;
    }
    // Encode the modified message back to buffer
    asn_enc_rval_t enc_rval;
    enc_rval = asn_encode_to_buffer(NULL, ATS_ALIGNED_BASIC_PER, &asn_DEF_S1AP_PDU, pdu, output_buffer, output_size);
    if (enc_rval.encoded < 0)
    {
        snprintf(output_buffer, output_size, "Failed to encode modified S1AP message.");
        ASN_STRUCT_FREE(asn_DEF_S1AP_PDU, pdu);
        return -1;
    }

    ASN_STRUCT_FREE(asn_DEF_S1AP_PDU, pdu);
    return enc_rval.encoded;
}

void process_initating_msg(InitiatingMessage_t *initMsg, char *output_buffer, int output_size, char *NEW_CELL_ID, int client_fd, int *client_id){
    switch (initMsg->value.present) {
        case InitiatingMessage__value_PR_S1SetupRequest: {
            S1SetupRequest_t *setupRequest = &initMsg->value.choice.S1SetupRequest;
            //replace_enb_id(setupRequest, output_buffer, output_size);
            printf("S1Setup Request Detected\n");
            check ++;
            break;
        }
        case InitiatingMessage__value_PR_InitialUEMessage: {
            printf("----------------------InitialUEMsg------------------\n");
            InitialUEMessage_t *initialUEMsg = &initMsg->value.choice.InitialUEMessage;
            replace_cell_id(initialUEMsg, output_buffer, output_size, NEW_CELL_ID);
            replace_enb_ue_id(initialUEMsg, output_buffer, output_size, client_fd);
            break;
        }
        case InitiatingMessage__value_PR_UplinkNASTransport: {
            printf("----------------------UplinkNASTransport------------------\n");
            UplinkNASTransport_t *uplink = &initMsg->value.choice.UplinkNASTransport;
            replace_uplink_cell_id(uplink, output_buffer, output_size, NEW_CELL_ID);
            uplinknasT_enb_ue_id_mapping(uplink, output_buffer, output_size, map);
            break;
        }
        case InitiatingMessage__value_PR_UEContextReleaseRequest: {
            printf("----------------------UEContextReleaseRequest------------------\n");
            UEContextReleaseRequest_t *releaseReq = &initMsg->value.choice.UEContextReleaseRequest;
            check_gw_context_release_indication(releaseReq, output_buffer, output_size);
            ueContextRelReq_ue_id_mapping(releaseReq, output_buffer, output_size, map);
            break;
        }
        case InitiatingMessage__value_PR_DownlinkNASTransport: {
            printf("----------------------DownlinkNASTransport------------------\n");
            DownlinkNASTransport_t *downLink = &initMsg->value.choice.DownlinkNASTransport;
            //replace_downlink_ue_id(downLink, output_buffer, output_size);
            downlinknasT_ue_id_mapping(downLink, output_buffer, output_size, map);
            DownlinkNASTransport_IEs_t *ie = downLink->protocolIEs.list.array[1];
            if (ie->id == ProtocolIE_ID_id_eNB_UE_S1AP_ID){
                ENB_UE_S1AP_ID_t *ueID = &ie->value.choice.ENB_UE_S1AP_ID;
                if (ueID)
                    {                       
                        int client  = get(packet_store, *ueID);
                        printf("Cleint ID is: %d\n", client);
                        printf("Client ID %d\n", *client_id);
                        *client_id = client;
                        printf("Client ID %d\n", client_id);

                    }
                    else
                    {
                        printf("ENB UE ID field is not properly initialized.\n");
                    }
            }


            break;
        }
        case InitiatingMessage__value_PR_InitialContextSetupRequest:{
            printf("----------------------Initial Context Setup------------------\n");
            InitialContextSetupRequest_t *context = &initMsg->value.choice.InitialContextSetupRequest;
            //replace_InitialContextSetup_ue_id(context,  output_buffer, output_size);
            initialContextSetup_ue_id_mapping(context, output_buffer, output_size, map);
            InitialContextSetupRequestIEs_t *ie = context->protocolIEs.list.array[1];
            if (ie->id == ProtocolIE_ID_id_eNB_UE_S1AP_ID){
                ENB_UE_S1AP_ID_t *ueID = &ie->value.choice.ENB_UE_S1AP_ID;
                if (ueID)
                    {                       
                        int client  = get(packet_store, *ueID);
                        printf("Cleint ID is: %d\n", client);
                        printf("Client ID %d\n", *client_id);
                        *client_id = client;
                        printf("Client ID %d\n", client_id);

                    }
                    else
                    {
                        printf("ENB UE ID field is not properly initialized.\n");
                    }
            }
            break;
        }
        case InitiatingMessage__value_PR_UECapabilityInfoIndication:{
            printf("----------------------UECapabilityInfoIndication------------------\n");
            UECapabilityInfoIndication_t *ueCap = &initMsg->value.choice.UECapabilityInfoIndication;
            //replace_UECap_enb_ue_id(ueCap, output_buffer, output_size);
            ueCapabilityInfoInd_ue_id_mapping(ueCap, output_buffer, output_size, map);
            break;
        }
        case InitiatingMessage__value_PR_E_RABSetupRequest:{
            printf("----------------------E-RABSetupRequest------------------\n");
            E_RABSetupRequest_t *eRab = &initMsg->value.choice.E_RABSetupRequest;
            //replace_E_RABSetupReq_ue_id(eRab, output_buffer, output_size);
            e_RABSetupReq_ue_id_mapping(eRab, output_buffer, output_size, map);
            E_RABSetupRequestIEs_t *ie = eRab->protocolIEs.list.array[1];
            if (ie->id == ProtocolIE_ID_id_eNB_UE_S1AP_ID){
                ENB_UE_S1AP_ID_t *ueID = &ie->value.choice.ENB_UE_S1AP_ID;
                if (ueID)
                    {                       
                        int client  = get(packet_store, *ueID);
                        printf("Cleint ID is: %d\n", client);
                        printf("Client ID %d\n", *client_id);
                        *client_id = client;
                        printf("Client ID %d\n", client_id);

                    }
                    else
                    {
                        printf("ENB UE ID field is not properly initialized.\n");
                    }
            }
            break;
        }
        case InitiatingMessage__value_PR_UEContextReleaseCommand:{
            printf("-------------------UEContextReleaseCommand---------------------\n");
            UEContextReleaseCommand_t *command = &initMsg->value.choice.UEContextReleaseCommand;
            ueContextRelCommand_ue_id_mapping(command, output_buffer, output_size, map);
            UEContextReleaseCommand_IEs_t *ie = command->protocolIEs.list.array[0];
            if (ie->id == ProtocolIE_ID_id_UE_S1AP_IDs){
                ENB_UE_S1AP_ID_t *ueID = &ie->value.choice.UE_S1AP_IDs.choice.uE_S1AP_ID_pair->eNB_UE_S1AP_ID;
                if (ueID)
                    {                       
                        int client  = get(packet_store, *ueID);
                        printf("Cleint ID is: %d\n", client);
                        printf("Client ID %d\n", *client_id);
                        *client_id = client;
                        printf("Client ID %d\n", client_id);

                    }
                    else
                    {
                        printf("ENB UE ID field is not properly initialized.\n");
                    }
            }
            break;
        }
        case InitiatingMessage__value_PR_HandoverRequired:{
            printf("-------------------HandoverRequired---------------------\n");
            HandoverRequired_t *handover = &initMsg->value.choice.HandoverRequired;
            //handOverRequired_ue_id_mapping(handover, output_buffer, output_size, map);
            modify_target_id(handover,  output_buffer, output_size, enbMap);
            break;
        }
        default:
            printf("----------------------Some other Protocol------------------\n");
            break;
    }
}

void process_sucessfullMsg(SuccessfulOutcome_t *success, char *output_buffer, int output_size, char *NEW_CELL_ID){
    switch (success->value.present)
    {
        case SuccessfulOutcome__value_PR_UEContextReleaseComplete:{
            printf("----------------------UEContextReleaseComplete------------------\n");
            UEContextReleaseComplete_t *releaseCom = &success->value.choice.UEContextReleaseComplete;
            ueContextRelCom_ue_id_mapping(releaseCom, output_buffer, output_size, map);
            break;
        }
        case SuccessfulOutcome__value_PR_E_RABSetupResponse:{
            printf("----------------------E_RABSetupResponse------------------\n");
            E_RABSetupResponse_t *eRabRes = &success->value.choice.E_RABSetupResponse;
            e_RABSetupRes_ue_id_mapping(eRabRes, output_buffer, output_size, map);
            break;
        }
        case SuccessfulOutcome__value_PR_InitialContextSetupResponse:{
            printf("----------------------Initial Context Response------------------\n");
            InitialContextSetupResponse_t *contex = &success->value.choice.InitialContextSetupResponse;
            initialContextResponse_ue_id_mapping(contex, output_buffer, output_size, map);
            break;
        }
        default:
            //printf("----------------------Some other Successfulloutcome Message------------------\n");
            break;
    }
}

void check_gw_context_release_indication(UEContextReleaseRequest_t *releaseRequest, char *output_buffer, int output_size)
{
    printf("Checking GW Context Release Indication...\n");

    // Traverse the protocolIEs to find the GWContextReleaseIndication
    for (int i = 0; i < releaseRequest->protocolIEs.list.count; i++)
    {
        UEContextReleaseRequest_IEs_t *ie = releaseRequest->protocolIEs.list.array[i];

        // Check if the current IE is GWContextReleaseIndication
        if (ie->id == ProtocolIE_ID_id_GWContextReleaseIndication)
        {
            // Extract the value of GWContextReleaseIndication (which should be a boolean)
            GWContextReleaseIndication_t *gwContextIndication = &ie->value.choice.GWContextReleaseIndication;

            // Check if the GWContextReleaseIndication is set (true or false)
            if (*gwContextIndication == 1)
            { // Assuming 1 means it's set (true)
                snprintf(output_buffer, output_size, "GWContextReleaseIndication is set: TRUE");
                printf("%s\n", output_buffer); // Optional: print confirmation to console
            }
            else
            {
                snprintf(output_buffer, output_size, "GWContextReleaseIndication is not set: FALSE");
                printf("%s\n", output_buffer); // Optional: print confirmation to console
            }
        }
    }
}

void replace_enb_id(S1SetupRequest_t *setupRequest, char *output_buffer, int output_size)
{
    printf("OK\n");
    decimalToHex(decimal, NEW_ENB_ID);

    // Traverse the protocolIEs to find Global-ENB-ID
    for (int i = 0; i < setupRequest->protocolIEs.list.count; i++)
    {
        S1SetupRequestIEs_t *ie = setupRequest->protocolIEs.list.array[i];

        // Check if the current IE is Global-ENB-ID
        if (ie->id == ProtocolIE_ID_id_Global_ENB_ID)
        {
            Global_ENB_ID_t *enbID = &ie->value.choice.Global_ENB_ID;

            // Modify eNB-ID value
            if (enbID->eNB_ID.present == ENB_ID_PR_macroENB_ID)
            {
                printf("Inserting value: 0x%X\n", *(uint32_t *)(enbID->eNB_ID.choice.macroENB_ID.buf));
                insert(map, NEW_ENB_ID, *(uint32_t *)enbID->eNB_ID.choice.macroENB_ID.buf);
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

void cellIdentityCal(uint8_t *cellIdentity)
{
    printf("ok\n");
    uint8_t randomCellID = rand() % 256; // Generate random Cell ID
    printf("ok\n");
    int cellid = decimal * 256 + randomCellID; // Calculate new Cell ID
    printf("Calculated Cell ID: 0x%X\n", cellid);

    // Copy the 4-byte integer into the buffer
    cellIdentity[0] = (cellid >> 24) & 0xFF;
    cellIdentity[1] = (cellid >> 16) & 0xFF;
    cellIdentity[2] = (cellid >> 8) & 0xFF;
    cellIdentity[3] = cellid & 0xFF;
}

void replace_cell_id(InitialUEMessage_t *initialUEMsg, char *output_buffer, int output_size, char *NEW_CELL_ID)
{
    printf("Starting Cell ID replacement.\n");
    for (int i = 0; i < initialUEMsg->protocolIEs.list.count; i++)
    {
        InitialUEMessage_IEs_t *ie = initialUEMsg->protocolIEs.list.array[i];
        if (ie->id == ProtocolIE_ID_id_EUTRAN_CGI)
        {
            EUTRAN_CGI_t *cellID = &ie->value.choice.EUTRAN_CGI;
            if (cellID->cell_ID.buf != NULL && cellID->cell_ID.size == 4)
            {
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
            }
            else
            {
                printf("Cell ID field is not properly initialized or has an unexpected size.\n");
            }

            // Stop processing further as we've found the desired IE
            break;
        }
    }
}

void replace_uplink_cell_id(UplinkNASTransport_t *uplink, char *output_buffer, int output_size, char *NEW_CELL_ID)
{
    printf("Starting Cell ID replacement.\n");
    for (int i = 0; i < uplink->protocolIEs.list.count; i++)
    {
        UplinkNASTransport_IEs_t *ie = uplink->protocolIEs.list.array[i];
        if (ie->id == ProtocolIE_ID_id_EUTRAN_CGI)
        {
            EUTRAN_CGI_t *cellID = &ie->value.choice.EUTRAN_CGI;
            if (cellID->cell_ID.buf != NULL && cellID->cell_ID.size == 4)
            {
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
            }
            else
            {
                printf("Cell ID field is not properly initialized or has an unexpected size.\n");
            }

            // Stop processing further as we've found the desired IE
            break;
        }
    }
}

u_int32_t increment_enb_s1ap_id()
{
    return ++NEW_ENB_S1AP_ID;
}

void replace_enb_ue_id(InitialUEMessage_t *initialUEMsg, char *output_buffer, int output_size, int client_fd)
{
    printf("Starting ENB UE ID Replacement.\n");

    for (int i = 0; i < initialUEMsg->protocolIEs.list.count; i++)
    {
        InitialUEMessage_IEs_t *ie = initialUEMsg->protocolIEs.list.array[i];
        if (ie->id == ProtocolIE_ID_id_eNB_UE_S1AP_ID)
        {
            ENB_UE_S1AP_ID_t *ueID = &ie->value.choice.ENB_UE_S1AP_ID;

            if (ueID)
            {
                // Print the old ENB UE ID
                printf("Old ENB UE ID: %u\n", *ueID);
                // Replace the ENB UE ID with the new value
                increment_enb_s1ap_id();
                insert(map, NEW_ENB_S1AP_ID, *ueID);
                insert(packet_store, *ueID, client_fd);
                printf("Get = %d\n", get(packet_store, *ueID));
                *ueID = NEW_ENB_S1AP_ID;
                snprintf(output_buffer, output_size, "Replaced ENB UE ID with new value: %u", NEW_ENB_S1AP_ID);

                // Print the confirmation message
                printf("%s\n", output_buffer);
            }
            else
            {
                printf("ENB UE ID field is not properly initialized.\n");
            }

            // Stop processing further as we've found the desired IE
            break;
        }
    }
}

void s1ap_setup_unit(int mme_fd){
    S1AP_PDU_t *pdu = build_s1ap_setup_request(MCC_MNC_BUF, MCC_MNC_LEN, TAC_BUF, TAC_LEN, ENB_NAME, enbMap);
    if (!pdu) {
        fprintf(stderr, "Failed to build S1AP Setup Request\n");
        close(mme_fd);
        return EXIT_FAILURE;
    }

    // Encode the PDU
    uint8_t *encoded_buffer = NULL;
    size_t encoded_size = 0;
    if (encode_s1ap_pdu(pdu, &encoded_buffer, &encoded_size) != 0) {
        fprintf(stderr, "Failed to encode S1AP Setup Request\n");
        ASN_STRUCT_FREE(asn_DEF_S1AP_PDU, pdu);
        close(mme_fd);
        return EXIT_FAILURE;
    }

    // Send the message
    if (send_s1ap_message(mme_fd, encoded_buffer, encoded_size) != 0) {
        fprintf(stderr, "Failed to send S1AP Setup Request\n");
        free(encoded_buffer);
        ASN_STRUCT_FREE(asn_DEF_S1AP_PDU, pdu);
        close(mme_fd);
        return EXIT_FAILURE;
    }

    free(encoded_buffer);
    ASN_STRUCT_FREE(asn_DEF_S1AP_PDU, pdu);
}

void *handle_client(void *arg)
{
    client_data_t *client_data = (client_data_t *)arg;
    int client_fd = client_data->client_fd;
    int client_id = 0;
    struct sockaddr_in client_addrr;
    client_addrr.sin_addr = client_data->client_addr.address;
    client_addrr.sin_port = client_data->client_addr.port;
    fd_set read_fds;
    struct timeval timeout;
    char buffer[BUFFER_SIZE];
    char output_buffer[BUFFER_SIZE];
    struct sctp_sndrcvinfo sndrcvinfo;
    int flags;
    char NEW_CELL_ID[4];
    u_int32_t enb_ue_id = 0;

    free(arg); // Free memory allocated for client_data

    while (1)
    {
        // Initialize the file descriptor set
        FD_ZERO(&read_fds);
        FD_SET(client_fd, &read_fds);   // Add client_fd to the read set
        FD_SET(mme_fd, &read_fds);      // Add mme_fd to the read set

        // Set timeout for select call (can be set to NULL for no timeout)
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        // Wait for data from either client or MME
        int nfds = select(FD_SETSIZE, &read_fds, NULL, NULL, &timeout);
        if (nfds < 0)
        {
            perror("select error");
            break;
        }

        // Check if the client sent data
        if (FD_ISSET(client_fd, &read_fds))
        {
            // Receive data from the client
            int bytes_received = sctp_recvmsg(client_fd, buffer, BUFFER_SIZE, NULL, 0, &sndrcvinfo, &flags);
            if (bytes_received <= 0)
            {
                printf("Client disconnected or error occurred. Closing connection.\n");
                break;
            }
            buffer[bytes_received] = '\0';
            printf("Received from client on stream %d with Association ID %u: %s\n", sndrcvinfo.sinfo_stream, sndrcvinfo.sinfo_assoc_id, buffer);
            printf("Handling client from %s:%d and the current client ID: %d\n", inet_ntoa(client_addrr.sin_addr), ntohs(client_addrr.sin_port), client_fd);
            
            // Decode the S1AP message
            

            int encoded_size = decode_s1ap_message(buffer, bytes_received, output_buffer, BUFFER_SIZE, NEW_CELL_ID, client_fd, &client_id);
            printf("Client ID after decodeing is: %d\n", client_id);
            if (encoded_size < 0)
            {
                printf("Failed to decode S1AP message. Forwarding original packet to MME.\n");
                memcpy(output_buffer, buffer, bytes_received);
                encoded_size = bytes_received;
            }
            else if (check > 0){
                printf("Intercepted S1SetupRequest. Sending stored response to client.\n");
                pthread_mutex_lock(&mme_mutex);
                if (pdu_length > 0) {
                    // Send the stored response back to the client
                    sctp_sendmsg(client_fd, pdu_buffer, pdu_length, NULL, 0, 0, 0, sndrcvinfo.sinfo_stream, 0, 0);
                } else {
                    printf("No stored S1AP response to send.\n");
                }
                pthread_mutex_unlock(&mme_mutex);
                check = 0;

                continue;
            }

            // Forward the packet to the MME
            pthread_mutex_lock(&mme_mutex);
            sctp_sendmsg(mme_fd, output_buffer, encoded_size, NULL, 0, 0, 0, sndrcvinfo.sinfo_stream, 0, 0);
            pthread_mutex_unlock(&mme_mutex);
        }

        // Check if the MME sent a response
        if (FD_ISSET(mme_fd, &read_fds))
        {
            // Receive response from the MME
            int bytes_from_mme = sctp_recvmsg(mme_fd, buffer, BUFFER_SIZE, NULL, 0, &sndrcvinfo, &flags);
            if (bytes_from_mme <= 0)
            {
                printf("MME disconnected or error occurred. Closing connection.\n");
                break;
            }

            buffer[bytes_from_mme] = '\0';
            printf("Received from MME on stream %d: %s\n", sndrcvinfo.sinfo_stream, buffer);

            // Decode the S1AP message from MME
            printf("Client ID before decodeing is: %d\n", &client_id);
            int re_encoded_size = decode_s1ap_message(buffer, bytes_from_mme, output_buffer, BUFFER_SIZE, NEW_CELL_ID, client_fd, &client_id);
            printf("Client ID after decodeing is: %d\n", client_id);
            if (re_encoded_size < 0)
            {
                printf("Failed to decode S1AP message from MME. Forwarding original packet to client.\n");
                memcpy(output_buffer, buffer, bytes_from_mme);
                re_encoded_size = bytes_from_mme;
            }

            // Forward the packet to the client
            pthread_mutex_lock(&mme_mutex);
            sctp_sendmsg(client_id, output_buffer, re_encoded_size, NULL, 0, 0, 0, sndrcvinfo.sinfo_stream, 0, 0);
            pthread_mutex_unlock(&mme_mutex);
        }
    // printf("Stored SCTP PDU Information:\n");
    // printPdu(pduHead);
    }

    close(client_fd); // Close client connection
    return NULL;
}

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr, mme_addr;
    socklen_t addr_len = sizeof(client_addr);
    pthread_t thread_id;
    map = createHashMap(10);
    packet_store = createHashMap(10);
    enbMap = createHashMap(10);

    
    //assoc_map = create_assoc_map();

    // Connect to the MME server
    mme_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
    if (mme_fd < 0)
    {
        perror("MME socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&mme_addr, 0, sizeof(mme_addr));
    mme_addr.sin_family = AF_INET;
    mme_addr.sin_port = htons(MME_PORT);
    if (inet_pton(AF_INET, MME_ADDRESS, &mme_addr.sin_addr) <= 0)
    {
        perror("Invalid MME address");
        close(mme_fd);
        exit(EXIT_FAILURE);
    }
    

    if (connect(mme_fd, (struct sockaddr *)&mme_addr, sizeof(mme_addr)) < 0)
    {
        perror("Failed to connect to MME");
        close(mme_fd);
        exit(EXIT_FAILURE);
    }
   

    printf("Connected to MME at %s:%d\n", MME_ADDRESS, MME_PORT);
    //send s1setup request
    s1ap_setup_unit(mme_fd);

    pdu_buffer = receive_s1ap_message(mme_fd, &pdu_length);

    // Set up SCTP server for client connections
    server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
    //server_fd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
    
    if (server_fd < 0)
    {
        perror("Client socket creation failed");
        close(mme_fd);
        exit(EXIT_FAILURE);
    }

    struct sctp_initmsg initmsg = {0};
    initmsg.sinit_num_ostreams = 1000;
    initmsg.sinit_max_instreams = 1000;
    initmsg.sinit_max_attempts = 1000;

    setsockopt(server_fd, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(initmsg));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(CLIENT_PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(server_fd);
        close(mme_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        close(mme_fd);
        exit(EXIT_FAILURE);
    }

    printf("Listening for client connections on port %d...\n", CLIENT_PORT);

    while (1)
    {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        
        if (client_fd < 0)
        {
            perror("Accept failed");
            continue;
        }

        printf("New client connected from %s:%d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // int *client_fd_ptr = malloc(sizeof(int));
        // *client_fd_ptr = client_fd;

        client_data_t *client_data = malloc(sizeof(client_data_t));
        if (client_data == NULL)
        {
            perror("Failed to allocate memory for client data");
            close(client_fd);
            continue;
        }

        client_data->client_fd = client_fd;
        client_data->client_addr.address = client_addr.sin_addr;
        client_data->client_addr.port = client_addr.sin_port;
        

        if (pthread_create(&thread_id, NULL, handle_client, client_data) != 0)
        {
            perror("Thread creation failed");
            close(client_fd);
        }
        else
        {
            pthread_detach(thread_id); // Detach thread for automatic cleanup
        }
    }
    //cleanup_assoc_map(assoc_map);
    close(server_fd);
    close(mme_fd);
    free(map->buckets);
    free(map);
    return 0;
}
