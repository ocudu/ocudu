// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "common.h" // IWYU pragma: keep
#include "lib/scheduler/trace/event_converter.h"
#include "roundtrip_test.h"
#include "tests/test_doubles/scheduler/trace/test_messages.h"
#include "ocudu/scheduler/result/srs_info.h"

namespace ocudu::schedtrace::roundtrip_test {

template <>
struct test_values<srs_nof_symbols> {
  static std::vector<srs_nof_symbols> get() { return {srs_nof_symbols::n1, srs_nof_symbols::n2, srs_nof_symbols::n4}; }
};

template <>
struct test_values<tx_comb_size> {
  static std::vector<tx_comb_size> get() { return {tx_comb_size::n2, tx_comb_size::n4}; }
};

template <>
struct test_values<srs_group_or_sequence_hopping> {
  static std::vector<srs_group_or_sequence_hopping> get()
  {
    return {srs_group_or_sequence_hopping::neither,
            srs_group_or_sequence_hopping::group_hopping,
            srs_group_or_sequence_hopping::sequence_hopping};
  }
};

template <>
struct test_values<srs_resource_type> {
  static std::vector<srs_resource_type> get()
  {
    return {srs_resource_type::aperiodic, srs_resource_type::semi_persistent, srs_resource_type::periodic};
  }
};

template <>
struct test_values<srs_periodicity> {
  static std::vector<srs_periodicity> get() { return {srs_periodicity::sl1, srs_periodicity::sl2560}; }
};

// bwp_cfg is not swept independently (it has no test_values<const bwp_configuration*>) but is still compared on
// every roundtrip check.
template <>
struct roundtrip_traits<srs_info> {
  static constexpr auto members = std::make_tuple(
      field("bwp_cfg", &srs_info::bwp_cfg),
      field("crnti", &srs_info::crnti),
      field("nof_antenna_ports", &srs_info::nof_antenna_ports),
      field("symbols", &srs_info::symbols),
      field("nof_repetitions", &srs_info::nof_repetitions),
      field("config_index", &srs_info::config_index),
      field("sequence_id", &srs_info::sequence_id),
      field("bw_index", &srs_info::bw_index),
      field("tx_comb", &srs_info::tx_comb),
      field("comb_offset", &srs_info::comb_offset),
      field("cyclic_shift", &srs_info::cyclic_shift),
      field("freq_position", &srs_info::freq_position),
      field("freq_shift", &srs_info::freq_shift),
      field("freq_hopping", &srs_info::freq_hopping),
      field("group_or_seq_hopping", &srs_info::group_or_seq_hopping),
      field("resource_type", &srs_info::resource_type),
      field("t_srs_period", &srs_info::t_srs_period),
      field("t_offset", &srs_info::t_offset),
      field("normalized_channel_iq_matrix_requested", &srs_info::normalized_channel_iq_matrix_requested),
      field("positioning_report_requested", &srs_info::positioning_report_requested));

  static srs_info roundtrip(const srs_info& srs)
  {
    srs_info                  result{};
    const cell_configuration& cell_cfg = test_helper::make_test_schedtrace_cell_cfg();
    convert_fb_to_srs(result, convert_srs_to_fb(srs), &cell_cfg.init_ul_bwp);
    return result;
  }
};

// bwp_cfg is not swept independently (it has no test_values<const bwp_configuration*>) but is still compared on
// every roundtrip check.
template <>
struct test_values<srs_info> {
  static std::vector<srs_info> get()
  {
    srs_info base{};
    base.bwp_cfg = &test_helper::make_test_schedtrace_cell_cfg().init_ul_bwp;
    return field_variations_from(base);
  }
};

} // namespace ocudu::schedtrace::roundtrip_test
