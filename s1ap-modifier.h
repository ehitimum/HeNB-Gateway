#ifndef S1AP_Modifier_H
#define S1AP_Modifier_H

#include "S1AP-PDU.h"
#include "InitiatingMessage.h"
#include "asn_application.h"
#include "asn_codecs.h"
#include "Global-ENB-ID.h"
#include "ProtocolIE-Field.h"
#include "S1SetupRequest.h"
#include "hashmap.h"
#include "SuccessfulOutcome.h"
#include "UE-S1AP-ID-pair.h"

void downlinknasT_ue_id_mapping(DownlinkNASTransport_t *downLink, char *output_buffer, int output_size, HashMap *map);
void initialContextSetup_ue_id_mapping(InitialContextSetupRequest_t *context, char *output_buffer, int output_size, HashMap *map);
void initialContextResponse_ue_id_mapping(InitialContextSetupResponse_t *context, char *output_buffer, int output_size, HashMap *map);
void ueCapabilityInfoInd_ue_id_mapping(UECapabilityInfoIndication_t *cap, char *output_buffer, int output_size, HashMap *map);
void e_RABSetupReq_ue_id_mapping(E_RABSetupRequest_t *eRab, char *output_buffer, int output_size, HashMap *map);
void e_RABSetupRes_ue_id_mapping(E_RABSetupResponse_t *eRab, char *output_buffer, int output_size, HashMap *map);
void ueContextRelReq_ue_id_mapping(UEContextReleaseRequest_t *ueCont, char *output_buffer, int output_size, HashMap *map);
void ueContextRelCom_ue_id_mapping(UEContextReleaseComplete_t *ueCont, char *output_buffer, int output_size, HashMap *map);
void ueContextRelCommand_ue_id_mapping(UEContextReleaseCommand_t *command, char *output_buffer, int output_size, HashMap *map);
void enb_id_mapping(S1SetupRequest_t *setupRequest, char *output_buffer, int output_size, HashMap *map);
void cell_id_mapping(InitialUEMessage_t *initialUEMsg, char *output_buffer, int output_size, char *NEW_CELL_ID, HashMap *map);
void uplinknasT_cell_id_mapping(UplinkNASTransport_t *uplink, char *output_buffer, int output_size, char *NEW_CELL_ID, HashMap *map);
u_int32_t increment_enb_s1ap_id();
void initialUeMsg_enb_ue_id_mapping(InitialUEMessage_t *initialUEMsg, char *output_buffer, int output_size, HashMap *map, char *NEW_ENB_S1AP_ID);
void uplinknasT_enb_ue_id_mapping(UplinkNASTransport_t *uplink, char *output_buffer, int output_size, HashMap *map);

#endif