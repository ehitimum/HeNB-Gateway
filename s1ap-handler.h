#ifndef S1AP_HANDLER_H
#define S1AP_HANDLER_H

#include "S1AP-PDU.h"
#include "InitiatingMessage.h"
#include "SuccessfulOutcome.h"
#include "S1SetupRequest.h"
#include "Global-ENB-ID.h"
#include "SupportedTAs.h"
#include "SupportedTAs-Item.h"
#include "ProtocolIE-Field.h"
#include "ProtocolIE-ID.h"
#include "Criticality.h"
#include "ProcedureCode.h"
#include "ENB-ID.h"
#include "BIT_STRING.h"
#include "OCTET_STRING.h"
#include "asn_application.h"
#include "asn_internal.h"
#include "per_encoder.h"
#include "per_decoder.h"
#include "hashmap.h"  // For aper_decode()

// #define MME_IP "192.168.220.9"
// #define MME_PORT 36412

// Define PLMN and TAC


#define BUFFER_SIZE 9999

int BIT_STRING_fromBuf(BIT_STRING_t *bit_str, const uint8_t *buf, size_t bit_len);
S1AP_PDU_t *build_s1ap_setup_request(char *MCC_MNC_BUF, int MCC_MNC_LEN, char *TAC_BUF, int TAC_LEN, char *ENB_NAME, HashMap *map);
int encode_s1ap_pdu(S1AP_PDU_t *pdu, uint8_t **buffer, size_t *buffer_size);
int send_s1ap_message(int sock, uint8_t *buffer, size_t buffer_size);
uint8_t* receive_s1ap_message(int sock, size_t *pdu_length);
void print_s1ap_pdu(S1AP_PDU_t *pdu);




#endif