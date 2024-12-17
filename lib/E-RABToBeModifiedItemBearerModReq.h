/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "S1AP-PDU-Contents"
 * 	found in "36413-g10.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps -no-gen-BER -no-gen-XER -no-gen-OER -no-gen-UPER -no-gen-JER`
 */

#ifndef	_E_RABToBeModifiedItemBearerModReq_H_
#define	_E_RABToBeModifiedItemBearerModReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include "E-RAB-ID.h"
#include "E-RABLevelQoSParameters.h"
#include "NAS-PDU.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct ProtocolExtensionContainer;

/* E-RABToBeModifiedItemBearerModReq */
typedef struct E_RABToBeModifiedItemBearerModReq {
	E_RAB_ID_t	 e_RAB_ID;
	E_RABLevelQoSParameters_t	 e_RABLevelQoSParameters;
	NAS_PDU_t	 nAS_PDU;
	struct ProtocolExtensionContainer	*iE_Extensions;	/* OPTIONAL */
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} E_RABToBeModifiedItemBearerModReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_E_RABToBeModifiedItemBearerModReq;
extern asn_SEQUENCE_specifics_t asn_SPC_E_RABToBeModifiedItemBearerModReq_specs_1;
extern asn_TYPE_member_t asn_MBR_E_RABToBeModifiedItemBearerModReq_1[4];

#ifdef __cplusplus
}
#endif

#endif	/* _E_RABToBeModifiedItemBearerModReq_H_ */
#include <asn_internal.h>