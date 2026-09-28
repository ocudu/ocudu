// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/f1ap/f1ap_pdu_items.h"
#include "ocudu/f1ap/common/f1ap_cho_types.h"
#include "ocudu/f1ap/du/f1ap_du_positioning_handler.h"
#include "ocudu/f1ap/ue_context_management_configs.h"
#include "ocudu/ran/nr_cgi.h"

namespace ocudu {

/// \brief Convert \c f1ap_cho_trigger to F1AP ASN.1 inter-DU type.
inline asn1::f1ap::ch_otrigger_inter_du_e cho_trigger_to_asn1_inter_du(f1ap_cho_trigger trigger)
{
  switch (trigger) {
    case f1ap_cho_trigger::cho_initiation:
      return asn1::f1ap::ch_otrigger_inter_du_opts::options::cho_initiation;
    case f1ap_cho_trigger::cho_replace:
      return asn1::f1ap::ch_otrigger_inter_du_opts::options::cho_replace;
    default:
      report_fatal_error("Cannot convert f1ap_cho_trigger to ASN.1 inter-DU type");
  }
}

/// \brief Convert F1AP ASN.1 inter-DU CHO trigger to \c f1ap_cho_trigger.
inline f1ap_cho_trigger asn1_to_cho_trigger_inter_du(asn1::f1ap::ch_otrigger_inter_du_e asn1_trigger)
{
  switch (asn1_trigger) {
    case asn1::f1ap::ch_otrigger_inter_du_opts::options::cho_initiation:
      return f1ap_cho_trigger::cho_initiation;
    case asn1::f1ap::ch_otrigger_inter_du_opts::options::cho_replace:
      return f1ap_cho_trigger::cho_replace;
    default:
      report_fatal_error("Cannot convert ASN.1 ch_otrigger_inter_du_e to f1ap_cho_trigger");
  }
}

/// \brief Convert F1AP ASN.1 Cause to \c f1ap_cause_t type.
/// \param asn1_cause The F1AP Cause.
/// \return The f1ap_cause_t type.
f1ap_cause_t asn1_to_cause(asn1::f1ap::cause_c asn1_cause);

/// \brief Convert \c f1ap_cause_t type to F1AP cause.
/// \param cause The f1ap_cause_t type.
/// \return The F1AP cause.
asn1::f1ap::cause_c cause_to_asn1(f1ap_cause_t cause);

/// \brief Extracts a \c drb_id_t from ASN.1 DRB Setup/Modified/Remove type.
/// \param drb_item ASN.1 item with DRB-Id.
/// \return drb_id_t object.
template <typename Asn1Type>
drb_id_t get_drb_id(const Asn1Type& drb_item)
{
  return static_cast<drb_id_t>(drb_item.drb_id);
}

/// \brief Converts ASN.1 CGI typo into internal struct. It also performs the byte to MCC/MNC conversion.
/// \param[in] asn1_cgi The ASN.1 encoded NR-CGI.
/// \return The CGI converted to flat internal struct.
expected<nr_cell_global_id_t> cgi_from_asn1(const asn1::f1ap::nr_cgi_s& asn1_cgi);
asn1::f1ap::nr_cgi_s          cgi_to_asn1(const nr_cell_global_id_t& cgi);

/// \brief Convert F1AP ASN.1 NR-CGI to NR Cell Identity.
/// \param f1ap_cgi The F1AP NR-CGI.
/// \return The NR Cell Identity.
expected<nr_cell_identity> nr_cell_id_from_asn1(asn1::f1ap::nr_cgi_s& f1ap_cgi);

f1ap_srb_to_setup                       make_srb_to_setup(const asn1::f1ap::srbs_to_be_setup_item_s& srb_item);
asn1::f1ap::srbs_to_be_setup_list_l     make_srb_to_setup_list(span<const f1ap_srb_to_setup> srbs);
asn1::f1ap::srbs_to_be_setup_mod_list_l make_srb_to_setupmod_list(span<const f1ap_srb_to_setup> srbs);

/// \brief Conversion helpers between SRB setup/modified common types and respective ASN.1 TS 38.473 types.
asn1::f1ap::srbs_setup_list_l     make_srb_setup_list(span<const srb_id_t> srbs);
asn1::f1ap::srbs_setup_mod_list_l make_srb_setupmod_list(span<const srb_id_t> srbs);

f1ap_srb_failed_to_setup make_srb_failed_to_setupmod(const asn1::f1ap::srbs_failed_to_be_setup_item_s& asn1_type);
f1ap_srb_failed_to_setup make_srb_failed_to_setupmod(const asn1::f1ap::srbs_failed_to_be_setup_mod_item_s& asn1_type);

/// \brief Conversion helpers between DRB to_setup/to_modify common types and respective ASN.1 TS 38.473 types.
f1ap_drb_to_setup                       make_drb_to_setup(const asn1::f1ap::drbs_to_be_setup_item_s& drb_item);
f1ap_drb_to_setup                       make_drb_to_setup(const asn1::f1ap::drbs_to_be_setup_mod_item_s& drb_item);
f1ap_drb_to_modify                      make_drb_to_modify(const asn1::f1ap::drbs_to_be_modified_item_s& drb_item);
asn1::f1ap::drbs_to_be_setup_item_s     make_drb_to_setup(const f1ap_drb_to_setup& drb_item);
asn1::f1ap::drbs_to_be_setup_mod_item_s make_drb_to_setupmod(const f1ap_drb_to_setup& drb_item);
asn1::f1ap::drbs_to_be_modified_item_s  make_drb_to_mod(const f1ap_drb_to_modify& drb_item);
asn1::f1ap::drbs_to_be_setup_list_l     make_drb_to_setup_list(span<const f1ap_drb_to_setup> drb_list);
asn1::f1ap::drbs_to_be_setup_mod_list_l make_drb_to_setupmod_list(span<const f1ap_drb_to_setup> drb_list);
asn1::f1ap::drbs_to_be_modified_list_l  make_drb_to_modify_list(span<const f1ap_drb_to_modify> drb_list);

/// Conversion helpers between DRB setup/setupmod/modified common types and respective ASN.1 TS 38.473 types.
asn1::f1ap::drbs_setup_list_l     make_drbs_setup_list(span<const f1ap_drb_setupmod> drbs);
asn1::f1ap::drbs_setup_mod_list_l make_drbs_setup_mod_list(span<const f1ap_drb_setupmod> drbs);
asn1::f1ap::drbs_modified_list_l  make_drbs_modified_list(span<const f1ap_drb_setupmod> drbs);
f1ap_drb_setupmod                 make_drb_setupmod(const asn1::f1ap::drbs_setup_item_s& drb);
f1ap_drb_setupmod                 make_drb_setupmod(const asn1::f1ap::drbs_setup_mod_item_s& drb);
f1ap_drb_setupmod                 make_drb_setupmod(const asn1::f1ap::drbs_modified_item_s& drb);

/// Convert F1AP failed to setup/modify types to respective ASN.1 TS 38.473 types.
asn1::f1ap::drbs_failed_to_be_setup_list_l
make_drbs_failed_to_be_setup_list(span<const f1ap_drb_failed_to_setupmod> failed_drbs);
asn1::f1ap::drbs_failed_to_be_setup_mod_list_l
make_drbs_failed_to_be_setup_mod_list(span<const f1ap_drb_failed_to_setupmod> failed_drbs);
asn1::f1ap::drbs_failed_to_be_modified_list_l
make_drbs_failed_to_be_modified_list(span<const f1ap_drb_failed_to_setupmod> failed_drbs);
asn1::f1ap::serving_cell_mo_encoded_in_cgc_list_l
                            make_serving_cell_mo_encoded_in_cgc_list(span<const uint8_t> serving_cell_mos);
f1ap_drb_failed_to_setupmod make_drb_failed_to_setupmod(const asn1::f1ap::drbs_failed_to_be_setup_item_s& asn1_type);
f1ap_drb_failed_to_setupmod
make_drb_failed_to_setupmod(const asn1::f1ap::drbs_failed_to_be_setup_mod_item_s& asn1_type);
f1ap_drb_failed_to_setupmod make_drb_failed_to_setupmod(const asn1::f1ap::drbs_failed_to_be_modified_item_s& asn1_type);

/// \brief Convert the geographical coordinates of a cell antenna to ASN.1, as per TS 38.473 section 9.3.1.191.
///
/// \remark The caller must set the position definition type to "direct".
asn1::f1ap::geographical_coordinates_s geographical_coordinates_to_asn1(const geographical_coordinates_t& geo);

/// Convert F1AP TRP info to ASN.1.
asn1::f1ap::trp_info_s trp_info_to_asn1(const odu::du_trp_info& trp);

/// Convert F1AP SRS configuration from ASN.1.
std::vector<odu::srs_carrier> from_asn1(const asn1::f1ap::srs_configuration_s& asn1cfg);

} // namespace ocudu
