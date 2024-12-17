/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "S1AP-IEs"
 * 	found in "36413-g10.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps -no-gen-BER -no-gen-XER -no-gen-OER -no-gen-UPER -no-gen-JER`
 */

#ifndef	_EnhancedCoverageRestricted_H_
#define	_EnhancedCoverageRestricted_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum EnhancedCoverageRestricted {
	EnhancedCoverageRestricted_restricted	= 0
	/*
	 * Enumeration is extensible
	 */
} e_EnhancedCoverageRestricted;

/* EnhancedCoverageRestricted */
typedef long	 EnhancedCoverageRestricted_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_EnhancedCoverageRestricted_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_EnhancedCoverageRestricted;
extern const asn_INTEGER_specifics_t asn_SPC_EnhancedCoverageRestricted_specs_1;
asn_struct_free_f EnhancedCoverageRestricted_free;
asn_struct_print_f EnhancedCoverageRestricted_print;
asn_constr_check_f EnhancedCoverageRestricted_constraint;
per_type_decoder_f EnhancedCoverageRestricted_decode_aper;
per_type_encoder_f EnhancedCoverageRestricted_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _EnhancedCoverageRestricted_H_ */
#include <asn_internal.h>
