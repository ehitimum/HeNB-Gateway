/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "S1AP-PDU-Contents"
 * 	found in "36413-g10.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps -no-gen-BER -no-gen-XER -no-gen-OER -no-gen-UPER -no-gen-JER`
 */

#include "E-RABToBeSetupListBearerSUReq.h"

#include "ProtocolIE-SingleContainer.h"
#if !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT)
asn_per_constraints_t asn_PER_type_E_RABToBeSetupListBearerSUReq_constr_1 CC_NOTUSED = {
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	{ APC_CONSTRAINED,	 8,  8,  1,  256 }	/* (SIZE(1..256)) */,
	0, 0	/* No PER value map */
};
#endif  /* !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT) */
asn_TYPE_member_t asn_MBR_E_RABToBeSetupListBearerSUReq_1[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_ProtocolIE_SingleContainer_7330P0,
		0,
		{
#if !defined(ASN_DISABLE_OER_SUPPORT)
			0,
#endif  /* !defined(ASN_DISABLE_OER_SUPPORT) */
#if !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT)
			0,
#endif  /* !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT) */
#if !defined(ASN_DISABLE_JER_SUPPORT)
			0,
#endif  /* !defined(ASN_DISABLE_JER_SUPPORT) */
			0
		},
		0, 0, /* No default value */
		""
		},
};
static const ber_tlv_tag_t asn_DEF_E_RABToBeSetupListBearerSUReq_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
asn_SET_OF_specifics_t asn_SPC_E_RABToBeSetupListBearerSUReq_specs_1 = {
	sizeof(struct E_RABToBeSetupListBearerSUReq),
	offsetof(struct E_RABToBeSetupListBearerSUReq, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
asn_TYPE_descriptor_t asn_DEF_E_RABToBeSetupListBearerSUReq = {
	"E-RABToBeSetupListBearerSUReq",
	"E-RABToBeSetupListBearerSUReq",
	&asn_OP_SEQUENCE_OF,
	asn_DEF_E_RABToBeSetupListBearerSUReq_tags_1,
	sizeof(asn_DEF_E_RABToBeSetupListBearerSUReq_tags_1)
		/sizeof(asn_DEF_E_RABToBeSetupListBearerSUReq_tags_1[0]), /* 1 */
	asn_DEF_E_RABToBeSetupListBearerSUReq_tags_1,	/* Same as above */
	sizeof(asn_DEF_E_RABToBeSetupListBearerSUReq_tags_1)
		/sizeof(asn_DEF_E_RABToBeSetupListBearerSUReq_tags_1[0]), /* 1 */
	{
#if !defined(ASN_DISABLE_OER_SUPPORT)
		0,
#endif  /* !defined(ASN_DISABLE_OER_SUPPORT) */
#if !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT)
		&asn_PER_type_E_RABToBeSetupListBearerSUReq_constr_1,
#endif  /* !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT) */
#if !defined(ASN_DISABLE_JER_SUPPORT)
		0,
#endif  /* !defined(ASN_DISABLE_JER_SUPPORT) */
		SEQUENCE_OF_constraint
	},
	asn_MBR_E_RABToBeSetupListBearerSUReq_1,
	1,	/* Single element */
	&asn_SPC_E_RABToBeSetupListBearerSUReq_specs_1	/* Additional specs */
};

