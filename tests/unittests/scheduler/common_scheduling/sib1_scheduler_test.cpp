// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/common_scheduling/sib1_scheduler.h"
#include "lib/scheduler/common_scheduling/ssb_scheduler.h"
#include "sub_scheduler_test_environment.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "ocudu/ran/duplex_mode.h"
#include "ocudu/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "ocudu/ran/ssb/ssb_mapping.h"
#include "ocudu/scheduler/config/time_domain_resource_helper.h"
#include "ocudu/support/enum_utils.h"
#include "ocudu/support/ocudu_test.h"
#include <gtest/gtest.h>

using namespace ocudu;

// Dummy PDCCH scheduler required to instantiate the SIB1 scheduler.
class dummy_pdcch_resource_allocator : public pdcch_resource_allocator
{
public:
  void slot_indication(slot_point /*sl_tx*/) {}

  pdcch_dl_information* alloc_dl_pdcch_common(cell_slot_resource_allocator& slot_alloc,
                                              rnti_t                        rnti,
                                              search_space_id               ss_id,
                                              aggregation_level             aggr_lvl) override
  {
    TESTASSERT_EQ(fmt::underlying(ss_id),
                  fmt::underlying(slot_alloc.cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.sib1_search_space_id));
    slot_alloc.result.dl.dl_pdcchs.emplace_back();
    slot_alloc.result.dl.dl_pdcchs.back().ctx.rnti    = rnti;
    slot_alloc.result.dl.dl_pdcchs.back().ctx.bwp_cfg = &slot_alloc.cfg.params.dl_cfg_common.init_dl_bwp.generic_params;
    slot_alloc.result.dl.dl_pdcchs.back().ctx.coreset_cfg =
        &*slot_alloc.cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0;
    slot_alloc.result.dl.dl_pdcchs.back().ctx.cces = {0, ocudu::aggregation_level::n4};
    return &slot_alloc.result.dl.dl_pdcchs[0];
  }

  pdcch_dl_information* alloc_dl_pdcch_ue(cell_slot_resource_allocator& slot_alloc,
                                          rnti_t                        rnti,
                                          const ue_cell_configuration&  user,
                                          search_space_id               ss_id,
                                          aggregation_level             aggr_lvl) override
  {
    ocudu_terminate("UE-dedicated PDCCHs should not be called while allocating RARs");
    return nullptr;
  }

  pdcch_ul_information* alloc_ul_pdcch_ue(cell_slot_resource_allocator& slot_alloc,
                                          rnti_t                        rnti,
                                          const ue_cell_configuration&  user,
                                          search_space_id               ss_id,
                                          aggregation_level             aggr_lvl) override
  {
    ocudu_terminate("UE-dedicated PDCCHs should not be called while allocating RARs");
    return nullptr;
  }

  pdcch_ul_information* alloc_ul_pdcch_common(cell_slot_resource_allocator& slot_alloc,
                                              rnti_t                        rnti,
                                              search_space_id               ss_id,
                                              aggregation_level             aggr_lvl) override
  {
    ocudu_terminate("Common PDCCHs should not be called while allocating RARs");
    return nullptr;
  }

  bool cancel_last_pdcch(cell_slot_resource_allocator& slot_alloc) override
  {
    ocudu_terminate("Not supported");
    return true;
  }
};

/// Helper class to initialize and store relevant objects for the test and provide helper methods.
class sib1_scheduler_setup : public sub_scheduler_test_environment
{
public:
  sib1_scheduler_setup(const scheduler_expert_config&                  expert_cfg,
                       const sched_cell_configuration_request_message& cell_req) :
    sub_scheduler_test_environment(expert_cfg, cell_req, std::make_unique<dummy_pdcch_resource_allocator>()),
    sib1_sched(cell_cfg, *pdcch_alloc, cell_req.si_scheduling.sib1_payload_size)
  {
    // SIB1 n0 values are pre-computed relative to SFN 0 — override the random base start.
    next_slot = slot_point_extended{slot_point{to_numerology_value(cell_cfg.scs_common()), 0}};
  }
  ~sib1_scheduler_setup() override { flush_events(); }

  const scheduler_si_expert_config& si_cfg{sched_cfg.si};

  cell_slot_resource_allocator& get_slot_res_grid() { return res_grid[0]; }

  /// Helper that tests if the PDCCH and DCI grants in the scheduled results have been filled properly.
  void assess_filled_grants()
  {
    // Test SIB_information message
    const sib_information& test_sib1 = res_grid[0].result.dl.bc.sibs.back();
    TESTASSERT_EQ(fmt::underlying(sib_information::si_indicator_type::sib1), fmt::underlying(test_sib1.si_indicator));
    TESTASSERT_EQ(rnti_t::SI_RNTI, test_sib1.pdsch_cfg.rnti);

    // Test PDCCH_grant and DCI
    const pdcch_dl_information* pdcch =
        std::find_if(res_grid[0].result.dl.dl_pdcchs.begin(),
                     res_grid[0].result.dl.dl_pdcchs.end(),
                     [](const auto& pdcch_) { return pdcch_.ctx.rnti == rnti_t::SI_RNTI; });
    TESTASSERT(pdcch != nullptr);
    TESTASSERT_EQ(fmt::underlying(dci_dl_rnti_config_type::si_f1_0), fmt::underlying(pdcch->dci.type()));
    TESTASSERT_EQ(si_cfg.sib1_mcs_index, pdcch->dci.as_si_rnti_f1_0().modulation_coding_scheme);
    TESTASSERT_EQ(0, pdcch->dci.as_si_rnti_f1_0().redundancy_version);
  }

  /// Tests if PRBs have been set as used in the resource grid for the current slot.
  void verify_prbs_allocation(bool got_allocated = true)
  {
    // Tests if PRBs have been allocated.
    if (got_allocated) {
      TESTASSERT(
          res_grid[0].dl_res_grid.used_crbs(cell_cfg.params.dl_cfg_common.init_dl_bwp.generic_params, {0, 14}).any());
    } else {
      // Tests if PRBs are still unused.
      TESTASSERT(
          res_grid[0].dl_res_grid.used_crbs(cell_cfg.params.dl_cfg_common.init_dl_bwp.generic_params, {0, 14}).none());
    }
  }

  static scheduler_expert_config make_scheduler_expert_cfg(const scheduler_si_expert_config& si_cfg_)
  {
    scheduler_expert_config expert_cfg = config_helpers::make_default_scheduler_expert_config();
    expert_cfg.si                      = si_cfg_;
    return expert_cfg;
  }

  // Create default configuration and change specific parameters based on input args.
  static sched_cell_configuration_request_message make_cell_cfg_req_for_sib_sched(subcarrier_spacing init_bwp_scs,
                                                                                  uint8_t            pdcch_config_sib1,
                                                                                  uint8_t            ssb_bitmap,
                                                                                  uint8_t            l_max,
                                                                                  ssb_periodicity    ssb_period,
                                                                                  uint16_t           carrier_bw_mhz,
                                                                                  duplex_mode        duplx_mode)
  {
    cell_config_builder_params cell_cfg = cell_config_builder_profiles::create(
        duplx_mode, frequency_range::FR1, static_cast<bs_channel_bandwidth>(carrier_bw_mhz));
    cell_cfg.scs_common = init_bwp_scs;
    if (duplx_mode == duplex_mode::FDD) {
      cell_cfg.dl_carrier.arfcn_f_ref = init_bwp_scs == subcarrier_spacing::kHz15 ? 536020 : 176300;
      cell_cfg.dl_carrier.band        = band_helper::get_band_from_dl_arfcn(cell_cfg.dl_carrier.arfcn_f_ref);
    } else {
      // With SCS 15kHz, we set band n38, which (for this SCS) is in SSB case A (L_max = 4); with SCS 30kHz, we set n40,
      // which is in SSB case C, which is in SSB case C and with L_max = 8.
      cell_cfg.dl_carrier.arfcn_f_ref = init_bwp_scs == subcarrier_spacing::kHz15 ? 518440 : 465000;
      cell_cfg.dl_carrier.band        = init_bwp_scs == subcarrier_spacing::kHz15 ? nr_band::n38 : nr_band::n40;
    }
    cell_cfg.cs0_index = coreset0_index{static_cast<uint8_t>((pdcch_config_sib1 >> 4U) & 0b00001111U)};
    cell_cfg.ss0_index = pdcch_config_sib1 & 0b00001111U;

    if (duplx_mode == duplex_mode::TDD) {
      tdd_ul_dl_config_common& tdd_cfg           = cell_cfg.tdd_ul_dl_cfg_common.emplace();
      tdd_cfg.ref_scs                            = init_bwp_scs;
      tdd_cfg.pattern1.dl_ul_tx_period_nof_slots = 10;
      tdd_cfg.pattern1.nof_dl_slots              = 5;
      tdd_cfg.pattern1.nof_dl_symbols            = 0;
      tdd_cfg.pattern1.nof_ul_slots              = 4;
      tdd_cfg.pattern1.nof_ul_symbols            = 0;
    }

    sched_cell_configuration_request_message msg =
        sched_config_helper::make_default_sched_cell_configuration_request(cell_cfg);
    msg.ran.ssb_cfg.ssb_bitmap.set_bitmap(ssb_bitmap, l_max);
    msg.ran.ssb_cfg.ssb_period = ssb_period;

    return msg;
  }

  // Create default configuration and change specific parameters based on input args.
  static sched_cell_configuration_request_message make_cell_cfg_req_for_sib_sched(arfcn_t            freq_arfcn,
                                                                                  uint16_t           offset_to_point_A,
                                                                                  uint8_t            k_ssb,
                                                                                  uint8_t            ssb_bitmap,
                                                                                  uint8_t            l_max,
                                                                                  subcarrier_spacing init_bwp_scs,
                                                                                  uint8_t            pdcch_config_sib1,
                                                                                  bs_channel_bandwidth carrier_bw_mhz)
  {
    cell_config_builder_params cell_cfg{};
    cell_cfg.dl_carrier.arfcn_f_ref = freq_arfcn;
    cell_cfg.scs_common             = init_bwp_scs;
    cell_cfg.dl_carrier.carrier_bw  = carrier_bw_mhz;
    cell_cfg.cs0_index              = coreset0_index{static_cast<uint8_t>((pdcch_config_sib1 >> 4U) & 0b00001111U)};
    cell_cfg.ss0_index              = pdcch_config_sib1 & 0b00001111U;
    cell_cfg.offset_to_point_a      = ssb_offset_to_pointA{offset_to_point_A};
    cell_cfg.k_ssb                  = k_ssb;
    cell_cfg.auto_derive_params();

    sched_cell_configuration_request_message msg =
        sched_config_helper::make_default_sched_cell_configuration_request(cell_cfg);
    msg.ran.dl_cfg_common.freq_info_dl.offset_to_point_a = offset_to_point_A;

    msg.ran.ssb_cfg.ssb_bitmap.set_bitmap(ssb_bitmap, l_max);
    msg.ran.ssb_cfg.ssb_period        = ssb_periodicity::ms10;
    msg.ran.ssb_cfg.offset_to_point_A = ssb_offset_to_pointA{offset_to_point_A};
    msg.ran.ssb_cfg.k_ssb             = k_ssb;
    msg.ran.dl_carrier.carrier_bw     = carrier_bw_mhz;

    if (band_helper::get_duplex_mode(band_helper::get_band_from_dl_arfcn(freq_arfcn)) == ocudu::duplex_mode::TDD) {
      // Change TDD pattern so that PDCCH slots falls in DL slot when using 5Mhz carrier BW.
      msg.ran.tdd_cfg.value().pattern1.dl_ul_tx_period_nof_slots = 20;
      msg.ran.tdd_cfg.value().pattern1.nof_dl_slots              = 12;
      msg.ran.tdd_cfg.value().pattern1.nof_ul_slots              = 7;
    }

    return msg;
  }

protected:
  sib1_scheduler sib1_sched;

private:
  void do_run_slot() override { sib1_sched.run_slot(res_grid[0]); }
};

/// Variant of sib1_scheduler_setup that also runs the SSB scheduler per slot (for collision tests).
class sib1_ssb_collision_setup : public sib1_scheduler_setup
{
public:
  sib1_ssb_collision_setup(const scheduler_expert_config&                  expert_cfg,
                           const sched_cell_configuration_request_message& cell_req) :
    sib1_scheduler_setup(expert_cfg, cell_req), ssb_sched(cell_cfg)
  {
  }

private:
  void do_run_slot() override
  {
    // res_grid.slot_indication (called in base run_slot before do_run_slot) already cleared res_grid[0].
    ssb_sched.schedule_ssb(res_grid[0]);
    sib1_sched.run_slot(res_grid[0]);
  }

  ssb_scheduler ssb_sched;
};

/// \brief Tests if the SIB1 scheduler schedules the SIB1s at the right slot n0.
/// \param[in] scs_common SCS corresponding to subCarrierSpacingCommon.
/// \param[in] sib1_pdcch_slots array of PDCCH slots; the n-th array's value is the PDCCH slot corresponding to the
/// n-th SSB beam.
/// \param[in] pdcch_config_sib1 is the parameter (in the MIB) determining the n0 for each beam.
/// \param[in] ssb_beam_bitmap corresponds to the ssb-PositionsInBurst in the TS 38.331, with L_max = 8.
/// \param[in] carrier_bw_mhz corresponds to the width of this carrier in MHz. Values: 5, 10, 15, 20, 25, 30, 40,
/// 50, 60, 70, 80, 90, 100, 200, 400.
/// \param[in] duplx_mode corresponds to duplex mode FDD or TDD.
void test_sib1_scheduler(subcarrier_spacing                         scs_common,
                         const std::array<unsigned, MAX_NUM_BEAMS>& sib1_pdcch_slots,
                         uint8_t                                    pdcch_config_sib1,
                         uint8_t                                    ssb_beam_bitmap,
                         uint8_t                                    l_max,
                         uint16_t                                   carrier_bw_mhz,
                         duplex_mode                                duplx_mode)
{
  sib1_scheduler_setup t_bench{
      sib1_scheduler_setup::make_scheduler_expert_cfg({10, aggregation_level::n4, 10, aggregation_level::n4}),
      sib1_scheduler_setup::make_cell_cfg_req_for_sib_sched(
          scs_common, pdcch_config_sib1, ssb_beam_bitmap, l_max, ssb_periodicity::ms5, carrier_bw_mhz, duplx_mode)};

  // SIB1 periodicity in slots.
  const unsigned sib1_period_slots = SIB1_PERIODICITY * t_bench.next_slot.nof_slots_per_subframe();

  // Run the test for 1000 slots.
  constexpr size_t test_length_slots = 1000;
  for (size_t sl_idx = 0; sl_idx != test_length_slots; ++sl_idx) {
    t_bench.run_slot();

    // Verify if for any active beam, the SIB1 got allocated within the proper n0 slots.
    for (size_t ssb_idx = 0; ssb_idx != l_max; ++ssb_idx) {
      // Only check for the active slots.
      if (t_bench.cell_cfg.params.ssb_cfg.ssb_bitmap.test(ssb_idx) &&
          (sl_idx % sib1_period_slots == sib1_pdcch_slots[ssb_idx])) {
        // Verify that the scheduler results list contain 1 element with the SIB1 information.
        ASSERT_EQ(1, t_bench.res_grid[0].result.dl.bc.sibs.size()) << fmt::format("Slot {}", t_bench.res_grid[0].slot);
        // Verify the PDCCH grants and DCI have been filled correctly.
        t_bench.assess_filled_grants();
        // Verify the PRBs in the res_grid are set as used.
        t_bench.verify_prbs_allocation();
      }
    }
  }
}

/// \brief Tests if the SIB1 scheduler schedules SIB1s according to the correct retransmission periodicity.
///
/// This test evaluates the correct SIB1 retransmission period, which we assume it should be the maximum between the SSB
/// periodicity and the SIB1 retx periodicity set as a parameter. This is due to the fact that the SIB1 requires the SSB
/// to be decoded, meaning there is no point in scheduling SIBs more frequently than SSBs.
/// This test only evaluates the periodicity of SIB1, therefore it uses a standard set of values for the remaining
/// parameters (e.g., SCS, pdcch_config_sib1, SSB bitmap).
///
/// \param[in] sib1_rtx_period period set for the SIB1 retransmissions.
/// \param[in] ssb_period period set for the SSB.
void test_sib1_periodicity(sib1_rtx_periodicity sib1_rtx_period, ssb_periodicity ssb_period)
{
  const uint8_t        L_max = 4U;
  sib1_scheduler_setup t_bench{
      sib1_scheduler_setup::make_scheduler_expert_cfg(
          {10, aggregation_level::n4, 10, aggregation_level::n4, sib1_rtx_period}),
      sib1_scheduler_setup::make_cell_cfg_req_for_sib_sched(
          subcarrier_spacing::kHz15, 9U, 0b1000, L_max, ssb_period, 20, ocudu::duplex_mode::FDD)};

  // Determine the expected SIB1 retx periodicity.
  const unsigned expected_sib1_period_ms =
      to_value(sib1_rtx_period) > to_value(ssb_period) ? to_value(sib1_rtx_period) : to_value(ssb_period);

  // SIB1 periodicity in slots.
  const unsigned expected_sib1_period_slots = expected_sib1_period_ms * t_bench.next_slot.nof_slots_per_subframe();

  // Slot (or offset) at which SIB1 PDCCH is allocated, measured as a delay compared to the slot with SSB. Specifically,
  // n0 = 5 is the offset of the SIB1 for the first beam, for searcSpaceZero = 9U, multiplexing pattern 1 (15kHz SCS,
  // FR1); as per Section 13, TS 38.213. As the scheduler allocate the SIB1 PDCCH in the n0 + 1 slot,
  // sib1_allocation_slot is 6.
  constexpr unsigned sib1_allocation_slot{6};

  // Run the test for 10000 slots.
  constexpr size_t test_length_slots = 10000;
  for (size_t sl_idx = 0; sl_idx < test_length_slots; sl_idx++) {
    t_bench.run_slot();

    // With the SSB bitmap set 0b10000000, only the SSB and SIB1 for the 1 beams are used; we perform the check for
    // this beam.
    if ((sl_idx % expected_sib1_period_slots) == sib1_allocation_slot) {
      // Verify that the scheduler results list contain 1 element with the SIB1 information.
      TESTASSERT_EQ(1, t_bench.res_grid[0].result.dl.bc.sibs.size());
    } else {
      TESTASSERT(t_bench.res_grid[0].result.dl.bc.sibs.empty());
    }
  }
}

/// \brief Tests if any potential collision occurs between SIB1 PDCCH/PDSCH and SSB.
///
/// \param[in] freq_arfcn ARFCN of point A for DL carrier.
/// \param[in] offset_to_point_A as per TS38.211, Section 4.4.4.2.
/// \param[in] k_ssb or ssb-SubcarrierOffset, as per TS38.211, Section 7.4.3.1.
/// \param[in] ssb_bitmap is \c ssb-PositionsInBurst.inOneGroup, as per TS38.331, \c ServingCellConfigCommonSIB.
/// \param[in] scs subcarrier spacing of SSB and SCScommon.
/// \param[in] pdcch_config_sib1 is \c pdcch-ConfigSIB1, as per TS38.213, Section 13.
/// \param[in] carrier_bw_mhz corresponds to the width of this carrier in MHz. Values: 5, 10, 15, 20, 25, 30, 40,
/// 50, 60, 70, 80, 90, 100, 200, 400.
void test_ssb_sib1_collision(arfcn_t              freq_arfcn,
                             uint16_t             offset_to_point_A,
                             uint8_t              k_ssb,
                             uint8_t              ssb_bitmap,
                             uint8_t              l_max,
                             subcarrier_spacing   scs,
                             uint8_t              pdcch_config_sib1,
                             bs_channel_bandwidth carrier_bw_mhz)
{
  sib1_ssb_collision_setup t_bench{
      sib1_scheduler_setup::make_scheduler_expert_cfg(
          {10, aggregation_level::n4, 10, aggregation_level::n4, sib1_rtx_periodicity::ms10}),
      sib1_scheduler_setup::make_cell_cfg_req_for_sib_sched(
          freq_arfcn, offset_to_point_A, k_ssb, ssb_bitmap, l_max, scs, pdcch_config_sib1, carrier_bw_mhz)};

  // Run the test for 100 slots.
  constexpr size_t test_length_slots = 100;
  for (size_t sl_idx = 0; sl_idx < test_length_slots; sl_idx++) {
    t_bench.run_slot();

    test_dl_resource_grid_collisions(t_bench.cell_cfg, t_bench.res_grid[0].result.dl);
  }
}

// Test for potential collisions between SIB1 PDCCH/PDSCH and SSB.
void test_sib_1_pdsch_collisions(arfcn_t freq_arfcn, subcarrier_spacing scs, bs_channel_bandwidth carrier_bw_mhz)
{
  const auto band      = band_helper::get_band_from_dl_arfcn(freq_arfcn);
  const auto min_ch_bw = band_helper::get_min_channel_bw(band, scs);
  ocudu_assert(bs_channel_bandwidth_to_MHz(carrier_bw_mhz) >= min_channel_bandwidth_to_MHz(min_ch_bw),
               "Invalid carrier BW");

  const auto nof_rbs_bpw = band_helper::get_n_rbs_from_bw(
      carrier_bw_mhz, scs, band_helper::get_freq_range(band_helper::get_band_from_dl_arfcn(freq_arfcn)));

  // NOTE: We only test 1 beam, as we don't have resource grids for multiple beams implemented yet.
  const uint8_t L_max = ssb_get_L_max(scs, freq_arfcn, band);
  ocudu_assert(L_max == 4U or L_max == 8U, "Only L_max = 4 and 8 currently supported in this test");
  const uint8_t ssb_bitmap = L_max == 4U ? 0b1000 : 0b10000000;
  // Allocate SIB1 in the same slot as SSB - searchspace0 = 0U.
  constexpr uint8_t searchspace0 = 0U;
  const uint8_t     coreset0_max = scs == subcarrier_spacing::kHz15 ? 15 : 16;

  // Test different combinations of offsetToPointA and k_SSB.
  const unsigned max_offset_to_point_A = nof_rbs_bpw - NOF_SSB_PRBS;
  // Consider a +2 increment for both offsetToPointA and k_SSB, to be compliant with 30kHz SCS.
  for (unsigned offset_to_point_A = 0; offset_to_point_A < max_offset_to_point_A; offset_to_point_A += 2) {
    for (uint8_t k_ssb_val = 0; k_ssb_val < 12; k_ssb_val += 2) {
      // Test all possible combinations of coreset0 position.
      for (uint8_t coreset0 = 0; coreset0 < coreset0_max; ++coreset0) {
        const pdcch_type0_css_coreset_description coreset0_param =
            pdcch_type0_css_coreset_get(band, scs, scs, coreset0, k_ssb_val);

        // If the Coreset 0 exceeds the BPW limit, skip this configuration.
        TESTASSERT(coreset0_param.offset >= 0, "FR2 not supported in this test");

        // CRB (with reference to SCScommon carrier) pointed to by offset_to_point_A.
        const unsigned crb_ssb = scs == subcarrier_spacing::kHz15 ? offset_to_point_A : offset_to_point_A / 2;

        // If the Coreset 0 exceeds the Initial DL BPW limits, skip this configuration.
        if (crb_ssb - static_cast<unsigned>(coreset0_param.offset) +
                    static_cast<unsigned>(coreset0_param.nof_rb_coreset) >=
                nof_rbs_bpw or
            static_cast<unsigned>(coreset0_param.offset) > crb_ssb) {
          continue;
        }
        if (not band_helper::get_ssb_arfcn(freq_arfcn,
                                           band,
                                           nof_rbs_bpw,
                                           scs,
                                           scs,
                                           ssb_offset_to_pointA{static_cast<uint16_t>(offset_to_point_A)},
                                           ssb_subcarrier_offset{k_ssb_val})
                    .has_value()) {
          continue;
        }

        const uint8_t pdcch_config_sib1 = static_cast<uint8_t>((coreset0 << 4U) + searchspace0);
        test_ssb_sib1_collision(
            freq_arfcn, offset_to_point_A, k_ssb_val, ssb_bitmap, L_max, scs, pdcch_config_sib1, carrier_bw_mhz);
      }
    }
  }
}

TEST(sib1_scheduler_test, test_sib1_scheduler_allocation_fdd)
{
  // Test SIB1 scheduler for different values of searchSpaceZero (4 LSBs of pdcch_config_sib1) and for different
  // SSB_bitmaps.
  // Depending on the SIB1 scheduler implementation, the array sib1_slots contains the expected slots n0 or n0 + 1, at
  // which the SIB1 is scheduled.
  // NOTE: As the current SIB1 scheduler implementation allocates the SIB1 PDCCH on the slot n0 + 1, i-th element of the
  // sib1_slots array refers to the n0 + 1 for the i-th SSB's beam. The slots n0 have been pre-computed based on
  // TS 38.213, Section 13.

  // NOTE: for FDD, there is no option to have L_max = 8, as all FDD frequencies are below 3GHz.

  // SCS Common: 15kHz
  std::array<unsigned, MAX_NUM_BEAMS> sib1_slots = {6, 8, 10, 12};
  constexpr uint8_t                   l_max      = 4U;
  // pdcch_config_sib1 = 9U => { coreset0 = 0U, searchspace0 = 9U).
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 9U, 0b1010, l_max, 20, ocudu::duplex_mode::FDD);
  // pdcch_config_sib1 = 57U => { coreset0 = 3U, searchspace0 = 9U).
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 57U, 0b0101, l_max, 20, ocudu::duplex_mode::FDD);
  // pdcch_config_sib1 = 105U => { coreset0 = 0U, searchspace0 = 9U).
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 105U, 0b1111, l_max, 20, ocudu::duplex_mode::FDD);

  // 5Mhz Carrier BW.
  sib1_slots = {6, 8, 10, 12};
  // pdcch_config_sib1 = 9U => { coreset0 = 0U, searchspace0 = 9U).
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 9U, 0b1010, l_max, 5, ocudu::duplex_mode::FDD);
  // pdcch_config_sib1 = 57U => { coreset0 = 3U, searchspace0 = 9U).
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 57U, 0b0101, l_max, 5, ocudu::duplex_mode::FDD);

  sib1_slots = {3, 4, 5, 6};
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 2U, 0b1010, l_max, 20, ocudu::duplex_mode::FDD);
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 2U, 0b0101, l_max, 20, ocudu::duplex_mode::FDD);
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 2U, 0b1111, l_max, 20, ocudu::duplex_mode::FDD);

  // 5Mhz Carrier BW.
  sib1_slots = {3, 4, 5, 6};
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 2U, 0b1010, l_max, 5, ocudu::duplex_mode::FDD);
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 2U, 0b0101, l_max, 5, ocudu::duplex_mode::FDD);
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 2U, 0b1111, l_max, 5, ocudu::duplex_mode::FDD);

  sib1_slots = {8, 9, 10, 11};
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 6U, 0b1010, l_max, 20, ocudu::duplex_mode::FDD);
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 6U, 0b0101, l_max, 20, ocudu::duplex_mode::FDD);
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 6U, 0b1111, l_max, 20, ocudu::duplex_mode::FDD);

  // 5Mhz Carrier BW.
  sib1_slots = {8, 9, 10, 11};
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 6U, 0b1010, l_max, 5, ocudu::duplex_mode::FDD);
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 6U, 0b0101, l_max, 5, ocudu::duplex_mode::FDD);
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 6U, 0b1111, l_max, 5, ocudu::duplex_mode::FDD);

  // SCS Common: 30kHz
  sib1_slots = {11, 13, 15, 17};
  test_sib1_scheduler(subcarrier_spacing::kHz30, sib1_slots, 9U, 0b1010, l_max, 10, ocudu::duplex_mode::FDD);
  test_sib1_scheduler(subcarrier_spacing::kHz30, sib1_slots, 9U, 0b0101, l_max, 10, ocudu::duplex_mode::FDD);
  test_sib1_scheduler(subcarrier_spacing::kHz30, sib1_slots, 9U, 0b1111, l_max, 10, ocudu::duplex_mode::FDD);

  sib1_slots = {11, 12, 13, 14};
  // pdcch_config_sib1 = 4U => { coreset0 = 0U, searchspace0 = 4U).
  test_sib1_scheduler(subcarrier_spacing::kHz30, sib1_slots, 4U, 0b1010, l_max, 10, ocudu::duplex_mode::FDD);
  // pdcch_config_sib1 = 84U => { coreset0 = 5U, searchspace0 = 4U).
  test_sib1_scheduler(subcarrier_spacing::kHz30, sib1_slots, 84U, 0b0101, l_max, 10, ocudu::duplex_mode::FDD);
}

TEST(sib1_scheduler_test, test_sib1_scheduler_allocation_tdd)
{
  // Test SIB1 scheduler for different values of searchSpaceZero (4 LSBs of pdcch_config_sib1), for
  // SSB_bitmap = 0b10000000.
  // Depending on the SIB1 scheduler implementation, the array sib1_slots contains the expected slots n0 or n0 + 1, at
  // which the SIB1 is scheduled.
  // NOTE: (i) As in this test we assume only the first SSB beam gets transmitted, sib1_slots only contains one element,
  // which is slot n0 + 1, as the current SIB1 scheduler implementation allocates the SIB1 PDCCH on the slot n0 + 1.
  // (ii) The slots n0 have been pre-computed based on TS 38.213, Section 13.

  // SCS Common: 15kHz.
  // With TDD pattern {period=10, dl=5, ul=4}, slots >= 5 fall in flexible/UL range.
  // Only ss0 indices that map n0+1 into [0,4] are valid.
  std::array<unsigned, MAX_NUM_BEAMS> sib1_slots = {3};
  uint8_t                             l_max      = 4U;
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 2U, 0b1000, l_max, 20, ocudu::duplex_mode::TDD);

  // 10Mhz Carrier BW.
  sib1_slots = {3};
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 2U, 0b1000, l_max, 10, ocudu::duplex_mode::TDD);

  // SCS Common: 30kHz, L_max = 8.
  sib1_slots = {11};
  l_max      = 8U;
  test_sib1_scheduler(subcarrier_spacing::kHz30, sib1_slots, 9U, 0b10000000, l_max, 10, ocudu::duplex_mode::TDD);

  sib1_slots = {11};
  // pdcch_config_sib1 = 4U => { coreset0 = 0U, searchspace0 = 4U).
  test_sib1_scheduler(subcarrier_spacing::kHz30, sib1_slots, 4U, 0b10000000, l_max, 10, ocudu::duplex_mode::TDD);
}

TEST(sib1_scheduler_test, test_sib1_periodicity)
{
  // Test the SIB1 scheduler periodicity for different combinations of SIB1 retx perdiod and SSB period values.
  // This test uses a standard set of values for SCS, searchSpaceSetZero and SSB bitmap.
  test_sib1_periodicity(sib1_rtx_periodicity::ms5, ssb_periodicity::ms40);
  test_sib1_periodicity(sib1_rtx_periodicity::ms80, ssb_periodicity::ms20);
  test_sib1_periodicity(sib1_rtx_periodicity::ms10, ssb_periodicity::ms10);
  test_sib1_periodicity(sib1_rtx_periodicity::ms20, ssb_periodicity::ms80);
  test_sib1_periodicity(sib1_rtx_periodicity::ms40, ssb_periodicity::ms10);
  test_sib1_periodicity(sib1_rtx_periodicity::ms40, ssb_periodicity::ms10);
  test_sib1_periodicity(sib1_rtx_periodicity::ms160, ssb_periodicity::ms80);
  test_sib1_periodicity(sib1_rtx_periodicity::ms80, ssb_periodicity::ms160);
}

TEST(sib1_scheduler_test, test_sib1_ssb_collision_for_15khz_scs)
{
  // TEST SIB1/SSB collision on the resource grid.
  // SCS 15kHz.
  constexpr subcarrier_spacing scs = subcarrier_spacing::kHz15;
  // This can be any frequency such that the DL band has SSB SCS 15kHz (case A, in this case).
  constexpr arfcn_t freq_arfcn = 536020;

  test_sib_1_pdsch_collisions(freq_arfcn, scs, bs_channel_bandwidth::MHz20);
}

TEST(sib1_scheduler_test, test_sib1_ssb_collision_for_30khz_scs)
{
  // TEST SIB1/SSB collision on the resource grid.
  // SCS 30kHz.
  constexpr subcarrier_spacing scs = subcarrier_spacing::kHz30;
  // This can be any frequency such that the DL band has SSB SCS 30kHz (case B, in this case).
  constexpr arfcn_t freq_arfcn = 176000;

  test_sib_1_pdsch_collisions(freq_arfcn, scs, bs_channel_bandwidth::MHz20);
}

/// Parameters used by partial slot TDD tests.
struct sib1_tdd_partial_slot_test_params {
  subcarrier_spacing      scs;
  tdd_ul_dl_config_common tdd_config;
  uint8_t                 pdcch_config_sib1;
  uint8_t                 ssb_beam_bitmap;
  uint8_t                 l_max;
  uint16_t                carrier_bw_mhz;
  sib1_rtx_periodicity    sib1_rtx_period;
  ssb_periodicity         ssb_period;
};

void PrintTo(const sib1_tdd_partial_slot_test_params& p, std::ostream* os)
{
  *os << "scs=" << to_string(p.scs) << " pdcch_cfg=" << +p.pdcch_config_sib1 << " ssb_bitmap=" << +p.ssb_beam_bitmap
      << " l_max=" << +p.l_max << " bw=" << p.carrier_bw_mhz << " rtx=" << static_cast<int>(p.sib1_rtx_period)
      << " ssb_period=" << static_cast<int>(p.ssb_period);
}

static sched_cell_configuration_request_message
build_sib1_partial_slot_cell_req(const sib1_tdd_partial_slot_test_params& params)
{
  sched_cell_configuration_request_message msg =
      sib1_scheduler_setup::make_cell_cfg_req_for_sib_sched(params.scs,
                                                            params.pdcch_config_sib1,
                                                            params.ssb_beam_bitmap,
                                                            params.l_max,
                                                            params.ssb_period,
                                                            params.carrier_bw_mhz,
                                                            duplex_mode::TDD);
  msg.ran.tdd_cfg = params.tdd_config;
  // Generate PDSCH Time domain allocation based on the partial slot TDD configuration.
  msg.ran.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list =
      time_domain_resource_helper::generate_dedicated_pdsch_td_res_list(
          params.tdd_config,
          msg.ran.dl_cfg_common.init_dl_bwp.generic_params.cp,
          time_domain_resource_helper::calculate_minimum_pdsch_symbol(msg.ran.dl_cfg_common.init_dl_bwp.pdcch_common,
                                                                      std::nullopt));
  msg.ran.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list =
      time_domain_resource_helper::generate_dedicated_pusch_td_res_list(
          params.tdd_config, msg.ran.ul_cfg_common.init_ul_bwp.generic_params.cp, msg.ran.init_bwp.pusch.min_k2);
  return msg;
}

class sib1_tdd_partial_slot_test : public ::testing::TestWithParam<sib1_tdd_partial_slot_test_params>
{
protected:
  const sib1_tdd_partial_slot_test_params params{GetParam()};
};

TEST_P(sib1_tdd_partial_slot_test, successful_sib1_allocation_in_partial_slot)
{
  sib1_scheduler_setup t_bench{sib1_scheduler_setup::make_scheduler_expert_cfg(
                                   {10, aggregation_level::n4, 10, aggregation_level::n4, params.sib1_rtx_period}),
                               build_sib1_partial_slot_cell_req(params)};

  // Determine the expected SIB1 retx periodicity.
  const unsigned expected_sib1_period_ms = to_value(params.sib1_rtx_period) > to_value(params.ssb_period)
                                               ? to_value(params.sib1_rtx_period)
                                               : to_value(params.ssb_period);

  // SIB1 periodicity in slots.
  const unsigned expected_sib1_period_slots = expected_sib1_period_ms * t_bench.next_slot.nof_slots_per_subframe();

  // NOTE: The function assumes that arguments provided results in SIB1 scheduler allocating SIB1 in partial slot.
  const unsigned          sib1_allocation_slot_pattern1 = params.tdd_config.pattern1.nof_dl_slots;
  std::optional<unsigned> sib1_allocation_slot_pattern2;
  if (params.tdd_config.pattern2.has_value()) {
    sib1_allocation_slot_pattern2 = params.tdd_config.pattern2->nof_dl_slots;
  }

  // Run the test for 10000 slots.
  constexpr size_t test_length_slots = 10000;
  for (size_t sl_idx = 0; sl_idx < test_length_slots; sl_idx++) {
    t_bench.run_slot();

    // With the SSB bitmap set 0b10000000, only the SSB and SIB1 for the 1 beams are used; we perform the check for
    // this beam.
    if (((sl_idx % expected_sib1_period_slots) == sib1_allocation_slot_pattern1) or
        (sib1_allocation_slot_pattern2.has_value() and
         (sl_idx % expected_sib1_period_slots) == sib1_allocation_slot_pattern2.value())) {
      // Verify that the scheduler results list contain 1 element with the SIB1 information.
      TESTASSERT_EQ(1, t_bench.res_grid[0].result.dl.bc.sibs.size());
    } else {
      TESTASSERT(t_bench.res_grid[0].result.dl.bc.sibs.empty());
    }
  }
}

INSTANTIATE_TEST_SUITE_P(sib1_scheduler_test,
                         sib1_tdd_partial_slot_test,
                         ::testing::Values(sib1_tdd_partial_slot_test_params{
                             .scs               = subcarrier_spacing::kHz30,
                             .tdd_config        = tdd_ul_dl_config_common{.ref_scs  = subcarrier_spacing::kHz30,
                                                                          .pattern1 = {.dl_ul_tx_period_nof_slots = 20,
                                                                                       .nof_dl_slots              = 11,
                                                                                       .nof_dl_symbols            = 8,
                                                                                       .nof_ul_slots              = 8,
                                                                                       .nof_ul_symbols            = 0}},
                             .pdcch_config_sib1 = 4U,
                             .ssb_beam_bitmap   = 0b10000000,
                             .l_max             = 8U,
                             .carrier_bw_mhz    = 20,
                             .sib1_rtx_period   = sib1_rtx_periodicity::ms10,
                             .ssb_period        = ssb_periodicity::ms10}));
