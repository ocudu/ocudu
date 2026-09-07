// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "bwp_configuration.h" // IWYU pragma: keep
#include "common.h"            // IWYU pragma: keep
#include "lib/scheduler/trace/cell_configuration.h"
#include "lib/scheduler/trace/event_converter.h"
#include "pucch_resource.h" // IWYU pragma: keep
#include "roundtrip_test.h"

namespace ocudu::schedtrace::roundtrip_test {

template <>
struct test_values<static_vector<pucch_resource, pucch_constants::MAX_NOF_TOT_CELL_RESOURCES>> {
  static std::vector<static_vector<pucch_resource, pucch_constants::MAX_NOF_TOT_CELL_RESOURCES>> get()
  {
    const std::vector<pucch_resource> contents = member_sweep_values<pucch_resource>();

    const auto make_list = [&contents](unsigned size) {
      static_vector<pucch_resource, pucch_constants::MAX_NOF_TOT_CELL_RESOURCES> list;
      for (unsigned i = 0; i != size; ++i) {
        pucch_resource res = contents[i % contents.size()];
        res.res_id         = i < pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES
                                 ? pucch_res_id_t::make_cmn(i)
                                 : pucch_res_id_t::make_ded(i - pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES, 0);
        list.push_back(res);
      }
      return list;
    };

    // Empty, common resources only, and a full list with both common and dedicated resources.
    return {make_list(0),
            make_list(pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES),
            make_list(pucch_constants::MAX_NOF_TOT_CELL_RESOURCES)};
  }
};

template <>
struct roundtrip_traits<cell_configuration> {
  static constexpr auto members = std::make_tuple(field("cell_index", &cell_configuration::cell_index),
                                                  field("init_ul_bwp", &cell_configuration::init_ul_bwp),
                                                  field("init_dl_bwp", &cell_configuration::init_dl_bwp),
                                                  field("pucch_resources", &cell_configuration::pucch_resources));

  static cell_configuration roundtrip(const cell_configuration& cfg)
  {
    return roundtrip_via<fbs::CellStartEvent>(
        cfg,
        [](flatbuffers::FlatBufferBuilder& fbb, const cell_configuration& c) { return convert_cell_cfg_to_fb(fbb, c); },
        [](cell_configuration& out, const fbs::CellStartEvent& fb) { convert_fb_to_cell_cfg(out, fb); });
  }
};

} // namespace ocudu::schedtrace::roundtrip_test
