// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/rrm/srs_resource_generator.h"
#include "lib/scheduler/rrm/srs_resource_manager_helpers.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/du/du_cell_config_helpers.h"
#include "ocudu/scheduler/config/serving_cell_config_factory.h"
#include "lib/scheduler/rrm/srs_resource_manager_aperiodic.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

struct srs_params {
  std::optional<tdd_ul_dl_config_common> tdd_cfg = std::nullopt;
  unsigned                               min_k   = 4;
  std::optional<unsigned>                slot_offset;
};

std::ostream& operator<<(std::ostream& out, const srs_params& params)
{
  if (params.tdd_cfg.has_value()) {
    const auto& tdd_p1 = params.tdd_cfg.value().pattern1;
    out << fmt::format("TDD_p1_P{}_D{}_d{}_U_{}_u{}",
                       tdd_p1.dl_ul_tx_period_nof_slots,
                       tdd_p1.nof_dl_slots,
                       tdd_p1.nof_dl_symbols,
                       tdd_p1.nof_ul_slots,
                       tdd_p1.nof_ul_symbols);
    if (params.tdd_cfg.value().pattern2.has_value()) {
      const auto& tdd_p2 = params.tdd_cfg.value().pattern2.value();
      out << fmt::format("_p2_P{}_D{}_d{}_U{}_u{}",
                         tdd_p2.dl_ul_tx_period_nof_slots,
                         tdd_p2.nof_dl_slots,
                         tdd_p2.nof_dl_symbols,
                         tdd_p2.nof_ul_slots,
                         tdd_p2.nof_ul_symbols);
    }
  } else {
    out << fmt::format("FDD");
  }

  out << fmt::format("_min_k_{}", params.min_k);

  if (params.slot_offset.has_value()) {
    out << fmt::format("_sl_offset_{}", params.slot_offset.value());
  }
  return out;
}

/// Class that keeps track of a given number or UE Ids, adds/removes random UEs to/from the repo.
class ue_random_generator
{
public:
  ue_random_generator() : bin(N) {}

  void add_ue_to_repo(unsigned n)
  {
    ocudu_assert(n < N, "Index out-of-bound");
    bin.set(n);
  }

  void remove_ue_from_repo(unsigned n)
  {
    ocudu_assert(n < N, "Index out-of-bound");
    bin.reset(n);
  }

  /// Draw a random UE from those still in the repository and remove it.
  std::optional<unsigned> remove_random_ue()
  {
    if (bin.count() == 0) {
      return std::nullopt;
    }
    auto draw_pos = test_rng::uniform_int<unsigned>(0, bin.count() - 1);
    for (unsigned i = 0; i != N; ++i) {
      if (bin.test(i)) {
        if (draw_pos == 0) {
          bin.reset(i);
          return i;
        }
        --draw_pos;
      }
    }
    report_fatal_error("Reached end of the function without returning a value");
    return std::nullopt;
  }

  /// Pick a random UE not in the repo and it.
  std::optional<unsigned> add_random_ue()
  {
    if (bin.count() == N) {
      return std::nullopt;
    }
    auto readd_pos = test_rng::uniform_int<unsigned>(0, N - bin.count() - 1);
    for (unsigned i = 0; i != N; ++i) {
      if (not bin.test(i)) {
        if (readd_pos == 0) {
          bin.set(i);
          return i;
        }
        --readd_pos;
      }
    }
    report_fatal_error("Reached end of the function without returning a value");
    return std::nullopt;
  }

private:
  static constexpr unsigned N = 1024;
  // Bitset that represents the state of the UEs with ID from 0 to N-1; "true" means that the UE is connected to the DU,
  // "false" means the DU hasn't attached or has been removed from the DU.
  bounded_bitset<N, true> bin;
};

} // namespace

template <>
struct fmt::formatter<srs_params> : ostream_formatter {};

static cell_config_builder_params make_cell_cfg_params(const srs_params& params = {})
{
  const bool                 is_tdd = params.tdd_cfg.has_value();
  cell_config_builder_params cell_params =
      cell_config_builder_profiles::create(is_tdd ? duplex_mode::TDD : duplex_mode::FDD);
  cell_params.tdd_ul_dl_cfg_common = params.tdd_cfg;

  cell_params.min_k1 = params.min_k;
  cell_params.min_k2 = params.min_k;

  return cell_params;
}

static ran_cell_config make_srs_base_cell_config(const cell_config_builder_params& params)
{
  // This function generates a configuration which potentially allows for a very large number of SRS resources.
  ran_cell_config cell_cfg = config_helpers::make_default_du_cell_config(params).ran;
  auto&           srs_cfg  = cell_cfg.init_bwp.srs_cfg;

  srs_cfg.srs_type_enabled = srs_type::aperiodic;

  // Generates a random SRS configuration.
  srs_cfg.tx_comb         = test_rng::bernoulli(0.5) ? tx_comb_size::n2 : tx_comb_size::n4;
  srs_cfg.max_nof_symbols = test_rng::uniform_int<unsigned>(1U, 6U);
  constexpr std::array<srs_nof_symbols, 3> nof_symb_values = {
      srs_nof_symbols::n1, srs_nof_symbols::n2, srs_nof_symbols::n4};
  unsigned max_nof_symbols = srs_cfg.max_nof_symbols.value();
  if (cell_cfg.tdd_cfg.has_value() and cell_cfg.tdd_cfg.value().pattern1.nof_ul_symbols != 0) {
    max_nof_symbols = std::min(cell_cfg.tdd_cfg.value().pattern1.nof_ul_symbols, srs_cfg.max_nof_symbols.value());
  }
  srs_cfg.nof_symbols = nof_symb_values[test_rng::uniform_int<unsigned>(0, nof_symb_values.size() - 1)];
  while (srs_cfg.nof_symbols > max_nof_symbols) {
    srs_cfg.nof_symbols = nof_symb_values[test_rng::uniform_int<unsigned>(0, nof_symb_values.size() - 1)];
  }

  // The TX comb cyclic shift value depends on the TX comb size.
  if (srs_cfg.tx_comb == tx_comb_size::n2) {
    constexpr std::array<nof_cyclic_shifts, 3> srs_cyclic_shift_values = {
        nof_cyclic_shifts::no_cyclic_shift, nof_cyclic_shifts::two, nof_cyclic_shifts::four};
    srs_cfg.cyclic_shift_reuse_factor =
        srs_cyclic_shift_values[test_rng::uniform_int<unsigned>(0, srs_cyclic_shift_values.size() - 1)];
  } else {
    constexpr std::array<nof_cyclic_shifts, 6> srs_cyclic_shift_values = {nof_cyclic_shifts::no_cyclic_shift,
                                                                          nof_cyclic_shifts::two,
                                                                          nof_cyclic_shifts::three,
                                                                          nof_cyclic_shifts::four,
                                                                          nof_cyclic_shifts::six,
                                                                          nof_cyclic_shifts::twelve};
    srs_cfg.cyclic_shift_reuse_factor =
        srs_cyclic_shift_values[test_rng::uniform_int<unsigned>(0, srs_cyclic_shift_values.size() - 1)];
  }
  // [Implementation-defined] These are the values in the gNB, \ref sequence_id_reuse_factor.
  constexpr std::array<unsigned, 8> srs_seq_id_values = {1, 2, 3, 5, 6, 10, 15, 30};
  srs_cfg.sequence_id_reuse_factor =
      srs_seq_id_values[test_rng::uniform_int<unsigned>(0, srs_seq_id_values.size() - 1)];

  srs_cfg.srs_period_prohib_time = srs_periodicity::sl80;

  return cell_cfg;
}

class srs_resource_manager_aperiodic_base_tester
{
protected:
  explicit srs_resource_manager_aperiodic_base_tester(const cell_config_builder_params& params_) :
    params(params_), cell_cfg_list({make_srs_base_cell_config(params_)}), srs_params(cell_cfg_list[0].init_bwp.srs_cfg)
  {
    srs_res_mng.add_cell(to_du_cell_index(0), cell_cfg_list[0]);

    // We only run the test for 1 cell.
    const auto& cell_cfg = cell_cfg_list.front();
    const bool  use_special_slot_only =
        cell_cfg.tdd_cfg.has_value() and (cell_cfg.tdd_cfg.value().pattern1.nof_ul_symbols != 0);
    max_nof_cell_srs_res = generate_cell_srs_list(cell_cfg, use_special_slot_only).size();
  }

  std::optional<ue_cell_config> add_ue(du_ue_index_t ue_idx)
  {
    if (ue_idx >= MAX_NOF_DU_UES) {
      return std::nullopt;
    }

    ues.insert(ue_idx, config_helpers::make_default_ue_cell_config(cell_cfg_list.front()));
    auto& ue = ues[ue_idx];
    ues_bin.add_ue_to_repo(static_cast<unsigned>(ue_idx));

    // Reset the SRS config before allocating resources.
    ue.serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.reset();

    if (srs_res_mng.alloc_resources(ue)) {
      return ue;
    }
    // If the allocation was not possible, remove the UE.
    ues.erase(ue_idx);
    ues_bin.remove_ue_from_repo(static_cast<unsigned>(ue_idx));
    return std::nullopt;
  }

  // Helper that computes the Frequency Shift parameter (see Section 6.4.1.4.3, TS 38.211).
  unsigned compute_freq_shift() const
  {
    // The function computes the frequency shift so that the SRS resources are placed in the center of the band.
    const unsigned ul_bw_nof_rbs        = cell_cfg_list[0].ul_cfg_common.init_ul_bwp.generic_params.crbs.length();
    const std::optional<unsigned> c_srs = srs_res_mng_details::compute_c_srs(ul_bw_nof_rbs);
    ocudu_assert(c_srs.has_value(), "C_SRS is required for this unittest");
    const auto srs_cfg = srs_configuration_get(c_srs.value(), 0U);
    ocudu_assert(srs_cfg.has_value(), "SRS config is required for this unittest");
    return (ul_bw_nof_rbs - srs_cfg.value().m_srs) / 2U;
  }

  cell_config_builder_params  params;
  std::vector<ran_cell_config> cell_cfg_list;
  // This is a reference to the first element of cell_cfg_list's SRS config.
  const srs_builder_params&                     srs_params;
  srs_resource_manager_aperiodic                srs_res_mng;
  unsigned                                      max_nof_cell_srs_res;
  slotted_array<ue_cell_config, MAX_NOF_DU_UES> ues;
  ue_random_generator                           ues_bin;
};

class srs_resource_manager_aperiodic_tester : public srs_resource_manager_aperiodic_base_tester,
                                              public ::testing::TestWithParam<srs_params>
{
protected:
  explicit srs_resource_manager_aperiodic_tester() :
    srs_resource_manager_aperiodic_base_tester(make_cell_cfg_params(GetParam())), res_counter(max_nof_cell_srs_res, 0U)
  {
  }

  static constexpr unsigned MAX_NOF_DU_UES_TO_TEST = 1024;
  // Keeps count of how many UEs have been assigned a given resource ID.
  std::vector<unsigned> res_counter;
};

TEST_P(srs_resource_manager_aperiodic_tester, when_ues_are_added_the_srs_res_with_min_counter_is_assigned)
{
  for (unsigned i = 0; i != MAX_NOF_DU_UES_TO_TEST; ++i) {
    std::optional<ue_cell_config> ue = add_ue(to_du_ue_index(i));
    ASSERT_TRUE(ue.has_value());

    // Check if the SRS has been assigned to the UE.
    const auto& srs_res_list = ue.value().serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg->srs_res_list;
    ASSERT_FALSE(srs_res_list.empty());
    const auto& srs_res = srs_res_list[0];
    ASSERT_LT(srs_res.id.cell_res_id, max_nof_cell_srs_res);
    const unsigned ue_cell_res_id = srs_res.id.cell_res_id;
    // Check that the SRS resource that gets assigned to the new UE is the one with the min counter.
    const auto min_res_cnt_it = std::min_element(res_counter.cbegin(), res_counter.cend());
    ASSERT_NE(min_res_cnt_it, res_counter.cend());
    ASSERT_EQ(res_counter[srs_res.id.cell_res_id], *min_res_cnt_it);
    ++res_counter[ue_cell_res_id];
  }
}

TEST_P(srs_resource_manager_aperiodic_tester, when_all_ues_are_removed_all_srs_resources_have_zero_counter)
{
  // Add all UEs.
  for (unsigned i = 0; i != MAX_NOF_DU_UES_TO_TEST; ++i) {
    std::optional<ue_cell_config> ue = add_ue(to_du_ue_index(i));
    ASSERT_TRUE(ue.has_value());

    // Check if the SRS has been assigned to the UE.
    const auto& srs_res_list = ue.value().serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg->srs_res_list;
    ASSERT_FALSE(srs_res_list.empty());
    const auto& srs_res = srs_res_list[0];
    ASSERT_LT(srs_res.id.cell_res_id, max_nof_cell_srs_res);
    ++res_counter[srs_res.id.cell_res_id];
  }

  // Removes all UEs, chosen in random order.
  for (unsigned i = 0; i != MAX_NOF_DU_UES_TO_TEST; ++i) {
    // Erase a random UE and attempt.
    std::optional<unsigned> ue_to_rem = ues_bin.remove_random_ue();
    ASSERT_TRUE(ue_to_rem.has_value());
    const du_ue_index_t ue_idx_to_rem    = to_du_ue_index(ue_to_rem.value());
    auto&               ue_to_be_removed = ues[ue_idx_to_rem];

    const auto& srs_res_list = ue_to_be_removed.serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg->srs_res_list;
    ASSERT_FALSE(srs_res_list.empty());
    const auto& srs_res = srs_res_list[0];

    ASSERT_LT(srs_res.id.cell_res_id, max_nof_cell_srs_res);
    ASSERT_GT(res_counter[srs_res.id.cell_res_id], 0);
    --res_counter[srs_res.id.cell_res_id];

    srs_res_mng.dealloc_resources(ues[ue_idx_to_rem]);
    ues.erase(ue_idx_to_rem);
  }

  // At the end, we expect all resources to have 0 counter (i.e, 0 UEs assigned).
  ASSERT_TRUE(std::all_of(res_counter.begin(), res_counter.end(), [](const unsigned cnt) { return cnt == 0; }));
}

TEST_P(srs_resource_manager_aperiodic_tester, when_random_ues_are_removed_and_added_new_ues_are_assign_srs_with_min_counter)
{
  // Lambda that adds a new UE, given that its index.
  // NOTE: This is the lambda that verifies the correct SRS resource is assigned to the UE.
  auto add_ues = [this](unsigned ue_idx) mutable {
    std::optional<ue_cell_config> ue = add_ue(to_du_ue_index(ue_idx));
    ASSERT_TRUE(ue.has_value());

    const auto& srs_res_list = ue.value().serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg->srs_res_list;
    ASSERT_FALSE(srs_res_list.empty());
    const auto& srs_res = srs_res_list[0];
    ASSERT_LT(srs_res.id.cell_res_id, max_nof_cell_srs_res);
    // Check that the SRS resource that gets assigned to the new UE is the one with the min counter.
    const auto min_res_cnt_it = std::min_element(res_counter.cbegin(), res_counter.cend());
    ASSERT_NE(min_res_cnt_it, res_counter.cend());
    ASSERT_EQ(res_counter[srs_res.id.cell_res_id], *min_res_cnt_it);
    ++res_counter[srs_res.id.cell_res_id];
  };

  // Lambda that removes a UE, chosen randomly from those in the DU.
  auto remove_ues = [this]() mutable {
    std::optional<unsigned> ue_to_rem = ues_bin.remove_random_ue();
    ASSERT_TRUE(ue_to_rem.has_value());
    const du_ue_index_t ue_idx_to_rem    = to_du_ue_index(ue_to_rem.value());
    auto&               ue_to_be_removed = ues[ue_idx_to_rem];

    const auto& srs_res_list = ue_to_be_removed.serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg->srs_res_list;
    ASSERT_FALSE(srs_res_list.empty());
    const auto& srs_res = srs_res_list[0];

    ASSERT_LT(srs_res.id.cell_res_id, max_nof_cell_srs_res);
    ASSERT_GT(res_counter[srs_res.id.cell_res_id], 0);
    --res_counter[srs_res.id.cell_res_id];

    srs_res_mng.dealloc_resources(ues[ue_idx_to_rem]);
    ues.erase(ue_idx_to_rem);
  };

  // Generate a given number or initial UEs.
  for (unsigned i = 0; i != MAX_NOF_DU_UES_TO_TEST / 2; ++i) {
    add_ues(i);
  }

  // Choose random UEs to be added or removed.
  for (unsigned n = 0; n != 20; ++n) {
    const int nof_ues_to_add_rem = test_rng::uniform_int<int>(-30, 30);
    if (nof_ues_to_add_rem > 0) {
      for (unsigned i = 0; i != static_cast<unsigned>(nof_ues_to_add_rem); ++i) {
        std::optional<unsigned> ue_to_reinsert = ues_bin.add_random_ue();
        if (not ue_to_reinsert.has_value()) {
          continue;
        }
        add_ues(ue_to_reinsert.value());
      }
    } else if (nof_ues_to_add_rem < 0) {
      for (unsigned i = 0; i != static_cast<unsigned>(-nof_ues_to_add_rem); ++i) {
        remove_ues();
      }
    }
  }
}

INSTANTIATE_TEST_SUITE_P(test_srs_res_mng_for_different_ul_symbols,
                         srs_resource_manager_aperiodic_tester,
                         ::testing::Values(
                             // clang-format off
    // FDD.
    srs_params{.tdd_cfg = std::nullopt},
    // TDD.
    srs_params{.tdd_cfg = std::optional<tdd_ul_dl_config_common>({subcarrier_spacing::kHz30,{10, 6, 10, 3, 2}})}

                             // clang-format on
                             ));

class srs_resource_manager_aperiodic_param_tester : public srs_resource_manager_aperiodic_base_tester,
                                                    public ::testing::TestWithParam<srs_params>
{
protected:
  explicit srs_resource_manager_aperiodic_param_tester() :
    srs_resource_manager_aperiodic_base_tester(make_cell_cfg_params(GetParam()))
  {
  }

  static constexpr unsigned MAX_NOF_DU_UES_TO_TEST = 1024;
};

TEST_P(srs_resource_manager_aperiodic_param_tester, when_ue_is_added_srs_resources_parameters_and_slot_offsets_are_valid)
{
  // > Created UEs.
  for (unsigned i = 0; i != MAX_NOF_DU_UES_TO_TEST; ++i) {
    std::optional<ue_cell_config> ue = add_ue(to_du_ue_index(i));
    if (not ue.has_value()) {
      break;
    }

    // Verify all parameters of the SRS resource are as expected.
    ASSERT_TRUE(ue.value().serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.has_value());
    const auto& ue_srs_config = ue.value().serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.value();
    ASSERT_EQ(ue_srs_config.srs_res_set_list.size(), 1U);
    // Check all SRS resource sets.
    const auto srs_res_set = ue_srs_config.srs_res_set_list.front();
    ASSERT_EQ(srs_res_set.id, srs_config::srs_res_set_id::MIN_SRS_RES_SET_ID);
    ASSERT_TRUE(std::holds_alternative<srs_config::srs_resource_set::aperiodic_resource_type>(srs_res_set.res_type));
    const auto& [aperiodic_srs_res_trigger, csi_rs, slot_offset, aperiodic_srs_res_trigger_list] =
        std::get<srs_config::srs_resource_set::aperiodic_resource_type>(srs_res_set.res_type);
    ASSERT_FALSE(csi_rs.has_value());
    ASSERT_TRUE(aperiodic_srs_res_trigger_list.empty());
    ASSERT_EQ(aperiodic_srs_res_trigger, static_cast<unsigned>(srs_config::srs_res_set_id::MIN_SRS_RES_SET_ID) + 1U);
    // Check the slot offsets have been correctly assigned.
    ASSERT_TRUE(GetParam().slot_offset.has_value()) << "Slot offset is not set in the input parameters";
    ASSERT_EQ(slot_offset, GetParam().slot_offset.value());
    ASSERT_EQ(srs_res_set.srs_res_id_list.size(), 1U);
    ASSERT_EQ(srs_res_set.srs_res_id_list[0], 0U);
    ASSERT_EQ(ue_srs_config.srs_res_list.size(), 1U);
    const auto& srs_res = ue_srs_config.srs_res_list[0];
    ASSERT_EQ(srs_res.id.ue_res_id, static_cast<srs_config::srs_res_id>(0U));
    ASSERT_EQ(srs_res.res_type, srs_resource_type::aperiodic);
    ASSERT_LT(srs_res.tx_comb.tx_comb_offset, static_cast<unsigned>(srs_res.tx_comb.size));
    ASSERT_LT(srs_res.tx_comb.tx_comb_cyclic_shift, srs_res.tx_comb.size == tx_comb_size::n2 ? 8U : 12U);

    // Verify that C_SRS corresponds to the maximum allowed BW.
    const std::optional<unsigned> c_srs =
        srs_res_mng_details::compute_c_srs(cell_cfg_list[0].ul_cfg_common.init_ul_bwp.generic_params.crbs.length());
    ASSERT_TRUE(c_srs.has_value());
    ASSERT_EQ(srs_res.freq_hop.c_srs, c_srs.value());
    ASSERT_EQ(srs_res.freq_domain_shift, compute_freq_shift());
    ASSERT_EQ(srs_res.freq_hop.b_srs, 0U);
    ASSERT_EQ(srs_res.freq_hop.b_hop, 0U);
    ASSERT_EQ(srs_res.freq_domain_pos, 0U);

    // Verify the symbols, depending on whether it's FDD, or TDD.
    ASSERT_EQ(srs_res.res_mapping.nof_symb, srs_params.nof_symbols);
    if (cell_cfg_list[0].tdd_cfg.has_value() and cell_cfg_list[0].tdd_cfg.value().pattern1.nof_ul_symbols != 0) {
      ASSERT_LT(srs_res.res_mapping.start_pos, cell_cfg_list[0].tdd_cfg.value().pattern1.nof_ul_symbols);
    } else {
      ASSERT_LT(srs_res.res_mapping.start_pos, srs_params.max_nof_symbols.value());
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
    test_srs_res_mng_for_different_ul_symbols,
    srs_resource_manager_aperiodic_param_tester,
    ::testing::Values(
        // clang-format off
    // FDD
    srs_params{.tdd_cfg = std::nullopt, .slot_offset = 5},
    // TDD with 1 pattern only.
    srs_params{.tdd_cfg = tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{10, 6, 10, 3, 2}}, .slot_offset = 11},
    srs_params{.tdd_cfg = tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{10, 6, 8, 3, 4}}, .slot_offset = 11},
    srs_params{.tdd_cfg = tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{10, 6, 6, 3, 6}}, .slot_offset = 11},
    srs_params{.tdd_cfg = tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{10, 7, 8, 2, 4}}, .slot_offset = 12},
    srs_params{.tdd_cfg = tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{10, 5, 8, 4, 4}}, .slot_offset = 10},
    srs_params{.tdd_cfg = tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{5, 3, 8, 1, 4}}, .slot_offset = 8},
    srs_params{.tdd_cfg = tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{4, 2, 8, 1, 4}}, .slot_offset = 8},
    // TDD without UL symbols in the Special slot.
    srs_params{.tdd_cfg = tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{10, 6, 12, 3, 0}}, .slot_offset = 12},
    srs_params{.tdd_cfg = tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{10, 7, 12, 2, 0}}, .slot_offset = 13},
    // TDD with 2 patterns.
    srs_params{.tdd_cfg = tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{6, 3, 8, 2, 4}, tdd_ul_dl_pattern{4, 1, 8, 2, 4}}, .min_k = 3, .slot_offset = 6},
    srs_params{.tdd_cfg = tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{5, 3, 8, 1, 4}, tdd_ul_dl_pattern{5, 2, 8, 2, 4}}, .min_k = 3, .slot_offset = 6},
    srs_params{.tdd_cfg = tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{6, 3, 8, 2, 4}, tdd_ul_dl_pattern{4, 4, 0, 0, 0}}, .slot_offset = 12},
    // TDD UL-heavy.
    srs_params{.tdd_cfg = tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{10, 3, 6, 6, 4}}, .slot_offset = 10},
    srs_params{.tdd_cfg = tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{10, 3, 6, 6, 4}}, .min_k = 3, .slot_offset = 10},
    srs_params{.tdd_cfg = tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{10, 2, 8, 7, 4}}, .slot_offset = 12},
    srs_params{.tdd_cfg = tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{10, 2, 8, 7, 4}}, .min_k = 3, .slot_offset = 12},
    srs_params{.tdd_cfg = tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{5, 1, 10, 3, 2}}, .min_k = 3, .slot_offset = 10},
    srs_params{.tdd_cfg = tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{6, 2, 10, 3, 4}, tdd_ul_dl_pattern{4, 1, 8, 2, 4}}, .min_k = 2, .slot_offset = 5}

        // clang-format on
        ),
    [](const testing::TestParamInfo<srs_params>& params_item) { return fmt::format("{}", params_item.param); });
