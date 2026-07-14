// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/mac/mac_dl/sib_pdu_assembler.h"
#include "mac_test_helpers.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace ocudu;

byte_buffer make_random_pdu(unsigned size = test_rng::uniform_int<unsigned>(10, 200))
{
  return byte_buffer::create(test_rng::vector_of_uniform_ints<uint8_t>(size)).value();
}

std::vector<byte_buffer> make_random_segmented_pdu(unsigned segment_size = test_rng::uniform_int<unsigned>(10, 200),
                                                   unsigned nof_segments = test_rng::uniform_int<unsigned>(2, 3))
{
  std::vector<byte_buffer> segmented_pdu;
  for (unsigned i_segment = 0; i_segment != nof_segments; ++i_segment) {
    segmented_pdu.emplace_back(make_random_pdu(segment_size));
  }

  return segmented_pdu;
}

byte_buffer make_pdu_with_padding(const byte_buffer& payload, units::bytes tbs)
{
  byte_buffer          result = payload.deep_copy().value();
  std::vector<uint8_t> zeros(tbs.value() - result.length(), 0);
  report_fatal_error_if_not(result.append(zeros), "Failed appending zeros");
  return result;
}

static sib_information make_sib_pdu(std::optional<unsigned> si_msg_index, si_version_type si_version, units::bytes tbs)
{
  sib_information result{};
  result.si_indicator  = si_msg_index.has_value() ? sib_information::other_si : sib_information::sib1;
  result.si_msg_index  = si_msg_index;
  result.version       = si_version;
  result.is_repetition = false;
  result.pdsch_cfg.codewords.emplace_back();
  result.pdsch_cfg.codewords[0].tb_size_bytes = tbs;
  return result;
}

class sib_pdu_assembler_test : public ::testing::Test
{
public:
  sib_pdu_assembler_test() :
    sys_info_cfg({make_random_pdu()}),
    assembler(to_du_cell_index(0), sys_info_cfg, timer_factory{timers, task_worker}, sched)
  {
  }

  byte_buffer update_si_pdus(const byte_buffer& sib1, span<const bcch_dl_sch_payload_type> si_msgs = {})
  {
    mac_cell_sys_info_config req;
    auto                     old_pdu = std::move(sys_info_cfg.sib1);
    sys_info_cfg.sib1                = sib1.copy();
    req.sib1                         = sib1.copy();
    req.si_messages.clear();
    for (const auto& si_msg : si_msgs) {
      req.si_messages.push_back(si_msg);
    }
    last_version             = next_version++;
    req.si_sched_cfg.version = last_version.value();
    assembler.handle_si_change_request(req);
    return old_pdu;
  }

  manual_task_worker                        task_worker{128};
  timer_manager                             timers;
  test_helpers::dummy_mac_scheduler_adapter sched;

  mac_cell_sys_info_config       sys_info_cfg;
  sib_pdu_assembler              assembler;
  si_version_type                next_version = 1;
  std::optional<si_version_type> last_version;

  slot_point_extended current_slot{subcarrier_spacing::kHz30, 0};
};

TEST_F(sib_pdu_assembler_test, when_sib1_is_scheduled_then_the_correct_payload_is_generated)
{
  units::bytes        padding_len{test_rng::uniform_int<unsigned>(0, 20)};
  units::bytes        tbs     = units::bytes{(unsigned)sys_info_cfg.sib1.length()} + padding_len;
  sib_information     si_info = make_sib_pdu(std::nullopt, 0, tbs);
  span<const uint8_t> pdu     = assembler.encode_si_pdu(current_slot, si_info);

  byte_buffer expected = make_pdu_with_padding(sys_info_cfg.sib1, tbs);
  ASSERT_EQ(expected, pdu) << fmt::format("Incorrect SIB1 payload returned.\n> expected=[{}]\n> result = [{}])",
                                          expected,
                                          byte_buffer::create(pdu).value());
}

TEST_F(sib_pdu_assembler_test, when_invalid_si_msg_index_is_scheduled_then_a_pdu_of_zeros_is_generated)
{
  units::bytes        padding_len{test_rng::uniform_int<unsigned>(0, 20)};
  units::bytes        tbs     = units::bytes{(unsigned)sys_info_cfg.sib1.length()} + padding_len;
  sib_information     si_info = make_sib_pdu(2, 0, tbs);
  span<const uint8_t> pdu     = assembler.encode_si_pdu(current_slot, si_info);

  ASSERT_EQ(pdu.size(), tbs.value());
  ASSERT_TRUE(std::all_of(pdu.begin(), pdu.end(), [](uint8_t c) { return c == 0; }));
}

TEST_F(sib_pdu_assembler_test, when_sib1_is_updated_and_old_version_is_scheduled_then_encoding_returns_old_version)
{
  auto new_msg = make_random_pdu();
  auto old_msg = this->update_si_pdus(new_msg);
  ASSERT_EQ(last_version, 1);

  const unsigned nof_tries = 4;
  for (unsigned i = 0; i != nof_tries; ++i) {
    current_slot++;

    // Old SIB1 version is scheduled, old SIB1 PDU is encoded.
    units::bytes        padding_len{test_rng::uniform_int<unsigned>(0, 20)};
    units::bytes        tbs         = units::bytes{(unsigned)old_msg.length()} + padding_len;
    sib_information     old_si_info = make_sib_pdu(std::nullopt, 0, tbs);
    span<const uint8_t> pdu         = assembler.encode_si_pdu(current_slot, old_si_info);
    byte_buffer         expected    = make_pdu_with_padding(old_msg, tbs);
    ASSERT_EQ(expected, pdu) << fmt::format("Incorrect SIB1 payload returned.\n> expected=[{}]\n> result = [{}])",
                                            expected,
                                            byte_buffer::create(pdu).value());
  }
}

TEST_F(sib_pdu_assembler_test, when_sib1_is_updated_then_encoding_accounts_for_new_version)
{
  auto new_msg = make_random_pdu();
  this->update_si_pdus(new_msg);
  ASSERT_EQ(last_version, 1);

  const unsigned nof_tries = 4;
  for (unsigned i = 0; i != nof_tries; ++i) {
    current_slot++;

    // Encoding new PDU.
    units::bytes        padding_len{test_rng::uniform_int<unsigned>(0, 20)};
    units::bytes        tbs         = units::bytes{(unsigned)new_msg.length()} + padding_len;
    sib_information     new_si_info = make_sib_pdu(std::nullopt, 1, tbs);
    span<const uint8_t> pdu         = assembler.encode_si_pdu(current_slot, new_si_info);

    auto expected = make_pdu_with_padding(new_msg, tbs);

    ASSERT_EQ(expected, pdu) << fmt::format("Incorrect SIB1 payload returned.\n> expected=[{}]\n> result = [{}])",
                                            expected,
                                            byte_buffer::create(pdu).value());
  }
}

TEST_F(sib_pdu_assembler_test, when_si_message_is_added_then_encoding_matched_added_si_message)
{
  auto new_msg = make_random_pdu();
  this->update_si_pdus(sys_info_cfg.sib1, std::vector<bcch_dl_sch_payload_type>{{new_msg.copy()}});
  ASSERT_EQ(last_version, 1);

  units::bytes        padding_len{test_rng::uniform_int<unsigned>(0, 20)};
  units::bytes        tbs         = units::bytes{(unsigned)new_msg.length()} + padding_len;
  sib_information     new_si_info = make_sib_pdu(0, 1, tbs);
  span<const uint8_t> pdu         = assembler.encode_si_pdu(current_slot, new_si_info);

  auto expected = make_pdu_with_padding(new_msg, tbs);
  ASSERT_EQ(expected, pdu) << fmt::format("Incorrect SI-message payload returned.\n> expected=[{}]\n> result = [{}])",
                                          expected,
                                          byte_buffer::create(pdu).value());
}

TEST_F(sib_pdu_assembler_test, when_si_message_does_not_require_activation_then_pws_broadcast_is_rejected)
{
  // sys_info_cfg's SI-message index 0 does not mark requires_activation, so no PWS broadcast state was allocated
  // for it -- a Write-Replace Warning targeting it must be rejected rather than silently misbehave.
  std::vector<byte_buffer>     segments = make_random_segmented_pdu(50, 1);
  mac_cell_sys_info_pdu_update req;
  req.si_msg_idx    = 0;
  req.sib_idx       = 6;
  req.si_messages   = span<byte_buffer>(segments);
  req.pws_broadcast = pws_broadcast_indication{std::chrono::seconds{1}, 1};

  ASSERT_FALSE(assembler.handle_si_message_pdu_updates(req));
  ASSERT_EQ(sched.nof_pws_broadcast_indications, 0);
}

/// Fixture with a single SI-message pre-provisioned at index 0, mirroring a cell configured with a reserved
/// SIB6/7/8 occasion (the PWS broadcast per-index state in \c sib_pdu_assembler is sized once, at construction,
/// from the initial number of SI-messages).
class sib_pdu_assembler_pws_test : public ::testing::Test
{
public:
  sib_pdu_assembler_pws_test() : assembler(to_du_cell_index(0), sys_info_cfg, timer_factory{timers, task_worker}, sched)
  {
  }

  static mac_cell_sys_info_config make_sys_info_cfg()
  {
    mac_cell_sys_info_config cfg;
    cfg.sib1 = make_random_pdu();
    cfg.si_messages.push_back(bcch_dl_sch_payload_type{make_random_pdu()});
    // Mark SI-message index 0 as requiring activation, mirroring a cell configured with a reserved SIB6/7/8
    // occasion -- sib_pdu_assembler only allocates PWS broadcast state for such indices.
    si_message_scheduling_config& si_msg_cfg = cfg.si_sched_cfg.si_sched_cfg.si_messages.emplace_back();
    si_msg_cfg.requires_activation           = true;
    return cfg;
  }

  manual_task_worker                        task_worker{128};
  timer_manager                             timers;
  test_helpers::dummy_mac_scheduler_adapter sched;

  mac_cell_sys_info_config sys_info_cfg = make_sys_info_cfg();
  sib_pdu_assembler        assembler;

  slot_point_extended current_slot{subcarrier_spacing::kHz30, 0};
};

TEST_F(sib_pdu_assembler_pws_test, when_pws_broadcast_is_pushed_then_scheduler_is_signalled_immediately_for_one_burst)
{
  std::vector<byte_buffer>     segments = make_random_segmented_pdu(50, 2);
  mac_cell_sys_info_pdu_update req;
  req.si_msg_idx    = 0;
  req.sib_idx       = 7;
  req.si_messages   = span<byte_buffer>(segments);
  req.pws_broadcast = pws_broadcast_indication{std::chrono::seconds{1}, 3};

  ASSERT_TRUE(assembler.handle_si_message_pdu_updates(req));
  ASSERT_EQ(sched.nof_pws_broadcast_indications, 1);
  ASSERT_EQ(sched.last_pws_si_msg_idx, 0);
  ASSERT_EQ(sched.last_pws_nof_segments, 2);
}

TEST_F(sib_pdu_assembler_pws_test, when_pws_broadcast_content_is_encoded_then_segments_cycle_in_order)
{
  std::vector<byte_buffer>     segments = make_random_segmented_pdu(50, 2);
  mac_cell_sys_info_pdu_update req;
  req.si_msg_idx    = 0;
  req.sib_idx       = 7;
  req.si_messages   = span<byte_buffer>(segments);
  req.pws_broadcast = pws_broadcast_indication{std::chrono::seconds{1}, 1};
  assembler.handle_si_message_pdu_updates(req);

  units::bytes    tbs{static_cast<unsigned>(segments[0].length())};
  sib_information si_info = make_sib_pdu(0, 0, tbs);

  si_info.is_repetition    = false;
  span<const uint8_t> pdu0 = assembler.encode_si_pdu(current_slot, si_info);
  ASSERT_EQ(byte_buffer::create(pdu0).value(), segments[0]);

  ++si_info.nof_txs;
  span<const uint8_t> pdu1 = assembler.encode_si_pdu(current_slot, si_info);
  ASSERT_EQ(byte_buffer::create(pdu1).value(), segments[1]);

  ++si_info.nof_txs;
  span<const uint8_t> pdu0_again = assembler.encode_si_pdu(current_slot, si_info);
  ASSERT_EQ(byte_buffer::create(pdu0_again).value(), segments[0])
      << "Segment cycle must wrap back to segment 0 to start the next broadcast";
}

TEST_F(sib_pdu_assembler_pws_test, when_multiple_broadcasts_requested_then_timer_re_triggers_scheduler_until_exhausted)
{
  auto                         segment = make_random_pdu();
  std::vector<byte_buffer>     segments{segment.copy()};
  mac_cell_sys_info_pdu_update req;
  req.si_msg_idx                = 0;
  req.sib_idx                   = 6;
  req.si_messages               = span<byte_buffer>(segments);
  const unsigned nof_broadcasts = 3;
  req.pws_broadcast             = pws_broadcast_indication{std::chrono::seconds{1}, nof_broadcasts};

  assembler.handle_si_message_pdu_updates(req);
  ASSERT_EQ(sched.nof_pws_broadcast_indications, 1);

  const unsigned ticks_per_broadcast = 1000; // repeat_period == 1 second == 1000 ms ticks.
  for (unsigned b = 1; b != nof_broadcasts; ++b) {
    for (unsigned t = 0; t != ticks_per_broadcast; ++t) {
      timers.tick();
      task_worker.run_pending_tasks();
    }
    ASSERT_EQ(sched.nof_pws_broadcast_indications, b + 1) << "Broadcast #" << (b + 1) << " was not signalled";
  }

  // No further broadcasts should be signalled once the requested count has been exhausted.
  for (unsigned t = 0; t != ticks_per_broadcast * 2; ++t) {
    timers.tick();
    task_worker.run_pending_tasks();
  }
  ASSERT_EQ(sched.nof_pws_broadcast_indications, nof_broadcasts);
}

TEST_F(sib_pdu_assembler_pws_test, when_new_pws_broadcast_replaces_previous_then_content_and_timer_are_reset)
{
  auto                         segment_a = make_random_pdu();
  std::vector<byte_buffer>     segments_a{segment_a.copy()};
  mac_cell_sys_info_pdu_update req_a;
  req_a.si_msg_idx    = 0;
  req_a.sib_idx       = 6;
  req_a.si_messages   = span<byte_buffer>(segments_a);
  req_a.pws_broadcast = pws_broadcast_indication{std::chrono::seconds{1}, 10};
  assembler.handle_si_message_pdu_updates(req_a);
  ASSERT_EQ(sched.nof_pws_broadcast_indications, 1);

  auto                         segment_b = make_random_pdu();
  std::vector<byte_buffer>     segments_b{segment_b.copy()};
  mac_cell_sys_info_pdu_update req_b;
  req_b.si_msg_idx    = 0;
  req_b.sib_idx       = 6;
  req_b.si_messages   = span<byte_buffer>(segments_b);
  req_b.pws_broadcast = pws_broadcast_indication{std::chrono::seconds{1}, 1};
  assembler.handle_si_message_pdu_updates(req_b);
  ASSERT_EQ(sched.nof_pws_broadcast_indications, 2);

  units::bytes        tbs{static_cast<unsigned>(segment_b.length())};
  sib_information     si_info = make_sib_pdu(0, 0, tbs);
  span<const uint8_t> pdu     = assembler.encode_si_pdu(current_slot, si_info);
  ASSERT_EQ(byte_buffer::create(pdu).value(), segment_b)
      << "Replacement content must be served from segment 0, not the superseded warning";

  // The old (10-broadcast) timer must not keep firing after being replaced by the new (1-broadcast) one.
  for (unsigned t = 0; t != 3000; ++t) {
    timers.tick();
    task_worker.run_pending_tasks();
  }
  ASSERT_EQ(sched.nof_pws_broadcast_indications, 2);
}

TEST_F(sib_pdu_assembler_pws_test, when_unrelated_si_reconfiguration_occurs_then_active_pws_broadcast_is_unaffected)
{
  // Start a multi-broadcast PWS sequence.
  auto                         segment = make_random_pdu();
  std::vector<byte_buffer>     segments{segment.copy()};
  mac_cell_sys_info_pdu_update pws_req;
  pws_req.si_msg_idx    = 0;
  pws_req.sib_idx       = 6;
  pws_req.si_messages   = span<byte_buffer>(segments);
  pws_req.pws_broadcast = pws_broadcast_indication{std::chrono::seconds{1}, 3};
  assembler.handle_si_message_pdu_updates(pws_req);
  ASSERT_EQ(sched.nof_pws_broadcast_indications, 1);

  // An unrelated SI reconfiguration arrives (e.g. a SIB2 content update), rebuilding all SI-messages, including a
  // placeholder for index 0 that has nothing to do with the active warning.
  mac_cell_sys_info_config unrelated_req;
  unrelated_req.sib1 = make_random_pdu();
  unrelated_req.si_messages.push_back(bcch_dl_sch_payload_type{make_random_pdu()});
  unrelated_req.si_sched_cfg.version = 1;
  assembler.handle_si_change_request(unrelated_req);

  // The active PWS broadcast's content must still be served, not the unrelated placeholder.
  units::bytes        tbs{static_cast<unsigned>(segment.length())};
  sib_information     si_info = make_sib_pdu(0, 1, tbs);
  span<const uint8_t> pdu     = assembler.encode_si_pdu(current_slot, si_info);
  ASSERT_EQ(byte_buffer::create(pdu).value(), segment)
      << "Unrelated SI reconfiguration must not disrupt the active PWS broadcast";

  // The repeat timer must still fire the remaining broadcasts.
  const unsigned ticks_per_broadcast = 1000;
  for (unsigned t = 0; t != ticks_per_broadcast * 2; ++t) {
    timers.tick();
    task_worker.run_pending_tasks();
  }
  ASSERT_EQ(sched.nof_pws_broadcast_indications, 3);
}
