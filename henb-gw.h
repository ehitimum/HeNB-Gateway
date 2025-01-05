#ifndef H_ENB_GW

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/select.h>
#include <netinet/sctp.h>

#include "S1AP-PDU.h"
#include "s1ap-modifier.h"
#include "s1ap-handler.h"
#include "InitiatingMessage.h"
#include "asn_application.h"
#include "asn_codecs.h"
#include "Global-ENB-ID.h"
#include "ProtocolIE-Field.h"
#include "S1SetupRequest.h"
#include "hashmap.h"
#include "SuccessfulOutcome.h"

typedef struct
{
    int client_fd;
    struct {
        in_port_t port;
        struct in_addr address;
    } client_addr;
    char s1ap_info_buffer[BUFFER_SIZE];
} client_data_t;


pthread_mutex_t packet_store_mutex = PTHREAD_MUTEX_INITIALIZER;
typedef struct {
    int client_fd;
    u_int32_t enb_ue_s1ap_id;
} sctp_packet_info_t;


void decimalToHex(int decimal, char *hexStr);
int decode_s1ap_message(const char *input, int input_size, char *output_buffer, int output_size, char *NEW_CELL_ID, int client_fd, int *client_id);
void process_initating_msg(InitiatingMessage_t *initMsg, char *output_buffer, int output_size, char *NEW_CELL_ID, int client_fd, int *client_id);
void process_sucessfullMsg(SuccessfulOutcome_t *success, char *output_buffer, int output_size, char *NEW_CELL_ID);
void check_gw_context_release_indication(UEContextReleaseRequest_t *releaseRequest, char *output_buffer, int output_size);
void replace_enb_id(S1SetupRequest_t *setupRequest, char *output_buffer, int output_size);
void cellIdentityCal(uint8_t *cellIdentity);
void replace_cell_id(InitialUEMessage_t *initialUEMsg, char *output_buffer, int output_size, char *NEW_CELL_ID);
void replace_uplink_cell_id(UplinkNASTransport_t *uplink, char *output_buffer, int output_size, char *NEW_CELL_ID);
void replace_enb_ue_id(InitialUEMessage_t *initialUEMsg, char *output_buffer, int output_size, int client_fd);
void s1ap_setup_unit(int mme_fd);
void *handle_client(void *arg);

#endif /*H_ENB_GW*/