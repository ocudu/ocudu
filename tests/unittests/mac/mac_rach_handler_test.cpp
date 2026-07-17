// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/mac/mac_sched/mac_rach_handler.h"
#include "lib/mac/rnti_manager.h"
#include "mac_test_helpers.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/band_helper.h"
#include "ocudu/ran/prach/prach_configuration.h"
#include "ocudu/ran/prach/ra_helper.h"
#include <gtest/gtest.h>

using namespace ocudu;

class mac_rach_handler_test : public ::testing::Test
{
protected:
  mac_rach_handler_test() :
    logger(ocudulog::fetch_basic_logger("MAC")),
    params(cell_config_builder_profiles::create(duplex_mode::TDD)),
    sched_cfg([this]() {
      auto cfg = sched_config_helper::make_default_sched_cell_configuration_request(params);
      // Leave some preambles for CFRA.
      cfg.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common->nof_cb_preambles_per_ssb = 56;
      return cfg;
    }()),
    handler(sched, rnti_mng, logger),
    cell_handler(handler.add_cell(sched_cfg))
  {
  }

  mac_rach_indication make_rach_indication(uint8_t preamble_id) const
  {
    mac_rach_indication rach;
    rach.slot_rx                                 = {to_numerology_value(params.scs_common), 0};
    mac_rach_indication::rach_occasion& occ      = rach.occasions.emplace_back();
    occ.frequency_index                          = 0;
    occ.slot_index                               = 0;
    occ.start_symbol                             = 0;
    mac_rach_indication::rach_preamble& preamble = occ.preambles.emplace_back();
    preamble.index                               = preamble_id;
    return rach;
  }

  uint8_t create_cb_preamble() const
  {
    return test_rng::uniform_int(
        0U, sched_cfg.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common->nof_cb_preambles_per_ssb - 1U);
  }

  uint8_t create_cf_preamble() const
  {
    return test_rng::uniform_int<unsigned>(
        sched_cfg.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common->nof_cb_preambles_per_ssb,
        sched_cfg.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common->total_nof_ra_preambles - 1U);
  }

  ocudulog::basic_logger&                  logger;
  test_helpers::dummy_mac_scheduler        sched;
  rnti_manager                             rnti_mng;
  cell_config_builder_params               params;
  sched_cell_configuration_request_message sched_cfg;

  mac_rach_handler            handler;
  mac_cell_rach_handler_impl& cell_handler;
};

TEST_F(mac_rach_handler_test, when_cb_rach_detected_then_tc_rnti_is_allocated_and_forwarded_to_sched)
{
  // Report detected RACH.
  mac_rach_indication rach = make_rach_indication(create_cb_preamble());
  cell_handler.handle_rach_indication(rach);

  // RACH is forwarded to scheduler with an allocated TC-RNTI.
  ASSERT_TRUE(sched.last_rach_ind.has_value());
  ASSERT_EQ(sched.last_rach_ind.value().occasions.size(), 1);
  ASSERT_EQ(sched.last_rach_ind.value().occasions[0].preambles.size(), 1);
  ASSERT_EQ(sched.last_rach_ind.value().occasions[0].preambles[0].preamble_id, rach.occasions[0].preambles[0].index);
  ASSERT_EQ(sched.last_rach_ind.value().occasions[0].preambles[0].tc_rnti, to_rnti(0x4601));
}

TEST_F(mac_rach_handler_test, when_cf_rach_detected_then_allocated_crnti_is_used)
{
  uint8_t cfra_preamble = create_cf_preamble();
  ASSERT_TRUE(cell_handler.handle_cfra_allocation(cfra_preamble, to_du_ue_index(0), to_rnti(0x5555)));

  // Report detected RACH.
  mac_rach_indication rach = make_rach_indication(cfra_preamble);
  cell_handler.handle_rach_indication(rach);

  ASSERT_TRUE(sched.last_rach_ind.has_value());
  ASSERT_EQ(sched.last_rach_ind.value().occasions.size(), 1);
  ASSERT_EQ(sched.last_rach_ind.value().occasions[0].preambles[0].preamble_id, cfra_preamble);
  ASSERT_EQ(sched.last_rach_ind.value().occasions[0].preambles[0].tc_rnti, to_rnti(0x5555));
}

TEST_F(mac_rach_handler_test, when_cf_preamble_detected_but_no_cfra_ue_exists_then_rach_is_not_forwarded)
{
  uint8_t cfra_preamble = create_cf_preamble();

  // Report detected RACH.
  mac_rach_indication rach = make_rach_indication(cfra_preamble);
  cell_handler.handle_rach_indication(rach);

  ASSERT_FALSE(sched.last_rach_ind.has_value());
}

TEST_F(mac_rach_handler_test, when_cf_preamble_is_deallocated_then_cf_rach_is_not_forwarded)
{
  uint8_t cfra_preamble = create_cf_preamble();
  ASSERT_TRUE(cell_handler.handle_cfra_allocation(cfra_preamble, to_du_ue_index(0), to_rnti(0x5555)));
  cell_handler.handle_cfra_deallocation(to_du_ue_index(0));

  // Report detected RACH.
  mac_rach_indication rach = make_rach_indication(cfra_preamble);
  cell_handler.handle_rach_indication(rach);

  ASSERT_FALSE(sched.last_rach_ind.has_value());
}

TEST_F(mac_rach_handler_test, when_same_cf_preamble_is_allocated_multiple_times_then_allocation_fails)
{
  uint8_t cfra_preamble = create_cf_preamble();
  ASSERT_TRUE(cell_handler.handle_cfra_allocation(cfra_preamble, to_du_ue_index(0), to_rnti(0x5555)));
  ASSERT_FALSE(cell_handler.handle_cfra_allocation(cfra_preamble, to_du_ue_index(1), to_rnti(0x5556)));
}

/// \brief Tests for the (RA-RNTI, RAPID) -> TC-RNTI resolution used to correctly identify the UE that sent a 2-step
/// RACH (MsgA) CCCH message.
///
/// Per TS 38.211, 6.3.1.1, MsgA PUSCH is decoded by the lower layers using the RA-RNTI of its PRACH occasion, not
/// the TC-RNTI mac_rach_handler allocated for the preamble. Since the RA-RNTI recurs periodically (it only depends
/// on the occasion's slot/symbol/frequency index), MAC must resolve it back to the real TC-RNTI before treating an
/// incoming CCCH message as coming from a specific UE -- see pdu_rx_handler::handle_ccch_msg.
class mac_rach_handler_msga_test : public ::testing::Test
{
protected:
  static constexpr unsigned MSGA_CB_PREAMBLES = 4;

  mac_rach_handler_msga_test() :
    logger(ocudulog::fetch_basic_logger("MAC")),
    params(cell_config_builder_profiles::create(duplex_mode::TDD)),
    sched_cfg([this]() {
      auto  cfg  = sched_config_helper::make_default_sched_cell_configuration_request(params);
      auto& rach = *cfg.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common;
      // Reserve preambles [56, 60) for 2-step CB RACH, leaving [60, 64) for CFRA.
      rach.nof_cb_preambles_per_ssb = 56;
      rach.two_step_rach_cfg.emplace();
      rach.two_step_rach_cfg->cb_preambles_per_ssb_per_shared_ro = MSGA_CB_PREAMBLES;
      return cfg;
    }()),
    handler(sched, rnti_mng, logger),
    cell_handler(handler.add_cell(sched_cfg))
  {
  }

  /// First preamble ID reserved for 2-step CB RACH, as configured above.
  static constexpr uint8_t MSGA_PREAMBLE_ID = 56;

  mac_rach_indication make_msga_rach_indication(slot_point slot_rx, uint8_t preamble_id) const
  {
    mac_rach_indication rach;
    rach.slot_rx                                 = slot_rx;
    mac_rach_indication::rach_occasion& occ      = rach.occasions.emplace_back();
    occ.frequency_index                          = 0;
    occ.slot_index                               = 0;
    occ.start_symbol                             = 0;
    mac_rach_indication::rach_preamble& preamble = occ.preambles.emplace_back();
    preamble.index                               = preamble_id;
    return rach;
  }

  /// Computes the RA-RNTI expected for a RACH occasion at (slot_rx, start_symbol=0, frequency_index=0), mirroring
  /// the exact same logic used by mac_cell_rach_handler_impl.
  rnti_t expected_ra_rnti(slot_point slot_rx) const
  {
    const auto&               rach_cfg = *sched_cfg.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common;
    const prach_configuration prach_cfg =
        prach_configuration_get(band_helper::get_freq_range(sched_cfg.ran.dl_carrier.band),
                                band_helper::get_duplex_mode(sched_cfg.ran.dl_carrier.band),
                                rach_cfg.rach_cfg_generic.prach_config_index);
    const unsigned slot_idx = is_long_preamble(prach_cfg.format) ? slot_rx.subframe_index() : slot_rx.slot_index();
    return ra_helper::get_ra_rnti(slot_idx, /*symbol_index=*/0, /*frequency_index=*/0);
  }

  ocudulog::basic_logger&                  logger;
  test_helpers::dummy_mac_scheduler        sched;
  rnti_manager                             rnti_mng;
  cell_config_builder_params               params;
  sched_cell_configuration_request_message sched_cfg;

  mac_rach_handler            handler;
  mac_cell_rach_handler_impl& cell_handler;
};

TEST_F(mac_rach_handler_msga_test, when_msga_preamble_detected_then_tc_rnti_resolves_via_ra_rnti_and_rapid)
{
  const slot_point    slot_rx = {to_numerology_value(params.scs_common), 0};
  mac_rach_indication rach    = make_msga_rach_indication(slot_rx, MSGA_PREAMBLE_ID);
  cell_handler.handle_rach_indication(rach);

  ASSERT_TRUE(sched.last_rach_ind.has_value());
  const rnti_t alloc_tc_rnti = sched.last_rach_ind.value().occasions[0].preambles[0].tc_rnti;

  const rnti_t                ra_rnti  = expected_ra_rnti(slot_rx);
  const std::optional<rnti_t> resolved = cell_handler.resolve_msga_tc_rnti(ra_rnti, MSGA_PREAMBLE_ID, slot_rx);
  ASSERT_TRUE(resolved.has_value());
  ASSERT_EQ(*resolved, alloc_tc_rnti);
}

TEST_F(mac_rach_handler_msga_test, when_tc_rnti_is_resolved_then_it_can_be_resolved_again_before_expiry)
{
  const slot_point slot_rx = {to_numerology_value(params.scs_common), 0};
  cell_handler.handle_rach_indication(make_msga_rach_indication(slot_rx, MSGA_PREAMBLE_ID));

  const rnti_t                ra_rnti = expected_ra_rnti(slot_rx);
  const std::optional<rnti_t> first   = cell_handler.resolve_msga_tc_rnti(ra_rnti, MSGA_PREAMBLE_ID, slot_rx);
  ASSERT_TRUE(first.has_value());
  // Resolution is a plain atomic read (not a consuming operation, to keep this latency-critical path lock-free and
  // wait-free), so a second lookup for the same, still-valid entry returns the same result.
  const std::optional<rnti_t> second = cell_handler.resolve_msga_tc_rnti(ra_rnti, MSGA_PREAMBLE_ID, slot_rx);
  ASSERT_TRUE(second.has_value());
  ASSERT_EQ(*first, *second);
}

TEST_F(mac_rach_handler_msga_test, when_non_msga_preamble_detected_then_it_is_never_resolvable)
{
  // Preamble 0 is a 4-step CB preamble, outside the 2-step CB range [56, 60).
  const slot_point slot_rx = {to_numerology_value(params.scs_common), 0};
  cell_handler.handle_rach_indication(make_msga_rach_indication(slot_rx, 0));

  const rnti_t ra_rnti = expected_ra_rnti(slot_rx);
  ASSERT_FALSE(cell_handler.resolve_msga_tc_rnti(ra_rnti, 0, slot_rx).has_value())
      << "A 4-step preamble must never populate the MsgA TC-RNTI table -- its FAPI indications never carry rapid "
         "in the first place, so nothing should ever be looked up for it";
}

/// \brief Reproduces the exact scenario behind GitLab issue #575: two 2-step RACH attempts land on the same
/// recurring PRACH occasion (same slot/symbol/frequency index -> same RA-RNTI) before the first is resolved.
///
/// Without last-writer-wins semantics, the first (now-stale) TC-RNTI would still be returned, and the real UE that
/// most recently transmitted MsgA would never get its C-RNTI, while a second, later 2-step attempt reusing the same
/// RA-RNTI would silently resolve to the wrong (evicted) UE.
TEST_F(mac_rach_handler_msga_test, when_same_ra_rnti_and_rapid_recur_before_resolution_then_latest_tc_rnti_wins)
{
  const slot_point slot_rx = {to_numerology_value(params.scs_common), 0};

  cell_handler.handle_rach_indication(make_msga_rach_indication(slot_rx, MSGA_PREAMBLE_ID));
  ASSERT_TRUE(sched.last_rach_ind.has_value());
  const rnti_t first_tc_rnti = sched.last_rach_ind.value().occasions[0].preambles[0].tc_rnti;

  // A second, independent 2-step RACH attempt lands on the exact same PRACH occasion (e.g. the next recurrence of
  // the same time/frequency resource) and picks the same preamble, before the first is ever resolved.
  cell_handler.handle_rach_indication(make_msga_rach_indication(slot_rx, MSGA_PREAMBLE_ID));
  ASSERT_TRUE(sched.last_rach_ind.has_value());
  const rnti_t second_tc_rnti = sched.last_rach_ind.value().occasions[0].preambles[0].tc_rnti;
  ASSERT_NE(first_tc_rnti, second_tc_rnti) << "Test setup error: rnti_manager should allocate a fresh TC-RNTI";

  const rnti_t                ra_rnti  = expected_ra_rnti(slot_rx);
  const std::optional<rnti_t> resolved = cell_handler.resolve_msga_tc_rnti(ra_rnti, MSGA_PREAMBLE_ID, slot_rx);
  ASSERT_TRUE(resolved.has_value());
  ASSERT_EQ(*resolved, second_tc_rnti) << "The most recent 2-step attempt on this RA-RNTI must win";

  // The stale first attempt's TC-RNTI must never resurface: the single slot for this RAPID was overwritten, not
  // appended to, so there is no leftover entry to (mis)resolve.
  const std::optional<rnti_t> resolved_again = cell_handler.resolve_msga_tc_rnti(ra_rnti, MSGA_PREAMBLE_ID, slot_rx);
  ASSERT_TRUE(resolved_again.has_value());
  ASSERT_EQ(*resolved_again, second_tc_rnti);
}

/// \brief Tests for the TC-RNTI -> UE Contention Resolution Identity registration used to echo the MsgA CCCH SDU
/// back in a successRAR (see rar_pdu_assembler::encode_successrar_payload).
///
/// Storage is direct-indexed by TC-RNTI rather than RAPID: unlike RAPID or RA-RNTI, a TC-RNTI is allocated fresh per
/// preamble detection and not reused while the attempt is in flight, so it alone is an unambiguous key: no RAPID
/// cross-check is needed, even if two attempts share the same, recurring RAPID.
class mac_rach_handler_con_res_id_test : public mac_rach_handler_msga_test
{
protected:
  static ue_con_res_id_t make_con_res_id(uint8_t seed)
  {
    ue_con_res_id_t id;
    for (unsigned i = 0; i != id.size(); ++i) {
      id[i] = static_cast<uint8_t>(seed + i);
    }
    return id;
  }
};

TEST_F(mac_rach_handler_con_res_id_test, when_con_res_id_registered_then_it_resolves_with_matching_tc_rnti)
{
  const slot_point slot_rx = {to_numerology_value(params.scs_common), 0};
  cell_handler.handle_rach_indication(make_msga_rach_indication(slot_rx, MSGA_PREAMBLE_ID));
  const rnti_t tc_rnti = sched.last_rach_ind.value().occasions[0].preambles[0].tc_rnti;

  const ue_con_res_id_t con_res_id = make_con_res_id(0x10);
  cell_handler.add_msga_con_res_id(tc_rnti, con_res_id);

  const std::optional<ue_con_res_id_t> resolved = cell_handler.resolve_msga_con_res_id(tc_rnti);
  ASSERT_TRUE(resolved.has_value());
  ASSERT_EQ(*resolved, con_res_id);
}

TEST_F(mac_rach_handler_con_res_id_test, when_con_res_id_resolved_then_entry_is_consumed)
{
  const slot_point slot_rx = {to_numerology_value(params.scs_common), 0};
  cell_handler.handle_rach_indication(make_msga_rach_indication(slot_rx, MSGA_PREAMBLE_ID));
  const rnti_t tc_rnti = sched.last_rach_ind.value().occasions[0].preambles[0].tc_rnti;

  cell_handler.add_msga_con_res_id(tc_rnti, make_con_res_id(0x20));

  ASSERT_TRUE(cell_handler.resolve_msga_con_res_id(tc_rnti).has_value());
  // A successRAR is only ever encoded once per preamble detection, so a repeated resolution must not find the
  // entry again.
  ASSERT_FALSE(cell_handler.resolve_msga_con_res_id(tc_rnti).has_value());
}

TEST_F(mac_rach_handler_con_res_id_test, when_two_attempts_share_the_same_rapid_then_con_res_ids_never_cross_over)
{
  const slot_point slot_rx = {to_numerology_value(params.scs_common), 0};

  cell_handler.handle_rach_indication(make_msga_rach_indication(slot_rx, MSGA_PREAMBLE_ID));
  const rnti_t          first_tc_rnti    = sched.last_rach_ind.value().occasions[0].preambles[0].tc_rnti;
  const ue_con_res_id_t first_con_res_id = make_con_res_id(0x30);
  cell_handler.add_msga_con_res_id(first_tc_rnti, first_con_res_id);

  // A second, unrelated preamble detection reuses the same RAPID (its recurring PRACH occasion) before the first
  // UE's successRAR is ever encoded.
  cell_handler.handle_rach_indication(make_msga_rach_indication(slot_rx, MSGA_PREAMBLE_ID));
  const rnti_t second_tc_rnti = sched.last_rach_ind.value().occasions[0].preambles[0].tc_rnti;
  ASSERT_NE(first_tc_rnti, second_tc_rnti) << "Test setup error: rnti_manager should allocate a fresh TC-RNTI";
  const ue_con_res_id_t second_con_res_id = make_con_res_id(0x40);
  cell_handler.add_msga_con_res_id(second_tc_rnti, second_con_res_id);

  // Each attempt resolves only its own Contention Resolution Id, since storage is keyed by TC-RNTI, not RAPID.
  const std::optional<ue_con_res_id_t> first_resolved  = cell_handler.resolve_msga_con_res_id(first_tc_rnti);
  const std::optional<ue_con_res_id_t> second_resolved = cell_handler.resolve_msga_con_res_id(second_tc_rnti);
  ASSERT_TRUE(first_resolved.has_value());
  ASSERT_EQ(*first_resolved, first_con_res_id);
  ASSERT_TRUE(second_resolved.has_value());
  ASSERT_EQ(*second_resolved, second_con_res_id);
}
