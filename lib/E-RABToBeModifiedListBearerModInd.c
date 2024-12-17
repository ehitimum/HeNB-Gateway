/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "S1AP-PDU-Contents"
 * 	found in "36413-g10.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps -no-gen-BER -no-gen-XER -no-gen-OER -no-gen-UPER -no-gen-JER`
 */

#include "E-RABToBeModifiedListBearerModInd.h"

int
E_RABToBeModifiedListBearerModInd_constraint(const asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	size_t size;
	
	if(!sptr) {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	/* Determine the number of elements */
	size = _A_CSEQUENCE_FROM_VOID(sptr)->count;
	
	if((size >= 1UL && size <= 256UL)) {
		/* Perform validation of the inner elements */
		return SEQUENCE_OF_constraint(td, sptr, ctfailcb, app_key);
	} else {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

/*
 * This type is implemented using E_RAB_IE_ContainerList_1199P6,
 * so here we adjust the DEF accordingly.
 */
#if !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT)
asn_per_constraints_t asn_PER_type_E_RABToBeModifiedListBearerModInd_constr_1 CC_NOTUSED = {
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	{ APC_CONSTRAINED,	 8,  8,  1,  256 }	/* (SIZE(1..256)) */,
	0, 0	/* No PER value map */
};
#endif  /* !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT) */
static const ber_tlv_tag_t asn_DEF_E_RABToBeModifiedListBearerModInd_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
asn_TYPE_descriptor_t asn_DEF_E_RABToBeModifiedListBearerModInd = {
	"E-RABToBeModifiedListBearerModInd",
	"E-RABToBeModifiedListBearerModInd",
	&asn_OP_SEQUENCE_OF,
	asn_DEF_E_RABToBeModifiedListBearerModInd_tags_1,
	sizeof(asn_DEF_E_RABToBeModifiedListBearerModInd_tags_1)
		/sizeof(asn_DEF_E_RABToBeModifiedListBearerModInd_tags_1[0]), /* 1 */
	asn_DEF_E_RABToBeModifiedListBearerModInd_tags_1,	/* Same as above */
	sizeof(asn_DEF_E_RABToBeModifiedListBearerModInd_tags_1)
		/sizeof(asn_DEF_E_RABToBeModifiedListBearerModInd_tags_1[0]), /* 1 */
	{
#if !defined(ASN_DISABLE_OER_SUPPORT)
		0,
#endif  /* !defined(ASN_DISABLE_OER_SUPPORT) */
#if !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT)
		&asn_PER_type_E_RABToBeModifiedListBearerModInd_constr_1,
#endif  /* !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT) */
#if !defined(ASN_DISABLE_JER_SUPPORT)
		0,
#endif  /* !defined(ASN_DISABLE_JER_SUPPORT) */
		E_RABToBeModifiedListBearerModInd_constraint
	},
	asn_MBR_ProtocolIE_ContainerList_7364P6_13,
	1,	/* Single element */
	&asn_SPC_ProtocolIE_ContainerList_7364P6_specs_13	/* Additional specs */
};

