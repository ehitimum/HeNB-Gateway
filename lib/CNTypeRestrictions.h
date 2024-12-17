/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "S1AP-IEs"
 * 	found in "36413-g10.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps -no-gen-BER -no-gen-XER -no-gen-OER -no-gen-UPER -no-gen-JER`
 */

#ifndef	_CNTypeRestrictions_H_
#define	_CNTypeRestrictions_H_


#include <asn_application.h>

/* Including external dependencies */
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct CNTypeRestrictions_Item;

/* CNTypeRestrictions */
typedef struct CNTypeRestrictions {
	A_SEQUENCE_OF(struct CNTypeRestrictions_Item) list;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CNTypeRestrictions_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CNTypeRestrictions;
extern asn_SET_OF_specifics_t asn_SPC_CNTypeRestrictions_specs_1;
extern asn_TYPE_member_t asn_MBR_CNTypeRestrictions_1[1];
extern asn_per_constraints_t asn_PER_type_CNTypeRestrictions_constr_1;

#ifdef __cplusplus
}
#endif

#endif	/* _CNTypeRestrictions_H_ */
#include <asn_internal.h>
