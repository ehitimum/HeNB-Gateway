/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "S1AP-IEs"
 * 	found in "36413-g10.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps -no-gen-BER -no-gen-XER -no-gen-OER -no-gen-UPER -no-gen-JER`
 */

#ifndef	_RAN_UE_NGAP_ID_H_
#define	_RAN_UE_NGAP_ID_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>

#ifdef __cplusplus
extern "C" {
#endif

/* RAN-UE-NGAP-ID */
typedef unsigned long	 RAN_UE_NGAP_ID_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_RAN_UE_NGAP_ID_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_RAN_UE_NGAP_ID;
extern const asn_INTEGER_specifics_t asn_SPC_RAN_UE_NGAP_ID_specs_1;
asn_struct_free_f RAN_UE_NGAP_ID_free;
asn_struct_print_f RAN_UE_NGAP_ID_print;
asn_constr_check_f RAN_UE_NGAP_ID_constraint;
per_type_decoder_f RAN_UE_NGAP_ID_decode_aper;
per_type_encoder_f RAN_UE_NGAP_ID_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _RAN_UE_NGAP_ID_H_ */
#include <asn_internal.h>
