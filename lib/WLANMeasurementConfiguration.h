/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "S1AP-IEs"
 * 	found in "36413-g10.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps -no-gen-BER -no-gen-XER -no-gen-OER -no-gen-UPER -no-gen-JER`
 */

#ifndef	_WLANMeasurementConfiguration_H_
#define	_WLANMeasurementConfiguration_H_


#include <asn_application.h>

/* Including external dependencies */
#include "WLANMeasConfig.h"
#include <NativeEnumerated.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum WLANMeasurementConfiguration__wlan_rssi {
	WLANMeasurementConfiguration__wlan_rssi_true	= 0
	/*
	 * Enumeration is extensible
	 */
} e_WLANMeasurementConfiguration__wlan_rssi;
typedef enum WLANMeasurementConfiguration__wlan_rtt {
	WLANMeasurementConfiguration__wlan_rtt_true	= 0
	/*
	 * Enumeration is extensible
	 */
} e_WLANMeasurementConfiguration__wlan_rtt;

/* Forward declarations */
struct WLANMeasConfigNameList;
struct ProtocolExtensionContainer;

/* WLANMeasurementConfiguration */
typedef struct WLANMeasurementConfiguration {
	WLANMeasConfig_t	 wlanMeasConfig;
	struct WLANMeasConfigNameList	*wlanMeasConfigNameList;	/* OPTIONAL */
	long	*wlan_rssi;	/* OPTIONAL */
	long	*wlan_rtt;	/* OPTIONAL */
	struct ProtocolExtensionContainer	*iE_Extensions;	/* OPTIONAL */
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} WLANMeasurementConfiguration_t;

/* Implementation */
/* extern asn_TYPE_descriptor_t asn_DEF_wlan_rssi_4;	// (Use -fall-defs-global to expose) */
/* extern asn_TYPE_descriptor_t asn_DEF_wlan_rtt_7;	// (Use -fall-defs-global to expose) */
extern asn_TYPE_descriptor_t asn_DEF_WLANMeasurementConfiguration;
extern asn_SEQUENCE_specifics_t asn_SPC_WLANMeasurementConfiguration_specs_1;
extern asn_TYPE_member_t asn_MBR_WLANMeasurementConfiguration_1[5];

#ifdef __cplusplus
}
#endif

#endif	/* _WLANMeasurementConfiguration_H_ */
#include <asn_internal.h>
