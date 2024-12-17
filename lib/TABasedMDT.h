/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "S1AP-IEs"
 * 	found in "36413-g10.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps -no-gen-BER -no-gen-XER -no-gen-OER -no-gen-UPER -no-gen-JER`
 */

#ifndef	_TABasedMDT_H_
#define	_TABasedMDT_H_


#include <asn_application.h>

/* Including external dependencies */
#include "TAListforMDT.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct ProtocolExtensionContainer;

/* TABasedMDT */
typedef struct TABasedMDT {
	TAListforMDT_t	 tAListforMDT;
	struct ProtocolExtensionContainer	*iE_Extensions;	/* OPTIONAL */
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} TABasedMDT_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_TABasedMDT;
extern asn_SEQUENCE_specifics_t asn_SPC_TABasedMDT_specs_1;
extern asn_TYPE_member_t asn_MBR_TABasedMDT_1[2];

#ifdef __cplusplus
}
#endif

#endif	/* _TABasedMDT_H_ */
#include <asn_internal.h>
