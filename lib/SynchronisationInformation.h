/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "S1AP-IEs"
 * 	found in "36413-g10.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps -no-gen-BER -no-gen-XER -no-gen-OER -no-gen-UPER -no-gen-JER`
 */

#ifndef	_SynchronisationInformation_H_
#define	_SynchronisationInformation_H_


#include <asn_application.h>

/* Including external dependencies */
#include "StratumLevel.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct ListeningSubframePattern;
struct ECGI_List;
struct ProtocolExtensionContainer;

/* SynchronisationInformation */
typedef struct SynchronisationInformation {
	StratumLevel_t	*sourceStratumLevel;	/* OPTIONAL */
	struct ListeningSubframePattern	*listeningSubframePattern;	/* OPTIONAL */
	struct ECGI_List	*aggressoreCGI_List;	/* OPTIONAL */
	struct ProtocolExtensionContainer	*iE_Extensions;	/* OPTIONAL */
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} SynchronisationInformation_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_SynchronisationInformation;
extern asn_SEQUENCE_specifics_t asn_SPC_SynchronisationInformation_specs_1;
extern asn_TYPE_member_t asn_MBR_SynchronisationInformation_1[4];

#ifdef __cplusplus
}
#endif

#endif	/* _SynchronisationInformation_H_ */
#include <asn_internal.h>
