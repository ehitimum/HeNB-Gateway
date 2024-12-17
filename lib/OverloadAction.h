/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "S1AP-IEs"
 * 	found in "36413-g10.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps -no-gen-BER -no-gen-XER -no-gen-OER -no-gen-UPER -no-gen-JER`
 */

#ifndef	_OverloadAction_H_
#define	_OverloadAction_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum OverloadAction {
	OverloadAction_reject_non_emergency_mo_dt	= 0,
	OverloadAction_reject_rrc_cr_signalling	= 1,
	OverloadAction_permit_emergency_sessions_and_mobile_terminated_services_only	= 2,
	/*
	 * Enumeration is extensible
	 */
	OverloadAction_permit_high_priority_sessions_and_mobile_terminated_services_only	= 3,
	OverloadAction_reject_delay_tolerant_access	= 4,
	OverloadAction_permit_high_priority_sessions_and_exception_reporting_and_mobile_terminated_services_only	= 5,
	OverloadAction_not_accept_mo_data_or_delay_tolerant_access_from_CP_CIoT	= 6
} e_OverloadAction;

/* OverloadAction */
typedef long	 OverloadAction_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_OverloadAction_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_OverloadAction;
extern const asn_INTEGER_specifics_t asn_SPC_OverloadAction_specs_1;
asn_struct_free_f OverloadAction_free;
asn_struct_print_f OverloadAction_print;
asn_constr_check_f OverloadAction_constraint;
per_type_decoder_f OverloadAction_decode_aper;
per_type_encoder_f OverloadAction_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _OverloadAction_H_ */
#include <asn_internal.h>
