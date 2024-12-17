/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "S1AP-IEs"
 * 	found in "36413-g10.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps -no-gen-BER -no-gen-XER -no-gen-OER -no-gen-UPER -no-gen-JER`
 */

#ifndef	_ProSeAuthorized_H_
#define	_ProSeAuthorized_H_


#include <asn_application.h>

/* Including external dependencies */
#include "ProSeDirectDiscovery.h"
#include "ProSeDirectCommunication.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct ProtocolExtensionContainer;

/* ProSeAuthorized */
typedef struct ProSeAuthorized {
	ProSeDirectDiscovery_t	*proSeDirectDiscovery;	/* OPTIONAL */
	ProSeDirectCommunication_t	*proSeDirectCommunication;	/* OPTIONAL */
	struct ProtocolExtensionContainer	*iE_Extensions;	/* OPTIONAL */
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} ProSeAuthorized_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_ProSeAuthorized;
extern asn_SEQUENCE_specifics_t asn_SPC_ProSeAuthorized_specs_1;
extern asn_TYPE_member_t asn_MBR_ProSeAuthorized_1[3];

#ifdef __cplusplus
}
#endif

#endif	/* _ProSeAuthorized_H_ */
#include <asn_internal.h>