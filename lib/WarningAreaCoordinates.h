/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "S1AP-IEs"
 * 	found in "36413-g10.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps -no-gen-BER -no-gen-XER -no-gen-OER -no-gen-UPER -no-gen-JER`
 */

#ifndef	_WarningAreaCoordinates_H_
#define	_WarningAreaCoordinates_H_


#include <asn_application.h>

/* Including external dependencies */
#include <OCTET_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

/* WarningAreaCoordinates */
typedef OCTET_STRING_t	 WarningAreaCoordinates_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_WarningAreaCoordinates_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_WarningAreaCoordinates;
asn_struct_free_f WarningAreaCoordinates_free;
asn_struct_print_f WarningAreaCoordinates_print;
asn_constr_check_f WarningAreaCoordinates_constraint;
per_type_decoder_f WarningAreaCoordinates_decode_aper;
per_type_encoder_f WarningAreaCoordinates_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _WarningAreaCoordinates_H_ */
#include <asn_internal.h>
