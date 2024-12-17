/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "S1AP-IEs"
 * 	found in "36413-g10.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps -no-gen-BER -no-gen-XER -no-gen-OER -no-gen-UPER -no-gen-JER`
 */

#ifndef	_MBSFN_ResultToLogInfo_H_
#define	_MBSFN_ResultToLogInfo_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include "EARFCN.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct ProtocolExtensionContainer;

/* MBSFN-ResultToLogInfo */
typedef struct MBSFN_ResultToLogInfo {
	long	*mBSFN_AreaId;	/* OPTIONAL */
	EARFCN_t	 carrierFreq;
	struct ProtocolExtensionContainer	*iE_Extensions;	/* OPTIONAL */
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} MBSFN_ResultToLogInfo_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_MBSFN_ResultToLogInfo;
extern asn_SEQUENCE_specifics_t asn_SPC_MBSFN_ResultToLogInfo_specs_1;
extern asn_TYPE_member_t asn_MBR_MBSFN_ResultToLogInfo_1[3];

#ifdef __cplusplus
}
#endif

#endif	/* _MBSFN_ResultToLogInfo_H_ */
#include <asn_internal.h>