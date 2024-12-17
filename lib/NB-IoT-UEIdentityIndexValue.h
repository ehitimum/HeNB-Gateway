/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "S1AP-IEs"
 * 	found in "36413-g10.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps -no-gen-BER -no-gen-XER -no-gen-OER -no-gen-UPER -no-gen-JER`
 */

#ifndef	_NB_IoT_UEIdentityIndexValue_H_
#define	_NB_IoT_UEIdentityIndexValue_H_


#include <asn_application.h>

/* Including external dependencies */
#include <BIT_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

/* NB-IoT-UEIdentityIndexValue */
typedef BIT_STRING_t	 NB_IoT_UEIdentityIndexValue_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_NB_IoT_UEIdentityIndexValue_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_NB_IoT_UEIdentityIndexValue;
asn_struct_free_f NB_IoT_UEIdentityIndexValue_free;
asn_struct_print_f NB_IoT_UEIdentityIndexValue_print;
asn_constr_check_f NB_IoT_UEIdentityIndexValue_constraint;
per_type_decoder_f NB_IoT_UEIdentityIndexValue_decode_aper;
per_type_encoder_f NB_IoT_UEIdentityIndexValue_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _NB_IoT_UEIdentityIndexValue_H_ */
#include <asn_internal.h>
