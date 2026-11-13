// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/slotted_vector.h"
#include "ocudu/adt/span.h"
#include "ocudu/adt/to_array.h"
#include "ocudu/asn1/asn1_containers.h"
#include "ocudu/asn1/e1ap/e1ap_ies.h"
#include "ocudu/e1ap/common/e1ap_types.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/ran/qos/qos_prio_level.h"
#include "ocudu/ran/sdap/sdap_config.h"
#include "ocudu/support/error_handling.h"
#include <optional>
#include <utility>
#include <vector>

namespace ocudu::details {

/// Number of octets of the S-NSSAI SST.
constexpr unsigned sst_nof_octets = 1;
/// Number of octets of the S-NSSAI SD.
constexpr unsigned sd_nof_octets = 3;
/// Number of octets of the PLMN identity.
constexpr unsigned plmn_id_nof_octets = 3;
/// Number of bits of the NR cell identity.
constexpr unsigned nr_cell_id_nof_bits = 36;
/// Cell group identifier set in the E1AP ASN.1 UP parameters, as the cell group ID is not propagated.
constexpr unsigned default_cell_group_id = 0;

/// \brief Converts a value to an \c std::optional only if it is present.
/// \param[in] is_present Flag that indicates whether the value is present.
/// \param[in] convert Callable that returns the converted value. It is only called when the value is present.
/// \return The converted value if it is present, \c std::nullopt otherwise.
template <typename Converter>
[[nodiscard]] inline auto convert_if_present(bool is_present, Converter&& convert) -> std::optional<decltype(convert())>
{
  if (!is_present) {
    return std::nullopt;
  }
  return convert();
}

/// Converts a number to an ASN.1 fixed-size octet string.
template <unsigned N>
[[nodiscard]] inline asn1::fixed_octstring<N, true> number_to_fixed_octstring(uint64_t value)
{
  asn1::fixed_octstring<N, true> octets;
  octets.from_number(value);
  return octets;
}

/// Converts a \c sdap_hdr_ul_cfg to the E1AP ASN.1 SDAP-Header-UL option.
[[nodiscard]] inline asn1::e1ap::sdap_hdr_ul_opts::options sdap_hdr_ul_cfg_to_e1ap_asn1(sdap_hdr_ul_cfg hdr_cfg)
{
  if (hdr_cfg == sdap_hdr_ul_cfg::absent) {
    return asn1::e1ap::sdap_hdr_ul_opts::absent;
  }
  return asn1::e1ap::sdap_hdr_ul_opts::present;
}

/// Converts a \c sdap_hdr_dl_cfg to the E1AP ASN.1 SDAP-Header-DL option.
[[nodiscard]] inline asn1::e1ap::sdap_hdr_dl_opts::options sdap_hdr_dl_cfg_to_e1ap_asn1(sdap_hdr_dl_cfg hdr_cfg)
{
  if (hdr_cfg == sdap_hdr_dl_cfg::absent) {
    return asn1::e1ap::sdap_hdr_dl_opts::absent;
  }
  return asn1::e1ap::sdap_hdr_dl_opts::present;
}

/// \brief Converts an E1AP ASN.1 SDAP-Header-UL option to \c sdap_hdr_ul_cfg.
///
/// An option other than absent or present is a fatal error.
[[nodiscard]] inline sdap_hdr_ul_cfg
e1ap_asn1_to_sdap_hdr_ul_cfg(asn1::e1ap::sdap_hdr_ul_opts::options asn1_hdr_ul_opts)
{
  switch (asn1_hdr_ul_opts) {
    case asn1::e1ap::sdap_hdr_ul_opts::absent:
      return sdap_hdr_ul_cfg::absent;
    case asn1::e1ap::sdap_hdr_ul_opts::present:
      return sdap_hdr_ul_cfg::present;
    default:
      report_fatal_error("Invalid SDAP-Header-UL option ({})", fmt::underlying(asn1_hdr_ul_opts));
  }
}

/// \brief Converts an E1AP ASN.1 SDAP-Header-DL option to \c sdap_hdr_dl_cfg.
///
/// An option other than absent or present is a fatal error.
[[nodiscard]] inline sdap_hdr_dl_cfg
e1ap_asn1_to_sdap_hdr_dl_cfg(asn1::e1ap::sdap_hdr_dl_opts::options asn1_hdr_dl_opts)
{
  switch (asn1_hdr_dl_opts) {
    case asn1::e1ap::sdap_hdr_dl_opts::absent:
      return sdap_hdr_dl_cfg::absent;
    case asn1::e1ap::sdap_hdr_dl_opts::present:
      return sdap_hdr_dl_cfg::present;
    default:
      report_fatal_error("Invalid SDAP-Header-DL option ({})", fmt::underlying(asn1_hdr_dl_opts));
  }
}

/// \brief Converts \c rlc_mode type to an E1AP ASN.1 type.
///
/// UM is converted to the bidirectional RLC UM mode and AM to the RLC AM mode. Any other mode is a fatal error.
///
/// \param[in] rlc_mode_value rlc_mode type.
/// \return The E1AP ASN.1 RLC mode.
[[nodiscard]] inline asn1::e1ap::rlc_mode_e rlc_mode_to_asn1(pdcp_rlc_mode rlc_mode_value)
{
  switch (rlc_mode_value) {
    case pdcp_rlc_mode::um:
      return asn1::e1ap::rlc_mode_opts::rlc_um_bidirectional;
    case pdcp_rlc_mode::am:
      return asn1::e1ap::rlc_mode_opts::rlc_am;
    default:
      report_fatal_error("Invalid RLC mode ({})", rlc_mode_value);
  }
}

/// \brief Converts E1AP ASN.1 type to an \c rlc_mode type.
///
/// All the RLC UM variants are converted to \c pdcp_rlc_mode::um. Any other value is a fatal error.
///
/// \param[in] asn1_rlc_mode E1AP ASN.1 type.
/// \return The RLC mode.
[[nodiscard]] inline pdcp_rlc_mode asn1_to_rlc_mode(asn1::e1ap::rlc_mode_e asn1_rlc_mode)
{
  switch (asn1_rlc_mode) {
    case asn1::e1ap::rlc_mode_opts::rlc_um_bidirectional:
    case asn1::e1ap::rlc_mode_opts::rlc_um_unidirectional_ul:
    case asn1::e1ap::rlc_mode_opts::rlc_um_unidirectional_dl:
      return pdcp_rlc_mode::um;
    case asn1::e1ap::rlc_mode_opts::rlc_am:
      return pdcp_rlc_mode::am;
    default:
      report_fatal_error("Invalid RLC mode: {}", asn1_rlc_mode.to_string());
  }
}

/// \brief Converts PDCP SN size from enum to E1AP ASN.1.
///
/// An unsupported PDCP SN size is a fatal error.
///
/// \param[in] sn_size PDCP SN size.
/// \return The E1AP ASN.1 PDCP SN size.
[[nodiscard]] inline asn1::e1ap::pdcp_sn_size_e pdcp_sn_size_to_asn1(pdcp_sn_size sn_size)
{
  switch (sn_size) {
    case pdcp_sn_size::size12bits:
      return asn1::e1ap::pdcp_sn_size_e::s_neg12;
    case pdcp_sn_size::size18bits:
      return asn1::e1ap::pdcp_sn_size_e::s_neg18;
    default:
      report_fatal_error("Unsupported PDCP SN size. PDCP SN size={}", sn_size);
  }
}

/// \brief Converts E1AP ASN.1 to PDCP SN size.
///
/// An unsupported E1AP ASN.1 PDCP SN size is a fatal error.
///
/// \param[in] asn1_sn_size E1AP ASN.1 PDCP SN size.
/// \return The PDCP SN size.
[[nodiscard]] inline pdcp_sn_size asn1_to_pdcp_sn_size(asn1::e1ap::pdcp_sn_size_e asn1_sn_size)
{
  switch (asn1_sn_size) {
    case asn1::e1ap::pdcp_sn_size_e::s_neg12:
      return pdcp_sn_size::size12bits;
    case asn1::e1ap::pdcp_sn_size_e::s_neg18:
      return pdcp_sn_size::size18bits;
    default:
      report_fatal_error("Unsupported PDCP SN size. PDCP SN size={}", fmt::underlying(asn1_sn_size.value));
  }
}

/// Mapping between the common PDCP discard timer values and the E1AP ASN.1 discard timer values.
constexpr auto pdcp_discard_timer_mappings =
    to_array<std::pair<pdcp_discard_timer, asn1::e1ap::discard_timer_opts::options>>({
        {pdcp_discard_timer::ms10, asn1::e1ap::discard_timer_opts::ms10},
        {pdcp_discard_timer::ms20, asn1::e1ap::discard_timer_opts::ms20},
        {pdcp_discard_timer::ms30, asn1::e1ap::discard_timer_opts::ms30},
        {pdcp_discard_timer::ms40, asn1::e1ap::discard_timer_opts::ms40},
        {pdcp_discard_timer::ms50, asn1::e1ap::discard_timer_opts::ms50},
        {pdcp_discard_timer::ms60, asn1::e1ap::discard_timer_opts::ms60},
        {pdcp_discard_timer::ms75, asn1::e1ap::discard_timer_opts::ms75},
        {pdcp_discard_timer::ms100, asn1::e1ap::discard_timer_opts::ms100},
        {pdcp_discard_timer::ms150, asn1::e1ap::discard_timer_opts::ms150},
        {pdcp_discard_timer::ms200, asn1::e1ap::discard_timer_opts::ms200},
        {pdcp_discard_timer::ms250, asn1::e1ap::discard_timer_opts::ms250},
        {pdcp_discard_timer::ms300, asn1::e1ap::discard_timer_opts::ms300},
        {pdcp_discard_timer::ms500, asn1::e1ap::discard_timer_opts::ms500},
        {pdcp_discard_timer::ms750, asn1::e1ap::discard_timer_opts::ms750},
        {pdcp_discard_timer::ms1500, asn1::e1ap::discard_timer_opts::ms1500},
        {pdcp_discard_timer::infinity, asn1::e1ap::discard_timer_opts::infinity},
    });

/// \brief Converts E1AP ASN.1 discard timer type to \c pdcp_discard_timer type.
///
/// A value without a counterpart in the mapping table is a fatal error.
[[nodiscard]] inline pdcp_discard_timer asn1_to_pdcp_discard_timer(asn1::e1ap::discard_timer_e asn1_discard_timer)
{
  for (const auto& [common_value, asn1_value] : pdcp_discard_timer_mappings) {
    if (asn1_value == asn1_discard_timer.value) {
      return common_value;
    }
  }
  report_fatal_error("Unsupported PDCP discard timer. PDCP discard timer={}",
                     fmt::underlying(asn1_discard_timer.value));
}

/// \brief Converts \c pdcp_discard_timer type to E1AP ASN.1 discard timer type.
///
/// A value without a counterpart in the mapping table is a fatal error.
[[nodiscard]] inline asn1::e1ap::discard_timer_e pdcp_discard_timer_to_asn1(pdcp_discard_timer discard_timer)
{
  for (const auto& [common_value, asn1_value] : pdcp_discard_timer_mappings) {
    if (common_value == discard_timer) {
      return asn1_value;
    }
  }
  report_fatal_error("Unsupported PDCP discard timer. PDCP discard timer={}", discard_timer);
}

/// \brief Mapping between the common PDCP t-Reordering values and the E1AP ASN.1 t-Reordering values.
///
/// The infinity value has no counterpart in the table, as it is represented by the ASN.1 \c nulltype.
constexpr auto pdcp_t_reordering_mappings =
    to_array<std::pair<pdcp_t_reordering, asn1::e1ap::t_reordering_opts::options>>({
        {pdcp_t_reordering::ms0, asn1::e1ap::t_reordering_opts::ms0},
        {pdcp_t_reordering::ms1, asn1::e1ap::t_reordering_opts::ms1},
        {pdcp_t_reordering::ms2, asn1::e1ap::t_reordering_opts::ms2},
        {pdcp_t_reordering::ms4, asn1::e1ap::t_reordering_opts::ms4},
        {pdcp_t_reordering::ms5, asn1::e1ap::t_reordering_opts::ms5},
        {pdcp_t_reordering::ms8, asn1::e1ap::t_reordering_opts::ms8},
        {pdcp_t_reordering::ms10, asn1::e1ap::t_reordering_opts::ms10},
        {pdcp_t_reordering::ms15, asn1::e1ap::t_reordering_opts::ms15},
        {pdcp_t_reordering::ms20, asn1::e1ap::t_reordering_opts::ms20},
        {pdcp_t_reordering::ms30, asn1::e1ap::t_reordering_opts::ms30},
        {pdcp_t_reordering::ms40, asn1::e1ap::t_reordering_opts::ms40},
        {pdcp_t_reordering::ms50, asn1::e1ap::t_reordering_opts::ms50},
        {pdcp_t_reordering::ms60, asn1::e1ap::t_reordering_opts::ms60},
        {pdcp_t_reordering::ms80, asn1::e1ap::t_reordering_opts::ms80},
        {pdcp_t_reordering::ms100, asn1::e1ap::t_reordering_opts::ms100},
        {pdcp_t_reordering::ms120, asn1::e1ap::t_reordering_opts::ms120},
        {pdcp_t_reordering::ms140, asn1::e1ap::t_reordering_opts::ms140},
        {pdcp_t_reordering::ms160, asn1::e1ap::t_reordering_opts::ms160},
        {pdcp_t_reordering::ms180, asn1::e1ap::t_reordering_opts::ms180},
        {pdcp_t_reordering::ms200, asn1::e1ap::t_reordering_opts::ms200},
        {pdcp_t_reordering::ms220, asn1::e1ap::t_reordering_opts::ms220},
        {pdcp_t_reordering::ms240, asn1::e1ap::t_reordering_opts::ms240},
        {pdcp_t_reordering::ms260, asn1::e1ap::t_reordering_opts::ms260},
        {pdcp_t_reordering::ms280, asn1::e1ap::t_reordering_opts::ms280},
        {pdcp_t_reordering::ms300, asn1::e1ap::t_reordering_opts::ms300},
        {pdcp_t_reordering::ms500, asn1::e1ap::t_reordering_opts::ms500},
        {pdcp_t_reordering::ms750, asn1::e1ap::t_reordering_opts::ms750},
        {pdcp_t_reordering::ms1000, asn1::e1ap::t_reordering_opts::ms1000},
        {pdcp_t_reordering::ms1250, asn1::e1ap::t_reordering_opts::ms1250},
        {pdcp_t_reordering::ms1500, asn1::e1ap::t_reordering_opts::ms1500},
        {pdcp_t_reordering::ms1750, asn1::e1ap::t_reordering_opts::ms1750},
        {pdcp_t_reordering::ms2000, asn1::e1ap::t_reordering_opts::ms2000},
        {pdcp_t_reordering::ms2250, asn1::e1ap::t_reordering_opts::ms2250},
        {pdcp_t_reordering::ms2500, asn1::e1ap::t_reordering_opts::ms2500},
        {pdcp_t_reordering::ms2750, asn1::e1ap::t_reordering_opts::ms2750},
        {pdcp_t_reordering::ms3000, asn1::e1ap::t_reordering_opts::ms3000},
    });

/// \brief Converts E1AP ASN.1 t-Reordering type to \c pdcp_t_reordering type.
///
/// A value without a counterpart in the mapping table, including the ASN.1 \c nulltype, is a fatal error.
[[nodiscard]] inline pdcp_t_reordering asn1_to_pdcp_t_reordering(asn1::e1ap::t_reordering_e asn1_t_reordering)
{
  for (const auto& [common_value, asn1_value] : pdcp_t_reordering_mappings) {
    if (asn1_value == asn1_t_reordering.value) {
      return common_value;
    }
  }
  report_fatal_error("Unsupported PDCP t-reordering timer. PDCP t-reordering timer={}",
                     fmt::underlying(asn1_t_reordering.value));
}

/// \brief Converts a \c pdcp_t_reordering type to the E1AP ASN.1 t-Reordering type.
///
/// The infinity value is converted to the ASN.1 \c nulltype. A value without a counterpart in the mapping table is a
/// fatal error.
[[nodiscard]] inline asn1::e1ap::t_reordering_e pdcp_t_reordering_to_asn1(pdcp_t_reordering t_reordering)
{
  if (t_reordering == pdcp_t_reordering::infinity) {
    return asn1::e1ap::t_reordering_e::nulltype;
  }

  for (const auto& [common_value, asn1_value] : pdcp_t_reordering_mappings) {
    if (common_value == t_reordering) {
      return asn1_value;
    }
  }
  report_fatal_error("Unsupported PDCP t-reordering timer. PDCP t-reordering timer={}", t_reordering);
}

/// Converts a \c pdcp_t_reordering optional to the E1AP ASN.1 t-Reordering timer, which is empty if there is no value.
[[nodiscard]] inline asn1::e1ap::t_reordering_timer_s
pdcp_t_reordering_timer_to_asn1(std::optional<pdcp_t_reordering> t_reordering_timer)
{
  auto create_t_reordering = [](std::optional<pdcp_t_reordering> timer) -> asn1::e1ap::t_reordering_e {
    if (!timer) {
      return asn1::e1ap::t_reordering_e{};
    }
    return pdcp_t_reordering_to_asn1(*timer);
  };

  return asn1::e1ap::t_reordering_timer_s{.ie_exts_present = false,
                                          .t_reordering    = create_t_reordering(t_reordering_timer),
                                          .ie_exts         = asn1::e1ap::t_reordering_timer_ext_ies_container{}};
}

/// Converts an optional UL data split threshold to the E1AP ASN.1 type, which is empty if there is no value.
[[nodiscard]] inline asn1::e1ap::ul_data_split_thres_e
ul_data_split_thres_to_asn1(std::optional<int> ul_data_split_thres)
{
  asn1::e1ap::ul_data_split_thres_e asn1_ul_data_split_thres;
  if (ul_data_split_thres) {
    asn1::number_to_enum(asn1_ul_data_split_thres, *ul_data_split_thres);
  }
  return asn1_ul_data_split_thres;
}

/// \brief Converts a boolean to an E1AP ASN.1 enumerated that only has the \c true_value option.
///
/// The ASN.1 enumerated is set to \c true_value if the flag is true. Otherwise, it is set to \c nulltype.
template <typename AsnEnum>
[[nodiscard]] inline AsnEnum bool_to_asn1(bool flag)
{
  return AsnEnum{flag ? AsnEnum::true_value : AsnEnum::nulltype};
}

/// \brief Converts an optional boolean to an E1AP ASN.1 enumerated that only has the \c true_value option.
///
/// The ASN.1 enumerated is set to \c true_value only if the boolean has a value that is true. Otherwise, it is set to
/// \c nulltype.
template <typename AsnEnum>
[[nodiscard]] inline AsnEnum optional_bool_to_asn1(std::optional<bool> flag)
{
  return bool_to_asn1<AsnEnum>(flag.value_or(false));
}

/// Converts an optional duplication activation to the E1AP ASN.1 type, which is empty if there is no value.
[[nodiscard]] inline asn1::e1ap::dupl_activation_e
dupl_activation_to_asn1(std::optional<e1ap_dupl_activation> dupl_activation)
{
  if (!dupl_activation) {
    return asn1::e1ap::dupl_activation_e{};
  }
  return asn1::e1ap::dupl_activation_e{
      static_cast<asn1::e1ap::dupl_activation_opts::options>(static_cast<int>(*dupl_activation))};
}

/// Creates the E1AP ASN.1 EHC parameters with no IE present.
[[nodiscard]] inline asn1::e1ap::ehc_params_s make_empty_ehc_params()
{
  return asn1::e1ap::ehc_params_s{
      .ehc_dl_present  = false,
      .ehc_ul_present  = false,
      .ie_exts_present = false,
      .ehc_common      = asn1::e1ap::ehc_common_params_s{.ie_exts_present = false,
                                                         .ehc_c_id_len = asn1::e1ap::ehc_common_params_s::ehc_c_id_len_e_{},
                                                         .ie_exts      = asn1::e1ap::ehc_common_params_ext_ies_container{}},
      .ehc_dl =
          asn1::e1ap::ehc_dl_params_s{
              .ie_exts_present     = false,
              .drb_continue_ehc_dl = asn1::e1ap::ehc_dl_params_s::drb_continue_ehc_dl_e_{},
              .ie_exts =
                  asn1::e1ap::ehc_dl_params_ext_ies_container{.max_c_id_ehc_dl_present = false, .max_c_id_ehc_dl = 0}},
      .ehc_ul =
          asn1::e1ap::ehc_ul_params_s{.ie_exts_present     = false,
                                      .drb_continue_ehc_ul = asn1::e1ap::ehc_ul_params_s::drb_continue_ehc_ul_e_{},
                                      .ie_exts             = asn1::e1ap::ehc_ul_params_ext_ies_container{}},
      .ie_exts = asn1::e1ap::ehc_params_ext_ies_container{}};
}

/// Creates the E1AP ASN.1 UDC parameters with no IE present.
[[nodiscard]] inline asn1::e1ap::udc_params_s make_empty_udc_params()
{
  return asn1::e1ap::udc_params_s{
      .dictionary_present   = false,
      .continue_udc_present = false,
      .ie_exts_present      = false,
      .buffer_size          = asn1::e1ap::buffer_size_e{},
      .dictionary           = asn1::e1ap::dictionary_e{},
      .continue_udc         = asn1::e1ap::udc_params_s::continue_udc_e_{},
      .ie_exts              = asn1::e1ap::udc_params_ext_ies_container{.version_id_present = false, .version_id = 0}};
}

/// Creates the E1AP ASN.1 PDCP configuration extension IEs with no IE present.
[[nodiscard]] inline asn1::e1ap::pdcp_cfg_ext_ies_container make_empty_pdcp_cfg_ext_ies()
{
  return asn1::e1ap::pdcp_cfg_ext_ies_container{.pdcp_status_report_ind_present  = false,
                                                .add_pdcp_dupl_info_present      = false,
                                                .ehc_params_present              = false,
                                                .udc_params_present              = false,
                                                .discard_timer_extended_present  = false,
                                                .psi_based_discard_timer_present = false,
                                                .pdcp_sn_gap_report_present      = false,
                                                .pdcp_status_report_ind  = asn1::e1ap::pdcp_status_report_ind_e{},
                                                .add_pdcp_dupl_info      = asn1::e1ap::add_pdcp_dupl_info_e{},
                                                .ehc_params              = make_empty_ehc_params(),
                                                .udc_params              = make_empty_udc_params(),
                                                .discard_timer_extended  = asn1::e1ap::discard_timer_extended_e{},
                                                .psi_based_discard_timer = asn1::e1ap::psi_based_discard_timer_e{},
                                                .pdcp_sn_gap_report      = asn1::e1ap::pdcp_sn_gap_report_e{}};
}

/// \brief Converts a \c e1ap_rohc_config to the E1AP ASN.1 ROHC parameters.
///
/// Note that the continue ROHC flag is present in the ASN.1 object whenever the optional has a value. An invalid ROHC
/// type is a fatal error.
///
/// \param[in] rohc_config The ROHC configuration.
/// \return The E1AP ASN.1 ROHC parameters.
[[nodiscard]] inline asn1::e1ap::rohc_params_c e1ap_rohc_config_to_asn1(e1ap_rohc_config rohc_config)
{
  switch (const e1ap_rohc_params& rohc_params = rohc_config.rohc_params; rohc_config.rohc_type) {
    case e1ap_rohc_type::rohc: {
      bool                      continue_rohc_present = rohc_params.continue_rohc.has_value();
      asn1::e1ap::rohc_params_c asn1_rohc_params;
      asn1_rohc_params.set_rohc() =
          asn1::e1ap::rohc_s{.continue_rohc_present = continue_rohc_present,
                             .ie_exts_present       = false,
                             .max_c_id              = rohc_params.max_cid,
                             .rohc_profiles         = rohc_params.rohc_profiles,
                             .continue_rohc = bool_to_asn1<asn1::e1ap::rohc_s::continue_rohc_e_>(continue_rohc_present),
                             .ie_exts       = asn1::e1ap::rohc_ext_ies_container{}};
      return asn1_rohc_params;
    }
    case e1ap_rohc_type::uplink_only_rohc: {
      bool                      continue_rohc_present = rohc_params.continue_rohc.has_value();
      asn1::e1ap::rohc_params_c asn1_rohc_params;
      asn1_rohc_params.set_ul_only_rohc() = asn1::e1ap::ul_only_rohc_s{
          .continue_rohc_present = continue_rohc_present,
          .ie_exts_present       = false,
          .max_c_id              = rohc_params.max_cid,
          .rohc_profiles         = rohc_params.rohc_profiles,
          .continue_rohc         = bool_to_asn1<asn1::e1ap::ul_only_rohc_s::continue_rohc_e_>(continue_rohc_present),
          .ie_exts               = asn1::e1ap::ul_only_rohc_ext_ies_container{}};
      return asn1_rohc_params;
    }
  }
  report_fatal_error("Invalid ROHC type ({})", fmt::underlying(rohc_config.rohc_type));
}

/// \brief Converts the E1AP ASN.1 ROHC parameters to a \c e1ap_rohc_config.
///
/// An ASN.1 ROHC type other than \c rohc and \c ul_only_rohc is a fatal error.
[[nodiscard]] inline e1ap_rohc_config e1ap_asn1_to_rohc_config(const asn1::e1ap::rohc_params_c& asn1_rohc_params)
{
  switch (asn1_rohc_params.type().value) {
    case asn1::e1ap::rohc_params_c::types_opts::rohc: {
      const asn1::e1ap::rohc_s& asn1_rohc = asn1_rohc_params.rohc();
      return e1ap_rohc_config{.rohc_type   = e1ap_rohc_type::rohc,
                              .rohc_params = e1ap_rohc_params{
                                  .max_cid       = asn1_rohc.max_c_id,
                                  .rohc_profiles = asn1_rohc.rohc_profiles,
                                  .continue_rohc = convert_if_present(asn1_rohc.continue_rohc_present, [&asn1_rohc] {
                                    return asn1_rohc.continue_rohc ==
                                           asn1::e1ap::rohc_s::continue_rohc_opts::options::true_value;
                                  })}};
    }
    case asn1::e1ap::rohc_params_c::types_opts::ul_only_rohc: {
      const asn1::e1ap::ul_only_rohc_s& asn1_rohc = asn1_rohc_params.ul_only_rohc();
      return e1ap_rohc_config{.rohc_type   = e1ap_rohc_type::uplink_only_rohc,
                              .rohc_params = e1ap_rohc_params{
                                  .max_cid       = asn1_rohc.max_c_id,
                                  .rohc_profiles = asn1_rohc.rohc_profiles,
                                  .continue_rohc = convert_if_present(asn1_rohc.continue_rohc_present, [&asn1_rohc] {
                                    return asn1_rohc.continue_rohc ==
                                           asn1::e1ap::ul_only_rohc_s::continue_rohc_opts::options::true_value;
                                  })}};
    }
    default:
      report_fatal_error("Cannot convert E1AP ASN.1 ROHC type {} to common type",
                         fmt::underlying(asn1_rohc_params.type().value));
  }
}

/// \brief Converts E1AP ASN.1 QoS Flow Map Item to \c e1ap_qos_flow_map_item type.
[[nodiscard]] inline e1ap_qos_flow_map_item
asn1_e1ap_to_qos_flow_map_item(asn1::e1ap::qos_flow_map_item_s asn1_qos_flow_map_item)
{
  return e1ap_qos_flow_map_item{
      .qos_flow_id = uint_to_qos_flow_id(asn1_qos_flow_map_item.qos_flow_id),
      .qos_flow_map_ind =
          convert_if_present(asn1_qos_flow_map_item.qos_flow_map_ind_present, [&asn1_qos_flow_map_item] {
            return static_cast<e1ap_qos_flow_map_ind>(static_cast<int>(asn1_qos_flow_map_item.qos_flow_map_ind));
          })};
}

/// Converts an E1AP ASN.1 dynamic 5QI descriptor to a \c dyn_5qi_descriptor.
[[nodiscard]] inline dyn_5qi_descriptor
e1ap_asn1_to_dyn_5qi_descriptor(const asn1::e1ap::dyn_5qi_descriptor_s& asn1_dyn_5qi)
{
  const asn1::e1ap::packet_error_rate_s& asn1_per = asn1_dyn_5qi.packet_error_rate;

  return dyn_5qi_descriptor{
      .qos_prio_level        = qos_prio_level_t{static_cast<uint8_t>(asn1_dyn_5qi.qos_prio_level)},
      .packet_delay_budget   = asn1_dyn_5qi.packet_delay_budget,
      .per                   = packet_error_rate_t{asn1_per.per_scalar, asn1_per.per_exponent},
      .five_qi               = convert_if_present(asn1_dyn_5qi.five_qi_present,
                                    [&asn1_dyn_5qi] { return uint_to_five_qi(asn1_dyn_5qi.five_qi); }),
      .is_delay_critical     = convert_if_present(asn1_dyn_5qi.delay_crit_present,
                                              [&asn1_dyn_5qi] {
                                                return asn1_dyn_5qi.delay_crit.value ==
                                                       asn1::e1ap::dyn_5qi_descriptor_s::delay_crit_opts::delay_crit;
                                              }),
      .averaging_win         = convert_if_present(asn1_dyn_5qi.averaging_win_present,
                                          [&asn1_dyn_5qi] { return asn1_dyn_5qi.averaging_win; }),
      .max_data_burst_volume = convert_if_present(asn1_dyn_5qi.max_data_burst_volume_present,
                                                  [&asn1_dyn_5qi] { return asn1_dyn_5qi.max_data_burst_volume; })};
}

/// Converts an E1AP ASN.1 non-dynamic 5QI descriptor to a \c non_dyn_5qi_descriptor.
[[nodiscard]] inline non_dyn_5qi_descriptor
e1ap_asn1_to_non_dyn_5qi_descriptor(const asn1::e1ap::non_dyn_5qi_descriptor_s& asn1_non_dyn_5qi)
{
  return non_dyn_5qi_descriptor{
      .five_qi        = uint_to_five_qi(asn1_non_dyn_5qi.five_qi),
      .qos_prio_level = convert_if_present(
          asn1_non_dyn_5qi.qos_prio_level_present,
          [&asn1_non_dyn_5qi] { return qos_prio_level_t{static_cast<uint8_t>(asn1_non_dyn_5qi.qos_prio_level)}; }),
      .averaging_win         = convert_if_present(asn1_non_dyn_5qi.averaging_win_present,
                                          [&asn1_non_dyn_5qi] { return asn1_non_dyn_5qi.averaging_win; }),
      .max_data_burst_volume = convert_if_present(asn1_non_dyn_5qi.max_data_burst_volume_present, [&asn1_non_dyn_5qi] {
        return asn1_non_dyn_5qi.max_data_burst_volume;
      })};
}

/// \brief Converts E1AP ASN.1 QoS characteristics to a \c qos_characteristics.
///
/// Any type other than \c dyn_5qi is treated as a non-dynamic 5QI, so the ASN.1 choice must be set.
[[nodiscard]] inline qos_characteristics
e1ap_asn1_to_qos_characteristics(const asn1::e1ap::qos_characteristics_c& asn1_qos_characteristics)
{
  if (asn1_qos_characteristics.type() == asn1::e1ap::qos_characteristics_c::types_opts::dyn_5qi) {
    return qos_characteristics{e1ap_asn1_to_dyn_5qi_descriptor(asn1_qos_characteristics.dyn_5qi())};
  }
  return qos_characteristics{e1ap_asn1_to_non_dyn_5qi_descriptor(asn1_qos_characteristics.non_dyn_5qi())};
}

/// Converts the E1AP ASN.1 NG-RAN allocation and retention priority to an \c alloc_and_retention_priority.
[[nodiscard]] inline alloc_and_retention_priority
e1ap_asn1_to_alloc_and_retention_priority(asn1::e1ap::ngran_alloc_and_retention_prio_s asn1_arp)
{
  return alloc_and_retention_priority{.prio_level_arp = asn1_arp.prio_level,
                                      .may_trigger_preemption =
                                          asn1_arp.pre_emption_cap.value ==
                                          asn1::e1ap::pre_emption_cap_opts::may_trigger_pre_emption,
                                      .is_preemptable = asn1_arp.pre_emption_vulnerability.value ==
                                                        asn1::e1ap::pre_emption_vulnerability_opts::pre_emptable};
}

/// Converts the E1AP ASN.1 GBR QoS flow information to a \c gbr_qos_flow_information.
[[nodiscard]] inline gbr_qos_flow_information
e1ap_asn1_to_gbr_qos_flow_info(const asn1::e1ap::gbr_qos_flow_info_s& asn1_gbr)
{
  return gbr_qos_flow_information{
      .max_br_dl               = asn1_gbr.max_flow_bit_rate_dl,
      .max_br_ul               = asn1_gbr.max_flow_bit_rate_ul,
      .gbr_dl                  = asn1_gbr.guaranteed_flow_bit_rate_dl,
      .gbr_ul                  = asn1_gbr.guaranteed_flow_bit_rate_ul,
      .max_packet_loss_rate_dl = convert_if_present(asn1_gbr.max_packet_loss_rate_dl_present,
                                                    [&asn1_gbr] { return asn1_gbr.max_packet_loss_rate_dl; }),
      .max_packet_loss_rate_ul = convert_if_present(asn1_gbr.max_packet_loss_rate_ul_present,
                                                    [&asn1_gbr] { return asn1_gbr.max_packet_loss_rate_ul; })};
}

/// Converts the E1AP ASN.1 QoS flow level QoS parameters to a \c e1ap_qos_flow_level_qos_params.
[[nodiscard]] inline e1ap_qos_flow_level_qos_params
e1ap_asn1_to_qos_flow_level_qos_params(const asn1::e1ap::qos_flow_level_qos_params_s& asn1_qos_params)
{
  return e1ap_qos_flow_level_qos_params{
      .qos_desc               = e1ap_asn1_to_qos_characteristics(asn1_qos_params.qos_characteristics),
      .ng_ran_alloc_retention = e1ap_asn1_to_alloc_and_retention_priority(asn1_qos_params.ngran_alloc_retention_prio),
      .gbr_qos_flow_info      = convert_if_present(
          asn1_qos_params.gbr_qos_flow_info_present,
          [&asn1_qos_params] { return e1ap_asn1_to_gbr_qos_flow_info(asn1_qos_params.gbr_qos_flow_info); }),
      .reflective_qos_attribute = convert_if_present(
          asn1_qos_params.reflective_qos_attribute_present,
          [&asn1_qos_params] { return asn1::enum_to_bool(asn1_qos_params.reflective_qos_attribute); }),
      .add_qos_info =
          convert_if_present(asn1_qos_params.add_qos_info_present,
                             [&asn1_qos_params] { return asn1::enum_to_bool(asn1_qos_params.add_qos_info); }),
      .paging_policy_ind  = convert_if_present(asn1_qos_params.paging_policy_idx_present,
                                              [&asn1_qos_params] { return asn1_qos_params.paging_policy_idx; }),
      .reflective_qos_ind = convert_if_present(asn1_qos_params.reflective_qos_ind_present, [&asn1_qos_params] {
        return asn1::enum_to_bool(asn1_qos_params.reflective_qos_ind);
      })};
}

/// \brief Converts E1AP ASN.1 Data Forwarding Request to \c e1ap_data_forwarding_request type.
///
/// Any value other than \c ul and \c dl is converted to \c e1ap_data_forwarding_request::both.
/// \param[in] asn1_data_forwarding_request The E1AP ASN.1 Data Forwarding Request.
/// \return The \c e1ap_data_forwarding_request type.
[[nodiscard]] inline e1ap_data_forwarding_request
e1ap_asn1_to_data_forwarding_request(asn1::e1ap::data_forwarding_request_e asn1_data_forwarding_request)
{
  switch (asn1_data_forwarding_request) {
    case asn1::e1ap::data_forwarding_request_opts::ul:
      return e1ap_data_forwarding_request::ul;
    case asn1::e1ap::data_forwarding_request_opts::dl:
      return e1ap_data_forwarding_request::dl;
    default:
      return e1ap_data_forwarding_request::both;
  }
}

/// \brief Converts \c e1ap_data_forwarding_request type to E1AP ASN.1 Data Forwarding Request.
///
/// Any value other than \c ul and \c dl is converted to \c data_forwarding_request_opts::both.
/// \param[in] data_forwarding_request The data forwarding request.
/// \return The E1AP ASN.1 Data Forwarding Request.
[[nodiscard]] inline asn1::e1ap::data_forwarding_request_e
e1ap_data_forwarding_request_to_asn1(e1ap_data_forwarding_request data_forwarding_request)
{
  switch (data_forwarding_request) {
    case e1ap_data_forwarding_request::ul:
      return asn1::e1ap::data_forwarding_request_opts::ul;
    case e1ap_data_forwarding_request::dl:
      return asn1::e1ap::data_forwarding_request_opts::dl;
    default:
      return asn1::e1ap::data_forwarding_request_opts::both;
  }
}

/// Converts the E1AP ASN.1 QoS flows forwarded on forwarding tunnels to their common representation.
[[nodiscard]] inline slotted_id_vector<qos_flow_id_t, e1ap_qos_flow_map_item>
e1ap_asn1_to_qos_flows_forwarded_on_fwd_tunnels(const asn1::e1ap::qos_flow_map_list_l& asn1_qos_flows_forwarded)
{
  slotted_id_vector<qos_flow_id_t, e1ap_qos_flow_map_item> qos_flows_forwarded;

  for (const auto& asn1_qos_flows_forwarded_item : asn1_qos_flows_forwarded) {
    qos_flows_forwarded.emplace(uint_to_qos_flow_id(asn1_qos_flows_forwarded_item.qos_flow_id),
                                asn1_e1ap_to_qos_flow_map_item(asn1_qos_flows_forwarded_item));
  }

  return qos_flows_forwarded;
}

/// Converts a \c e1ap_qos_flow_map_item to the E1AP ASN.1 QoS flow map item.
[[nodiscard]] inline asn1::e1ap::qos_flow_map_item_s e1ap_qos_flow_map_item_to_asn1(e1ap_qos_flow_map_item item)
{
  auto create_qos_flow_map_ind = [](std::optional<e1ap_qos_flow_map_ind> map_ind) -> asn1::e1ap::qos_flow_map_ind_e {
    if (!map_ind) {
      return {};
    }
    return static_cast<asn1::e1ap::qos_flow_map_ind_opts::options>(static_cast<int>(*map_ind));
  };

  return asn1::e1ap::qos_flow_map_item_s{.qos_flow_map_ind_present = item.qos_flow_map_ind.has_value(),
                                         .ie_exts_present          = false,
                                         .qos_flow_id              = to_underlying(item.qos_flow_id),
                                         .qos_flow_map_ind         = create_qos_flow_map_ind(item.qos_flow_map_ind),
                                         .ie_exts                  = asn1::e1ap::qos_flow_map_item_ext_ies_container{}};
}

/// Converts the QoS flows forwarded on forwarding tunnels to the E1AP ASN.1 QoS flow map list.
[[nodiscard]] inline asn1::e1ap::qos_flow_map_list_l
e1ap_qos_flows_forwarded_to_asn1(const slotted_id_vector<qos_flow_id_t, e1ap_qos_flow_map_item>& qos_flows_forwarded)
{
  asn1::e1ap::qos_flow_map_list_l asn1_qos_flows_forwarded;

  for (const auto& qos_flow_map_item : qos_flows_forwarded) {
    asn1_qos_flows_forwarded.push_back(e1ap_qos_flow_map_item_to_asn1(qos_flow_map_item));
  }

  return asn1_qos_flows_forwarded;
}

/// Converts a \c up_transport_layer_info to the E1AP ASN.1 UP TNL information.
[[nodiscard]] inline asn1::e1ap::up_tnl_info_c up_transport_layer_info_to_e1ap_asn1(const up_transport_layer_info& info)
{
  asn1::e1ap::up_tnl_info_c asn1_up_tnl_info;
  up_transport_layer_info_to_asn1(asn1_up_tnl_info, info);
  return asn1_up_tnl_info;
}

/// Converts a list of QoS flow IDs to the E1AP ASN.1 data forwarding to NG-RAN QoS flow information list.
[[nodiscard]] inline asn1::e1ap::data_forwardingto_ng_ran_qos_flow_info_list_l
qos_flow_ids_to_data_forwarding_asn1(span<const qos_flow_id_t> qos_flow_ids)
{
  asn1::e1ap::data_forwardingto_ng_ran_qos_flow_info_list_l asn1_qos_flows;

  for (qos_flow_id_t qos_flow_id : qos_flow_ids) {
    asn1_qos_flows.push_back(asn1::e1ap::data_forwardingto_ng_ran_qos_flow_info_list_item_s{
        .ie_exts_present = false,
        .qos_flow_id     = to_underlying(qos_flow_id),
        .ie_exts         = asn1::e1ap::data_forwardingto_ng_ran_qos_flow_info_list_item_ext_ies_container{}});
  }

  return asn1_qos_flows;
}

/// Converts a list of QoS flow IDs to the E1AP ASN.1 data forwarding information extension IEs.
[[nodiscard]] inline asn1::e1ap::data_forwarding_info_ext_ies_container
e1ap_data_forwarding_ie_exts_to_asn1(span<const qos_flow_id_t> qos_flow_ids)
{
  return asn1::e1ap::data_forwarding_info_ext_ies_container{
      .data_forwardingto_ng_ran_qos_flow_info_list_present = !qos_flow_ids.empty(),
      .pdu_setbased_handling_ind_present                   = false,
      .data_forwardingto_ng_ran_qos_flow_info_list         = qos_flow_ids_to_data_forwarding_asn1(qos_flow_ids),
      .pdu_setbased_handling_ind                           = asn1::e1ap::pdu_setbased_handling_ind_e{}};
}

/// \brief Converts the data forwarding QoS flow list of the E1AP ASN.1 extension IEs to QoS flow IDs.
///
/// An empty list is returned if the list IE is not present.
[[nodiscard]] inline std::vector<qos_flow_id_t>
e1ap_asn1_to_data_forwarding_qos_flow_ids(const asn1::e1ap::data_forwarding_info_ext_ies_container& ie_exts)
{
  std::vector<qos_flow_id_t> qos_flow_ids;

  if (!ie_exts.data_forwardingto_ng_ran_qos_flow_info_list_present) {
    return qos_flow_ids;
  }

  for (const auto& asn1_qos_flow : ie_exts.data_forwardingto_ng_ran_qos_flow_info_list) {
    qos_flow_ids.push_back(uint_to_qos_flow_id(asn1_qos_flow.qos_flow_id));
  }

  return qos_flow_ids;
}

/// Creates the E1AP ASN.1 data forwarding information with no IE present.
[[nodiscard]] inline asn1::e1ap::data_forwarding_info_s make_empty_data_forwarding_info()
{
  return asn1::e1ap::data_forwarding_info_s{.ul_data_forwarding_present = false,
                                            .dl_data_forwarding_present = false,
                                            .ie_exts_present            = false,
                                            .ul_data_forwarding         = asn1::e1ap::up_tnl_info_c{},
                                            .dl_data_forwarding         = asn1::e1ap::up_tnl_info_c{},
                                            .ie_exts =
                                                e1ap_data_forwarding_ie_exts_to_asn1(span<const qos_flow_id_t>{})};
}

/// \brief Converts the E1AP UL UP transport parameters to the E1AP ASN.1 UP parameters list.
///
/// Note that the cell group ID is not propagated to the ASN.1 object.
[[nodiscard]] inline asn1::e1ap::up_params_l
e1ap_up_params_to_asn1(span<const e1ap_up_params_item> ul_up_transport_params)
{
  asn1::e1ap::up_params_l asn1_up_params;

  for (const auto& ul_up_transport_param : ul_up_transport_params) {
    asn1_up_params.push_back(asn1::e1ap::up_params_item_s{
        .ie_exts_present = false,
        .up_tnl_info     = up_transport_layer_info_to_e1ap_asn1(ul_up_transport_param.up_tnl_info),
        .cell_group_id   = default_cell_group_id,
        .ie_exts         = asn1::e1ap::up_params_item_ext_ies_container{
                    .qos_map_info_present      = false,
                    .indirect_path_ind_present = false,
                    .qos_map_info =
                asn1::e1ap::qos_map_info_s{
                            .dscp_present = false, .flow_label_present = false, .dscp = {}, .flow_label = {}},
                    .indirect_path_ind = asn1::e1ap::indirect_path_ind_e{}}});
  }

  return asn1_up_params;
}

/// Converts the E1AP QoS flow setup list to the E1AP ASN.1 QoS flow list.
[[nodiscard]] inline asn1::e1ap::qos_flow_list_l
e1ap_qos_flow_setup_list_to_asn1(const slotted_id_vector<qos_flow_id_t, e1ap_qos_flow_item>& flow_setup_list)
{
  asn1::e1ap::qos_flow_list_l asn1_flow_list;

  for (const auto& qos_flow_item : flow_setup_list) {
    asn1_flow_list.push_back(
        asn1::e1ap::qos_flow_item_s{.ie_exts_present = false,
                                    .qos_flow_id     = to_underlying(qos_flow_item.qos_flow_id),
                                    .ie_exts         = asn1::e1ap::qos_flow_item_ext_ies_container{
                                                .qos_flow_map_ind_present                            = false,
                                                .data_forwarding_source_ip_address_present           = false,
                                                .ecn_markingor_congestion_info_report_status_present = false,
                                                .qos_flow_map_ind                  = asn1::e1ap::qos_flow_map_ind_e{},
                                                .data_forwarding_source_ip_address = {},
                                                .ecn_markingor_congestion_info_report_status =
                                            asn1::e1ap::ecn_markingor_congestion_info_report_status_e{}}});
  }

  return asn1_flow_list;
}

/// \brief Converts the E1AP QoS flow failed list to the E1AP ASN.1 QoS flow failed list.
///
/// Note that the cause is not propagated to the ASN.1 object.
[[nodiscard]] inline asn1::e1ap::qos_flow_failed_list_l
e1ap_qos_flow_failed_list_to_asn1(const slotted_id_vector<qos_flow_id_t, e1ap_qos_flow_failed_item>& flow_failed_list)
{
  asn1::e1ap::qos_flow_failed_list_l asn1_flow_failed_list;

  for (const auto& flow_failed_item : flow_failed_list) {
    asn1_flow_failed_list.push_back(
        asn1::e1ap::qos_flow_failed_item_s{.ie_exts_present = false,
                                           .qos_flow_id     = to_underlying(flow_failed_item.qos_flow_id),
                                           .cause           = asn1::e1ap::cause_c{},
                                           .ie_exts         = asn1::e1ap::qos_flow_failed_item_ext_ies_container{}});
  }

  return asn1_flow_failed_list;
}

} // namespace ocudu::details
