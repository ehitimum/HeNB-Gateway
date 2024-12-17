/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "S1AP-IEs"
 * 	found in "36413-g10.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps -no-gen-BER -no-gen-XER -no-gen-OER -no-gen-UPER -no-gen-JER`
 */

#ifndef	_SecondaryRATDataUsageRequest_H_
#define	_SecondaryRATDataUsageRequest_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum SecondaryRATDataUsageRequest {
	SecondaryRATDataUsageRequest_requested	= 0
	/*
	 * Enumeration is extensible
	 */
} e_SecondaryRATDataUsageRequest;

/* SecondaryRATDataUsageRequest */
typedef long	 SecondaryRATDataUsageRequest_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_SecondaryRATDataUsageRequest_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_SecondaryRATDataUsageRequest;
extern const asn_INTEGER_specifics_t asn_SPC_SecondaryRATDataUsageRequest_specs_1;
asn_struct_free_f SecondaryRATDataUsageRequest_free;
asn_struct_print_f SecondaryRATDataUsageRequest_print;
asn_constr_check_f SecondaryRATDataUsageRequest_constraint;
per_type_decoder_f SecondaryRATDataUsageRequest_decode_aper;
per_type_encoder_f SecondaryRATDataUsageRequest_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _SecondaryRATDataUsageRequest_H_ */
#include <asn_internal.h>