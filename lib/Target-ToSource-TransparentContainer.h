/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "S1AP-IEs"
 * 	found in "36413-g10.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps -no-gen-BER -no-gen-XER -no-gen-OER -no-gen-UPER -no-gen-JER`
 */

#ifndef	_Target_ToSource_TransparentContainer_H_
#define	_Target_ToSource_TransparentContainer_H_


#include <asn_application.h>

/* Including external dependencies */
#include <OCTET_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Target-ToSource-TransparentContainer */
typedef OCTET_STRING_t	 Target_ToSource_TransparentContainer_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_Target_ToSource_TransparentContainer;
asn_struct_free_f Target_ToSource_TransparentContainer_free;
asn_struct_print_f Target_ToSource_TransparentContainer_print;
asn_constr_check_f Target_ToSource_TransparentContainer_constraint;
per_type_decoder_f Target_ToSource_TransparentContainer_decode_aper;
per_type_encoder_f Target_ToSource_TransparentContainer_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _Target_ToSource_TransparentContainer_H_ */
#include <asn_internal.h>
