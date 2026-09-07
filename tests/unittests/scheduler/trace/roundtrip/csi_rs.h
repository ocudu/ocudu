// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "common.h" // IWYU pragma: keep
#include "lib/scheduler/trace/event_converter.h"
#include "roundtrip_test.h"
#include "tests/test_doubles/scheduler/trace/test_messages.h"
#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/scheduler/result/csi_rs_info.h"

namespace ocudu::schedtrace::roundtrip_test {

template <>
struct test_values<csi_rs_type> {
  static std::vector<csi_rs_type> get() { return {csi_rs_type::TRS, csi_rs_type::CSI_RS_NZP, csi_rs_type::CSI_RS_ZP}; }
};

template <>
struct test_values<csi_rs_cdm_type> {
  static std::vector<csi_rs_cdm_type> get()
  {
    return {csi_rs_cdm_type::no_CDM,
            csi_rs_cdm_type::fd_CDM2,
            csi_rs_cdm_type::cdm4_FD2_TD2,
            csi_rs_cdm_type::cdm8_FD2_TD4};
  }
};

template <>
struct test_values<csi_rs_freq_density_type> {
  static std::vector<csi_rs_freq_density_type> get()
  {
    return {csi_rs_freq_density_type::dot5_even_RB,
            csi_rs_freq_density_type::dot5_odd_RB,
            csi_rs_freq_density_type::one,
            csi_rs_freq_density_type::three};
  }
};

template <>
struct test_values<bounded_bitset<12, false>> {
  static std::vector<bounded_bitset<12, false>> get()
  {
    bounded_bitset<12, false> full(12);
    full.from_uint64(0xfffU);
    return {bounded_bitset<12, false>(0), full};
  }
};

// bwp_cfg is not swept independently (it has no test_values<const bwp_configuration*>) but is still compared on
// every roundtrip check.
template <>
struct test_values<csi_rs_info> {
  static std::vector<csi_rs_info> get()
  {
    csi_rs_info base{};
    base.bwp_cfg = &test_helper::make_test_schedtrace_cell_cfg().init_dl_bwp;
    return field_variations_from(base);
  }
};

template <>
struct roundtrip_traits<csi_rs_info> {
  static constexpr auto members = std::make_tuple(field("bwp_cfg", &csi_rs_info::bwp_cfg),
                                                  field("crbs", &csi_rs_info::crbs),
                                                  field("type", &csi_rs_info::type),
                                                  field("row", &csi_rs_info::row),
                                                  field("freq_domain", &csi_rs_info::freq_domain),
                                                  field("symbol0", &csi_rs_info::symbol0),
                                                  field("symbol1", &csi_rs_info::symbol1),
                                                  field("cdm_type", &csi_rs_info::cdm_type),
                                                  field("freq_density", &csi_rs_info::freq_density),
                                                  field("scrambling_id", &csi_rs_info::scrambling_id),
                                                  field("power_ctrl_offset", &csi_rs_info::power_ctrl_offset),
                                                  field("power_ctrl_offset_ss", &csi_rs_info::power_ctrl_offset_ss));

  static csi_rs_info roundtrip(const csi_rs_info& csi)
  {
    csi_rs_info               result{};
    const cell_configuration& cell_cfg = test_helper::make_test_schedtrace_cell_cfg();
    convert_fb_to_csi_rs(result, convert_csi_rs_to_fb(csi), &cell_cfg.init_dl_bwp);
    return result;
  }
};

} // namespace ocudu::schedtrace::roundtrip_test
