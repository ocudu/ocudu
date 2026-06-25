// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/uci_scheduling/uci_indication_selector.h"
#include "tests/test_doubles/utils/test_rng.h"
#include <algorithm>
#include <gtest/gtest.h>
#include <vector>

using namespace ocudu;

namespace {

class test_timeout_notifier : public uci_indication_timeout_notifier
{
public:
  struct timeout_event {
    slot_point sl_rx;
    rnti_t     crnti;
    uci_action action;
  };

  void on_timeout(slot_point sl_rx, rnti_t crnti, const uci_action& action) override
  {
    events.push_back({sl_rx, crnti, action});
  }

  std::vector<timeout_event> events;
};

class uci_indication_selector_test : public ::testing::Test
{
protected:
  static constexpr unsigned timeout_slots = 16;
  static constexpr rnti_t   first_rnti    = to_rnti(0x4601);
  static constexpr rnti_t   second_rnti   = to_rnti(0x4602);

  explicit uci_indication_selector_test(std::optional<float> pucch_sinr_threshold_dB = std::nullopt) :
    selector(timeout_notifier, timeout_slots, MAX_PUCCH_PDUS_PER_SLOT, pucch_sinr_threshold_dB)
  {
  }

  static sched_result make_sched_result(std::initializer_list<pucch_info> pucchs)
  {
    sched_result result{};
    for (const auto& pucch : pucchs) {
      result.ul.pucchs.emplace(pucch);
    }
    return result;
  }

  static pucch_info make_pucch_grant(rnti_t       crnti,
                                     pucch_format format,
                                     unsigned     nof_harq_bits,
                                     bool         sr           = false,
                                     unsigned     nof_csi_bits = 0)
  {
    pucch_info pucch{};
    pucch.crnti                       = crnti;
    pucch.bwp_cfg                     = nullptr;
    pucch.uci_bits.harq_ack_nof_bits  = nof_harq_bits;
    pucch.uci_bits.sr_bits            = sr ? sr_nof_bits::one : sr_nof_bits::no_sr;
    pucch.uci_bits.csi_part1_nof_bits = nof_csi_bits;
    pucch.set_format(format);
    return pucch;
  }

  static uci_indication::uci_pdu make_f0_or_f1_pdu(rnti_t                                            crnti,
                                                   std::initializer_list<mac_harq_ack_report_status> harq_bits,
                                                   bool                 sr_detected = false,
                                                   std::optional<float> snr_dB      = std::nullopt)
  {
    uci_indication::uci_pdu pdu{};
    pdu.ue_index = to_du_ue_index(0);
    pdu.crnti    = crnti;
    auto& f0f1   = pdu.pdu.emplace<uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu>();

    f0f1.sr_detected = sr_detected;
    f0f1.ul_sinr_dB  = snr_dB;
    f0f1.harqs.resize(harq_bits.size());
    std::copy(harq_bits.begin(), harq_bits.end(), f0f1.harqs.begin());

    return pdu;
  }

  static uci_indication::uci_pdu make_f2_pdu(rnti_t                                            crnti,
                                             std::initializer_list<mac_harq_ack_report_status> harq_bits,
                                             std::initializer_list<bool>                       sr_bits,
                                             bool                                              has_csi = false,
                                             std::optional<float>                              snr_dB  = std::nullopt)
  {
    uci_indication::uci_pdu pdu{};
    pdu.ue_index = to_du_ue_index(0);
    pdu.crnti    = crnti;
    auto& f2     = pdu.pdu.emplace<uci_indication::uci_pdu::uci_pucch_f2_or_f3_or_f4_pdu>();

    f2.harqs.resize(harq_bits.size());
    std::copy(harq_bits.begin(), harq_bits.end(), f2.harqs.begin());

    if (sr_bits.size() > 0) {
      f2.sr_info = bounded_bitset<uci_indication::uci_pdu::uci_pucch_f2_or_f3_or_f4_pdu::MAX_SR_PAYLOAD_SIZE_BITS>(
          sr_bits.begin(), sr_bits.end());
    }

    if (has_csi) {
      f2.csi.emplace();
      f2.csi->valid = true;
      f2.csi->first_tb_wideband_cqi.emplace(15);
    }

    f2.ul_sinr_dB = snr_dB;

    return pdu;
  }

  void advance_slots(unsigned nof_slots)
  {
    for (slot_point slot_timeout = next_sl_tx + nof_slots; next_sl_tx != slot_timeout; ++next_sl_tx) {
      selector.handle_result(next_sl_tx, make_sched_result({}));
    }
  }

  test_timeout_notifier   timeout_notifier;
  uci_indication_selector selector;

  slot_point next_sl_tx{
      subcarrier_spacing::kHz30,
      test_rng::uniform_int<unsigned>(
          0,
          NOF_SFNS * NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(subcarrier_spacing::kHz30) - 1)};
};

TEST_F(uci_indication_selector_test, returns_selected_action_for_matching_pdu_without_harq_bits)
{
  // Event: One PUCCH grant got scheduled.
  selector.handle_result(next_sl_tx,
                         make_sched_result({make_pucch_grant(first_rnti, pucch_format::FORMAT_2, 0, false, 4)}));

  // Event: Respective UCI gets processed.
  auto action = selector.handle_uci_ind_pdu(next_sl_tx, make_f2_pdu(first_rnti, {}, {false}, true, 10.0F));

  // Test: UCI action matches UCI PDU content.
  ASSERT_TRUE(action.has_value());
  ASSERT_TRUE(action->uci_valid);
  ASSERT_FALSE(action->sr_detected);
  ASSERT_TRUE(action->harq_ack_bits.empty());
  ASSERT_TRUE(action->csi.has_value());
  ASSERT_TRUE(action->csi->first_tb_wideband_cqi.has_value());

  // Test: No timeout is ever triggered.
  ++next_sl_tx;
  advance_slots(timeout_slots);
  ASSERT_TRUE(timeout_notifier.events.empty());
}

TEST_F(uci_indication_selector_test, returns_selected_action_for_matching_pdu_with_harq_bits)
{
  // Event: One PUCCH grant got scheduled.
  selector.handle_result(next_sl_tx, make_sched_result({make_pucch_grant(first_rnti, pucch_format::FORMAT_1, 2)}));

  // Event: Respective UCI gets processed.
  auto action = selector.handle_uci_ind_pdu(
      next_sl_tx,
      make_f0_or_f1_pdu(first_rnti, {mac_harq_ack_report_status::ack, mac_harq_ack_report_status::nack}, true, 10.0F));

  // Test: UCI action matches UCI PDU content.
  ASSERT_TRUE(action.has_value());
  ASSERT_TRUE(action->sr_detected);
  ASSERT_EQ(action->harq_ack_bits.size(), 2U);
  ASSERT_TRUE(action->harq_ack_bits.test(0));
  ASSERT_FALSE(action->harq_ack_bits.test(1));
  ASSERT_TRUE(action->ul_sinr_dB.has_value());
  ASSERT_TRUE(timeout_notifier.events.empty());

  // Test: No timeout is ever triggered.
  ++next_sl_tx;
  advance_slots(timeout_slots);
  ASSERT_TRUE(timeout_notifier.events.empty());
}

TEST_F(uci_indication_selector_test, returns_nullopt_when_rnti_is_not_found)
{
  // Event: PUCCH grant with HARQ-ACK bits got scheduled.
  selector.handle_result(next_sl_tx, make_sched_result({make_pucch_grant(first_rnti, pucch_format::FORMAT_1, 1)}));

  // Event: UCI arrives but with wrong C-RNTI.
  auto action =
      selector.handle_uci_ind_pdu(next_sl_tx, make_f0_or_f1_pdu(second_rnti, {mac_harq_ack_report_status::ack}));

  // Test: No action is performed.
  ASSERT_FALSE(action.has_value());
  ASSERT_TRUE(timeout_notifier.events.empty());
}

TEST_F(uci_indication_selector_test, handles_out_of_order_uci_pdu_arrival)
{
  selector.handle_result(next_sl_tx,
                         make_sched_result({make_pucch_grant(first_rnti, pucch_format::FORMAT_1, 1),
                                            make_pucch_grant(second_rnti, pucch_format::FORMAT_1, 1)}));

  auto second_rnti_action =
      selector.handle_uci_ind_pdu(next_sl_tx, make_f0_or_f1_pdu(second_rnti, {mac_harq_ack_report_status::ack}));
  auto first_rnti_action =
      selector.handle_uci_ind_pdu(next_sl_tx, make_f0_or_f1_pdu(first_rnti, {mac_harq_ack_report_status::nack}));

  ASSERT_TRUE(first_rnti_action.has_value());
  ASSERT_FALSE(first_rnti_action->harq_ack_bits.test(0));
  ASSERT_TRUE(second_rnti_action.has_value());
  ASSERT_TRUE(second_rnti_action->harq_ack_bits.test(0));
}

TEST_F(uci_indication_selector_test, triggers_timeout_when_no_uci_indication_arrives)
{
  // Event: PUCCH grant with HARQ-ACK bits got scheduled.
  slot_point start_sl_tx = next_sl_tx;
  selector.handle_result(next_sl_tx++, make_sched_result({make_pucch_grant(first_rnti, pucch_format::FORMAT_1, 2)}));

  // Event: No timeout event gets triggered before deadline expires.
  for (slot_point last_slot = next_sl_tx + timeout_slots; next_sl_tx != last_slot; ++next_sl_tx) {
    ASSERT_TRUE(timeout_notifier.events.empty());
    selector.handle_result(next_sl_tx, make_sched_result({}));
  }

  // Test: Timeout expired.
  ASSERT_EQ(timeout_notifier.events.size(), 1U);
  auto& ev = timeout_notifier.events.front();
  ASSERT_EQ(ev.sl_rx, start_sl_tx);
  ASSERT_EQ(ev.crnti, first_rnti);
  ASSERT_FALSE(ev.action.uci_valid);
  ASSERT_EQ(ev.action.harq_ack_bits.size(), 2U);
  ASSERT_TRUE(ev.action.harq_ack_bits.none());
}

TEST_F(uci_indication_selector_test, large_slot_jump_beyond_timeout_wheel_forces_timeout_and_clears_state)
{
  const slot_point uci_slot = next_sl_tx;
  selector.handle_result(next_sl_tx, make_sched_result({make_pucch_grant(first_rnti, pucch_format::FORMAT_1, 1)}));

  next_sl_tx = next_sl_tx + timeout_slots + 1;
  selector.handle_result(next_sl_tx, make_sched_result({}));

  ASSERT_EQ(timeout_notifier.events.size(), 1U);
  const auto& ev = timeout_notifier.events.front();
  ASSERT_EQ(ev.sl_rx, uci_slot);
  ASSERT_EQ(ev.crnti, first_rnti);
  ASSERT_FALSE(ev.action.uci_valid);
  ASSERT_EQ(ev.action.harq_ack_bits.size(), 1U);
  ASSERT_TRUE(ev.action.harq_ack_bits.none());

  auto late_action =
      selector.handle_uci_ind_pdu(uci_slot, make_f0_or_f1_pdu(first_rnti, {mac_harq_ack_report_status::ack}));
  ASSERT_FALSE(late_action.has_value());

  timeout_notifier.events.clear();
  selector.handle_result(++next_sl_tx, make_sched_result({}));
  ASSERT_TRUE(timeout_notifier.events.empty());
}

TEST_F(uci_indication_selector_test, large_slot_jump_beyond_timeout_wheel_preserves_partial_uci_action)
{
  const slot_point uci_slot = next_sl_tx;
  selector.handle_result(next_sl_tx,
                         make_sched_result({make_pucch_grant(first_rnti, pucch_format::FORMAT_1, 2),
                                            make_pucch_grant(first_rnti, pucch_format::FORMAT_1, 2, true)}));

  auto first_action = selector.handle_uci_ind_pdu(
      uci_slot,
      make_f0_or_f1_pdu(first_rnti, {mac_harq_ack_report_status::ack, mac_harq_ack_report_status::nack}, true, 10.0F));
  ASSERT_FALSE(first_action.has_value());

  next_sl_tx = next_sl_tx + timeout_slots + 1;
  selector.handle_result(next_sl_tx, make_sched_result({}));

  ASSERT_EQ(timeout_notifier.events.size(), 1U);
  const auto& ev = timeout_notifier.events.front();
  ASSERT_EQ(ev.sl_rx, uci_slot);
  ASSERT_EQ(ev.crnti, first_rnti);
  ASSERT_TRUE(ev.action.uci_valid);
  ASSERT_EQ(ev.action.harq_ack_bits.size(), 2U);
  ASSERT_TRUE(ev.action.harq_ack_bits.test(0));
  ASSERT_FALSE(ev.action.harq_ack_bits.test(1));
  ASSERT_TRUE(ev.action.sr_detected);
}

TEST_F(uci_indication_selector_test, selects_pucch_with_highest_snr_for_multiple_expected_pucch_indications)
{
  auto uci_slot = next_sl_tx;
  selector.handle_result(next_sl_tx,
                         make_sched_result({make_pucch_grant(first_rnti, pucch_format::FORMAT_1, 1),
                                            make_pucch_grant(first_rnti, pucch_format::FORMAT_1, 1, true)}));

  const bool     first_is_higher          = test_rng::uniform_int(0, 1) == 0;
  const unsigned second_pucch_delay_slots = test_rng::uniform_int(0, 4);
  const float    first_snr                = first_is_higher ? 10.0 : 1.0;
  const float    second_snr               = first_is_higher ? 1.0 : 10.0;
  auto           first_action             = selector.handle_uci_ind_pdu(
      uci_slot, make_f0_or_f1_pdu(first_rnti, {mac_harq_ack_report_status::ack}, false, first_snr));
  for (unsigned i = 0; i != second_pucch_delay_slots; ++i) {
    ASSERT_TRUE(timeout_notifier.events.empty());
    selector.handle_result(++next_sl_tx, make_sched_result({}));
  }
  auto second_action = selector.handle_uci_ind_pdu(
      uci_slot, make_f0_or_f1_pdu(first_rnti, {mac_harq_ack_report_status::nack}, true, second_snr));

  ASSERT_TRUE(timeout_notifier.events.empty());
  ASSERT_FALSE(first_action.has_value());
  ASSERT_TRUE(second_action.has_value());
  ASSERT_EQ(second_action->harq_ack_bits.size(), 1U);
  ASSERT_EQ(first_is_higher, second_action->harq_ack_bits.test(0));
  ASSERT_NE(first_is_higher, second_action->sr_detected);
}

TEST_F(uci_indication_selector_test, selects_non_dtx_for_multiple_expected_pucch_indications)
{
  auto uci_slot = next_sl_tx;
  selector.handle_result(next_sl_tx,
                         make_sched_result({make_pucch_grant(first_rnti, pucch_format::FORMAT_1, 1),
                                            make_pucch_grant(first_rnti, pucch_format::FORMAT_1, 1, true)}));
  const bool     first_is_dtx             = test_rng::uniform_int(0, 1) == 0;
  const unsigned second_pucch_delay_slots = test_rng::uniform_int(0, 4);
  auto           first_action             = selector.handle_uci_ind_pdu(
      uci_slot,
      make_f0_or_f1_pdu(
          first_rnti, {first_is_dtx ? mac_harq_ack_report_status::dtx : mac_harq_ack_report_status::ack}, false, -10));
  for (unsigned i = 0; i != second_pucch_delay_slots; ++i) {
    ASSERT_TRUE(timeout_notifier.events.empty());
    selector.handle_result(++next_sl_tx, make_sched_result({}));
  }
  auto second_action = selector.handle_uci_ind_pdu(
      uci_slot,
      make_f0_or_f1_pdu(
          first_rnti, {first_is_dtx ? mac_harq_ack_report_status::ack : mac_harq_ack_report_status::dtx}, true, -10));

  ASSERT_TRUE(timeout_notifier.events.empty());
  ASSERT_FALSE(first_action.has_value());
  ASSERT_TRUE(second_action.has_value());
  ASSERT_EQ(second_action->harq_ack_bits.size(), 1U);
  ASSERT_TRUE(second_action->harq_ack_bits.test(0));
  ASSERT_EQ(first_is_dtx, second_action->sr_detected);
}

TEST_F(uci_indication_selector_test, timeout_is_triggered_if_only_one_of_two_pucch_ucis_is_received)
{
  // Event: Two PUCCH grants are scheduled.
  slot_point uci_slot = next_sl_tx;
  selector.handle_result(next_sl_tx,
                         make_sched_result({make_pucch_grant(first_rnti, pucch_format::FORMAT_1, 2, true),
                                            make_pucch_grant(first_rnti, pucch_format::FORMAT_1, 2)}));

  // Event: One slot has elapsed.
  ++next_sl_tx;
  selector.handle_result(next_sl_tx, make_sched_result({}));
  ASSERT_TRUE(timeout_notifier.events.empty());

  // Event: First PUCCH arrives.
  const auto first_bit  = static_cast<mac_harq_ack_report_status>(test_rng::uniform_int(0, 2));
  const auto second_bit = static_cast<mac_harq_ack_report_status>(test_rng::uniform_int(0, 2));
  auto first_action     = selector.handle_uci_ind_pdu(uci_slot, make_f0_or_f1_pdu(first_rnti, {first_bit, second_bit}));
  ASSERT_FALSE(first_action.has_value());

  // Event: Advance slots until short timeout trigger.
  for (slot_point slot_timeout = next_sl_tx + uci_indication_selector::SHORT_PUCCH_TIMEOUT_SLOTS + 1;
       next_sl_tx != slot_timeout;
       ++next_sl_tx) {
    ASSERT_TRUE(timeout_notifier.events.empty());
    selector.handle_result(next_sl_tx, make_sched_result({}));
  }

  // Test Case: Short timeout got triggered.
  ASSERT_EQ(timeout_notifier.events.size(), 1U);
  const auto& ev = timeout_notifier.events.front();
  ASSERT_EQ(ev.sl_rx, uci_slot);
  ASSERT_EQ(ev.crnti, first_rnti);
  const bool expected_uci_valid =
      first_bit != mac_harq_ack_report_status::dtx and second_bit != mac_harq_ack_report_status::dtx;
  ASSERT_EQ(ev.action.uci_valid, expected_uci_valid);
  ASSERT_EQ(ev.action.harq_ack_bits.size(), 2U);
  ASSERT_EQ(ev.action.harq_ack_bits.test(0), expected_uci_valid and first_bit == mac_harq_ack_report_status::ack);
  ASSERT_EQ(ev.action.harq_ack_bits.test(1), expected_uci_valid and second_bit == mac_harq_ack_report_status::ack);
  ASSERT_FALSE(ev.action.sr_detected);

  // Test Case: Ensure timeout does not get triggered again.
  timeout_notifier.events.clear();
  for (slot_point slot_timeout = next_sl_tx + timeout_slots; next_sl_tx != slot_timeout; ++next_sl_tx) {
    selector.handle_result(next_sl_tx, make_sched_result({}));
    ASSERT_TRUE(timeout_notifier.events.empty());
  }
}

TEST_F(uci_indication_selector_test, timeout_is_not_triggered_if_second_pucch_uci_arrives_before_timeout)
{
  slot_point slot_start_tx      = next_sl_tx;
  slot_point second_pucch_sl_rx = next_sl_tx + uci_indication_selector::SHORT_PUCCH_TIMEOUT_SLOTS;

  // Event: Grant with two PUCCHs for same RNTI.
  selector.handle_result(next_sl_tx,
                         make_sched_result({make_pucch_grant(first_rnti, pucch_format::FORMAT_1, 2, true),
                                            make_pucch_grant(first_rnti, pucch_format::FORMAT_1, 2)}));

  // Event: First PUCCH is received.
  auto first_action = selector.handle_uci_ind_pdu(
      next_sl_tx++,
      make_f0_or_f1_pdu(first_rnti, {mac_harq_ack_report_status::ack, mac_harq_ack_report_status::nack}, false, 10.0F));
  ASSERT_FALSE(first_action.has_value());

  // Test Case: No trigger before short timeout.
  for (; next_sl_tx != second_pucch_sl_rx; ++next_sl_tx) {
    selector.handle_result(next_sl_tx, make_sched_result({}));
    ASSERT_TRUE(timeout_notifier.events.empty());
  }

  // Second UCI received before short timeout.
  auto second_action = selector.handle_uci_ind_pdu(
      slot_start_tx,
      make_f0_or_f1_pdu(first_rnti, {mac_harq_ack_report_status::nack, mac_harq_ack_report_status::ack}, false, 1.0F));
  ASSERT_TRUE(second_action.has_value());
  ASSERT_TRUE(second_action->harq_ack_bits.test(0));
  ASSERT_FALSE(second_action->harq_ack_bits.test(1));

  // Test Case: timeout does not get triggered.
  for (slot_point slot_timeout = next_sl_tx + timeout_slots; next_sl_tx != slot_timeout; ++next_sl_tx) {
    selector.handle_result(next_sl_tx, make_sched_result({}));
    ASSERT_TRUE(timeout_notifier.events.empty());
  }
}

TEST_F(uci_indication_selector_test, dtx_pdu_does_not_override_decoded_action)
{
  selector.handle_result(next_sl_tx,
                         make_sched_result({make_pucch_grant(first_rnti, pucch_format::FORMAT_1, 2, true),
                                            make_pucch_grant(first_rnti, pucch_format::FORMAT_1, 2)}));

  auto first_action = selector.handle_uci_ind_pdu(
      next_sl_tx,
      make_f0_or_f1_pdu(first_rnti, {mac_harq_ack_report_status::ack, mac_harq_ack_report_status::nack}, true, 5.0F));
  ASSERT_FALSE(first_action.has_value());

  auto second_action = selector.handle_uci_ind_pdu(
      next_sl_tx,
      make_f0_or_f1_pdu(first_rnti, {mac_harq_ack_report_status::dtx, mac_harq_ack_report_status::dtx}, false, 20.0F));
  ASSERT_TRUE(second_action.has_value());
  ASSERT_EQ(second_action->harq_ack_bits.size(), 2U);
  ASSERT_TRUE(second_action->harq_ack_bits.test(0));
  ASSERT_FALSE(second_action->harq_ack_bits.test(1));
  ASSERT_TRUE(second_action->sr_detected);
  ASSERT_TRUE(timeout_notifier.events.empty());
}

TEST_F(uci_indication_selector_test, multiple_rntis_timeout_in_same_slot)
{
  slot_point start_sl_tx = next_sl_tx;
  selector.handle_result(next_sl_tx++,
                         make_sched_result({make_pucch_grant(first_rnti, pucch_format::FORMAT_1, 1),
                                            make_pucch_grant(second_rnti, pucch_format::FORMAT_1, 2)}));

  for (slot_point last_slot = next_sl_tx + timeout_slots; next_sl_tx != last_slot; ++next_sl_tx) {
    ASSERT_TRUE(timeout_notifier.events.empty());
    selector.handle_result(next_sl_tx, make_sched_result({}));
  }

  ASSERT_EQ(timeout_notifier.events.size(), 2U);

  auto first_it  = std::find_if(timeout_notifier.events.begin(), timeout_notifier.events.end(), [](const auto& event) {
    return event.crnti == first_rnti;
  });
  auto second_it = std::find_if(timeout_notifier.events.begin(), timeout_notifier.events.end(), [](const auto& event) {
    return event.crnti == second_rnti;
  });

  ASSERT_NE(first_it, timeout_notifier.events.end());
  ASSERT_NE(second_it, timeout_notifier.events.end());
  ASSERT_EQ(first_it->sl_rx, start_sl_tx);
  ASSERT_EQ(second_it->sl_rx, start_sl_tx);
  ASSERT_EQ(first_it->action.harq_ack_bits.size(), 1U);
  ASSERT_EQ(second_it->action.harq_ack_bits.size(), 2U);
  ASSERT_FALSE(first_it->action.harq_ack_bits.test(0));
  ASSERT_FALSE(second_it->action.harq_ack_bits.test(0));
  ASSERT_FALSE(second_it->action.harq_ack_bits.test(1));
}

TEST_F(uci_indication_selector_test, wrong_slot_for_correct_rnti_is_ignored)
{
  slot_point start_sl_tx = next_sl_tx;
  selector.handle_result(next_sl_tx++, make_sched_result({make_pucch_grant(first_rnti, pucch_format::FORMAT_1, 1)}));

  // Event: UCI PDU has the wrong slot.
  auto action_wrong_slot = selector.handle_uci_ind_pdu(
      start_sl_tx + 1, make_f0_or_f1_pdu(first_rnti, {mac_harq_ack_report_status::ack}, false, 10.0F));

  // Test Case: No action is produced.
  ASSERT_FALSE(action_wrong_slot.has_value());
  ASSERT_TRUE(timeout_notifier.events.empty());

  // Event: Slots elapse until timeout.
  for (slot_point last_slot = next_sl_tx + timeout_slots; next_sl_tx != last_slot; ++next_sl_tx) {
    ASSERT_TRUE(timeout_notifier.events.empty());
    selector.handle_result(next_sl_tx, make_sched_result({}));
  }

  // Test Case: There should be a timeout.
  ASSERT_EQ(timeout_notifier.events.size(), 1U);
  ASSERT_EQ(timeout_notifier.events.front().crnti, first_rnti);
  ASSERT_EQ(timeout_notifier.events.front().sl_rx, start_sl_tx);
  ASSERT_EQ(timeout_notifier.events.front().action.harq_ack_bits.size(), 1U);
  ASSERT_FALSE(timeout_notifier.events.front().action.harq_ack_bits.test(0));
}

TEST_F(uci_indication_selector_test, on_slot_discard_multiple_ucis_timeout)
{
  // Event: Two PUCCH grants are scheduled.
  auto uci_slot = next_sl_tx;
  selector.handle_result(next_sl_tx,
                         make_sched_result({make_pucch_grant(first_rnti, pucch_format::FORMAT_1, 1),
                                            make_pucch_grant(second_rnti, pucch_format::FORMAT_1, 2)}));

  // Event: Slot elapses.
  ++next_sl_tx;
  selector.handle_result(next_sl_tx, make_sched_result({}));
  ASSERT_TRUE(timeout_notifier.events.empty());

  // Event: Error indication arrived.
  selector.handle_discarded_ucis(uci_slot);

  // Test Case: Timeouts for the respective UCIs are forwarded.
  ASSERT_EQ(timeout_notifier.events.size(), 2U);
  bounded_bitset<2> visited_rntis(2);
  for (unsigned i = 0; i != 2U; ++i) {
    auto& ev = timeout_notifier.events[i];
    if (ev.crnti == first_rnti) {
      visited_rntis.set(0);
      ASSERT_FALSE(ev.action.uci_valid);
      ASSERT_EQ(ev.action.harq_ack_bits.size(), 1);
      ASSERT_TRUE(ev.action.harq_ack_bits.none());
    } else {
      visited_rntis.set(1);
      ASSERT_EQ(ev.crnti, second_rnti);
      ASSERT_FALSE(ev.action.uci_valid);
      ASSERT_EQ(ev.action.harq_ack_bits.size(), 2);
      ASSERT_TRUE(ev.action.harq_ack_bits.none());
    }
  }
  ASSERT_TRUE(visited_rntis.all());
}

class uci_indication_selector_sinr_threshold_test : public uci_indication_selector_test
{
protected:
  static constexpr float sinr_threshold_dB = 0.0F;

  uci_indication_selector_sinr_threshold_test() : uci_indication_selector_test(sinr_threshold_dB) {}

  /// Schedules a PUCCH grant with one HARQ-ACK bit plus SR and processes an ACK+SR UCI PDU with the given SINR.
  std::optional<uci_action> handle_harq_sr_pdu_with_sinr(std::optional<float> sinr_dB)
  {
    selector.handle_result(next_sl_tx,
                           make_sched_result({make_pucch_grant(first_rnti, pucch_format::FORMAT_1, 1, true)}));
    return selector.handle_uci_ind_pdu(next_sl_tx,
                                       make_f0_or_f1_pdu(first_rnti, {mac_harq_ack_report_status::ack}, true, sinr_dB));
  }
};

TEST_F(uci_indication_selector_sinr_threshold_test, pdu_with_sinr_below_threshold_is_invalidated)
{
  auto action = handle_harq_sr_pdu_with_sinr(sinr_threshold_dB - 1.0F);

  // Test: The UCI is treated as if nothing was detected.
  ASSERT_TRUE(action.has_value());
  ASSERT_FALSE(action->uci_valid);
  ASSERT_FALSE(action->sr_detected);
  ASSERT_TRUE(action->harq_ack_bits.none());
}

TEST_F(uci_indication_selector_sinr_threshold_test, pdu_with_sinr_at_threshold_is_valid)
{
  auto action = handle_harq_sr_pdu_with_sinr(sinr_threshold_dB);

  ASSERT_TRUE(action.has_value());
  ASSERT_TRUE(action->uci_valid);
  ASSERT_TRUE(action->sr_detected);
  ASSERT_TRUE(action->harq_ack_bits.test(0));
}

TEST_F(uci_indication_selector_sinr_threshold_test, pdu_without_sinr_is_not_filtered)
{
  auto action = handle_harq_sr_pdu_with_sinr(std::nullopt);

  ASSERT_TRUE(action.has_value());
  ASSERT_TRUE(action->uci_valid);
  ASSERT_TRUE(action->sr_detected);
  ASSERT_TRUE(action->harq_ack_bits.test(0));
}

TEST_F(uci_indication_selector_sinr_threshold_test, csi_pdu_with_sinr_below_threshold_is_discarded)
{
  // Note: CSI-only PUCCHs are not registered for timeout tracking, so no grant needs to be scheduled.
  auto action =
      selector.handle_uci_ind_pdu(next_sl_tx, make_f2_pdu(first_rnti, {}, {false}, true, sinr_threshold_dB - 1.0F));

  ASSERT_TRUE(action.has_value());
  ASSERT_FALSE(action->uci_valid);
  ASSERT_FALSE(action->csi.has_value());
}

TEST_F(uci_indication_selector_sinr_threshold_test, below_threshold_pdu_does_not_override_valid_pdu)
{
  // Event: Two PUCCH grants (HARQ and HARQ+SR) got scheduled for the same RNTI. The first UCI arrives with an SINR
  // above the threshold, the second one below.
  selector.handle_result(next_sl_tx,
                         make_sched_result({make_pucch_grant(first_rnti, pucch_format::FORMAT_1, 1),
                                            make_pucch_grant(first_rnti, pucch_format::FORMAT_1, 1, true)}));
  auto first_action = selector.handle_uci_ind_pdu(
      next_sl_tx, make_f0_or_f1_pdu(first_rnti, {mac_harq_ack_report_status::ack}, false, sinr_threshold_dB + 1.0F));
  auto second_action = selector.handle_uci_ind_pdu(
      next_sl_tx, make_f0_or_f1_pdu(first_rnti, {mac_harq_ack_report_status::nack}, true, sinr_threshold_dB - 1.0F));

  // Test: The action is derived from the valid UCI PDU only.
  ASSERT_FALSE(first_action.has_value());
  ASSERT_TRUE(second_action.has_value());
  ASSERT_TRUE(second_action->uci_valid);
  ASSERT_TRUE(second_action->harq_ack_bits.test(0));
  ASSERT_FALSE(second_action->sr_detected);
}

} // namespace
