#ifndef SCTP_PDU_LIST_H
#define SCTP_PDU_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

typedef struct 
{
    uint32_t assoc_id;               // Association ID
    int client_fd;
    char address;
    int port;                   // Client socket file descriptor
    // struct sockaddr_in client_addr;  // Client address (e.g., HeNB IP)
    // struct {
    //     //in_port_t port;
    //     //struct in_addr address;
    //     char address;
    //     int port;
    // } client_addr;
}Sctp_Pdu_Info_t;

typedef struct Nexus_Sctp_t{
    Sctp_Pdu_Info_t pduinfo;
    struct Nexus_Sctp_t* next;

}Nexus_Sctp_t;


Nexus_Sctp_t* createSCTPnode(Sctp_Pdu_Info_t pduinfo);

/**
 * Add new Sctp Info in the linked List
 */

void store_sctp_info(Nexus_Sctp_t** head, Sctp_Pdu_Info_t pduinfo);
/**
 * Seacrh via association ID
 */
Nexus_Sctp_t* searchPdu(Nexus_Sctp_t* head, unsigned int assocID);
void printPdu(Nexus_Sctp_t* head);
void freePduList(Nexus_Sctp_t** head);


#endif