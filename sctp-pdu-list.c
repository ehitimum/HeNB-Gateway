#include "sctp-pdu-list.h"


Nexus_Sctp_t* createSCTPnode(Sctp_Pdu_Info_t pduinfo){
    Nexus_Sctp_t* newNode = (Nexus_Sctp_t*)malloc(sizeof(Nexus_Sctp_t));
    if (!newNode)
    {
        perror("Memory Allocation Failed.");
        exit(EXIT_FAILURE);
    }
    newNode->pduinfo = pduinfo;
    newNode->next = NULL;
    return newNode;
}

void store_sctp_info(Nexus_Sctp_t** head, Sctp_Pdu_Info_t pduinfo){
    Nexus_Sctp_t* newNode = createSCTPnode(pduinfo);
    if (*head == NULL)
    {
        *head = newNode;
    }
    else
    {
        Nexus_Sctp_t* temp = *head;
        while(temp->next != NULL){
            temp = temp->next;
        }
        temp->next = newNode;
    }
    printf("Succesful");
}

Nexus_Sctp_t* searchPdu(Nexus_Sctp_t* head, unsigned int assocID){
    Nexus_Sctp_t* temp = head;
    while (temp != NULL)
    {
        if (temp->pduinfo.assoc_id == assocID){}
        {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

void printPdu(Nexus_Sctp_t* head){
    Nexus_Sctp_t* temp = head;
    while (temp != NULL)
    {
        //printf("Source IP: %s, Source Port: %d\n", temp->pduinfo.address, temp->pduinfo.port);
        printf("Client ID: %d\n", temp->pduinfo.client_fd);
        printf("Association ID: %u\n\n", temp->pduinfo.assoc_id);
        temp = temp->next;
    }
    
}

void freePduList(Nexus_Sctp_t** head){
    Nexus_Sctp_t* temp;
    while (*head != NULL)
    {
        temp = *head;
        *head = (*head)->next;
        free(temp);
    }
}