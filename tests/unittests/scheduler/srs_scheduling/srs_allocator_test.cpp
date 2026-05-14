// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../test_utils/config_generators.h"
#include "lib/scheduler/cell/resource_grid.h"
#include "lib/scheduler/srs/srs_allocator_impl.h"
#include "lib/scheduler/srs/srs_scheduler_impl.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "ocudu/ran/srs/srs_bandwidth_configuration.h"
#include "ocudu/scheduler/config/scheduler_expert_config_factory.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

using namespace ocudu;

static unsigned compute_c_srs(unsigned nof_ul_crbs)
{
  // In this test, we only consider the case where B_SRS is 0.
  constexpr uint8_t b_srs           = 0U;
  unsigned          candidate_c_srs = 0U;
  unsigned          candidate_m_srs = 0U;
  // Spans over Table 6.4.1.4.3-1 in TS 38.211 and find the smallest C_SRS that maximizes m_srs_0 under the
  // constraint of m_SRS <= nof_BW_RBs.
  for (unsigned c_srs_it = 0; c_srs_it != 64; ++c_srs_it) {
    std::optional<srs_configuration> srs_cfg = srs_configuration_get(c_srs_it, b_srs);
    ocudu_assert(srs_cfg.has_value(), "C_SRS is required for this unittest");
    if (srs_cfg.value().m_srs <= nof_ul_crbs and srs_cfg.value().m_srs > candidate_m_srs) {
      candidate_m_srs = srs_cfg->m_srs;
      candidate_c_srs = c_srs_it;
    }
  }
  return candidate_c_srs;
}

static void validate_default_ue_cfg_req(const sched_ue_creation_request_message& ue_req)
{
  ocudu_assert(ue_req.cfg.cells.value().front().serv_cell_cfg.ul_config.has_value(),
               "UL config is required for this test");
  ocudu_assert(ue_req.cfg.cells.value().front().serv_cell_cfg.ul_config.value().init_ul_bwp.srs_cfg.has_value(),
               "SRS config is required for this test");
  const auto& srs_cfg = ue_req.cfg.cells.value().front().serv_cell_cfg.ul_config.value().init_ul_bwp.srs_cfg.value();
  ocudu_assert(srs_cfg.srs_res_set_list.size() == 1, "SRS Resource Set is expected to have size 1");
  const auto& srs_set = srs_cfg.srs_res_set_list.front();
  ocudu_assert(srs_set.srs_res_id_list.size() == 1 and
                   srs_set.srs_res_id_list.front() == srs_cfg.srs_res_list.front().id.ue_res_id,
               "SRS Resource Set is expected to have size 1");
}

namespace {

struct srs_params {
  bool is_tdd;
  // The slot offsets depend on TDD/FDD, but they are passed as a parameter to allow future extensions. In this test,
  // they are used to build the SRS config for the different TDD/FDD, or future different TDD patterns.
  unsigned        slot_offset;
  srs_periodicity srs_prohib_time;
  unsigned        nof_ues_per_test = 1;
};

std::ostream& operator<<(std::ostream& os, const srs_params& params)
{
  // Make sure there are no spaces nor colons in this string, otherwise Gtest complains about the test name being
  // invalid.
  if (params.is_tdd) {
    os << "TDD_";
  } else {
    os << "FDD_";
  }
  // No need to print the slot offsets, as they depend on TDD/FDD.
  os << "srs_prohib_time_" << fmt::format("{}", fmt::underlying(params.srs_prohib_time)) << "_nof_ues_"
     << params.nof_ues_per_test;

  return os;
}

} // namespace

template <>
struct fmt::formatter<srs_params> : ostream_formatter {};

class srs_alloc_test_bench
{
public:
  srs_alloc_test_bench(const srs_params& params) :
    expert_cfg{[](srs_periodicity srs_prohib_time) {
      auto cfg                 = config_helpers::make_default_scheduler_expert_config();
      cfg.ue.srs_prohibit_time = srs_prohib_time;
      return cfg;
    }(params.srs_prohib_time)},
    cfg_mng{cell_config_builder_profiles::create(params.is_tdd ? duplex_mode::TDD : duplex_mode::FDD), expert_cfg},
    cell_cfg(*cfg_mng.add_cell(cfg_mng.get_default_cell_config_request())),
    ues(expert_cfg.ue),
    cell_ues(ues.add_cell(cell_cfg, nullptr)),
    srs_alloc(cell_cfg, expert_cfg.ue.srs_prohibit_time.value()),
    current_sl_tx{to_numerology_value(cell_cfg.params.dl_cfg_common.init_dl_bwp.generic_params.scs), 0}
  {
    slot_indication(current_sl_tx);
    mac_logger.set_level(ocudulog::basic_levels::debug);

    ocudulog::init();
  }

  // Class members.
  scheduler_expert_config                 expert_cfg;
  test_helpers::test_sched_config_manager cfg_mng;
  const cell_configuration&               cell_cfg;
  ue_repository                           ues;
  ue_cell_repository&                     cell_ues;
  std::vector<const ue_configuration*>    ue_ded_cfgs;
  cell_resource_allocator                 res_grid{cell_cfg};
  srs_allocator_impl                      srs_alloc;
  slot_point                              current_sl_tx;
  du_ue_index_t                           next_du_ue_idx  = to_du_ue_index(0U);
  rnti_t                                  next_rnti       = to_rnti(0x4601);
  unsigned                                next_srs_res_id = 0;
  // Arbitrary number to test SRS allocation collisions.
  static constexpr unsigned max_cell_srs_res = 11;

  ocudulog::basic_logger& mac_logger  = ocudulog::fetch_basic_logger("SCHED", true);
  ocudulog::basic_logger& test_logger = ocudulog::fetch_basic_logger("TEST");

  // Class methods.
  du_ue_index_t add_ue(const srs_params& params)
  {
    sched_ue_creation_request_message ue_req = create_sched_ue_creation_request_for_srs_cfg(
        cfg_mng.get_default_ue_config_request(),
        cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.crbs.length(),
        params.slot_offset);
    ue_req.ue_index = next_du_ue_idx;
    ue_req.crnti    = next_rnti;
    next_du_ue_idx  = to_du_ue_index(static_cast<unsigned>(next_du_ue_idx) + 1);
    next_rnti       = to_rnti(static_cast<unsigned>(next_rnti) + 1);
    ue_ded_cfgs.emplace_back(cfg_mng.add_ue(ue_req));
    test_logger.info("Checking ue rnti={}", ue_req.crnti);
    report_error_if_not(ue_ded_cfgs.back() != nullptr, "Failed to create UE configuration");
    ues.add_ue(*ue_ded_cfgs.back(), ue_req.starts_in_fallback, ue_req.ul_ccch_slot_rx, ue_req.cfra_enabled);
    return ue_req.ue_index;
  }

  void slot_indication(slot_point slot_tx)
  {
    mac_logger.set_context(slot_tx.sfn(), slot_tx.slot_index());
    test_logger.set_context(slot_tx.sfn(), slot_tx.slot_index());
    res_grid.slot_indication(slot_tx);
    srs_alloc.slot_indication(slot_tx);
  }

  expected<bool, std::string> test_srs_pdu(const srs_info& srs_pdu, du_ue_index_t du_ue_idx) const
  {
    if (not ues.contains(du_ue_idx)) {
      return make_unexpected(std::string("DU UE index not found in UEs repo"));
    }

    const auto& srs_cfg     = ues[du_ue_idx].get_pcell().cfg().init_bwp().ul.ded()->srs_cfg.value();
    const auto& srs_res_cfg = srs_cfg.srs_res_list.front();

    if (srs_pdu.crnti != ues[du_ue_idx].crnti) {
      return make_unexpected(std::string("RNTI mismatch"));
    }
    if (srs_pdu.bwp_cfg != &cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params) {
      return make_unexpected(std::string("BWP mismatch"));
    }
    if (srs_pdu.nof_antenna_ports != cell_cfg.params.ul_carrier.nof_ant) {
      return make_unexpected(std::string("Nof antenna ports mismatch"));
    }
    if (srs_pdu.symbols != ofdm_symbol_range{NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - 1 - srs_res_cfg.res_mapping.start_pos,
                                             NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - 1 - srs_res_cfg.res_mapping.start_pos +
                                                 static_cast<uint8_t>(srs_res_cfg.res_mapping.nof_symb)}) {
      return make_unexpected(std::string("Symbols mismatch"));
    }
    if (srs_pdu.nof_repetitions != srs_res_cfg.res_mapping.rept_factor) {
      return make_unexpected(std::string("Repetition factor mismatch"));
    }
    if (srs_pdu.config_index != srs_res_cfg.freq_hop.c_srs) {
      return make_unexpected(std::string("Frequency domain shift mismatch"));
    }
    if (srs_pdu.sequence_id != static_cast<unsigned>(srs_res_cfg.sequence_id)) {
      return make_unexpected(std::string("Sequence ID mismatch"));
    }
    if (srs_pdu.bw_index != srs_res_cfg.freq_hop.b_srs) {
      return make_unexpected(std::string("B_srs mismatch"));
    }
    if (srs_pdu.tx_comb != srs_res_cfg.tx_comb.size) {
      return make_unexpected(std::string("TX comb size mismatch"));
    }
    if (srs_pdu.comb_offset != srs_res_cfg.tx_comb.tx_comb_offset) {
      return make_unexpected(std::string("TX comb offset mismatch"));
    }
    if (srs_pdu.cyclic_shift != srs_res_cfg.tx_comb.tx_comb_cyclic_shift) {
      return make_unexpected(std::string("TX comb cyclic shift mismatch"));
    }
    if (srs_pdu.freq_position != srs_res_cfg.freq_domain_pos) {
      return make_unexpected(std::string("Freq. domain position mismatch"));
    }
    if (srs_pdu.freq_shift != static_cast<unsigned>(srs_res_cfg.freq_domain_shift)) {
      return make_unexpected(std::string("Freq. domain position mismatch"));
    }
    if (srs_pdu.freq_hopping != srs_res_cfg.freq_hop.b_hop) {
      return make_unexpected(std::string("b_hop mismatch"));
    }
    if (srs_pdu.group_or_seq_hopping != srs_res_cfg.grp_or_seq_hop) {
      return make_unexpected(std::string("group_or_seq_hopping mismatch"));
    }
    if (srs_pdu.resource_type != srs_res_cfg.res_type) {
      return make_unexpected(std::string("group_or_seq_hopping mismatch"));
    }

    return true;
  }

  sched_ue_creation_request_message
  create_sched_ue_creation_request_for_srs_cfg(const sched_ue_creation_request_message& base_ue_req,
                                               unsigned                                 nof_ul_crbs,
                                               unsigned                                 slot_offset)
  {
    validate_default_ue_cfg_req(base_ue_req);

    sched_ue_creation_request_message ue_req = base_ue_req;

    // Set SRS resource aperiodic.
    srs_config::srs_resource& srs_res = ue_req.cfg.cells.value()
                                            .front()
                                            .serv_cell_cfg.ul_config.value()
                                            .init_ul_bwp.srs_cfg.value()
                                            .srs_res_list.front();
    srs_res.res_type = srs_resource_type::aperiodic;

    // The TX comb size is not used in this test to check collision among SRS resources, but only to check that the
    // parameters are correctly passed to the SRS PDU.
    srs_res.tx_comb.size                 = tx_comb_size::n4;
    srs_res.tx_comb.tx_comb_offset       = 0U;
    srs_res.tx_comb.tx_comb_cyclic_shift = 0U;

    // Set SRS in symbols [13, 14).
    srs_res.res_mapping.start_pos   = 0U;
    srs_res.res_mapping.nof_symb    = n1;
    srs_res.res_mapping.rept_factor = n1;

    // Frequency hopping is disabled.
    srs_res.freq_domain_pos = 0U;
    srs_res.freq_hop.b_hop  = 0U;
    srs_res.freq_hop.b_srs  = 0U;
    srs_res.freq_hop.c_srs  = compute_c_srs(nof_ul_crbs);

    // SRS placed in the middle of the BW.
    std::optional<srs_configuration> srs_bw_cfg = srs_configuration_get(srs_res.freq_hop.c_srs, srs_res.freq_hop.b_srs);
    ocudu_assert(srs_bw_cfg.has_value(), "C_SRS is required for this unittest");
    srs_res.freq_domain_shift = (nof_ul_crbs - srs_bw_cfg->m_srs) / 2U;

    // Select a non-zero sequence identifier that requires more than 8 bits.
    srs_res.sequence_id = 667U;

    srs_res.id.cell_res_id = next_srs_res_id;
    next_srs_res_id        = (next_srs_res_id + 1) % max_cell_srs_res;

    auto& srs_set_cfg = ue_req.cfg.cells.value()
                            .front()
                            .serv_cell_cfg.ul_config.value()
                            .init_ul_bwp.srs_cfg.value()
                            .srs_res_set_list.front();
    srs_set_cfg.id      = srs_config::srs_res_set_id::MIN_SRS_RES_SET_ID;
    auto& aperiodic_set = std::get<srs_config::srs_resource_set::aperiodic_resource_type>(srs_set_cfg.res_type);
    aperiodic_set.aperiodic_srs_res_trigger = static_cast<unsigned>(srs_config::srs_res_set_id::MIN_SRS_RES_SET_ID) + 1;
    aperiodic_set.slot_offset.emplace(slot_offset);

    return ue_req;
  }
};

class srs_alloc_tester : public ::testing::TestWithParam<srs_params>, public srs_alloc_test_bench
{
protected:
  srs_alloc_tester() : srs_alloc_test_bench(GetParam()) {}

  // Keep track of the last slot when the SRS was allocated.
  slot_point last_srs_slot;

  // This is to check that the scheduler allocates the SRS at least once; without this, the test would pass if the
  // scheduler never allocates anything.
  bool at_least_one_alloc = false;
  // With 1 UE only, the allocation is periodic. After the first SRS allocation, we check that the next one will be the
  // expected slot.
  std::optional<slot_point> expected_srs_slot;
};

TEST_P(srs_alloc_tester, with_only_1_ue_srs_is_allocated_every_time_prohibit_time)
{
  const auto srs_prohib_time_uint = static_cast<unsigned>(GetParam().srs_prohib_time);

  const auto add_ue_slot = test_rng::uniform_int<unsigned>(0, res_grid.ring_size());
  // Check at the allocation for at least 4 the size of the resource grid.
  const unsigned nof_slots_to_test = add_ue_slot + std::max(srs_prohib_time_uint * 4, res_grid.ring_size() * 4);

  for (unsigned sl_cnt = 0; sl_cnt < nof_slots_to_test; ++sl_cnt, slot_indication(++current_sl_tx)) {
    // Add the UE at the specified random slot.
    if (sl_cnt == add_ue_slot) {
      add_ue(GetParam());
    }

    if (not ues.empty()) {
      const auto& u = ues[to_du_ue_index(0U)];
      if (not cell_cfg.is_dl_enabled(current_sl_tx)) {
        continue;
      }
      // Attempt aperiodic SRS collision.
      const aperiodic_srs_alloc_info res =
          srs_alloc.allocate_aperiodic_srs(res_grid, last_srs_slot, u.get_pcell().cfg());
      // When the SRS is allocated, the aperiodic SRS trigger is different from 0.
      if (res.aperiodic_srs_res_trigger != 0) {
        if (last_srs_slot.valid()) {
          ASSERT_GE(current_sl_tx + res.slot_offset - last_srs_slot, srs_prohib_time_uint)
              << fmt::format("Expected SRS shouldn't be allocated more often that the prohibit time");
        }
        last_srs_slot = current_sl_tx + res.slot_offset;
        at_least_one_alloc |= true;
        expected_srs_slot = last_srs_slot + srs_prohib_time_uint;
        ASSERT_EQ(1, res_grid[res.slot_offset].result.ul.srss.size());
        expected<bool, std::string> pdu_test =
            test_srs_pdu(res_grid[res.slot_offset].result.ul.srss.front(), u.ue_index);
        ASSERT_TRUE(pdu_test.has_value())
            << fmt::format("UE did not pass the SRS PDU check. Cause: {}", pdu_test.error());
      }

      // With 1 UE only, the aperiodic SRS allocation is expected periodically.
      if (expected_srs_slot.has_value() and current_sl_tx == expected_srs_slot) {
        ASSERT_EQ(1, res_grid[0].result.ul.srss.size()) << fmt::format("Expected SRS PDU at slot {}", current_sl_tx);
      }
    }
  }

  // Given that all the test check are performed when the SRS is allocated, ensure there is at least 1 allocation
  // throughout the test.
  ASSERT_TRUE(at_least_one_alloc);
}

INSTANTIATE_TEST_SUITE_P(
    test_srs_alloc,
    srs_alloc_tester,
    testing::Values(srs_params{.is_tdd = false, .slot_offset = 5, .srs_prohib_time = srs_periodicity::sl40},
                    srs_params{.is_tdd = false, .slot_offset = 5, .srs_prohib_time = srs_periodicity::sl80},
                    srs_params{.is_tdd = false, .slot_offset = 5, .srs_prohib_time = srs_periodicity::sl160},
                    srs_params{.is_tdd = true, .slot_offset = 11, .srs_prohib_time = srs_periodicity::sl40},
                    srs_params{.is_tdd = true, .slot_offset = 11, .srs_prohib_time = srs_periodicity::sl80},
                    srs_params{.is_tdd = true, .slot_offset = 11, .srs_prohib_time = srs_periodicity::sl160}),
    [](const testing::TestParamInfo<srs_params>& params_item) { return fmt::format("{}", params_item.param); });

class srs_alloc_multi_ue_tester : public ::testing::TestWithParam<srs_params>, public srs_alloc_test_bench
{
protected:
  srs_alloc_multi_ue_tester() : srs_alloc_test_bench(GetParam()) {}

  static unsigned get_ue_srs_cell_id(const ue& u)
  {
    ocudu_assert(u.get_pcell().cfg().init_bwp().ul.ded()->srs_cfg.has_value() and
                     u.get_pcell().cfg().init_bwp().ul.ded()->srs_cfg.value().srs_res_list.size() == 1,
                 "UE configuration is wrong");
    return u.get_pcell().cfg().init_bwp().ul.ded()->srs_cfg.value().srs_res_list.front().id.cell_res_id;
  }

  struct ue_tracker {
    slot_point last_srs_slot;
    // This is to check that the scheduler allocates the SRS at least once; without this, the test would pass if the
    // scheduler never allocates anything.
    bool          at_least_one_alloc = false;
    du_ue_index_t du_ue_idx          = INVALID_DU_UE_INDEX;
  };

  std::vector<ue_tracker>                test_ues;
  std::array<unsigned, max_cell_srs_res> cell_srs_res_usage;
};

TEST_P(srs_alloc_multi_ue_tester, multiple_ues_with_orthogonal_srs_res_is_allocated_at_most_every_time_prohibit_time)
{
  const auto srs_prohib_time_uint = static_cast<unsigned>(GetParam().srs_prohib_time);

  auto get_next_add_ue_slot = [](unsigned max_rnd_ue_gen_slot) {
    return test_rng::uniform_int<unsigned>(1, max_rnd_ue_gen_slot);
  };

  // The first UE is randomly generated at a given slot (up to the res_grid.ring_size).
  auto add_ue_slot = get_next_add_ue_slot(res_grid.ring_size());
  // Add a few extra srs_prohib_time to ensure there is enough time to allocate the SRS after the last UE has been
  // created.
  constexpr unsigned extra_srs_prohib_time = 2U;
  // Check the allocation for at least 10 the size of the resource grid.
  const unsigned nof_slots_to_test =
      add_ue_slot +
      std::max(srs_prohib_time_uint * (GetParam().nof_ues_per_test + extra_srs_prohib_time), res_grid.ring_size() * 10);

  for (unsigned sl_cnt = 0; sl_cnt < nof_slots_to_test; ++sl_cnt, slot_indication(++current_sl_tx)) {
    // Add the UE at the specified random slot.
    if (sl_cnt == add_ue_slot and test_ues.size() < GetParam().nof_ues_per_test) {
      const du_ue_index_t ue_idx = add_ue(GetParam());
      test_ues.emplace_back(ue_tracker{.du_ue_idx = ue_idx});
      // After the first UE, only generate max every SRS prohibit time.
      add_ue_slot += get_next_add_ue_slot(srs_prohib_time_uint);
    }

    if (not ues.empty()) {
      if (not cell_cfg.is_dl_enabled(current_sl_tx)) {
        continue;
      }

      for (auto& test_ue : test_ues) {
        const auto&                    u = ues[test_ue.du_ue_idx];
        const aperiodic_srs_alloc_info res =
            srs_alloc.allocate_aperiodic_srs(res_grid, test_ue.last_srs_slot, u.get_pcell().cfg());
        // When the SRS is allocated, the aperiodic SRS trigger is different from 0.
        if (res.aperiodic_srs_res_trigger != 0) {
          if (test_ue.last_srs_slot.valid()) {
            ASSERT_GE(current_sl_tx + res.slot_offset - test_ue.last_srs_slot, srs_prohib_time_uint)
                << fmt::format("Expected SRS shouldn't be allocated more often that the prohibit time");
          }
          test_ue.last_srs_slot = current_sl_tx + res.slot_offset;
          test_ue.at_least_one_alloc |= true;
          ASSERT_FALSE(res_grid[res.slot_offset].result.ul.srss.empty());
          auto* const ue_srs_pdu_it = std::find_if(res_grid[res.slot_offset].result.ul.srss.begin(),
                                                   res_grid[res.slot_offset].result.ul.srss.end(),
                                                   [rnti = u.crnti](const srs_info& srs) { return srs.crnti == rnti; });
          ASSERT_NE(ue_srs_pdu_it, res_grid[res.slot_offset].result.ul.srss.end());
          expected<bool, std::string> pdu_test = test_srs_pdu(*ue_srs_pdu_it, u.ue_index);
          ASSERT_TRUE(pdu_test.has_value())
              << fmt::format("rnti={} did not pass the SRS pdu check. Cause: {}", u.crnti, pdu_test.error());
        }
      }

      // Check possible collisions.
      cell_srs_res_usage.fill(0U);
      for (const auto& srs_pdu : res_grid[0U].result.ul.srss) {
        const ue* srs_ue = ues.find_by_rnti(srs_pdu.crnti);
        ASSERT_NE(srs_ue, nullptr);
        const unsigned srs_cell_id = get_ue_srs_cell_id(*srs_ue);
        ASSERT_LT(srs_cell_id, cell_srs_res_usage.size());
        ++cell_srs_res_usage[srs_cell_id];
      }
      ASSERT_FALSE(std::any_of(
          cell_srs_res_usage.cbegin(), cell_srs_res_usage.cend(), [](const unsigned usage) { return usage > 1U; }));
    }
  }
  ASSERT_EQ(test_ues.size(), GetParam().nof_ues_per_test);
  for (auto& test_ue : test_ues) {
    ASSERT_TRUE(test_ue.at_least_one_alloc);
  }
}

INSTANTIATE_TEST_SUITE_P(
    test_srs_alloc,
    srs_alloc_multi_ue_tester,
    testing::Values(
        srs_params{.is_tdd = false, .slot_offset = 5, .srs_prohib_time = srs_periodicity::sl40, .nof_ues_per_test = 5},
        srs_params{.is_tdd = false, .slot_offset = 5, .srs_prohib_time = srs_periodicity::sl80, .nof_ues_per_test = 5},
        srs_params{.is_tdd = false, .slot_offset = 5, .srs_prohib_time = srs_periodicity::sl160, .nof_ues_per_test = 5},
        srs_params{.is_tdd = false, .slot_offset = 5, .srs_prohib_time = srs_periodicity::sl40, .nof_ues_per_test = 37},
        srs_params{.is_tdd = false, .slot_offset = 5, .srs_prohib_time = srs_periodicity::sl80, .nof_ues_per_test = 59},
        srs_params{.is_tdd           = false,
                   .slot_offset      = 5,
                   .srs_prohib_time  = srs_periodicity::sl160,
                   .nof_ues_per_test = 97},
        srs_params{.is_tdd = true, .slot_offset = 11, .srs_prohib_time = srs_periodicity::sl40, .nof_ues_per_test = 7},
        srs_params{.is_tdd = true, .slot_offset = 11, .srs_prohib_time = srs_periodicity::sl80, .nof_ues_per_test = 7},
        srs_params{.is_tdd = true, .slot_offset = 11, .srs_prohib_time = srs_periodicity::sl40, .nof_ues_per_test = 17},
        srs_params{.is_tdd           = true,
                   .slot_offset      = 11,
                   .srs_prohib_time  = srs_periodicity::sl40,
                   .nof_ues_per_test = 37}),
    [](const testing::TestParamInfo<srs_params>& params_item) { return fmt::format("{}", params_item.param); });
