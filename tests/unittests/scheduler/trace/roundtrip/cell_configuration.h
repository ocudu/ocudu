// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "bwp_configuration.h" // IWYU pragma: keep
#include "common.h"            // IWYU pragma: keep
#include "lib/scheduler/trace/cell_configuration.h"
#include "lib/scheduler/trace/event_converter.h"
#include "pdcch.h"          // IWYU pragma: keep
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
        res.res_id         = pucch_res_id_t::from_idx(i);
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
struct test_values<coreset_id> {
  static std::vector<coreset_id> get() { return {to_coreset_id(0), MAX_CORESET_ID}; }
};

template <>
struct test_values<search_space_configuration::monitoring_symbols_within_slot_t> {
  static std::vector<search_space_configuration::monitoring_symbols_within_slot_t> get()
  {
    search_space_configuration::monitoring_symbols_within_slot_t full(NOF_OFDM_SYM_PER_SLOT_NORMAL_CP);
    full.fill();
    search_space_configuration::monitoring_symbols_within_slot_t first_and_last(NOF_OFDM_SYM_PER_SLOT_NORMAL_CP);
    first_and_last.set(0);
    first_and_last.set(NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - 1);
    return {search_space_configuration::monitoring_symbols_within_slot_t(NOF_OFDM_SYM_PER_SLOT_NORMAL_CP),
            first_and_last,
            full};
  }
};

template <>
struct test_values<vrb_to_prb::mapping_type> {
  static std::vector<vrb_to_prb::mapping_type> get()
  {
    return {vrb_to_prb::mapping_type::non_interleaved,
            vrb_to_prb::mapping_type::interleaved_n2,
            vrb_to_prb::mapping_type::interleaved_n4};
  }
};

template <>
struct roundtrip_traits<coreset_configuration::interleaved_mapping_type> {
  using type                    = coreset_configuration::interleaved_mapping_type;
  static constexpr auto members = std::make_tuple(field("reg_bundle_sz", &type::reg_bundle_sz),
                                                  field("interleaver_sz", &type::interleaver_sz),
                                                  field("shift_index", &type::shift_index));
};

template <>
struct roundtrip_traits<cell_coreset> {
  static constexpr auto members = std::make_tuple(field("id", &cell_coreset::id),
                                                  field("crbs", &cell_coreset::crbs),
                                                  field("duration", &cell_coreset::duration),
                                                  field("interleaving", &cell_coreset::interleaving));
};

template <>
struct roundtrip_traits<cell_search_space> {
  static constexpr auto members = std::make_tuple(field("id", &cell_search_space::id),
                                                  field("cs_id", &cell_search_space::cs_id),
                                                  field("is_common", &cell_search_space::is_common),
                                                  field("monitoring_symbols", &cell_search_space::monitoring_symbols));
};

template <>
struct roundtrip_traits<cell_prach_config> {
  static constexpr auto members = std::make_tuple(field("config_index", &cell_prach_config::config_index),
                                                  field("frequency_start", &cell_prach_config::frequency_start),
                                                  field("scs", &cell_prach_config::scs));
};

template <>
struct test_values<cell_prach_config> {
  static std::vector<cell_prach_config> get()
  {
    std::vector<cell_prach_config> values = field_variations_from(cell_prach_config{0, 0, subcarrier_spacing::kHz15});
    // An invalid SCS stands for an SCS derived from the configuration index, which the wire encodes as absent.
    cell_prach_config derived_scs = values.back();
    derived_scs.scs               = subcarrier_spacing::invalid;
    values.push_back(derived_scs);
    return values;
  }
};

template <>
struct roundtrip_traits<dl_bwp_configuration> {
  static constexpr auto members = std::make_tuple(field("generic_params", &dl_bwp_configuration::generic_params),
                                                  field("coresets", &dl_bwp_configuration::coresets),
                                                  field("search_spaces", &dl_bwp_configuration::search_spaces),
                                                  field("pdsch_vrb_to_prb", &dl_bwp_configuration::pdsch_vrb_to_prb));
};

template <>
struct roundtrip_traits<ul_bwp_configuration> {
  static constexpr auto members = std::make_tuple(field("generic_params", &ul_bwp_configuration::generic_params),
                                                  field("pucch_resources", &ul_bwp_configuration::pucch_resources),
                                                  field("prach", &ul_bwp_configuration::prach));
};

template <>
struct roundtrip_traits<cell_configuration> {
  static constexpr auto members = std::make_tuple(field("pci", &cell_configuration::pci),
                                                  field("init_dl_bwp", &cell_configuration::init_dl_bwp),
                                                  field("init_ul_bwp", &cell_configuration::init_ul_bwp));

  static cell_configuration roundtrip(const cell_configuration& cfg)
  {
    return roundtrip_via<fbs::CellStartEvent>(
        cfg,
        [](flatbuffers::FlatBufferBuilder& fbb, const cell_configuration& c) { return convert_cell_cfg_to_fb(fbb, c); },
        [](cell_configuration& out, const fbs::CellStartEvent& fb) { convert_fb_to_cell_cfg(out, fb); });
  }
};

} // namespace ocudu::schedtrace::roundtrip_test
