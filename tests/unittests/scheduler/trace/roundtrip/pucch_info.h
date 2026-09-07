// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "common.h"
#include "lib/scheduler/trace/event_converter.h"
#include "roundtrip_test.h"
#include "tests/test_doubles/scheduler/trace/test_messages.h"
#include "ocudu/scheduler/result/pucch_info.h"
#include <gtest/gtest.h>

namespace ocudu::schedtrace::roundtrip_test {

template <>
struct test_values<sr_nof_bits> {
  static std::vector<sr_nof_bits> get()
  {
    return {sr_nof_bits::no_sr, sr_nof_bits::one, sr_nof_bits::two, sr_nof_bits::three, sr_nof_bits::four};
  }
};

template <>
struct roundtrip_traits<pucch_uci_bits> {
  static constexpr auto members = std::make_tuple(field("harq_ack_nof_bits", &pucch_uci_bits::harq_ack_nof_bits),
                                                  field("sr_bits", &pucch_uci_bits::sr_bits),
                                                  field("csi_part1_nof_bits", &pucch_uci_bits::csi_part1_nof_bits));
};

template <>
struct test_values<pucch_group_hopping> {
  static std::vector<pucch_group_hopping> get()
  {
    return {pucch_group_hopping::NEITHER, pucch_group_hopping::ENABLE, pucch_group_hopping::DISABLE};
  }
};

template <>
struct test_values<pucch_repetition_tx_slot> {
  static std::vector<pucch_repetition_tx_slot> get()
  {
    return {pucch_repetition_tx_slot::no_multi_slot,
            pucch_repetition_tx_slot::starts,
            pucch_repetition_tx_slot::continues,
            pucch_repetition_tx_slot::ends};
  }
};

template <>
struct roundtrip_traits<pucch_info::f0_config> {
  static constexpr auto members = std::make_tuple(field("group_hopping", &pucch_info::f0_config::group_hopping),
                                                  field("n_id_hopping", &pucch_info::f0_config::n_id_hopping));
};

template <>
struct roundtrip_traits<pucch_info::f1_config> {
  static constexpr auto members = std::make_tuple(field("group_hopping", &pucch_info::f1_config::group_hopping),
                                                  field("n_id_hopping", &pucch_info::f1_config::n_id_hopping));
};

template <>
struct roundtrip_traits<pucch_info::f2_config> {
  static constexpr auto members = std::make_tuple(field("n_id_scrambling", &pucch_info::f2_config::n_id_scrambling),
                                                  field("n_id_0_scrambling", &pucch_info::f2_config::n_id_0_scrambling),
                                                  field("nof_prbs", &pucch_info::f2_config::nof_prbs));
};

template <>
struct roundtrip_traits<pucch_info::f3_config> {
  static constexpr auto members = std::make_tuple(field("group_hopping", &pucch_info::f3_config::group_hopping),
                                                  field("n_id_hopping", &pucch_info::f3_config::n_id_hopping),
                                                  field("n_id_scrambling", &pucch_info::f3_config::n_id_scrambling),
                                                  field("n_id_0_scrambling", &pucch_info::f3_config::n_id_0_scrambling),
                                                  field("nof_prbs", &pucch_info::f3_config::nof_prbs));
};

template <>
struct roundtrip_traits<pucch_info::f4_config> {
  static constexpr auto members =
      std::make_tuple(field("group_hopping", &pucch_info::f4_config::group_hopping),
                      field("n_id_hopping", &pucch_info::f4_config::n_id_hopping),
                      field("n_id_scrambling", &pucch_info::f4_config::n_id_scrambling),
                      field("n_id_0_scrambling", &pucch_info::f4_config::n_id_0_scrambling));
};

/// The anchor_slot's numerology must match the cell's UL BWP scs, since convert_fb_to_pucch_info reconstructs it
/// from the wire's raw slot count using that BWP's scs (see pucch_info::repetition_info).
template <>
struct test_values<pucch_info::repetition_info> {
  static std::vector<pucch_info::repetition_info> get()
  {
    const subcarrier_spacing scs = test_helper::make_test_bwp_cfg().scs;
    return {pucch_info::repetition_info{slot_point(scs, 0), pucch_repetition_tx_slot::starts},
            pucch_info::repetition_info{slot_point(scs, 1), pucch_repetition_tx_slot::continues},
            pucch_info::repetition_info{slot_point(scs, 2), pucch_repetition_tx_slot::ends}};
  }
};

template <>
struct roundtrip_traits<pucch_info::repetition_info> {
  static constexpr auto members = std::make_tuple(field("anchor_slot", &pucch_info::repetition_info::anchor_slot),
                                                  field("position", &pucch_info::repetition_info::position));
};

template <>
struct test_values<const pucch_resource*> {
  static std::vector<const pucch_resource*> get()
  {
    const auto& res_list = test_helper::make_test_schedtrace_cell_cfg().pucch_resources;
    // First and last common and dedicated resources.
    return {&res_list.front(),
            &res_list[pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES - 1],
            &res_list[pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES],
            &res_list.back()};
  }
};

template <>
struct test_values<pucch_info> {
  static std::vector<pucch_info> get()
  {
    return field_variations_from(test_helper::make_test_pucch(test_values<rnti_t>::get()[0],
                                                              *test_values<const pucch_resource*>::get()[0],
                                                              test_values<pucch_uci_bits>::get()[0]));
  }
};

template <>
struct roundtrip_traits<pucch_info> {
  static constexpr auto members = std::make_tuple(field("crnti", &pucch_info::crnti),
                                                  field("bwp_cfg", &pucch_info::bwp_cfg),
                                                  field("res", &pucch_info::res),
                                                  field("format_params", &pucch_info::format_params),
                                                  field("uci_bits", &pucch_info::uci_bits),
                                                  field("repetition", &pucch_info::repetition));

  static pucch_info roundtrip(const pucch_info& info)
  {
    return roundtrip_via<fbs::Pucch>(
        info,
        [](flatbuffers::FlatBufferBuilder& fbb, const pucch_info& pucch) {
          return convert_pucch_info_to_fb(fbb, pucch);
        },
        [](pucch_info& out, const fbs::Pucch& fb) {
          const cell_configuration& cell_cfg = test_helper::make_test_schedtrace_cell_cfg();
          convert_fb_to_pucch_info(out, fb, &cell_cfg.init_ul_bwp, cell_cfg.pucch_resources);
        });
  }
};

} // namespace ocudu::schedtrace::roundtrip_test
