// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Converters between E1AP ASN.1 types and OCUDU common E1AP types.

#pragma once

#include "e1ap_asn1_converters_helpers.h"
#include "ocudu/adt/slotted_vector.h"
#include "ocudu/asn1/asn1_containers.h"
#include "ocudu/asn1/asn1_types.h"
#include "ocudu/asn1/e1ap/e1ap_ies.h"
#include "ocudu/e1ap/common/e1ap_types.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/cause/common.h"
#include "ocudu/ran/cause/e1ap_cause.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/ran/nr_cell_identity.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/ran/qos/qos_flow_id.h"
#include "ocudu/ran/s_nssai.h"
#include "ocudu/ran/up_transport_layer_info.h"
#include "ocudu/security/security.h"
#include "ocudu/support/enum_utils.h"
#include "ocudu/support/error_handling.h"
#include <cstdint>
#include <optional>
#include <variant>
#include <vector>

namespace ocudu {

/// \brief Converts type \c ciphering_algorithm to an E1AP ASN.1 type.
///
/// \warning The application is terminated if the ciphering algorithm is not valid.
/// \param[in] ciph_algo Ciphering algorithm object.
/// \return The E1AP ASN.1 object where the result of the conversion is stored.
[[nodiscard]] inline asn1::e1ap::ciphering_algorithm_e
ciphering_algorithm_to_e1ap_asn1(security::ciphering_algorithm ciph_algo)
{
  switch (ciph_algo) {
    case security::ciphering_algorithm::nea0:
      return asn1::e1ap::ciphering_algorithm_opts::nea0;
    case security::ciphering_algorithm::nea1:
      return asn1::e1ap::ciphering_algorithm_opts::c_neg128_nea1;
    case security::ciphering_algorithm::nea2:
      return asn1::e1ap::ciphering_algorithm_opts::c_neg128_nea2;
    case security::ciphering_algorithm::nea3:
      return asn1::e1ap::ciphering_algorithm_opts::c_neg128_nea3;
    default:
      report_fatal_error("Invalid ciphering algorithm ({})", ciph_algo);
  }
}

/// \brief Converts an E1AP ASN.1 ciphering algorithm to a \c ciphering_algorithm type.
///
/// \warning The application is terminated if the ASN.1 ciphering algorithm is not recognized.
/// \param[in] asn1_ciph_algo ASN.1 ciphering algorithm object.
/// \return The \c ciphering_algorithm object where the result of the conversion is stored.
[[nodiscard]] inline security::ciphering_algorithm
e1ap_asn1_to_ciphering_algorithm(asn1::e1ap::ciphering_algorithm_e asn1_ciph_algo)
{
  switch (asn1_ciph_algo) {
    case asn1::e1ap::ciphering_algorithm_opts::nea0:
      return security::ciphering_algorithm::nea0;
    case asn1::e1ap::ciphering_algorithm_opts::c_neg128_nea1:
      return security::ciphering_algorithm::nea1;
    case asn1::e1ap::ciphering_algorithm_opts::c_neg128_nea2:
      return security::ciphering_algorithm::nea2;
    case asn1::e1ap::ciphering_algorithm_opts::c_neg128_nea3:
      return security::ciphering_algorithm::nea3;
    default:
      report_fatal_error("Invalid ciphering algorithm ({})", fmt::underlying(asn1_ciph_algo.value));
  }
}

/// \brief Converts type \c integrity_algorithm to an E1AP ASN.1 type.
///
/// \warning The application is terminated if the integrity algorithm is not valid.
/// \param[in] int_algo Integrity Protection Algorithm object.
/// \return The E1AP ASN.1 object where the result of the conversion is stored.
[[nodiscard]] inline asn1::e1ap::integrity_protection_algorithm_e
integrity_algorithm_to_e1ap_asn1(security::integrity_algorithm int_algo)
{
  switch (int_algo) {
    case security::integrity_algorithm::nia0:
      return asn1::e1ap::integrity_protection_algorithm_opts::nia0;
    case security::integrity_algorithm::nia1:
      return asn1::e1ap::integrity_protection_algorithm_opts::i_neg128_nia1;
    case security::integrity_algorithm::nia2:
      return asn1::e1ap::integrity_protection_algorithm_opts::i_neg128_nia2;
    case security::integrity_algorithm::nia3:
      return asn1::e1ap::integrity_protection_algorithm_opts::i_neg128_nia3;
    default:
      report_fatal_error("Invalid integrity protection algorithm ({})", int_algo);
  }
}

/// \brief Converts an E1AP ASN.1 integrity protection algorithm to an \c integrity_algorithm type.
///
/// \warning The application is terminated if the ASN.1 integrity protection algorithm is not recognized.
/// \param[in] asn1_int_algo ASN.1 Integrity Protection Algorithm object.
/// \return The \c integrity_algorithm object where the result of the conversion is stored.
[[nodiscard]] inline security::integrity_algorithm
e1ap_asn1_to_integrity_algorithm(asn1::e1ap::integrity_protection_algorithm_e asn1_int_algo)
{
  switch (asn1_int_algo) {
    case asn1::e1ap::integrity_protection_algorithm_opts::nia0:
      return security::integrity_algorithm::nia0;
    case asn1::e1ap::integrity_protection_algorithm_opts::i_neg128_nia1:
      return security::integrity_algorithm::nia1;
    case asn1::e1ap::integrity_protection_algorithm_opts::i_neg128_nia2:
      return security::integrity_algorithm::nia2;
    case asn1::e1ap::integrity_protection_algorithm_opts::i_neg128_nia3:
      return security::integrity_algorithm::nia3;
    default:
      report_fatal_error("Invalid integrity protection algorithm ({})", fmt::underlying(asn1_int_algo.value));
  }
}

/// \brief Converts \c s_nssai_t type to E1AP ASN.1 S-NSSAI.
/// \param[in] snssai The S-NSSAI type.
/// \return The E1AP ASN.1 object where the result of the conversion is stored.
[[nodiscard]] inline asn1::e1ap::snssai_s snssai_to_e1ap_asn1(s_nssai_t snssai)
{
  const slice_differentiator& sd         = snssai.sd;
  bool                        sd_present = sd.is_set();

  return asn1::e1ap::snssai_s{.sd_present      = sd_present,
                              .ie_exts_present = false,
                              .sst = details::number_to_fixed_octstring<details::sst_nof_octets>(snssai.sst.value()),
                              .sd  = sd_present ? details::number_to_fixed_octstring<details::sd_nof_octets>(sd.value())
                                                : asn1::fixed_octstring<details::sd_nof_octets, true>{},
                              .ie_exts = asn1::e1ap::snssai_ext_ies_container{}};
}

/// \brief Converts E1AP ASN.1 S-NSSAI to \c s_nssai_t type.
/// \param[in] asn1_snssai The E1AP ASN.1 S-NSSAI.
/// \return The \c s_nssai_t type object where the result of the conversion is stored.
[[nodiscard]] inline s_nssai_t e1ap_asn1_to_snssai(const asn1::e1ap::snssai_s& asn1_snssai)
{
  return s_nssai_t{.sst = slice_service_type{static_cast<uint8_t>(asn1_snssai.sst.to_number())},
                   .sd  = asn1_snssai.sd_present ? slice_differentiator::create(asn1_snssai.sd.to_number()).value()
                                                 : slice_differentiator{}};
}

/// \brief Converts \c nr_cell_global_id_t to E1AP ASN.1.
/// \param[in] nr_cgi The common type NR CGI.
/// \return The ASN.1 NR CGI.
[[nodiscard]] inline asn1::e1ap::nr_cgi_s nr_cgi_to_e1ap_asn1(nr_cell_global_id_t nr_cgi)
{
  auto create_nr_cell_id = [](uint64_t nr_cell_identity_value) {
    asn1::fixed_bitstring<details::nr_cell_id_nof_bits, false, true> nr_cell_id;
    nr_cell_id.from_number(nr_cell_identity_value);
    return nr_cell_id;
  };

  return asn1::e1ap::nr_cgi_s{.ie_exts_present = false,
                              .plmn_id =
                                  asn1::fixed_octstring<details::plmn_id_nof_octets, true>{nr_cgi.plmn_id.to_bytes()},
                              .nr_cell_id = create_nr_cell_id(nr_cgi.nci.value()),
                              .ie_exts    = asn1::e1ap::nr_cgi_ext_ies_container{}};
}

/// \brief Converts type \c sdap_config to an E1AP ASN.1 type.
/// \param[in] sdap_cfg SDAP configuration object.
/// \return The E1AP ASN.1 object where the result of the conversion is stored.
[[nodiscard]] inline asn1::e1ap::sdap_cfg_s sdap_config_to_e1ap_asn1(const sdap_config_t& sdap_cfg)
{
  return asn1::e1ap::sdap_cfg_s{.ie_exts_present = false,
                                .default_drb     = sdap_cfg.default_drb
                                                       ? asn1::e1ap::default_drb_opts::options::true_value
                                                       : asn1::e1ap::default_drb_opts::options::false_value,
                                .sdap_hdr_ul     = details::sdap_hdr_ul_cfg_to_e1ap_asn1(sdap_cfg.sdap_hdr_ul),
                                .sdap_hdr_dl     = details::sdap_hdr_dl_cfg_to_e1ap_asn1(sdap_cfg.sdap_hdr_dl),
                                .ie_exts         = asn1::e1ap::sdap_cfg_ext_ies_container{}};
}

/// \brief Converts an E1AP ASN.1 SDAP configuration to a \c sdap_config_t type.
/// \param[in] asn1_sdap_cfg E1AP ASN.1 config object.
/// \return The \c sdap_config_t object where the result of the conversion is stored.
[[nodiscard]] inline sdap_config_t e1ap_asn1_to_sdap_config(const asn1::e1ap::sdap_cfg_s& asn1_sdap_cfg)
{
  return sdap_config_t{.pdu_session = pdu_session_id_t::invalid,
                       .sdap_hdr_dl = details::e1ap_asn1_to_sdap_hdr_dl_cfg(asn1_sdap_cfg.sdap_hdr_dl),
                       .sdap_hdr_ul = details::e1ap_asn1_to_sdap_hdr_ul_cfg(asn1_sdap_cfg.sdap_hdr_ul),
                       .default_drb = asn1_sdap_cfg.default_drb == asn1::e1ap::default_drb_opts::options::true_value,
                       .mapped_qos_flows_to_add     = std::vector<qos_flow_id_t>{},
                       .mapped_qos_flows_to_release = std::vector<qos_flow_id_t>{}};
}

/// \brief Converts type \c e1ap_pdcp_config to an E1AP ASN.1 type.
/// \param[in] pdcp_cfg PDCP configuration object.
/// \return The E1AP ASN.1 object where the result of the conversion is stored.
[[nodiscard]] inline asn1::e1ap::pdcp_cfg_s pdcp_config_to_e1ap_asn1(const e1ap_pdcp_config& pdcp_cfg)
{
  const std::optional<e1ap_rohc_config>&   rohc_config   = pdcp_cfg.rohc_config;
  const std::optional<pdcp_discard_timer>& discard_timer = pdcp_cfg.discard_timer;

  return asn1::e1ap::pdcp_cfg_s{
      .rohc_params_present           = rohc_config.has_value(),
      .t_reordering_timer_present    = pdcp_cfg.t_reordering_timer.has_value(),
      .discard_timer_present         = discard_timer.has_value(),
      .ul_data_split_thres_present   = pdcp_cfg.ul_data_split_thres.has_value(),
      .pdcp_dupl_present             = pdcp_cfg.pdcp_dupl.has_value(),
      .pdcp_reest_present            = pdcp_cfg.pdcp_reest.has_value(),
      .pdcp_data_recovery_present    = pdcp_cfg.pdcp_data_recovery.has_value(),
      .dupl_activation_present       = pdcp_cfg.dupl_activation.has_value(),
      .out_of_order_delivery_present = pdcp_cfg.out_of_order_delivery.has_value(),
      .ie_exts_present               = false,
      .pdcp_sn_size_ul               = details::pdcp_sn_size_to_asn1(pdcp_cfg.pdcp_sn_size_ul),
      .pdcp_sn_size_dl               = details::pdcp_sn_size_to_asn1(pdcp_cfg.pdcp_sn_size_dl),
      .rlc_mode                      = details::rlc_mode_to_asn1(pdcp_cfg.rlc_mod),
      .rohc_params        = rohc_config ? details::e1ap_rohc_config_to_asn1(*rohc_config) : asn1::e1ap::rohc_params_c{},
      .t_reordering_timer = details::pdcp_t_reordering_timer_to_asn1(pdcp_cfg.t_reordering_timer),
      .discard_timer =
          discard_timer ? details::pdcp_discard_timer_to_asn1(*discard_timer) : asn1::e1ap::discard_timer_e{},
      .ul_data_split_thres = details::ul_data_split_thres_to_asn1(pdcp_cfg.ul_data_split_thres),
      .pdcp_dupl           = details::optional_bool_to_asn1<asn1::e1ap::pdcp_dupl_e>(pdcp_cfg.pdcp_dupl),
      .pdcp_reest          = details::optional_bool_to_asn1<asn1::e1ap::pdcp_reest_e>(pdcp_cfg.pdcp_reest),
      .pdcp_data_recovery =
          details::optional_bool_to_asn1<asn1::e1ap::pdcp_data_recovery_e>(pdcp_cfg.pdcp_data_recovery),
      .dupl_activation = details::dupl_activation_to_asn1(pdcp_cfg.dupl_activation),
      .out_of_order_delivery =
          details::optional_bool_to_asn1<asn1::e1ap::out_of_order_delivery_e>(pdcp_cfg.out_of_order_delivery),
      .ie_exts = details::make_empty_pdcp_cfg_ext_ies()};
}

/// \brief Converts E1AP ASN.1 type to an \c e1ap_pdcp_config type.
/// \param[in] asn1_pdcp_cfg E1AP ASN.1 object.
/// \return The PDCP configuration object where the result of the conversion is stored.
[[nodiscard]] inline e1ap_pdcp_config e1ap_asn1_to_pdcp_config(const asn1::e1ap::pdcp_cfg_s& asn1_pdcp_cfg)
{
  return e1ap_pdcp_config{
      .pdcp_sn_size_ul = details::asn1_to_pdcp_sn_size(asn1_pdcp_cfg.pdcp_sn_size_ul),
      .pdcp_sn_size_dl = details::asn1_to_pdcp_sn_size(asn1_pdcp_cfg.pdcp_sn_size_dl),
      .rlc_mod         = details::asn1_to_rlc_mode(asn1_pdcp_cfg.rlc_mode),
      .rohc_config     = details::convert_if_present(
          asn1_pdcp_cfg.rohc_params_present,
          [&asn1_pdcp_cfg] { return details::e1ap_asn1_to_rohc_config(asn1_pdcp_cfg.rohc_params); }),
      .t_reordering_timer = details::convert_if_present(asn1_pdcp_cfg.t_reordering_timer_present,
                                                        [&asn1_pdcp_cfg] {
                                                          return details::asn1_to_pdcp_t_reordering(
                                                              asn1_pdcp_cfg.t_reordering_timer.t_reordering);
                                                        }),
      .discard_timer      = details::convert_if_present(
          asn1_pdcp_cfg.discard_timer_present,
          [&asn1_pdcp_cfg] { return details::asn1_to_pdcp_discard_timer(asn1_pdcp_cfg.discard_timer); }),
      .ul_data_split_thres =
          details::convert_if_present(asn1_pdcp_cfg.ul_data_split_thres_present,
                                      [&asn1_pdcp_cfg] { return asn1_pdcp_cfg.ul_data_split_thres.to_number(); }),
      .pdcp_dupl = details::convert_if_present(
          asn1_pdcp_cfg.pdcp_dupl_present,
          [&asn1_pdcp_cfg] { return asn1_pdcp_cfg.pdcp_dupl == asn1::e1ap::pdcp_dupl_opts::options::true_value; }),
      .pdcp_reest = details::convert_if_present(
          asn1_pdcp_cfg.pdcp_reest_present,
          [&asn1_pdcp_cfg] { return asn1_pdcp_cfg.pdcp_reest == asn1::e1ap::pdcp_reest_opts::options::true_value; }),
      .pdcp_data_recovery = details::convert_if_present(
          asn1_pdcp_cfg.pdcp_data_recovery_present,
          [&asn1_pdcp_cfg] {
            return asn1_pdcp_cfg.pdcp_data_recovery == asn1::e1ap::pdcp_data_recovery_opts::options::true_value;
          }),
      .dupl_activation = details::convert_if_present(asn1_pdcp_cfg.dupl_activation_present,
                                                     [&asn1_pdcp_cfg] {
                                                       return static_cast<e1ap_dupl_activation>(
                                                           static_cast<int>(asn1_pdcp_cfg.dupl_activation));
                                                     }),
      .out_of_order_delivery =
          details::convert_if_present(asn1_pdcp_cfg.out_of_order_delivery_present, [&asn1_pdcp_cfg] {
            return asn1_pdcp_cfg.out_of_order_delivery == asn1::e1ap::out_of_order_delivery_opts::options::true_value;
          })};
}

/// \brief Converts E1AP Cause to \c e1ap_cause_t type.
/// \param[in] e1ap_cause The E1AP Cause.
/// \return The cause type. If the ASN.1 cause type is unknown, \c cause_misc_t::unspecified is returned, as the ASN.1
/// defines no Cause extension.
[[nodiscard]] inline e1ap_cause_t asn1_to_cause(const asn1::e1ap::cause_c& e1ap_cause)
{
  switch (e1ap_cause.type()) {
    case asn1::e1ap::cause_c::types_opts::radio_network:
      return static_cast<e1ap_cause_radio_network_t>(e1ap_cause.radio_network().value);
    case asn1::e1ap::cause_c::types_opts::transport:
      return static_cast<e1ap_cause_transport_t>(e1ap_cause.transport().value);
    case asn1::e1ap::cause_c::types_opts::protocol:
      return static_cast<cause_protocol_t>(e1ap_cause.protocol().value);
    case asn1::e1ap::cause_c::types_opts::misc:
      return static_cast<cause_misc_t>(e1ap_cause.misc().value);
    default:
      return cause_misc_t::unspecified;
  }
}

/// \brief Converts \c e1ap_cause_t type to E1AP ASN.1 cause.
///
/// \warning The application is terminated if the cause cannot be converted to an E1AP cause.
/// \param[in] cause The \c e1ap_cause_t type.
/// \return The E1AP ASN.1 cause.
[[nodiscard]] inline asn1::e1ap::cause_c cause_to_asn1(e1ap_cause_t cause)
{
  asn1::e1ap::cause_c asn1_cause;

  if (const auto* result = std::get_if<e1ap_cause_radio_network_t>(&cause)) {
    asn1_cause.set_radio_network() = static_cast<asn1::e1ap::cause_radio_network_opts::options>(*result);
    return asn1_cause;
  }
  if (const auto* result = std::get_if<e1ap_cause_transport_t>(&cause)) {
    asn1_cause.set_transport() = static_cast<asn1::e1ap::cause_transport_opts::options>(*result);
    return asn1_cause;
  }
  if (const auto* result = std::get_if<cause_protocol_t>(&cause)) {
    asn1_cause.set_protocol() = static_cast<asn1::e1ap::cause_protocol_opts::options>(*result);
    return asn1_cause;
  }
  if (const auto* result = std::get_if<cause_misc_t>(&cause)) {
    asn1_cause.set_misc() = static_cast<asn1::e1ap::cause_misc_opts::options>(*result);
    return asn1_cause;
  }

  report_fatal_error("Cannot convert cause to E1AP type: {}", cause);
  return asn1_cause;
}

/// \brief Converts E1AP NG DL UP Unchanged to its boolean representation.
/// \param[in] ng_dl_up_unchanged The E1AP NG DL UP Unchanged.
/// \return The boolean representation of E1AP NG DL UP Unchanged.
[[nodiscard]] inline bool
e1ap_asn1_ng_dl_up_unchanged_to_bool(asn1::e1ap::pdu_session_res_setup_item_s::ng_dl_up_unchanged_e_ ng_dl_up_unchanged)
{
  return ng_dl_up_unchanged.value ==
         asn1::e1ap::pdu_session_res_setup_item_s::ng_dl_up_unchanged_opts::options::true_value;
}

/// \brief Converts E1AP ASN.1 PDCP Count to \c e1ap_pdcp_count type.
/// \param[in] asn1_pdcp_count The E1AP ASN.1 PDCP Count.
/// \return The \c e1ap_pdcp_count object where the result of the conversion is stored.
[[nodiscard]] inline e1ap_pdcp_count e1ap_asn1_pdcp_count_to_pdcp_count(const asn1::e1ap::pdcp_count_s& asn1_pdcp_count)
{
  return e1ap_pdcp_count{.pdcp_sn = asn1_pdcp_count.pdcp_sn, .hfn = asn1_pdcp_count.hfn};
}

/// \brief Converts E1AP ASN.1 CGI to \c nr_cell_global_id_t type.
///
/// The input is expected to hold a valid NCI and PLMN identity.
/// \param[in] asn1_cgi The E1AP ASN.1 CGI.
/// \return The \c nr_cell_global_id_t object where the result of the conversion is stored.
[[nodiscard]] inline nr_cell_global_id_t e1ap_asn1_to_cgi(const asn1::e1ap::nr_cgi_s& asn1_cgi)
{
  return nr_cell_global_id_t(plmn_identity::from_bytes(asn1_cgi.plmn_id.to_bytes()).value(),
                             nr_cell_identity::create(asn1_cgi.nr_cell_id.to_number()).value());
}

/// \brief Converts E1AP ASN.1 Cell Group Info to \c std::vector<e1ap_cell_group_info_item> type.
/// \param[in] asn1_cell_group_info The E1AP ASN.1 Cell Group Info.
/// \return The cell group info items where the result of the conversion is stored.
[[nodiscard]] inline std::vector<e1ap_cell_group_info_item>
e1ap_asn1_to_cell_group_info(const asn1::e1ap::cell_group_info_l& asn1_cell_group_info)
{
  std::vector<e1ap_cell_group_info_item> cell_group_info;

  for (const auto& asn1_cell_group_info_item : asn1_cell_group_info) {
    cell_group_info.push_back(e1ap_cell_group_info_item{
        .cell_group_id = asn1_cell_group_info_item.cell_group_id,
        .ul_cfg        = details::convert_if_present(asn1_cell_group_info_item.ul_cfg_present,
                                              [&asn1_cell_group_info_item] {
                                                return static_cast<e1ap_ul_cfg>(
                                                    static_cast<int>(asn1_cell_group_info_item.ul_cfg));
                                              }),
        .dl_tx_stop    = details::convert_if_present(asn1_cell_group_info_item.dl_tx_stop_present,
                                                  [&asn1_cell_group_info_item] {
                                                    return static_cast<e1ap_dl_tx_stop>(
                                                        static_cast<int>(asn1_cell_group_info_item.dl_tx_stop));
                                                  }),
        .rat_type =
            details::convert_if_present(asn1_cell_group_info_item.rat_type_present, [&asn1_cell_group_info_item] {
              return static_cast<e1ap_rat_type>(static_cast<int>(asn1_cell_group_info_item.rat_type));
            })});
  }

  return cell_group_info;
}

/// \brief Converts E1AP ASN.1 QoS Flow Map Info to \c slotted_id_vector<qos_flow_id_t, e1ap_qos_flow_qos_param_item>
/// type.
/// \param[in] asn1_flow_map_list The E1AP ASN.1 QoS Flow Map Info.
/// \return The QoS flow map info where the result of the conversion is stored.
[[nodiscard]] inline slotted_id_vector<qos_flow_id_t, e1ap_qos_flow_qos_param_item>
e1ap_asn1_to_flow_map_info(const asn1::e1ap::qos_flow_qos_param_list_l& asn1_flow_map_list)
{
  slotted_id_vector<qos_flow_id_t, e1ap_qos_flow_qos_param_item> flow_map_info;

  for (const auto& asn1_flow_map_item : asn1_flow_map_list) {
    qos_flow_id_t qos_flow_id = uint_to_qos_flow_id(asn1_flow_map_item.qos_flow_id);
    flow_map_info.emplace(
        qos_flow_id,
        e1ap_qos_flow_qos_param_item{
            .qos_flow_id = qos_flow_id,
            .qos_flow_level_qos_params =
                details::e1ap_asn1_to_qos_flow_level_qos_params(asn1_flow_map_item.qos_flow_level_qos_params),
            .qos_flow_map_ind =
                details::convert_if_present(asn1_flow_map_item.qos_flow_map_ind_present, [&asn1_flow_map_item] {
                  return static_cast<e1ap_qos_flow_map_ind>(static_cast<int>(asn1_flow_map_item.qos_flow_map_ind));
                })});
  }

  return flow_map_info;
}

/// \brief Converts E1AP ASN.1 Data Forwarding Info Request to \c e1ap_data_forwarding_info_request type.
/// \param[in] asn1_data_forwarding_info_request The E1AP ASN.1 Data Forwarding Info Request.
/// \return The \c e1ap_data_forwarding_info_request object where the result of the conversion is stored.
[[nodiscard]] inline e1ap_data_forwarding_info_request e1ap_asn1_to_data_forwarding_info_request(
    const asn1::e1ap::data_forwarding_info_request_s& asn1_data_forwarding_info_request)
{
  return e1ap_data_forwarding_info_request{
      .data_forwarding_request =
          details::e1ap_asn1_to_data_forwarding_request(asn1_data_forwarding_info_request.data_forwarding_request),
      .qos_flows_forwarded_on_fwd_tunnels = details::e1ap_asn1_to_qos_flows_forwarded_on_fwd_tunnels(
          asn1_data_forwarding_info_request.qos_flows_forwarded_on_fwd_tunnels)};
}

/// \brief Converts \c e1ap_data_forwarding_info_request type to E1AP ASN.1 Data Forwarding Info Request.
/// \param[in] data_forwarding_info_request The data forwarding info request.
/// \return The E1AP ASN.1 Data Forwarding Info Request.
[[nodiscard]] inline asn1::e1ap::data_forwarding_info_request_s
e1ap_data_forwarding_info_request_to_asn1(const e1ap_data_forwarding_info_request& data_forwarding_info_request)
{
  return asn1::e1ap::data_forwarding_info_request_s{
      .ie_exts_present = false,
      .data_forwarding_request =
          details::e1ap_data_forwarding_request_to_asn1(data_forwarding_info_request.data_forwarding_request),
      .qos_flows_forwarded_on_fwd_tunnels =
          details::e1ap_qos_flows_forwarded_to_asn1(data_forwarding_info_request.qos_flows_forwarded_on_fwd_tunnels),
      .ie_exts = asn1::e1ap::data_forwarding_info_request_ext_ies_container{}};
}

/// \brief Converts E1AP ASN.1 Data Forwarding Information to \c e1ap_data_forwarding_info type.
/// \param[in] asn1_data_forwarding_info The E1AP ASN.1 Data Forwarding Information.
/// \return The \c e1ap_data_forwarding_info object where the result of the conversion is stored.
[[nodiscard]] inline e1ap_data_forwarding_info
e1ap_asn1_to_data_forwarding_info(const asn1::e1ap::data_forwarding_info_s& asn1_data_forwarding_info)
{
  return e1ap_data_forwarding_info{
      .ul_data_forwarding = details::convert_if_present(asn1_data_forwarding_info.ul_data_forwarding_present,
                                                        [&asn1_data_forwarding_info] {
                                                          return asn1_to_up_transport_layer_info(
                                                              asn1_data_forwarding_info.ul_data_forwarding);
                                                        }),
      .dl_data_forwarding = details::convert_if_present(asn1_data_forwarding_info.dl_data_forwarding_present,
                                                        [&asn1_data_forwarding_info] {
                                                          return asn1_to_up_transport_layer_info(
                                                              asn1_data_forwarding_info.dl_data_forwarding);
                                                        }),
      .data_forwarding_to_ng_ran_qos_flow_info_list =
          details::e1ap_asn1_to_data_forwarding_qos_flow_ids(asn1_data_forwarding_info.ie_exts)};
}

/// \brief Converts \c e1ap_data_forwarding_info type to E1AP ASN.1 Data Forwarding Information.
/// \param[in] data_forwarding_info The data forwarding information.
/// \return The E1AP ASN.1 Data Forwarding Information.
[[nodiscard]] inline asn1::e1ap::data_forwarding_info_s
e1ap_data_forwarding_info_to_asn1(const e1ap_data_forwarding_info& data_forwarding_info)
{
  const std::optional<up_transport_layer_info>& ul_data_forwarding = data_forwarding_info.ul_data_forwarding;
  const std::optional<up_transport_layer_info>& dl_data_forwarding = data_forwarding_info.dl_data_forwarding;
  const std::vector<qos_flow_id_t>& qos_flow_ids = data_forwarding_info.data_forwarding_to_ng_ran_qos_flow_info_list;

  return asn1::e1ap::data_forwarding_info_s{
      .ul_data_forwarding_present = ul_data_forwarding.has_value(),
      .dl_data_forwarding_present = dl_data_forwarding.has_value(),
      .ie_exts_present            = !qos_flow_ids.empty(),
      .ul_data_forwarding = ul_data_forwarding ? details::up_transport_layer_info_to_e1ap_asn1(*ul_data_forwarding)
                                               : asn1::e1ap::up_tnl_info_c{},
      .dl_data_forwarding = dl_data_forwarding ? details::up_transport_layer_info_to_e1ap_asn1(*dl_data_forwarding)
                                               : asn1::e1ap::up_tnl_info_c{},
      .ie_exts            = details::e1ap_data_forwarding_ie_exts_to_asn1(qos_flow_ids)};
}

/// \brief Converts E1AP ASN.1 PDCP SN Status Info to \c e1ap_pdcp_sn_status_info type.
/// \param[in] asn1_pdcp_sn_status_info The E1AP ASN.1 PDCP SN Status Info.
/// \return The \c e1ap_pdcp_sn_status_info object where the result of the conversion is stored.
[[nodiscard]] inline e1ap_pdcp_sn_status_info
e1ap_asn1_to_pdcp_sn_status_info(const asn1::e1ap::pdcp_sn_status_info_s& asn1_pdcp_sn_status_info)
{
  const asn1::e1ap::drb_b_status_transfer_s& asn1_ul = asn1_pdcp_sn_status_info.pdcp_status_transfer_ul;

  return e1ap_pdcp_sn_status_info{
      .pdcp_status_transfer_ul =
          e1ap_drb_status_transfer{
              .count_value                = e1ap_asn1_pdcp_count_to_pdcp_count(asn1_ul.count_value),
              .receive_status_of_pdcp_sdu = details::convert_if_present(
                  asn1_ul.receive_statusof_pdcp_sdu_present,
                  [&asn1_ul] { return static_cast<uint64_t>(asn1_ul.receive_statusof_pdcp_sdu.to_number()); })},
      .pdcp_status_transfer_dl = e1ap_asn1_pdcp_count_to_pdcp_count(asn1_pdcp_sn_status_info.pdcp_status_transfer_dl)};
}

/// \brief Converts the DRB setup list of the NG-RAN to the ASN.1 DRB item list.
/// \tparam template_asn1_item ASN.1 DRB setup item type.
/// \param[in] drb_setup_list_ng_ran DRB setup list of the NG-RAN.
/// \return The ASN.1 DRB item list.
template <typename template_asn1_item>
[[nodiscard]] inline asn1::dyn_array<template_asn1_item>
e1ap_drb_item_list_to_asn1(const slotted_id_vector<drb_id_t, e1ap_drb_setup_item_ng_ran>& drb_setup_list_ng_ran)
{
  asn1::dyn_array<template_asn1_item> asn1_drb_item_list;

  for (const auto& drb_setup_item_ng_ran : drb_setup_list_ng_ran) {
    const std::optional<e1ap_data_forwarding_info>& fwd_info_resp = drb_setup_item_ng_ran.drb_data_forwarding_info_resp;

    asn1_drb_item_list.push_back(template_asn1_item{
        .drb_data_forwarding_info_resp_present = fwd_info_resp.has_value(),
        .ie_exts_present                       = false,
        .drb_id                                = to_underlying(drb_setup_item_ng_ran.drb_id),
        .drb_data_forwarding_info_resp         = fwd_info_resp ? e1ap_data_forwarding_info_to_asn1(*fwd_info_resp)
                                                               : details::make_empty_data_forwarding_info(),
        .ul_up_transport_params = details::e1ap_up_params_to_asn1(drb_setup_item_ng_ran.ul_up_transport_params),
        .flow_setup_list        = details::e1ap_qos_flow_setup_list_to_asn1(drb_setup_item_ng_ran.flow_setup_list),
        .flow_failed_list       = details::e1ap_qos_flow_failed_list_to_asn1(drb_setup_item_ng_ran.flow_failed_list),
        .ie_exts                = decltype(template_asn1_item::ie_exts){}});
  }

  return asn1_drb_item_list;
}

/// \brief Converts the failed DRB list of the NG-RAN to the ASN.1 DRB item list.
/// \tparam template_asn1_item ASN.1 DRB failed item type.
/// \param[in] drb_failed_list_ng_ran Failed DRB list of the NG-RAN.
/// \return The ASN.1 DRB item list.
template <typename template_asn1_item>
[[nodiscard]] inline asn1::dyn_array<template_asn1_item> e1ap_drb_failed_item_list_to_asn1(
    const slotted_id_vector<drb_id_t, e1ap_drb_failed_item_ng_ran>& drb_failed_list_ng_ran)
{
  asn1::dyn_array<template_asn1_item> asn1_drb_item_list;

  for (const auto& drb_failed_item : drb_failed_list_ng_ran) {
    asn1_drb_item_list.push_back(template_asn1_item{.ie_exts_present = false,
                                                    .drb_id          = to_underlying(drb_failed_item.drb_id),
                                                    .cause           = cause_to_asn1(drb_failed_item.cause),
                                                    .ie_exts         = decltype(template_asn1_item::ie_exts){}});
  }

  return asn1_drb_item_list;
}

/// \brief Converts an E1AP ASN.1 activity notification level to \c activity_notification_level_t.
///
/// Unknown values are converted to \c activity_notification_level_t::invalid.
/// \param[in] asn1_activity_notification The E1AP ASN.1 activity notification level.
/// \return The activity notification level.
[[nodiscard]] inline activity_notification_level_t
asn1_to_activity_notification_level(asn1::e1ap::activity_notif_level_e asn1_activity_notification)
{
  switch (asn1_activity_notification.value) {
    case asn1::e1ap::activity_notif_level_opts::options::drb:
      return activity_notification_level_t::drb;
    case asn1::e1ap::activity_notif_level_opts::options::pdu_session:
      return activity_notification_level_t::pdu_session;
    case asn1::e1ap::activity_notif_level_opts::options::ue:
      return activity_notification_level_t::ue;
    default:
      return activity_notification_level_t::invalid;
  }
}

/// \brief Converts type \c security_result_t to an ASN.1 type.
///
/// \warning The application is terminated if a result cannot be converted to its E1AP counterpart.
/// \param[in] security_result Security Result IE contents.
/// \return The ASN.1 object where the result of the conversion is stored.
[[nodiscard]] inline asn1::e1ap::security_result_s security_result_to_asn1(security_result_t security_result)
{
  auto integrity_protection_result_to_asn1 =
      [](integrity_protection_result_t result) -> asn1::e1ap::integrity_protection_result_e {
    switch (result) {
      case integrity_protection_result_t::performed:
      case integrity_protection_result_t::not_performed:
        return static_cast<asn1::e1ap::integrity_protection_result_opts::options>(result);
      default:
        report_fatal_error("Cannot convert security result to E1AP type");
    }
  };

  auto confidentiality_protection_result_to_asn1 =
      [](confidentiality_protection_result_t result) -> asn1::e1ap::confidentiality_protection_result_e {
    switch (result) {
      case confidentiality_protection_result_t::performed:
      case confidentiality_protection_result_t::not_performed:
        return static_cast<asn1::e1ap::confidentiality_protection_result_opts::options>(result);
      default:
        report_fatal_error("Cannot convert security result to E1AP type");
    }
  };

  return asn1::e1ap::security_result_s{
      .ie_exts_present             = false,
      .integrity_protection_result = integrity_protection_result_to_asn1(security_result.integrity_protection_result),
      .confidentiality_protection_result =
          confidentiality_protection_result_to_asn1(security_result.confidentiality_protection_result),
      .ie_exts = asn1::e1ap::security_result_ext_ies_container{}};
}

/// \brief Converts ASN.1 type to \c security_result_t.
///
/// If a result of the ASN.1 object cannot be converted, an error is logged and the result keeps its default value,
/// which is performed.
///
/// \param[in] asn1_security_result ASN.1 object from which the result is taken.
/// \param[in,out] logger Logger used to report the conversion errors.
/// \return The Security Result IE contents.
[[nodiscard]] inline security_result_t
asn1_to_security_result(const asn1::e1ap::security_result_s& asn1_security_result, ocudulog::basic_logger& logger)
{
  auto asn1_to_integrity_protection_result = [&logger](asn1::e1ap::integrity_protection_result_e asn1_result) {
    switch (asn1_result) {
      case asn1::e1ap::integrity_protection_result_opts::performed:
      case asn1::e1ap::integrity_protection_result_opts::not_performed:
        return static_cast<integrity_protection_result_t>(asn1_result.value);
      default:
        logger.error("Cannot convert security result to E1AP type");
        return integrity_protection_result_t{};
    }
  };

  auto asn1_to_confidentiality_protection_result =
      [&logger](asn1::e1ap::confidentiality_protection_result_e asn1_result) {
        switch (asn1_result) {
          case asn1::e1ap::confidentiality_protection_result_opts::performed:
          case asn1::e1ap::confidentiality_protection_result_opts::not_performed:
            return static_cast<confidentiality_protection_result_t>(asn1_result.value);
          default:
            logger.error("Cannot convert security result to E1AP type");
            return confidentiality_protection_result_t{};
        }
      };

  return security_result_t{.integrity_protection_result =
                               asn1_to_integrity_protection_result(asn1_security_result.integrity_protection_result),
                           .confidentiality_protection_result = asn1_to_confidentiality_protection_result(
                               asn1_security_result.confidentiality_protection_result)};
}

/// \brief Converts type \c security_indication_t to an ASN.1 type.
///
/// \warning The application is terminated if an indication cannot be converted to its E1AP counterpart.
/// \param[in] security_ind Security Indication IE contents.
/// \return The ASN.1 object where the result of the conversion is stored.
[[nodiscard]] inline asn1::e1ap::security_ind_s security_indication_to_asn1(security_indication_t security_ind)
{
  auto integrity_protection_ind_to_asn1 =
      [](integrity_protection_indication_t ind) -> asn1::e1ap::integrity_protection_ind_e {
    switch (ind) {
      case integrity_protection_indication_t::not_needed:
      case integrity_protection_indication_t::preferred:
      case integrity_protection_indication_t::required:
        return static_cast<asn1::e1ap::integrity_protection_ind_opts::options>(ind);
      default:
        report_fatal_error("Cannot convert security indication to E1AP type");
    }
  };

  auto confidentiality_protection_ind_to_asn1 =
      [](confidentiality_protection_indication_t ind) -> asn1::e1ap::confidentiality_protection_ind_e {
    switch (ind) {
      case confidentiality_protection_indication_t::not_needed:
      case confidentiality_protection_indication_t::preferred:
      case confidentiality_protection_indication_t::required:
        return static_cast<asn1::e1ap::confidentiality_protection_ind_opts::options>(ind);
      default:
        report_fatal_error("Cannot convert security indication to E1AP type");
    }
  };

  return asn1::e1ap::security_ind_s{
      .max_ip_datarate_present  = false,
      .ie_exts_present          = false,
      .integrity_protection_ind = integrity_protection_ind_to_asn1(security_ind.integrity_protection_ind),
      .confidentiality_protection_ind =
          confidentiality_protection_ind_to_asn1(security_ind.confidentiality_protection_ind),
      .max_ip_datarate = asn1::e1ap::max_ip_datarate_s{.ie_exts_present = false,
                                                       .max_ip_rate     = asn1::e1ap::max_ip_rate_e{},
                                                       .ie_exts = asn1::e1ap::max_ip_datarate_ext_ies_container{}},
      .ie_exts         = asn1::e1ap::security_ind_ext_ies_container{}};
}

/// \brief Converts an ASN.1 security indication to \c security_indication_t.
///
/// If an indication of the ASN.1 object cannot be converted, an error is logged and the indication keeps its default
/// value, which is required.
///
/// \param[in] asn1_security_ind ASN.1 object from which the Security Indication IE contents are taken.
/// \param[in,out] logger Logger used to report the conversion errors.
/// \return The Security Indication IE contents.
[[nodiscard]] inline security_indication_t
asn1_to_security_indication(const asn1::e1ap::security_ind_s& asn1_security_ind, ocudulog::basic_logger& logger)
{
  auto asn1_to_integrity_protection_ind = [&logger](asn1::e1ap::integrity_protection_ind_e asn1_ind) {
    switch (asn1_ind) {
      case asn1::e1ap::integrity_protection_ind_opts::not_needed:
      case asn1::e1ap::integrity_protection_ind_opts::preferred:
      case asn1::e1ap::integrity_protection_ind_opts::required:
        return static_cast<integrity_protection_indication_t>(asn1_ind.value);
      default:
        logger.error("Cannot convert security indication to E1AP type");
        return integrity_protection_indication_t::required;
    }
  };

  auto asn1_to_confidentiality_protection_ind = [&logger](asn1::e1ap::confidentiality_protection_ind_e asn1_ind) {
    switch (asn1_ind) {
      case asn1::e1ap::confidentiality_protection_ind_opts::not_needed:
      case asn1::e1ap::confidentiality_protection_ind_opts::preferred:
      case asn1::e1ap::confidentiality_protection_ind_opts::required:
        return static_cast<confidentiality_protection_indication_t>(asn1_ind.value);
      default:
        logger.error("Cannot convert security indication to E1AP type");
        return confidentiality_protection_indication_t::required;
    }
  };

  return security_indication_t{
      .integrity_protection_ind = asn1_to_integrity_protection_ind(asn1_security_ind.integrity_protection_ind),
      .confidentiality_protection_ind =
          asn1_to_confidentiality_protection_ind(asn1_security_ind.confidentiality_protection_ind)};
}

/// \brief Converts ASN.1 PDU session type to \c pdu_session_type_t.
/// \param[in] asn1_pdu_session_type The ASN.1 PDU session type.
/// \param[in,out] logger Logger used to report the conversion errors.
/// \return The common type PDU session type if the conversion was successful, \c std::nullopt otherwise.
[[nodiscard]] inline std::optional<pdu_session_type_t>
asn1_to_pdu_session_type(asn1::e1ap::pdu_session_type_e asn1_pdu_session_type, ocudulog::basic_logger& logger)
{
  switch (asn1_pdu_session_type) {
    case asn1::e1ap::pdu_session_type_e::pdu_session_type_opts::ipv4:
      return pdu_session_type_t::ipv4;
    case asn1::e1ap::pdu_session_type_e::pdu_session_type_opts::ipv6:
      return pdu_session_type_t::ipv6;
    case asn1::e1ap::pdu_session_type_e::pdu_session_type_opts::ipv4v6:
      return pdu_session_type_t::ipv4v6;
    case asn1::e1ap::pdu_session_type_e::pdu_session_type_opts::ethernet:
      return pdu_session_type_t::ethernet;
    default:
      logger.warning("Cannot convert ASN.1 PDU session type to common type");
      return std::nullopt;
  }
}

} // namespace ocudu
