#include "s1ap-modifier.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void map_old_ue_id(ENB_UE_S1AP_ID_t *ueID, char *output_buffer, int output_size, HashMap *map)
{
    if (ueID)
    {
        printf("Current ENB UE ID: %u\n", *ueID);
        uint32_t value = get(map, *ueID);
        *ueID = value;
        snprintf(output_buffer, output_size, "Replaced ENB UE ID with old value: %u", value);
        printf("UEID IS NOW: %u\n", *ueID);
        printf("%s\n", output_buffer);
    }
    else
    {
        printf("ENB UE ID field is not properly initialized.\n");
    }
}

void map_new_ue_id(ENB_UE_S1AP_ID_t *ueID, char *output_buffer, int output_size, HashMap *map)
{
    if (ueID)
    {
        printf("Current ENB UE ID: %u\n", *ueID);
        u_int32_t value = getHashKey(map, *ueID);
        *ueID = value;
        snprintf(output_buffer, output_size, "Replaced ENB UE ID with new value: %u", value);
        printf("%s\n", output_buffer);
    }
    else
    {
        printf("ENB UE ID field is not properly initialized.\n");
    }
}


void initialUeMsg_enb_ue_id_mapping(InitialUEMessage_t *initialUEMsg, char *output_buffer, int output_size, HashMap *map, char *NEW_ENB_S1AP_ID)
{
    InitialUEMessage_IEs_t *ie = initialUEMsg->protocolIEs.list.array[0];
    if (ie->id == ProtocolIE_ID_id_eNB_UE_S1AP_ID){
        insert(map, NEW_ENB_S1AP_ID, &ie->value.choice.ENB_UE_S1AP_ID);
        map_new_ue_id(&ie->value.choice.ENB_UE_S1AP_ID, output_buffer, output_size, map);
    } 
    
}

void uplinknasT_enb_ue_id_mapping(UplinkNASTransport_t *uplink, char *output_buffer, int output_size, HashMap *map)
{
    UplinkNASTransport_IEs_t *ie = uplink->protocolIEs.list.array[1];
    if (ie->id == ProtocolIE_ID_id_eNB_UE_S1AP_ID) map_new_ue_id(&ie->value.choice.ENB_UE_S1AP_ID, output_buffer, output_size, map);
}

void downlinknasT_ue_id_mapping(DownlinkNASTransport_t *downLink, char *output_buffer, int output_size, HashMap *map)
{
    DownlinkNASTransport_IEs_t *ie = downLink->protocolIEs.list.array[1];
    if (ie->id == ProtocolIE_ID_id_eNB_UE_S1AP_ID) map_old_ue_id(&ie->value.choice.ENB_UE_S1AP_ID, output_buffer, output_size, map);
}

void initialContextSetup_ue_id_mapping(InitialContextSetupRequest_t *context, char *output_buffer, int output_size, HashMap *map)
{
    InitialContextSetupRequestIEs_t *ie = context->protocolIEs.list.array[1];
    if (ie->id == ProtocolIE_ID_id_eNB_UE_S1AP_ID) map_old_ue_id(&ie->value.choice.ENB_UE_S1AP_ID, output_buffer, output_size, map);
}

void initialContextResponse_ue_id_mapping(InitialContextSetupResponse_t *context, char *output_buffer, int output_size, HashMap *map)
{
    InitialContextSetupResponseIEs_t *ie = context->protocolIEs.list.array[1];
    if (ie->id == ProtocolIE_ID_id_eNB_UE_S1AP_ID) map_new_ue_id(&ie->value.choice.ENB_UE_S1AP_ID, output_buffer, output_size, map);
}

void ueCapabilityInfoInd_ue_id_mapping(UECapabilityInfoIndication_t *cap, char *output_buffer, int output_size, HashMap *map)
{
    UECapabilityInfoIndicationIEs_t *ie = cap->protocolIEs.list.array[1];
    if (ie->id == ProtocolIE_ID_id_eNB_UE_S1AP_ID) map_new_ue_id(&ie->value.choice.ENB_UE_S1AP_ID, output_buffer, output_size, map);
}

void e_RABSetupReq_ue_id_mapping(E_RABSetupRequest_t *eRab, char *output_buffer, int output_size, HashMap *map)
{
    E_RABSetupRequestIEs_t *ie = eRab->protocolIEs.list.array[1];
    if (ie->id == ProtocolIE_ID_id_eNB_UE_S1AP_ID) map_old_ue_id(&ie->value.choice.ENB_UE_S1AP_ID, output_buffer, output_size, map);
}

void e_RABSetupRes_ue_id_mapping(E_RABSetupResponse_t *eRab, char *output_buffer, int output_size, HashMap *map)
{
    E_RABSetupResponseIEs_t *ie = eRab->protocolIEs.list.array[1];
    if (ie->id == ProtocolIE_ID_id_eNB_UE_S1AP_ID) map_new_ue_id(&ie->value.choice.ENB_UE_S1AP_ID, output_buffer, output_size, map);
}

void ueContextRelReq_ue_id_mapping(UEContextReleaseRequest_t *ueCont, char *output_buffer, int output_size, HashMap *map)
{
    UEContextReleaseRequest_IEs_t *ie = ueCont->protocolIEs.list.array[1];
    if (ie->id == ProtocolIE_ID_id_eNB_UE_S1AP_ID) map_new_ue_id(&ie->value.choice.ENB_UE_S1AP_ID, output_buffer, output_size, map);
}

void ueContextRelCom_ue_id_mapping(UEContextReleaseComplete_t *ueCont, char *output_buffer, int output_size, HashMap *map)
{
    UEContextReleaseComplete_IEs_t *ie = ueCont->protocolIEs.list.array[1];
    if (ie->id == ProtocolIE_ID_id_eNB_UE_S1AP_ID) map_new_ue_id(&ie->value.choice.ENB_UE_S1AP_ID, output_buffer, output_size, map);
}

void ueContextRelCommand_ue_id_mapping(UEContextReleaseCommand_t *command, char *output_buffer, int output_size, HashMap *map)
{
    UEContextReleaseCommand_IEs_t *ie = command->protocolIEs.list.array[0];
    if (ie->id == ProtocolIE_ID_id_UE_S1AP_IDs){
        //UE_S1AP_ID_pair_t *uePair = &ie->value.choice.UE_S1AP_IDs.choice.uE_S1AP_ID_pair;
        ENB_UE_S1AP_ID_t *ueID = &ie->value.choice.UE_S1AP_IDs.choice.uE_S1AP_ID_pair->eNB_UE_S1AP_ID;
        printf("Current ENB UE ID: %u\n", *ueID);
        map_old_ue_id(ueID, output_buffer, output_size, map);
    }
}






