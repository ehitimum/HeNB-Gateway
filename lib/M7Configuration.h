/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "S1AP-IEs"
 * 	found in "36413-g10.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps -no-gen-BER -no-gen-XER -no-gen-OER -no-gen-UPER -no-gen-JER`
 */

#ifndef	_M7Configuration_H_
#define	_M7Configuration_H_


#include <asn_application.h>

/* Including external dependencies */
#include "M7period.h"
#include "Links-to-log.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct ProtocolExtensionContainer;

/* M7Configuration */
typedef struct M7Configuration {
	M7period_t	 m7period;
	Links_to_log_t	 m7_links_to_log;
	struct ProtocolExtensionContainer	*iE_Extensions;	/* OPTIONAL */
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} M7Configuration_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_M7Configuration;
extern asn_SEQUENCE_specifics_t asn_SPC_M7Configuration_specs_1;
extern asn_TYPE_member_t asn_MBR_M7Configuration_1[3];

#ifdef __cplusplus
}
#endif

#endif	/* _M7Configuration_H_ */
#include <asn_internal.h>
