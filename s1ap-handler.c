#include "s1ap-handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Define MME IP and PORT (adjust as needed)


int BIT_STRING_fromBuf(BIT_STRING_t *bit_str, const uint8_t *buf, size_t bit_len) {
    if (!bit_str || !buf) {
        fprintf(stderr, "BIT_STRING_fromBuf: Invalid parameters\n");
        return -1;
    }

    bit_str->buf = calloc(1, (bit_len + 7) / 8);
    if (!bit_str->buf) {
        perror("calloc failed");
        return -1;
    }

    memcpy(bit_str->buf, buf, (bit_len + 7) / 8);
    bit_str->size = (bit_len + 7) / 8;
    bit_str->bits_unused = 8 - (bit_len % 8);
    if (bit_str->bits_unused == 8) bit_str->bits_unused = 0;

    return 0;
}

void decimalToHexadecimal(int decimal, char *hexStr)
{
    // Use sprintf to convert decimal to hexadecimal
    sprintf(hexStr, "%X", decimal);
}

void decimalToASCII(int decimal, char *asciiStr) {
    // Use sprintf to convert decimal to its ASCII string representation
    sprintf(asciiStr, "%d", decimal);
}

// Build Global-ENB-ID IE
static S1SetupRequestIEs_t *build_global_enb_id_ie(char *MCC_MNC_BUF, int MCC_MNC_LEN, HashMap *map) {
    S1SetupRequestIEs_t *ie_global_enb_id = calloc(1, sizeof(S1SetupRequestIEs_t));
    if (!ie_global_enb_id) {
        perror("calloc failed");
        return NULL;
    }

    ie_global_enb_id->id = ProtocolIE_ID_id_Global_ENB_ID;
    ie_global_enb_id->criticality = Criticality_reject;
    ie_global_enb_id->value.present = S1SetupRequestIEs__value_PR_Global_ENB_ID;

    Global_ENB_ID_t *global_enb_id = &ie_global_enb_id->value.choice.Global_ENB_ID;

    // Set PLMN ID: MCC=001, MNC=01
    if (OCTET_STRING_fromBuf(&global_enb_id->pLMNidentity, MCC_MNC_BUF, MCC_MNC_LEN) != 0) {
        perror("OCTET_STRING_fromBuf failed");
        free(ie_global_enb_id);
        return NULL;
    }

    // Example eNB ID (20-bit)
    global_enb_id->eNB_ID.present = ENB_ID_PR_macroENB_ID;

    uint32_t enb_id_data[] = { 0xAA, 0xBC, 0x6E };
    int decimal_enbId = 104050;
    char hex_enb_id[10];
    decimalToHexadecimal(decimal_enbId, hex_enb_id);

    //uint32_t enb_id_packed = 0; 
    //enb_id_packed = (enb_id_data[0] << 16) | (enb_id_data[1] << 8) | enb_id_data[2];
    //printf("Packed enb ID is: %u (0x%X)\n", enb_id_packed, enb_id_packed);
    printf("ENB ID: 0x%X and %u\n", hex_enb_id, hex_enb_id);
    if (BIT_STRING_fromBuf(&global_enb_id->eNB_ID.choice.macroENB_ID, hex_enb_id, 20) != 0) {
        perror("BIT_STRING_fromBuf failed");
        free(ie_global_enb_id);
        return NULL;
    }
    insert(map, 1,  *(uint32_t *)global_enb_id->eNB_ID.choice.macroENB_ID.buf);
    //insert(map, 1, enb_id_packed);
    //printf("Stored enb ID is: %X\n", get(map, 1));
    uint32_t retrieved_enb_id = get(map, 1);
    printf("Retrieved enb ID is: %X\n", retrieved_enb_id);

    return ie_global_enb_id;
}

// Build SupportedTAs IE
static S1SetupRequestIEs_t *build_supported_tas_ie(char *MCC_MNC_BUF, int MCC_MNC_LEN, char *TAC_BUF, int TAC_LEN) {
    S1SetupRequestIEs_t *ie_supported_tas = calloc(1, sizeof(S1SetupRequestIEs_t));
    if (!ie_supported_tas) {
        perror("calloc failed");
        return NULL;
    }

    ie_supported_tas->id = ProtocolIE_ID_id_SupportedTAs;
    ie_supported_tas->criticality = Criticality_ignore;
    ie_supported_tas->value.present = S1SetupRequestIEs__value_PR_SupportedTAs;

    SupportedTAs_t *supported_tas = &ie_supported_tas->value.choice.SupportedTAs;

    SupportedTAs_Item_t *supported_ta_item = calloc(1, sizeof(SupportedTAs_Item_t));
    if (!supported_ta_item) {
        perror("calloc failed");
        free(ie_supported_tas);
        return NULL;
    }

    // TAC = 1
    if (OCTET_STRING_fromBuf(&supported_ta_item->tAC, TAC_BUF, TAC_LEN) != 0) {
        perror("OCTET_STRING_fromBuf failed");
        free(supported_ta_item);
        free(ie_supported_tas);
        return NULL;
    }

    PLMNidentity_t *plmn_id = calloc(1, sizeof(PLMNidentity_t));
    if (!plmn_id) {
        perror("calloc failed");
        free(supported_ta_item);
        free(ie_supported_tas);
        return NULL;
    }

    if (OCTET_STRING_fromBuf(plmn_id, MCC_MNC_BUF, MCC_MNC_LEN) != 0) {
        perror("OCTET_STRING_fromBuf failed");
        free(plmn_id);
        free(supported_ta_item);
        free(ie_supported_tas);
        return NULL;
    }

    if (ASN_SEQUENCE_ADD(&supported_ta_item->broadcastPLMNs.list, plmn_id) != 0) {
        perror("ASN_SEQUENCE_ADD failed");
        free(plmn_id);
        free(supported_ta_item);
        free(ie_supported_tas);
        return NULL;
    }

    if (ASN_SEQUENCE_ADD(&supported_tas->list, supported_ta_item) != 0) {
        perror("ASN_SEQUENCE_ADD failed");
        free(ie_supported_tas);
        return NULL;
    }

    return ie_supported_tas;
}

static S1SetupRequestIEs_t *build_enb_name_ie(char *ENB_NAME){
    S1SetupRequestIEs_t *ie_enb_name = calloc(1, sizeof(S1SetupRequestIEs_t));\
    if(!ie_enb_name){
        perror("calloc failed");
        return NULL;
    }

    ie_enb_name->id = ProtocolIE_ID_id_eNBname;
    ie_enb_name->criticality = Criticality_ignore;
    ie_enb_name->value.present = S1SetupRequestIEs__value_PR_ENBname;

    ENBname_t *enb_Name = &ie_enb_name->value.choice.ENBname;
    //const char *ENB_NAME = "Test Small Cell";
    if (OCTET_STRING_fromBuf(&ie_enb_name->value.choice.ENBname, ENB_NAME, strlen(ENB_NAME)) != 0) {
        perror("OCTET_STRING_fromBuf failed");
        free(ie_enb_name);
        return NULL;
    }

    return ie_enb_name;
    
}

// Build the entire S1SetupRequest PDU
S1AP_PDU_t *build_s1ap_setup_request(char *MCC_MNC_BUF, int MCC_MNC_LEN, char *TAC_BUF, int TAC_LEN, char *ENB_NAME, HashMap *map) {
    S1AP_PDU_t *pdu = calloc(1, sizeof(S1AP_PDU_t));
    if (!pdu) {
        perror("calloc failed");
        return NULL;
    }

    pdu->present = S1AP_PDU_PR_initiatingMessage;
    pdu->choice.initiatingMessage = calloc(1, sizeof(InitiatingMessage_t));
    if (!pdu->choice.initiatingMessage) {
        perror("calloc failed");
        free(pdu);
        return NULL;
    }

    InitiatingMessage_t *init_msg = pdu->choice.initiatingMessage;
    init_msg->procedureCode = ProcedureCode_id_S1Setup;
    init_msg->criticality = Criticality_reject;
    init_msg->value.present = InitiatingMessage__value_PR_S1SetupRequest;

    S1SetupRequest_t *setup_request = &init_msg->value.choice.S1SetupRequest;

    // Add Global ENB ID IE
    S1SetupRequestIEs_t *ie_global_enb_id = build_global_enb_id_ie(MCC_MNC_BUF, MCC_MNC_LEN, map);
    if (!ie_global_enb_id) {
        ASN_STRUCT_FREE(asn_DEF_S1AP_PDU, pdu);
        return NULL;
    }

    if (ASN_SEQUENCE_ADD(&setup_request->protocolIEs.list, ie_global_enb_id) != 0) {
        perror("ASN_SEQUENCE_ADD failed");
        ASN_STRUCT_FREE(asn_DEF_S1AP_PDU, pdu);
        return NULL;
    }

    S1SetupRequestIEs_t *ie_enb_name = build_enb_name_ie(ENB_NAME);
    if(!ie_enb_name){
        ASN_STRUCT_FREE(asn_DEF_S1AP_PDU, pdu);
        return NULL;
    }

    if (ASN_SEQUENCE_ADD(&setup_request->protocolIEs.list, ie_enb_name) != 0){
        perror("ASN_SEQUENCE_ADD failed");
        ASN_STRUCT_FREE(asn_DEF_S1AP_PDU, pdu);
        return NULL;
    }
    // Add SupportedTAs IE
    S1SetupRequestIEs_t *ie_supported_tas = build_supported_tas_ie(MCC_MNC_BUF, MCC_MNC_LEN, TAC_BUF, TAC_LEN);
    if (!ie_supported_tas) {
        ASN_STRUCT_FREE(asn_DEF_S1AP_PDU, pdu);
        return NULL;
    }

    if (ASN_SEQUENCE_ADD(&setup_request->protocolIEs.list, ie_supported_tas) != 0) {
        perror("ASN_SEQUENCE_ADD failed");
        ASN_STRUCT_FREE(asn_DEF_S1AP_PDU, pdu);
        return NULL;
    }

    return pdu;
}

// Encode the S1AP PDU using APER
int encode_s1ap_pdu(S1AP_PDU_t *pdu, uint8_t **buffer, size_t *buffer_size) {
    ssize_t encoded_size = aper_encode_to_new_buffer(&asn_DEF_S1AP_PDU, NULL, pdu, (void **)buffer);
    if (encoded_size < 0) {
        fprintf(stderr, "Encoding failed: Unable to encode PDU\n");
        return -1;
    }

    *buffer_size = (size_t)encoded_size;
    return 0;
}

// Send S1AP message to MME via SCTP
int send_s1ap_message(int sock, uint8_t *buffer, size_t buffer_size) {
    ssize_t sent_bytes = send(sock, buffer, buffer_size, 0);
    if (sent_bytes < 0) {
        perror("send");
        return -1;
    }

    printf("Message sent successfully: %ld bytes\n", sent_bytes);
    return 0;
}

uint8_t* receive_s1ap_message(int sock, size_t *pdu_length) {
    uint8_t recv_buf[BUFFER_SIZE];
    ssize_t received = recv(sock, recv_buf, sizeof(recv_buf), 0);
    if (received < 0) {
        perror("recv");
        return NULL;
    } else if (received == 0) {
        printf("MME closed the connection.\n");
        return NULL;
    }

    printf("Received %ld bytes from MME\n", received);

    // Decode the received message
    S1AP_PDU_t *pdu = calloc(1, sizeof(S1AP_PDU_t));
    if (!pdu) {
        perror("calloc failed");
        return NULL;
    }

    // Provide skip_bits=0 and unused_bits=0 as required by aper_decode signature
    asn_dec_rval_t rval = aper_decode(
        NULL,                 // No special codec context
        &asn_DEF_S1AP_PDU,    // The type descriptor
        (void **)&pdu,        // Pointer to output structure
        recv_buf,             // Input buffer
        received,             // Size of input buffer
        0,                    // skip_bits
        0                     // unused_bits
    );

    if (rval.code != RC_OK) {
        fprintf(stderr, "Decoding failed with code %d\n", rval.code);
        ASN_STRUCT_FREE(asn_DEF_S1AP_PDU, pdu);
        return NULL;
    }

    // Check if the PDU is an S1 Response
    if (pdu->present == S1AP_PDU_PR_successfulOutcome) {
        SuccessfulOutcome_t *successfulOutcome = pdu->choice.successfulOutcome;
        if (successfulOutcome->procedureCode == ProcedureCode_id_S1Setup) {
            printf("Received an S1 Setup Response PDU.\n");
        } else {
            printf("Received a successful outcome PDU, but not S1 Setup Response.\n");
        }
    } else {
        printf("Received PDU is not a successful outcome (not an S1 Response).\n");
        ASN_STRUCT_FREE(asn_DEF_S1AP_PDU, pdu);
        return NULL;
    }

    // Encode the PDU using the provided encoder function
    uint8_t *encoded_buf = NULL;
    if (encode_s1ap_pdu(pdu, &encoded_buf, pdu_length) < 0) {
        fprintf(stderr, "Failed to encode S1AP PDU.\n");
        ASN_STRUCT_FREE(asn_DEF_S1AP_PDU, pdu);
        return NULL;
    }

    // Free the decoded PDU structure
    ASN_STRUCT_FREE(asn_DEF_S1AP_PDU, pdu);

    // Return the encoded buffer
    return encoded_buf;
}




// Receive and decode S1AP message from MME
// int receive_s1ap_message(int sock) {
//     uint8_t recv_buf[BUFFER_SIZE];
//     ssize_t received = recv(sock, recv_buf, sizeof(recv_buf), 0);
//     if (received < 0) {
//         perror("recv");
//         return -1;
//     } else if (received == 0) {
//         printf("MME closed the connection.\n");
//         return -1;
//     }

//     printf("Received %ld bytes from MME\n", received);

//     // Decode the received message
//     S1AP_PDU_t *pdu = calloc(1, sizeof(S1AP_PDU_t));
//     if (!pdu) {
//         perror("calloc failed");
//         return -1;
//     }

//     // Provide skip_bits=0 and unused_bits=0 as required by aper_decode signature
//     asn_dec_rval_t rval = aper_decode(
//         NULL,                 // No special codec context
//         &asn_DEF_S1AP_PDU,    // The type descriptor
//         (void **)&pdu,        // Pointer to output structure
//         recv_buf,             // Input buffer
//         received,             // Size of input buffer
//         0,                    // skip_bits
//         0                     // unused_bits
//     );

//     if (rval.code != RC_OK) {
//         fprintf(stderr, "Decoding failed with code %d\n", rval.code);
//         ASN_STRUCT_FREE(asn_DEF_S1AP_PDU, pdu);
//         return -1;
//     }

//     // Print and understand the PDU
//     print_s1ap_pdu(pdu);

//     ASN_STRUCT_FREE(asn_DEF_S1AP_PDU, pdu);
//     return 0;
// }

// A simple print function to understand the response PDU
void print_s1ap_pdu(S1AP_PDU_t *pdu) {
    switch (pdu->present) {
        case S1AP_PDU_PR_initiatingMessage:
            printf("Received an InitiatingMessage from MME (Unexpected for S1 Setup Response)\n");
            break;
        case S1AP_PDU_PR_successfulOutcome:
            printf("Received a SuccessfulOutcome message from MME.\n");
            // Further parsing of pdu->choice.successfulOutcome->value.choice.S1SetupResponse is possible here.
            if (pdu->choice.successfulOutcome != NULL) {
                printf("  Procedure Code: %ld\n", pdu->choice.successfulOutcome->procedureCode);
                printf("  Criticality: %d\n", pdu->choice.successfulOutcome->criticality);
                printf("  Decoding Successful Outcome...\n");
                // Further parsing of pdu->choice.successfulOutcome->value.choice.S1SetupResponse
                // Use the specific ASN.1 type for the message.
            } else {
                printf("  SuccessfulOutcome is NULL.\n");
            }
            break;
        case S1AP_PDU_PR_unsuccessfulOutcome:
            printf("Received an UnsuccessfulOutcome message from MME. S1 Setup failed.\n");
            break;
        default:
            printf("Received an unknown type of message.\n");
            break;
    }
}