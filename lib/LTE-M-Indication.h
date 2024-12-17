/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "S1AP-IEs"
 * 	found in "36413-g10.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps -no-gen-BER -no-gen-XER -no-gen-OER -no-gen-UPER -no-gen-JER`
 */

#ifndef	_LTE_M_Indication_H_
#define	_LTE_M_Indication_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum LTE_M_Indication {
	LTE_M_Indication_lte_m	= 0
	/*
	 * Enumeration is extensible
	 */
} e_LTE_M_Indication;

/* LTE-M-Indication */
typedef long	 LTE_M_Indication_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_LTE_M_Indication_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_LTE_M_Indication;
extern const asn_INTEGER_specifics_t asn_SPC_LTE_M_Indication_specs_1;
asn_struct_free_f LTE_M_Indication_free;
asn_struct_print_f LTE_M_Indication_print;
asn_constr_check_f LTE_M_Indication_constraint;
per_type_decoder_f LTE_M_Indication_decode_aper;
per_type_encoder_f LTE_M_Indication_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _LTE_M_Indication_H_ */
#include <asn_internal.h>
