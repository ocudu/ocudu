// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "lib/scheduler/trace/cell_configuration.h"
#include "ocudu/ran/du_cell_index.h"
#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/scheduler/config/pucch_resource_builder_params.h"
#include "ocudu/scheduler/config/pucch_resource_generator.h"
#include "ocudu/scheduler/result/pucch_info.h"

namespace ocudu::schedtrace::test_helper {

/// Creates a test BWP configuration.
inline const bwp_configuration& make_test_bwp_cfg()
{
  static const bwp_configuration bwp_cfg{cyclic_prefix::NORMAL, subcarrier_spacing::kHz30, crb_interval{0, 52}};
  return bwp_cfg;
}

/// Returns a stable cell configuration to use in the tests.
inline const cell_configuration& make_test_schedtrace_cell_cfg()
{
  static const cell_configuration cfg = [] {
    cell_configuration c;
    c.cell_index  = to_du_cell_index(0);
    c.init_dl_bwp = make_test_bwp_cfg();
    c.init_ul_bwp = make_test_bwp_cfg();

    constexpr unsigned pucch_res_common = 11;
    for (const auto& res :
         config_helpers::generate_cell_common_pucch_res_list(pucch_res_common, c.init_ul_bwp.crbs.length())) {
      c.pucch_resources.push_back(res);
    }
    for (const auto& res :
         config_helpers::generate_cell_pucch_res_list(pucch_resource_builder_params{}, c.init_ul_bwp.crbs.length())) {
      c.pucch_resources.push_back(res);
    }
    return c;
  }();
  return cfg;
}

/// Returns the index, within make_test_schedtrace_cell_cfg().pucch_resources, of the first resource with the given
/// format. Returns 0 if none is found.
inline unsigned find_test_pucch_res_idx(pucch_format format)
{
  const auto& res_list = make_test_schedtrace_cell_cfg().pucch_resources;
  for (unsigned i = 0; i != res_list.size(); ++i) {
    if (res_list[i].format() == format) {
      return i;
    }
  }
  return 0;
}

/// Creates a PUCCH allocation pointing to the given cell PUCCH resource, with the given UCI bits.
/// The caller may override individual fields after construction.
inline pucch_info make_test_pucch(rnti_t rnti, const pucch_resource& res, pucch_uci_bits uci = {})
{
  pucch_info alloc{};
  alloc.crnti    = rnti;
  alloc.bwp_cfg  = &make_test_bwp_cfg();
  alloc.res      = &res;
  alloc.uci_bits = uci;
  alloc.set_format(res.format());
  if (auto* f2 = std::get_if<pucch_info::f2_config>(&alloc.format_params)) {
    f2->nof_prbs = std::get<pucch_resource::f2_config>(res.format_params).nof_prbs;
  } else if (auto* f3 = std::get_if<pucch_info::f3_config>(&alloc.format_params)) {
    f3->nof_prbs = std::get<pucch_resource::f3_config>(res.format_params).nof_prbs;
  }
  return alloc;
}

} // namespace ocudu::schedtrace::test_helper
