/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "S1AP-IEs"
 * 	found in "36413-g10.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps -no-gen-BER -no-gen-XER -no-gen-OER -no-gen-UPER -no-gen-JER`
 */

#ifndef	_MMEPagingTarget_H_
#define	_MMEPagingTarget_H_


#include <asn_application.h>

/* Including external dependencies */
#include <constr_CHOICE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum MMEPagingTarget_PR {
	MMEPagingTarget_PR_NOTHING,	/* No components present */
	MMEPagingTarget_PR_global_ENB_ID,
	MMEPagingTarget_PR_tAI
	/* Extensions may appear below */
	
} MMEPagingTarget_PR;

/* Forward declarations */
struct Global_ENB_ID;
struct TAI;

/* MMEPagingTarget */
typedef struct MMEPagingTarget {
	MMEPagingTarget_PR present;
	union MMEPagingTarget_u {
		struct Global_ENB_ID	*global_ENB_ID;
		struct TAI	*tAI;
		/*
		 * This type is extensible,
		 * possible extensions are below.
		 */
	} choice;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} MMEPagingTarget_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_MMEPagingTarget;
extern asn_CHOICE_specifics_t asn_SPC_MMEPagingTarget_specs_1;
extern asn_TYPE_member_t asn_MBR_MMEPagingTarget_1[2];
extern asn_per_constraints_t asn_PER_type_MMEPagingTarget_constr_1;

#ifdef __cplusplus
}
#endif

#endif	/* _MMEPagingTarget_H_ */
#include <asn_internal.h>
