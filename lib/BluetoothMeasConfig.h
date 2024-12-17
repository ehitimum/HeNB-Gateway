/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "S1AP-IEs"
 * 	found in "36413-g10.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps -no-gen-BER -no-gen-XER -no-gen-OER -no-gen-UPER -no-gen-JER`
 */

#ifndef	_BluetoothMeasConfig_H_
#define	_BluetoothMeasConfig_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum BluetoothMeasConfig {
	BluetoothMeasConfig_setup	= 0
	/*
	 * Enumeration is extensible
	 */
} e_BluetoothMeasConfig;

/* BluetoothMeasConfig */
typedef long	 BluetoothMeasConfig_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_BluetoothMeasConfig_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_BluetoothMeasConfig;
extern const asn_INTEGER_specifics_t asn_SPC_BluetoothMeasConfig_specs_1;
asn_struct_free_f BluetoothMeasConfig_free;
asn_struct_print_f BluetoothMeasConfig_print;
asn_constr_check_f BluetoothMeasConfig_constraint;
per_type_decoder_f BluetoothMeasConfig_decode_aper;
per_type_encoder_f BluetoothMeasConfig_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _BluetoothMeasConfig_H_ */
#include <asn_internal.h>