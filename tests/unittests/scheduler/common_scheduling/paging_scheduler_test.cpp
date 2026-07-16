// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/common_scheduling/paging_scheduler.h"
#include "sub_scheduler_test_environment.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/ran/duplex_mode.h"
#include "ocudu/ran/pcch/pcch_configuration.h"
#include "ocudu/scheduler/config/time_domain_resource_helper.h"
#include <gtest/gtest.h>

using namespace ocudu;

static pcch_config::nof_po_per_pf get_random_nof_po_per_pf()
{
  constexpr static std::array<pcch_config::nof_po_per_pf, 3> possible_ns_values = {
      pcch_config::nof_po_per_pf::one, pcch_config::nof_po_per_pf::two, pcch_config::nof_po_per_pf::four};
  return possible_ns_values[test_rng::uniform_int<unsigned>(0, possible_ns_values.size() - 1)];
}

static pcch_config::nof_pf_per_drx_cycle
get_random_nof_pf_per_drx_cycle(const sched_cell_configuration_request_message& cell_cfg)
{
  constexpr static std::array<pcch_config::nof_pf_per_drx_cycle, 5> possible_nof_pf_per_drx_values = {
      pcch_config::nof_pf_per_drx_cycle::oneT,
      pcch_config::nof_pf_per_drx_cycle::halfT,
      pcch_config::nof_pf_per_drx_cycle::quarterT,
      pcch_config::nof_pf_per_drx_cycle::oneEighthT,
      pcch_config::nof_pf_per_drx_cycle::oneSixteethT};
  // oneT is invalid for SS/PBCH and CORESET multiplexing pattern 1 (paging_search_space_id == 0).
  const unsigned start_idx =
      cell_cfg.ran.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id == to_search_space_id(0) ? 1U : 0U;
  return possible_nof_pf_per_drx_values[test_rng::uniform_int<unsigned>(start_idx,
                                                                        possible_nof_pf_per_drx_values.size() - 1)];
}

class base_paging_scheduler_test : public sub_scheduler_test_environment
{
public:
  using five_g_s_tmsi = uint64_t;

  base_paging_scheduler_test(const scheduler_expert_config&                  expert_cfg,
                             const sched_cell_configuration_request_message& cell_req) :
    sub_scheduler_test_environment(expert_cfg, cell_req), pg_sch(cell_cfg, *pdcch_alloc, 0)
  {
  }
  ~base_paging_scheduler_test() override { flush_events(); }

  void push_paging_information(five_g_s_tmsi s_tmsi, unsigned paging_drx_)
  {
    // UE_ID: 5G-S-TMSI mod 1024. See TS 38.304, clause 7.1.
    const unsigned ue_id = s_tmsi % 1024;

    const sched_paging_information info{.paging_type_indicator = paging_identity_type::cn_ue_paging_identity,
                                        .paging_identity       = s_tmsi,
                                        .ue_identity           = ue_id,
                                        .paging_drx            = radio_frames{paging_drx_}};
    pg_sch.handle_paging_information(info);
  }

  bool is_ue_allocated_paging_grant(five_g_s_tmsi s_tmsi)
  {
    return std::any_of(res_grid[0].result.dl.paging_grants.begin(),
                       res_grid[0].result.dl.paging_grants.end(),
                       [&s_tmsi](const auto& grant) {
                         return std::any_of(grant.paging_ue_list.begin(),
                                            grant.paging_ue_list.end(),
                                            [&s_tmsi](const auto& ue) { return ue.paging_identity == s_tmsi; });
                       });
  }

  static five_g_s_tmsi generate_five_g_s_tmsi()
  {
    // 5G-S-TMSI: Concatenation of the AMF Set Identity (10 bits), the AMF Pointer (6 bits) and the 5G-TMSI (32 bits).
    // Values of AMF Set Id and AMF pointer are assumed. Lower 32-bits are masked to fill randomly generated 5G-TMSI.
    static const uint64_t five_g_s_tmsi_with_5g_tmsi_masked = 0x010100000000;
    return five_g_s_tmsi_with_5g_tmsi_masked | test_rng::uniform_int<uint64_t>(0, 0xfffffffe);
  }

  static scheduler_expert_config create_expert_config(sch_mcs_index paging_mcs_index_, unsigned max_paging_retries_)
  {
    auto cfg                  = config_helpers::make_default_scheduler_expert_config();
    cfg.pg.paging_mcs_index   = paging_mcs_index_;
    cfg.pg.max_paging_retries = max_paging_retries_;
    return cfg;
  }

  static sched_cell_configuration_request_message
  create_custom_cell_config_request(duplex_mode          duplx_mode,
                                    bs_channel_bandwidth carrier_bw = bs_channel_bandwidth::MHz20)
  {
    cell_config_builder_params cell_cfg =
        cell_config_builder_profiles::create(duplx_mode, frequency_range::FR1, carrier_bw);
    return sched_config_helper::make_default_sched_cell_configuration_request(cell_cfg);
  }

protected:
  paging_scheduler pg_sch;

private:
  void do_run_slot() override { pg_sch.run_slot(res_grid, next_slot.hyper_sfn()); }
};

// Parameters to be passed to test.
struct paging_scheduler_test_params {
  unsigned    max_paging_mcs;
  unsigned    max_paging_retries;
  uint16_t    drx_cycle_in_nof_rf;
  duplex_mode duplx_mode;
};

void PrintTo(const paging_scheduler_test_params& p, std::ostream* os)
{
  *os << "max_mcs=" << p.max_paging_mcs << " retries=" << p.max_paging_retries << " drx_rf=" << p.drx_cycle_in_nof_rf
      << " duplex=" << to_string(p.duplx_mode);
}

class paging_scheduler_test : public ::testing::TestWithParam<paging_scheduler_test_params>
{
protected:
  paging_scheduler_test_params params{GetParam()};
};

TEST_P(paging_scheduler_test, successfully_allocated_paging_grant_ss_gt_0)
{
  auto cell_cfg_request = base_paging_scheduler_test::create_custom_cell_config_request(params.duplx_mode);
  // Modify to have more than one Paging occasion per PF.
  cell_cfg_request.ran.dl_cfg_common.pcch_cfg.ns     = get_random_nof_po_per_pf();
  cell_cfg_request.ran.dl_cfg_common.pcch_cfg.nof_pf = get_random_nof_pf_per_drx_cycle(cell_cfg_request);
  base_paging_scheduler_test bench{
      base_paging_scheduler_test::create_expert_config(params.max_paging_mcs, params.max_paging_retries),
      cell_cfg_request};

  // Notify scheduler of paging message.
  const uint64_t fiveg_s_tmsi = bench.generate_five_g_s_tmsi();
  bench.push_paging_information(fiveg_s_tmsi, params.drx_cycle_in_nof_rf);

  unsigned paging_attempts = 0;
  for (unsigned i = 0;
       i != (params.max_paging_retries + 1) * params.drx_cycle_in_nof_rf * bench.next_slot.nof_slots_per_frame();
       ++i) {
    bench.run_slot();
    if (bench.is_ue_allocated_paging_grant(fiveg_s_tmsi)) {
      paging_attempts++;
    }
  }

  ASSERT_EQ(paging_attempts, params.max_paging_retries);
}

TEST_P(paging_scheduler_test, successfully_allocated_paging_grant_ss_eq_0)
{
  auto sched_cell_cfg = base_paging_scheduler_test::create_custom_cell_config_request(params.duplx_mode);
  // In default config Paging Search Space is set to 1. Therefore, modify it to be equal to 0 for this test case.
  sched_cell_cfg.ran.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id = to_search_space_id(0);
  sched_cell_cfg.ran.dl_cfg_common.pcch_cfg.nof_pf = get_random_nof_pf_per_drx_cycle(sched_cell_cfg);
  base_paging_scheduler_test bench{
      base_paging_scheduler_test::create_expert_config(params.max_paging_mcs, params.max_paging_retries),
      sched_cell_cfg};

  // Notify scheduler of paging message.
  const auto s_tmsi = bench.generate_five_g_s_tmsi();
  bench.push_paging_information(s_tmsi, params.drx_cycle_in_nof_rf);

  unsigned paging_attempts = 0;
  for (unsigned i = 0;
       i != (params.max_paging_retries + 1) * params.drx_cycle_in_nof_rf * bench.next_slot.nof_slots_per_frame();
       ++i) {
    bench.run_slot();
    if (bench.is_ue_allocated_paging_grant(s_tmsi)) {
      paging_attempts++;
    }
  }

  ASSERT_EQ(paging_attempts, params.max_paging_retries);
}

TEST_P(paging_scheduler_test, successfully_paging_multiple_ues)
{
  base_paging_scheduler_test bench{
      base_paging_scheduler_test::create_expert_config(params.max_paging_mcs, params.max_paging_retries),
      base_paging_scheduler_test::create_custom_cell_config_request(params.duplx_mode)};

  std::map<base_paging_scheduler_test::five_g_s_tmsi, unsigned> fiveg_s_tmsi_to_paging_attempts_lookup;

  static constexpr unsigned nof_ues = 5;

  // Notify scheduler of paging message.
  for (unsigned ue_num = 0; ue_num < nof_ues; ue_num++) {
    const uint64_t fiveg_s_tmsi_ue = bench.generate_five_g_s_tmsi();
    bench.push_paging_information(fiveg_s_tmsi_ue, params.drx_cycle_in_nof_rf);
    fiveg_s_tmsi_to_paging_attempts_lookup[fiveg_s_tmsi_ue] = 0;
  }

  for (unsigned i = 0; i != nof_ues * (params.max_paging_retries + 1) * params.drx_cycle_in_nof_rf *
                                bench.next_slot.nof_slots_per_frame();
       ++i) {
    bench.run_slot();
    for (auto& it : fiveg_s_tmsi_to_paging_attempts_lookup) {
      if (bench.is_ue_allocated_paging_grant(it.first)) {
        it.second++;
      }
    }
  }

  for (auto& it : fiveg_s_tmsi_to_paging_attempts_lookup) {
    ASSERT_EQ(it.second, params.max_paging_retries) << fmt::format("For UE with 5G-S-TMSI: {:#x}", it.first);
  }
}

class paging_sched_special_case_tester : public base_paging_scheduler_test, public ::testing::Test
{
protected:
  paging_sched_special_case_tester() : base_paging_scheduler_test(create_expert_config(3, 3), build_cell_req()) {}

private:
  static sched_cell_configuration_request_message build_cell_req()
  {
    auto cfg = create_custom_cell_config_request(duplex_mode::FDD, bs_channel_bandwidth::MHz5);
    // Shuffle between SearchSpace#0 and SearchSpace#1.
    const auto ss_id = to_search_space_id(test_rng::uniform_int<unsigned>(0, 1));
    if (ss_id == to_search_space_id(0)) {
      // In default config Paging Search Space is set to 1. Therefore, modify it to be equal to 0 for this test case.
      cfg.ran.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id = to_search_space_id(0);
      // Since we support CORESET multiplexing pattern 1. The value of N (Number of Paging Frames per DRX Cycle) can
      // be 2, 4, 8, 16).
      cfg.ran.dl_cfg_common.pcch_cfg.nof_pf = pcch_config::nof_pf_per_drx_cycle::halfT;
    }
    return cfg;
  }
};

TEST_F(paging_sched_special_case_tester, successfully_allocated_paging_grant_5mhz_carrier_bw)
{
  static constexpr unsigned max_paging_retries  = 3;
  static constexpr uint16_t drx_cycle_in_nof_rf = 128;

  // Notify scheduler of paging message.
  const five_g_s_tmsi s_tmsi = generate_five_g_s_tmsi();
  push_paging_information(s_tmsi, drx_cycle_in_nof_rf);

  unsigned paging_attempts = 0;
  for (unsigned i = 0; i != (max_paging_retries + 1) * drx_cycle_in_nof_rf * next_slot.nof_slots_per_frame(); ++i) {
    run_slot();
    if (is_ue_allocated_paging_grant(s_tmsi)) {
      paging_attempts++;
    }
  }

  ASSERT_EQ(paging_attempts, max_paging_retries);
}

class paging_sched_partial_slot_tester : public base_paging_scheduler_test, public ::testing::Test
{
protected:
  paging_sched_partial_slot_tester() : base_paging_scheduler_test(create_expert_config(3, 3), build_cell_req()) {}

private:
  static sched_cell_configuration_request_message build_cell_req()
  {
    static const tdd_ul_dl_config_common tdd_cfg{.ref_scs  = subcarrier_spacing::kHz30,
                                                 .pattern1 = {.dl_ul_tx_period_nof_slots = 5,
                                                              .nof_dl_slots              = 2,
                                                              .nof_dl_symbols            = 8,
                                                              .nof_ul_slots              = 2,
                                                              .nof_ul_symbols            = 0}};

    auto req        = create_custom_cell_config_request(duplex_mode::TDD);
    req.ran.tdd_cfg = tdd_cfg;
    // Generate PDSCH Time domain allocation based on the partial slot TDD configuration.
    req.ran.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list =
        time_domain_resource_helper::generate_dedicated_pdsch_td_res_list(
            req.ran.tdd_cfg,
            req.ran.dl_cfg_common.init_dl_bwp.generic_params.cp,
            time_domain_resource_helper::calculate_minimum_pdsch_symbol(req.ran.dl_cfg_common.init_dl_bwp.pdcch_common,
                                                                        std::nullopt));
    req.ran.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list =
        time_domain_resource_helper::generate_dedicated_pusch_td_res_list(
            req.ran.tdd_cfg, req.ran.ul_cfg_common.init_ul_bwp.generic_params.cp, req.ran.init_bwp.pusch.min_k2);
    req.ran.init_bwp.csi = std::nullopt;

    // Set Paging configuration to a particular value in order to derive the 5G-S-TMSI, such that Paging Occasion
    // of UE fall in partial slot of above set TDD configuration. i.e. we would like to force Paging Occasion index of
    // UE to be 3rd PDCCH Monitoring Occasion which corresponds to 3rd in SearchSpace#1 used for Paging (SS#1 is
    // monitored in every DL slot).
    req.ran.dl_cfg_common.pcch_cfg.ns     = pcch_config::nof_po_per_pf::four;
    req.ran.dl_cfg_common.pcch_cfg.nof_pf = pcch_config::nof_pf_per_drx_cycle::oneT;
    return req;
  }
};

TEST_F(paging_sched_partial_slot_tester, successfully_allocated_paging_grant_ss_gt_0_in_partial_slot_tdd)
{
  static constexpr unsigned max_paging_retries  = 3;
  static constexpr uint16_t drx_cycle_in_nof_rf = 128;

  // N value used in equation found at TS 38.304, clause 7.1.
  const unsigned nof_paging_frames =
      static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.default_paging_cycle) /
      static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.nof_pf);

  unsigned i_s          = 0;
  uint64_t fiveg_s_tmsi = 0;
  while (i_s != cell_cfg.params.tdd_cfg->pattern1.nof_dl_slots) {
    fiveg_s_tmsi = generate_five_g_s_tmsi();

    // UE_ID: 5G-S-TMSI mod 1024. See TS 38.304, clause 7.1.
    const unsigned ue_id = fiveg_s_tmsi % 1024;

    // Index (i_s), indicating the index of the PO.
    // i_s = floor (UE_ID/N) mod Ns.
    i_s = (ue_id / nof_paging_frames) % static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.ns);
  }

  // Notify scheduler of paging message.
  push_paging_information(fiveg_s_tmsi, drx_cycle_in_nof_rf);

  unsigned paging_attempts = 0;
  for (unsigned i = 0; i != (max_paging_retries + 1) * drx_cycle_in_nof_rf * next_slot.nof_slots_per_frame(); ++i) {
    run_slot();
    if (is_ue_allocated_paging_grant(fiveg_s_tmsi)) {
      paging_attempts++;
    }
  }

  ASSERT_EQ(paging_attempts, max_paging_retries);
}

INSTANTIATE_TEST_SUITE_P(paging_sched_tester,
                         paging_scheduler_test,
                         testing::Values(paging_scheduler_test_params{.max_paging_mcs      = 3,
                                                                      .max_paging_retries  = 3,
                                                                      .drx_cycle_in_nof_rf = 128,
                                                                      .duplx_mode          = duplex_mode::FDD},
                                         paging_scheduler_test_params{.max_paging_mcs      = 3,
                                                                      .max_paging_retries  = 3,
                                                                      .drx_cycle_in_nof_rf = 128,
                                                                      .duplx_mode          = duplex_mode::TDD}));
