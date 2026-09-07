// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "common.h" // IWYU pragma: keep
#include "lib/scheduler/trace/event_converter.h"
#include "roundtrip_test.h"
#include "ocudu/ran/pucch/pucch_configuration.h"

namespace ocudu::schedtrace::roundtrip_test {

template <>
struct test_values<pucch_res_id_t> {
  static std::vector<pucch_res_id_t> get()
  {
    return {pucch_res_id_t::make_ded(0, 0),
            pucch_res_id_t::make_ded(255, 255),
            pucch_res_id_t::make_cmn(0),
            pucch_res_id_t::make_cmn(15)};
  }
};

template <>
struct roundtrip_traits<pucch_resource::f0_config> {
  static constexpr auto members =
      std::make_tuple(field("initial_cyclic_shift", &pucch_resource::f0_config::initial_cyclic_shift));
};

template <>
struct roundtrip_traits<pucch_resource::f1_config> {
  static constexpr auto members =
      std::make_tuple(field("initial_cyclic_shift", &pucch_resource::f1_config::initial_cyclic_shift),
                      field("time_domain_occ", &pucch_resource::f1_config::time_domain_occ));
};

template <>
struct roundtrip_traits<pucch_resource::f2_config> {
  static constexpr auto members = std::make_tuple(field("nof_prbs", &pucch_resource::f2_config::nof_prbs));
};

template <>
struct roundtrip_traits<pucch_resource::f3_config> {
  static constexpr auto members =
      std::make_tuple(field("nof_prbs", &pucch_resource::f3_config::nof_prbs),
                      field("pi_2_bpsk", &pucch_resource::f3_config::pi_2_bpsk),
                      field("additional_dmrs", &pucch_resource::f3_config::additional_dmrs));
};

template <>
struct test_values<pucch_f4_occ_idx> {
  static std::vector<pucch_f4_occ_idx> get() { return {pucch_f4_occ_idx::n0, pucch_f4_occ_idx::n3}; }
};

template <>
struct test_values<pucch_f4_occ_len> {
  static std::vector<pucch_f4_occ_len> get() { return {pucch_f4_occ_len::n2, pucch_f4_occ_len::n4}; }
};

template <>
struct roundtrip_traits<pucch_resource::f4_config> {
  static constexpr auto members =
      std::make_tuple(field("occ_index", &pucch_resource::f4_config::occ_index),
                      field("occ_length", &pucch_resource::f4_config::occ_length),
                      field("pi_2_bpsk", &pucch_resource::f4_config::pi_2_bpsk),
                      field("additional_dmrs", &pucch_resource::f4_config::additional_dmrs));
};

template <>
struct test_values<pucch_repetition_factor> {
  static std::vector<pucch_repetition_factor> get()
  {
    return {pucch_repetition_factor::n1, pucch_repetition_factor::n8};
  }
};

template <>
struct roundtrip_traits<pucch_resource> {
  static constexpr auto members = std::make_tuple(field("res_id", &pucch_resource::res_id),
                                                  field("starting_prb", &pucch_resource::starting_prb),
                                                  field("syms", &pucch_resource::syms),
                                                  field("second_hop_prb", &pucch_resource::second_hop_prb),
                                                  field("format_params", &pucch_resource::format_params),
                                                  field("rep_factor", &pucch_resource::rep_factor));

  static pucch_resource roundtrip(const pucch_resource& res)
  {
    return roundtrip_via<fbs::PucchResource>(
        res, convert_pucch_resource_to_fb, [&res](pucch_resource& result, const fbs::PucchResource& fb_res) {
          convert_fb_to_pucch_resource(result, fb_res, res.res_id);
        });
  }
};

} // namespace ocudu::schedtrace::roundtrip_test
