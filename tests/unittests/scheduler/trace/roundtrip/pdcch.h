// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "common.h" // IWYU pragma: keep
#include "lib/scheduler/trace/event_converter.h"
#include "roundtrip_test.h"
#include "ocudu/ran/pdcch/coreset.h"
#include "ocudu/scheduler/result/pdcch_info.h"

namespace ocudu::schedtrace::roundtrip_test {

/// A coreset_configuration has no formatter and no reflected members, so it would otherwise report as
/// "<unprintable>" on a mismatch. Only the ID is on the wire, so that is all this needs to print.
template <>
struct printer<coreset_configuration> {
  static std::string print(const coreset_configuration& cs)
  {
    return fmt::format("coreset#{}", static_cast<unsigned>(cs.get_id()));
  }
};

/// The wire carries only the coreset ID, which the decoders resolve back to one of these stubs, so pointing at a stub
/// is what makes the pointer compare equal after a roundtrip.
template <>
struct test_values<const coreset_configuration*> {
  static std::vector<const coreset_configuration*> get()
  {
    return {&get_stub_coreset_cfg(1), &get_stub_coreset_cfg(0), &get_stub_coreset_cfg(MAX_CORESET_ID)};
  }
};

template <>
struct test_values<aggregation_level> {
  static std::vector<aggregation_level> get() { return {aggregation_level::n1, aggregation_level::n16}; }
};

template <>
struct test_values<search_space_id> {
  static std::vector<search_space_id> get() { return {MIN_SEARCH_SPACE_ID, MAX_SEARCH_SPACE_ID}; }
};

// --- Allocation context ------------------------------------------------------------------------

template <>
struct roundtrip_traits<cce_position> {
  static constexpr auto members =
      std::make_tuple(field("ncce", &cce_position::ncce), field("aggr_lvl", &cce_position::aggr_lvl));
};

template <>
struct roundtrip_traits<dci_context_information::decision_context> {
  static constexpr auto members = std::make_tuple(field("ss_id", &dci_context_information::decision_context::ss_id));
};

template <>
struct roundtrip_traits<dci_context_information> {
  static constexpr auto members = std::make_tuple(field("coreset_cfg", &dci_context_information::coreset_cfg),
                                                  field("rnti", &dci_context_information::rnti),
                                                  field("cces", &dci_context_information::cces),
                                                  field("context", &dci_context_information::context));
};

// --- DCI payloads ------------------------------------------------------------------------------

template <>
struct roundtrip_traits<dci_1_0_si_rnti_configuration> {
  using dci = dci_1_0_si_rnti_configuration;
  static constexpr auto members =
      std::make_tuple(field("rv", &dci::redundancy_version), field("mcs", &dci::modulation_coding_scheme));
};

template <>
struct roundtrip_traits<dci_1_0_ra_rnti_configuration> {
  static constexpr auto members =
      std::make_tuple(field("mcs", &dci_1_0_ra_rnti_configuration::modulation_coding_scheme));
};

template <>
struct roundtrip_traits<dci_1_0_p_rnti_configuration> {
  static constexpr auto members =
      std::make_tuple(field("mcs", &dci_1_0_p_rnti_configuration::modulation_coding_scheme));
};

template <>
struct roundtrip_traits<dci_1_0_c_rnti_configuration> {
  using dci                     = dci_1_0_c_rnti_configuration;
  static constexpr auto members = std::make_tuple(field("harq_id", &dci::harq_process_number),
                                                  field("ndi", &dci::new_data_indicator),
                                                  field("rv", &dci::redundancy_version),
                                                  field("mcs", &dci::modulation_coding_scheme),
                                                  field("pucch_res_ind", &dci::pucch_resource_indicator));
};

template <>
struct roundtrip_traits<dci_1_0_tc_rnti_configuration> {
  using dci                     = dci_1_0_tc_rnti_configuration;
  static constexpr auto members = std::make_tuple(field("harq_id", &dci::harq_process_number),
                                                  field("ndi", &dci::new_data_indicator),
                                                  field("rv", &dci::redundancy_version),
                                                  field("mcs", &dci::modulation_coding_scheme),
                                                  field("pucch_res_ind", &dci::pucch_resource_indicator));
};

template <>
struct roundtrip_traits<dci_1_1_configuration> {
  using dci                     = dci_1_1_configuration;
  static constexpr auto members = std::make_tuple(field("harq_id", &dci::harq_process_number),
                                                  field("ndi", &dci::tb1_new_data_indicator),
                                                  field("rv", &dci::tb1_redundancy_version),
                                                  field("mcs", &dci::tb1_modulation_coding_scheme),
                                                  field("pucch_res_ind", &dci::pucch_resource_indicator));
};

template <>
struct roundtrip_traits<dci_0_0_tc_rnti_configuration> {
  using dci = dci_0_0_tc_rnti_configuration;
  static constexpr auto members =
      std::make_tuple(field("rv", &dci::redundancy_version), field("mcs", &dci::modulation_coding_scheme));
};

template <>
struct roundtrip_traits<dci_0_0_c_rnti_configuration> {
  using dci                     = dci_0_0_c_rnti_configuration;
  static constexpr auto members = std::make_tuple(field("harq_id", &dci::harq_process_number),
                                                  field("ndi", &dci::new_data_indicator),
                                                  field("rv", &dci::redundancy_version),
                                                  field("mcs", &dci::modulation_coding_scheme));
};

template <>
struct roundtrip_traits<dci_0_1_configuration> {
  using dci                     = dci_0_1_configuration;
  static constexpr auto members = std::make_tuple(field("harq_id", &dci::harq_process_number),
                                                  field("ndi", &dci::new_data_indicator),
                                                  field("rv", &dci::redundancy_version),
                                                  field("mcs", &dci::modulation_coding_scheme));
};

// --- PDCCH -------------------------------------------------------------------------------------

template <>
struct roundtrip_traits<dci_dl_info> {
  static constexpr auto members = std::make_tuple(field("payload", &dci_dl_info::payload));
};

template <>
struct roundtrip_traits<dci_ul_info> {
  static constexpr auto members = std::make_tuple(field("payload", &dci_ul_info::payload));
};

template <>
struct roundtrip_traits<pdcch_dl_information> {
  static constexpr auto members =
      std::make_tuple(field("ctx", &pdcch_dl_information::ctx), field("dci", &pdcch_dl_information::dci));

  static pdcch_dl_information roundtrip(const pdcch_dl_information& pdcch)
  {
    pdcch_dl_information result{};
    convert_fb_to_dl_pdcch(result, convert_dl_pdcch_to_fb(pdcch));
    return result;
  }
};

template <>
struct roundtrip_traits<pdcch_ul_information> {
  static constexpr auto members =
      std::make_tuple(field("ctx", &pdcch_ul_information::ctx), field("dci", &pdcch_ul_information::dci));

  static pdcch_ul_information roundtrip(const pdcch_ul_information& pdcch)
  {
    pdcch_ul_information result{};
    convert_fb_to_ul_pdcch(result, convert_ul_pdcch_to_fb(pdcch));
    return result;
  }
};

} // namespace ocudu::schedtrace::roundtrip_test
