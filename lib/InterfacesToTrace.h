/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "S1AP-IEs"
 * 	found in "36413-g10.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps -no-gen-BER -no-gen-XER -no-gen-OER -no-gen-UPER -no-gen-JER`
 */

#ifndef	_InterfacesToTrace_H_
#define	_InterfacesToTrace_H_


#include <asn_application.h>

/* Including external dependencies */
#include <BIT_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

/* InterfacesToTrace */
typedef BIT_STRING_t	 InterfacesToTrace_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_InterfacesToTrace_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_InterfacesToTrace;
asn_struct_free_f InterfacesToTrace_free;
asn_struct_print_f InterfacesToTrace_print;
asn_constr_check_f InterfacesToTrace_constraint;
per_type_decoder_f InterfacesToTrace_decode_aper;
per_type_encoder_f InterfacesToTrace_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _InterfacesToTrace_H_ */
#include <asn_internal.h>
