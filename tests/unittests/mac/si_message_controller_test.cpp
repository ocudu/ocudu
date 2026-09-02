// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "si_test_helpers.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocudu::test_helpers;

/// SI window length, in slots, that make_sib1_with_si_sched_info states in the packed SIB1.
static constexpr unsigned SI_WINDOW_LEN_SLOTS = 20;

/// \brief Provisions a cell for a warning carried by a given SIB.
/// \param content Content the cell broadcasts from its start, as test_mode does. Empty to leave the warning dormant.
static void add_pws_si_message(mac_cell_sys_info_config& cfg, sib_type sib, bcch_dl_sch_payload_type content = {})
{
  cfg.si_sched_cfg.si_window_len_slots     = SI_WINDOW_LEN_SLOTS;
  si_message_scheduling_config& pws_si_msg = cfg.si_sched_cfg.pws_si_messages.emplace_back();
  pws_si_msg.sibs                          = sib_type_set{sib};
  pws_si_msg.period_radio_frames           = 32;
  pws_si_msg.test_mode_auto_broadcast      = not content.empty();
  cfg.pws_si_messages.push_back(std::move(content));
}

class si_message_controller_test : public ::testing::Test
{
public:
  si_message_controller_test() : bench(make_sys_info_cfg()) {}

  static mac_cell_sys_info_config make_sys_info_cfg()
  {
    mac_cell_sys_info_config cfg;
    cfg.sib1 = make_random_pdu();
    cfg.si_messages.push_back(bcch_dl_sch_payload_type{make_random_pdu()});
    cfg.si_sched_cfg.si_messages.emplace_back();
    cfg.si_sched_cfg.si_messages.back().sibs.add(sib_type::sib2);
    return cfg;
  }

  si_bench bench;
};

TEST_F(si_message_controller_test, when_cell_is_created_then_initial_command_holds_version_zero_and_encoders)
{
  const si_update_command& cmd = bench.si_mng.last_command();

  ASSERT_EQ(cmd.version, 0);
  ASSERT_NE(cmd.sib1, nullptr);
  ASSERT_EQ(cmd.si_msgs.size(), 1);
  ASSERT_NE(cmd.si_msgs[0], nullptr);
}

TEST_F(si_message_controller_test, when_system_information_is_unchanged_then_no_command_is_generated)
{
  ASSERT_FALSE(bench.update_si(bench.sys_info_cfg).has_value());
  ASSERT_FALSE(bench.update_si(bench.sys_info_cfg).has_value())
      << "A repeated no-op SI update must keep being discarded";
}

TEST_F(si_message_controller_test, when_sib1_changes_then_version_is_bumped_and_sib1_encoder_is_replaced)
{
  const si_update_command previous = bench.si_mng.last_command();

  mac_cell_sys_info_config req = bench.sys_info_cfg;
  req.sib1                     = make_random_pdu();

  std::optional<si_update_command> cmd = bench.update_si(req);
  ASSERT_TRUE(cmd.has_value());
  ASSERT_EQ(cmd->version, previous.version + 1);
  ASSERT_NE(cmd->sib1, previous.sib1);
  ASSERT_EQ(cmd->si_msgs[0], previous.si_msgs[0]) << "An unchanged SI-message must reuse its encoder";
}

TEST_F(si_message_controller_test, when_only_si_scheduling_config_changes_then_version_is_bumped)
{
  const si_update_command previous = bench.si_mng.last_command();

  mac_cell_sys_info_config req            = bench.sys_info_cfg;
  req.si_sched_cfg.si_messages[0].msg_len = units::bytes{123};

  std::optional<si_update_command> cmd = bench.update_si(req);
  ASSERT_TRUE(cmd.has_value());
  ASSERT_EQ(cmd->version, previous.version + 1);
  ASSERT_EQ(cmd->si_sched_cfg.si_messages[0].msg_len, units::bytes{123});
  ASSERT_EQ(cmd->sib1, previous.sib1) << "An unchanged SIB1 must reuse its encoder";
}

TEST_F(si_message_controller_test, when_si_message_is_removed_then_readded_with_same_content_then_it_is_reencoded)
{
  // Removing an SI-message and re-adding it with identical content must re-encode the re-added index, not leave a
  // stale null encoder that broadcasts zeros (#658).
  mac_cell_sys_info_config two_msgs = bench.sys_info_cfg;
  two_msgs.si_messages.push_back(bcch_dl_sch_payload_type{make_random_pdu()});
  two_msgs.si_sched_cfg.si_messages.emplace_back();
  two_msgs.si_sched_cfg.si_messages.back().sibs.add(sib_type::sib3);
  ASSERT_TRUE(bench.update_si(two_msgs).has_value());
  ASSERT_TRUE(bench.update_si(bench.sys_info_cfg).has_value());

  std::optional<si_update_command> cmd = bench.update_si(two_msgs);
  ASSERT_TRUE(cmd.has_value());
  ASSERT_EQ(cmd->si_msgs.size(), 2);
  ASSERT_NE(cmd->si_msgs[1], nullptr)
      << "A removed-then-re-added SI-message must be re-encoded, not broadcasting zeros";
}

TEST_F(si_message_controller_test, when_cell_is_not_provisioned_for_a_warning_then_pws_broadcast_is_rejected)
{
  // The cell carries no SI message for a warning, so no PWS broadcast state was allocated -- a Write-Replace Warning
  // must be rejected rather than silently misbehave.
  std::vector<byte_buffer>     segments = make_random_segmented_pdu(50, 1);
  mac_cell_sys_info_pdu_update req;
  req.sib_idx       = sib_type::sib6;
  req.si_messages   = span<byte_buffer>(segments);
  req.pws_broadcast = pws_broadcast_indication{std::chrono::seconds{1}, 1};

  ASSERT_FALSE(bench.push_si_pdu_updates(req));
  ASSERT_EQ(bench.sched.nof_pws_broadcast_indications, 0);
}

/// Fixture for a cell provisioned for an ETWS secondary notification and nothing else, with no content configured for
/// it -- the warning stays dormant until a Write-Replace Warning provides one.
class si_message_controller_pws_test : public ::testing::Test
{
public:
  si_message_controller_pws_test() : bench(make_sys_info_cfg()) {}

  static mac_cell_sys_info_config make_sys_info_cfg()
  {
    mac_cell_sys_info_config cfg;
    cfg.sib1 = make_sib1_with_si_sched_info({});
    add_pws_si_message(cfg, sib_type::sib7);
    return cfg;
  }

  si_bench bench;
};

TEST_F(si_message_controller_pws_test,
       when_pws_broadcast_is_pushed_then_scheduler_is_signalled_immediately_for_one_burst)
{
  std::vector<byte_buffer>     segments = make_random_segmented_pdu(50, 2);
  mac_cell_sys_info_pdu_update req;
  req.sib_idx       = sib_type::sib7;
  req.si_messages   = span<byte_buffer>(segments);
  req.pws_broadcast = pws_broadcast_indication{std::chrono::seconds{1}, 3};

  ASSERT_TRUE(bench.push_si_pdu_updates(req));
  ASSERT_EQ(bench.sched.nof_pws_broadcast_indications, 1);

  ASSERT_TRUE(bench.last_pws_cmd.has_value());
  const pws_broadcasting_si_message broadcasting = bench.only_broadcasting_si_message();
  ASSERT_EQ(broadcasting.sib_set, sib_type_set{sib_type::sib7});
  ASSERT_EQ(broadcasting.nof_segments, 2);
  // Regression test: the epoch must state the real (activation-time) content length, not whatever was
  // configured/encoded for this SI-message at cell startup.
  ASSERT_EQ(broadcasting.msg_len, units::bytes{50});
}

TEST_F(si_message_controller_pws_test, when_pws_broadcast_content_is_encoded_then_segments_cycle_in_order)
{
  std::vector<byte_buffer>     segments = make_random_segmented_pdu(50, 2);
  mac_cell_sys_info_pdu_update req;
  req.sib_idx       = sib_type::sib7;
  req.si_messages   = span<byte_buffer>(segments);
  req.pws_broadcast = pws_broadcast_indication{std::chrono::seconds{1}, 1};
  bench.push_si_pdu_updates(req);

  // The warning content is broadcast from the ETWS/CMAS epoch.
  const std::optional<si_update_command>& pws_cmd = bench.last_pws_cmd;
  ASSERT_TRUE(pws_cmd.has_value());

  units::bytes    tbs{static_cast<unsigned>(segments[0].length())};
  sib_information si_info = make_sib_pdu(0, pws_cmd->version, tbs);

  si_info.is_repetition    = false;
  span<const uint8_t> pdu0 = bench.assembler.encode_si_pdu(bench.current_slot, si_info);
  ASSERT_EQ(byte_buffer::create(pdu0).value(), segments[0]);

  ++si_info.nof_txs;
  span<const uint8_t> pdu1 = bench.assembler.encode_si_pdu(bench.current_slot, si_info);
  ASSERT_EQ(byte_buffer::create(pdu1).value(), segments[1]);

  ++si_info.nof_txs;
  span<const uint8_t> pdu0_again = bench.assembler.encode_si_pdu(bench.current_slot, si_info);
  ASSERT_EQ(byte_buffer::create(pdu0_again).value(), segments[0])
      << "Segment cycle must wrap back to segment 0 to start the next broadcast";
}

TEST_F(si_message_controller_pws_test,
       when_multiple_broadcasts_requested_then_timer_re_triggers_scheduler_until_exhausted)
{
  auto                         segment = make_random_pdu();
  std::vector<byte_buffer>     segments{segment.copy()};
  mac_cell_sys_info_pdu_update req;
  req.sib_idx                   = sib_type::sib7;
  req.si_messages               = span<byte_buffer>(segments);
  const unsigned nof_broadcasts = 3;
  req.pws_broadcast             = pws_broadcast_indication{std::chrono::seconds{1}, nof_broadcasts};

  const units::bytes seg_len{static_cast<unsigned>(segment.length())};

  bench.push_si_pdu_updates(req);
  ASSERT_EQ(bench.sched.nof_pws_broadcast_indications, 1);
  ASSERT_TRUE(bench.last_pws_cmd.has_value());
  ASSERT_EQ(bench.only_broadcasting_si_message().msg_len, seg_len);

  const unsigned ticks_per_broadcast = 1000; // repeat_period == 1 second == 1000 ms ticks.
  for (unsigned b = 1; b != nof_broadcasts; ++b) {
    bench.tick(ticks_per_broadcast);
    ASSERT_EQ(bench.sched.nof_pws_broadcast_indications, b + 1) << "Broadcast #" << (b + 1) << " was not signalled";
    // Repeats carry no content of their own: the epoch keeps stating the content length of the on-going warning.
    ASSERT_EQ(bench.only_broadcasting_si_message().msg_len, seg_len);
  }

  // No further broadcasts should be signalled once the requested count has been exhausted.
  bench.tick(ticks_per_broadcast * 2);
  ASSERT_EQ(bench.sched.nof_pws_broadcast_indications, nof_broadcasts);
}

TEST_F(si_message_controller_pws_test, when_new_pws_broadcast_replaces_previous_then_content_and_timer_are_reset)
{
  auto                         segment_a = make_random_pdu();
  std::vector<byte_buffer>     segments_a{segment_a.copy()};
  mac_cell_sys_info_pdu_update req_a;
  req_a.sib_idx       = sib_type::sib7;
  req_a.si_messages   = span<byte_buffer>(segments_a);
  req_a.pws_broadcast = pws_broadcast_indication{std::chrono::seconds{1}, 10};
  bench.push_si_pdu_updates(req_a);
  ASSERT_EQ(bench.sched.nof_pws_broadcast_indications, 1);

  auto                         segment_b = make_random_pdu();
  std::vector<byte_buffer>     segments_b{segment_b.copy()};
  mac_cell_sys_info_pdu_update req_b;
  req_b.sib_idx       = sib_type::sib7;
  req_b.si_messages   = span<byte_buffer>(segments_b);
  req_b.pws_broadcast = pws_broadcast_indication{std::chrono::seconds{1}, 1};
  bench.push_si_pdu_updates(req_b);
  ASSERT_EQ(bench.sched.nof_pws_broadcast_indications, 2);

  const std::optional<si_update_command>& pws_cmd = bench.last_pws_cmd;
  ASSERT_TRUE(pws_cmd.has_value());

  units::bytes        tbs{static_cast<unsigned>(segment_b.length())};
  sib_information     si_info = make_sib_pdu(0, pws_cmd->version, tbs);
  span<const uint8_t> pdu     = bench.assembler.encode_si_pdu(bench.current_slot, si_info);
  ASSERT_EQ(byte_buffer::create(pdu).value(), segment_b)
      << "Replacement content must be served from segment 0, not the superseded warning";

  // The old (10-broadcast) timer must not keep firing after being replaced by the new (1-broadcast) one.
  bench.tick(3000);
  ASSERT_EQ(bench.sched.nof_pws_broadcast_indications, 2);
}

TEST_F(si_message_controller_pws_test, when_unrelated_si_reconfiguration_occurs_then_active_pws_broadcast_is_unaffected)
{
  // Start a multi-broadcast PWS sequence.
  auto                         segment = make_random_pdu();
  std::vector<byte_buffer>     segments{segment.copy()};
  mac_cell_sys_info_pdu_update pws_req;
  pws_req.sib_idx       = sib_type::sib7;
  pws_req.si_messages   = span<byte_buffer>(segments);
  pws_req.pws_broadcast = pws_broadcast_indication{std::chrono::seconds{1}, 3};
  bench.push_si_pdu_updates(pws_req);
  ASSERT_EQ(bench.sched.nof_pws_broadcast_indications, 1);

  // An unrelated SI reconfiguration arrives, adding a SIB2 SI-message that has nothing to do with the active warning.
  static const std::array<sib_type, 1> reconf_sibs{sib_type::sib2};

  mac_cell_sys_info_config unrelated_req;
  unrelated_req.sib1 = make_sib1_with_si_sched_info(reconf_sibs);
  unrelated_req.si_messages.push_back(bcch_dl_sch_payload_type{make_random_pdu()});
  unrelated_req.si_sched_cfg.si_messages.emplace_back().sibs = sib_type_set{sib_type::sib2};
  add_pws_si_message(unrelated_req, sib_type::sib7);
  ASSERT_TRUE(bench.update_si(unrelated_req).has_value());

  // The active PWS broadcast's content must still be served, not the unrelated placeholder.
  units::bytes        tbs{static_cast<unsigned>(segment.length())};
  sib_information     si_info = make_sib_pdu(1, bench.last_pws_cmd->version, tbs);
  span<const uint8_t> pdu     = bench.assembler.encode_si_pdu(bench.current_slot, si_info);
  ASSERT_EQ(byte_buffer::create(pdu).value(), segment)
      << "Unrelated SI reconfiguration must not disrupt the active PWS broadcast";

  // The repeat timer must still fire the remaining broadcasts.
  bench.tick(2000);
  ASSERT_EQ(bench.sched.nof_pws_broadcast_indications, 3);
}

TEST_F(si_message_controller_pws_test,
       when_cell_only_broadcasts_warnings_then_the_epoch_sib1_adds_the_si_scheduling_info)
{
  const si_update_command& normal_cmd = bench.si_mng.last_command();
  ASSERT_FALSE(get_si_window_len(encode_epoch_sib1(normal_cmd, bench.current_slot)).has_value())
      << "A cell with no warning on air and nothing else to broadcast has no schedulingInfoList";

  std::vector<byte_buffer>     segments = make_random_segmented_pdu(50, 1);
  mac_cell_sys_info_pdu_update req;
  req.sib_idx       = sib_type::sib7;
  req.si_messages   = span<byte_buffer>(segments);
  req.pws_broadcast = pws_broadcast_indication{std::chrono::seconds{1}, 1};
  ASSERT_TRUE(bench.push_si_pdu_updates(req));
  ASSERT_TRUE(bench.last_pws_cmd.has_value());

  // si-WindowLength comes with the schedulingInfoList, so the epoch must add both.
  const span<const uint8_t> pws_sib1 = encode_epoch_sib1(*bench.last_pws_cmd, bench.current_slot);
  ASSERT_EQ(get_listed_sibs(pws_sib1), (std::vector<sib_type>{sib_type::sib7}));
  ASSERT_EQ(get_si_window_len(pws_sib1), SI_WINDOW_LEN_SLOTS);
}

TEST_F(si_message_controller_pws_test, when_si_layout_changes_then_active_warning_stays_attached_to_its_sibs)
{
  // Start a warning on the SIB7 SI-message, the only one the epoch holds.
  auto                         segment = make_random_pdu();
  std::vector<byte_buffer>     segments{segment.copy()};
  mac_cell_sys_info_pdu_update pws_req;
  pws_req.sib_idx       = sib_type::sib7;
  pws_req.si_messages   = span<byte_buffer>(segments);
  pws_req.pws_broadcast = pws_broadcast_indication{std::chrono::seconds{1}, 3};
  ASSERT_TRUE(bench.push_si_pdu_updates(pws_req));
  ASSERT_TRUE(bench.last_pws_cmd.has_value());
  ASSERT_EQ(bench.only_broadcasting_si_message().sib_set, sib_type_set{sib_type::sib7});

  // An SI reconfiguration adds a normal SI-message, pushing the SIB7 one from position 0 to position 1 of the epoch.
  static const std::array<sib_type, 1> reconf_sibs{sib_type::sib2};

  mac_cell_sys_info_config reconf;
  reconf.sib1 = make_sib1_with_si_sched_info(reconf_sibs);
  reconf.si_messages.push_back(bcch_dl_sch_payload_type{make_random_pdu()});
  reconf.si_sched_cfg.si_messages.emplace_back().sibs = sib_type_set{sib_type::sib2};
  add_pws_si_message(reconf, sib_type::sib7);
  ASSERT_TRUE(bench.update_si(reconf).has_value());

  // The on-going warning must follow its SIBs to the new position, rather than staying bound to its old one.
  bench.tick(1000);
  ASSERT_EQ(bench.sched.nof_pws_broadcast_indications, 2);
  ASSERT_EQ(bench.only_broadcasting_si_message().sib_set, sib_type_set{sib_type::sib7})
      << "The warning must stay attached to the SI-message carrying SIB7, not to whatever sits at its old index";

  // A further Write-Replace Warning must reach the same encoder.
  ASSERT_TRUE(bench.push_si_pdu_updates(pws_req));
  ASSERT_TRUE(bench.last_pws_cmd.has_value());
  ASSERT_EQ(bench.only_broadcasting_si_message().sib_set, sib_type_set{sib_type::sib7});
}

/// Fixture for a cell provisioned for an ETWS secondary notification marked test_mode_auto_broadcast, mirroring a cell
/// whose ETWS test content is set -- the content is already present at construction time (built by the DU-manager
/// translators from that configuration), and the controller must broadcast it right away, indefinitely.
class si_message_controller_auto_broadcast_test : public ::testing::Test
{
public:
  si_message_controller_auto_broadcast_test() : bench(make_sys_info_cfg()) {}

  static mac_cell_sys_info_config make_sys_info_cfg()
  {
    mac_cell_sys_info_config cfg;
    cfg.sib1 = make_sib1_with_si_sched_info({});
    add_pws_si_message(cfg, sib_type::sib7, make_random_segmented_pdu(50, 2));
    return cfg;
  }

  si_bench bench;
};

TEST_F(si_message_controller_auto_broadcast_test,
       when_controller_is_constructed_then_scheduler_is_signalled_for_indefinite_broadcast)
{
  ASSERT_EQ(bench.sched.nof_pws_broadcast_indications, 1);
  ASSERT_TRUE(bench.last_pws_cmd.has_value());
  const pws_broadcasting_si_message broadcasting = bench.only_broadcasting_si_message();
  ASSERT_EQ(broadcasting.sib_set, sib_type_set{sib_type::sib7});
  ASSERT_FALSE(broadcasting.nof_segments.has_value()) << "test_mode auto-broadcast must never auto-deactivate";
  ASSERT_EQ(broadcasting.msg_len, units::bytes{50});
}

TEST_F(si_message_controller_auto_broadcast_test, when_content_is_encoded_then_it_matches_configured_si_message)
{
  const auto& segments = bench.sys_info_cfg.pws_si_messages[0];

  units::bytes    tbs{static_cast<unsigned>(segments[0].length())};
  sib_information si_info = make_sib_pdu(0, bench.last_pws_cmd->version, tbs);

  si_info.is_repetition    = false;
  span<const uint8_t> pdu0 = bench.assembler.encode_si_pdu(bench.current_slot, si_info);
  ASSERT_EQ(byte_buffer::create(pdu0).value(), segments[0]);

  ++si_info.nof_txs;
  span<const uint8_t> pdu1 = bench.assembler.encode_si_pdu(bench.current_slot, si_info);
  ASSERT_EQ(byte_buffer::create(pdu1).value(), segments[1]);
}

TEST_F(si_message_controller_auto_broadcast_test,
       when_real_write_replace_warning_arrives_then_it_overrides_the_test_mode_broadcast)
{
  std::vector<byte_buffer>     segments = make_random_segmented_pdu(50, 1);
  mac_cell_sys_info_pdu_update req;
  req.sib_idx       = sib_type::sib7;
  req.si_messages   = span<byte_buffer>(segments);
  req.pws_broadcast = pws_broadcast_indication{std::chrono::seconds{1}, 1};

  ASSERT_TRUE(bench.push_si_pdu_updates(req));
  ASSERT_EQ(bench.sched.nof_pws_broadcast_indications, 2);
  ASSERT_TRUE(bench.last_pws_cmd.has_value());
  const pws_broadcasting_si_message broadcasting = bench.only_broadcasting_si_message();
  ASSERT_TRUE(broadcasting.nof_segments.has_value());
  ASSERT_EQ(broadcasting.nof_segments, 1);
  ASSERT_EQ(broadcasting.msg_len, units::bytes{50});
}

/// Fixture whose SIB1 is a real ASN.1 payload listing a SIB2 SI message that is always broadcast and a SIB7 one that
/// only carries a warning, so that the schedulingInfoList of each epoch can be read back from the generated payloads.
class si_message_controller_sched_info_test : public ::testing::Test
{
public:
  si_message_controller_sched_info_test() : bench(make_sys_info_cfg()) {}

  static mac_cell_sys_info_config make_sys_info_cfg()
  {
    mac_cell_sys_info_config cfg;
    cfg.sib1 = make_sib1_with_si_sched_info(cell_sibs);
    cfg.si_messages.push_back(bcch_dl_sch_payload_type{make_random_pdu()});
    cfg.si_sched_cfg.si_messages.emplace_back().sibs = sib_type_set{sib_type::sib2};
    add_pws_si_message(cfg, sib_type::sib7);
    return cfg;
  }

  /// Encodes SIB1 out of a given epoch and returns the SIBs its schedulingInfoList lists.
  std::vector<sib_type> listed_sibs_of(const si_update_command& cmd)
  {
    return get_sib1_listed_sibs(cmd, bench.current_slot);
  }

  /// Position, in an SI epoch broadcasting the warning, of the SI message carrying it.
  static constexpr unsigned pws_si_msg_idx = 1;

  static const std::array<sib_type, 1> cell_sibs;

  si_bench bench;
};

const std::array<sib_type, 1> si_message_controller_sched_info_test::cell_sibs{sib_type::sib2};

TEST_F(si_message_controller_sched_info_test, when_no_warning_is_on_air_then_no_pws_epoch_is_generated)
{
  ASSERT_FALSE(bench.last_pws_cmd.has_value()) << "No ETWS/CMAS epoch must be generated while no warning is on air";
  ASSERT_EQ(listed_sibs_of(bench.si_mng.last_command()), (std::vector<sib_type>{sib_type::sib2}))
      << "A dormant warning must not be advertised in SIB1";
}

TEST_F(si_message_controller_sched_info_test, when_warning_starts_then_pws_epoch_advertises_it)
{
  std::vector<byte_buffer>     segments = make_random_segmented_pdu(50, 1);
  mac_cell_sys_info_pdu_update req;
  req.sib_idx       = sib_type::sib7;
  req.si_messages   = span<byte_buffer>(segments);
  req.pws_broadcast = pws_broadcast_indication{std::chrono::seconds{1}, 1};
  ASSERT_TRUE(bench.push_si_pdu_updates(req));

  const std::optional<si_update_command>& pws_cmd = bench.last_pws_cmd;
  ASSERT_TRUE(pws_cmd.has_value()) << "Starting a warning must generate an ETWS/CMAS epoch";
  ASSERT_EQ(pws_cmd->active_pws_si_messages[0].version, pws_cmd->version)
      << "A warning starting a broadcast must be stamped with the version of the epoch it triggers";
  ASSERT_EQ(listed_sibs_of(*pws_cmd), (std::vector<sib_type>{sib_type::sib2, sib_type::sib7}))
      << "The warning must be advertised after the SI messages that are always broadcast";

  // The normal operation epoch keeps it out of SIB1, so that it goes back to dormant once the warning stops.
  ASSERT_EQ(listed_sibs_of(bench.si_mng.last_command()), (std::vector<sib_type>{sib_type::sib2}));
  ASSERT_NE(pws_cmd->version, bench.si_mng.last_command().version)
      << "Both epochs must be distinguishable by version alone";
}

TEST_F(si_message_controller_sched_info_test, when_si_changes_mid_warning_then_pws_epoch_is_derived_again)
{
  // Start a warning, and keep the SI-message encoder it produced.
  std::vector<byte_buffer>     segments = make_random_segmented_pdu(50, 1);
  mac_cell_sys_info_pdu_update pws_req;
  pws_req.sib_idx       = sib_type::sib7;
  pws_req.si_messages   = span<byte_buffer>(segments);
  pws_req.pws_broadcast = pws_broadcast_indication{std::chrono::seconds{1}, 1};
  ASSERT_TRUE(bench.push_si_pdu_updates(pws_req));

  ASSERT_TRUE(bench.last_pws_cmd.has_value());
  const si_update_command first_pws = *bench.last_pws_cmd;

  // An unrelated SI change arrives while the warning is on air: the content of the SIB2 SI-message changes, so both
  // SIB1 and the SI messages differ from the ones the warning epoch was derived from.
  mac_cell_sys_info_config reconf = bench.sys_info_cfg;
  reconf.si_messages[0]           = bcch_dl_sch_payload_type{make_random_pdu()};

  std::optional<si_update_command> baseline = bench.update_si(reconf);
  ASSERT_TRUE(baseline.has_value());

  const std::optional<si_update_command>& second_pws = bench.last_pws_cmd;
  ASSERT_TRUE(second_pws.has_value()) << "The warning epoch must be derived again from the new System Information";
  ASSERT_NE(second_pws->version, first_pws.version);
  ASSERT_NE(second_pws->version, baseline->version) << "Both epochs must stay distinguishable by version alone";
  ASSERT_LT(second_pws->active_pws_si_messages[0].version, second_pws->version)
      << "An SI change did not trigger the warning, so it must not prolong its broadcast";
  ASSERT_EQ(second_pws->active_pws_si_messages[0].version, first_pws.active_pws_si_messages[0].version);

  // It still advertises the warning, while the normal operation epoch keeps it out of SIB1.
  ASSERT_EQ(listed_sibs_of(*second_pws), (std::vector<sib_type>{sib_type::sib2, sib_type::sib7}));
  ASSERT_EQ(listed_sibs_of(*baseline), (std::vector<sib_type>{sib_type::sib2}));

  // The warning content itself is untouched, so its segment cycle is not restarted.
  ASSERT_EQ(second_pws->si_msgs[pws_si_msg_idx], first_pws.si_msgs[pws_si_msg_idx]);
}

TEST_F(si_message_controller_sched_info_test, when_warning_ends_then_a_later_si_change_does_not_bring_it_back)
{
  // Start a warning and let the cell finish broadcasting it.
  std::vector<byte_buffer>     segments = make_random_segmented_pdu(50, 1);
  mac_cell_sys_info_pdu_update pws_req;
  pws_req.sib_idx       = sib_type::sib7;
  pws_req.si_messages   = span<byte_buffer>(segments);
  pws_req.pws_broadcast = pws_broadcast_indication{std::chrono::seconds{1}, 1};
  ASSERT_TRUE(bench.push_si_pdu_updates(pws_req));
  ASSERT_TRUE(bench.last_pws_cmd.has_value());

  // The cell broadcasts from the warning epoch, and then goes back to the one of the normal operation.
  bench.serve_sib1_grant(bench.last_pws_cmd->version);
  bench.serve_sib1_grant(bench.si_mng.last_command().version);

  const unsigned nof_epochs_before = bench.nof_pws_epochs;

  // An unrelated SI change arrives: the content of the SIB2 SI-message changes.
  mac_cell_sys_info_config reconf = bench.sys_info_cfg;
  reconf.si_messages[0]           = bcch_dl_sch_payload_type{make_random_pdu()};
  ASSERT_TRUE(bench.update_si(reconf).has_value());

  ASSERT_EQ(bench.nof_pws_epochs, nof_epochs_before)
      << "A warning that is over must not be put back on air by an unrelated SI change";
  ASSERT_EQ(listed_sibs_of(bench.si_mng.last_command()), (std::vector<sib_type>{sib_type::sib2}));
}

/// \brief Fixture whose cell carries a SIB2 SI message that is always broadcast, a SIB7 warning, and a SIB19 one whose
/// content bypasses the SI change modification window.
///
/// The SIB19 SI message states its SI window position, so it is listed in the SIB1 schedulingInfoList2 and follows the
/// warning in every SI epoch. Its position therefore shifts as the warning goes on and off air, which is what makes it
/// the case to cover for the SI PDU updates served outside the modification window.
class si_message_controller_si_pdu_update_test : public ::testing::Test
{
public:
  si_message_controller_si_pdu_update_test() : bench(make_sys_info_cfg()) {}

  static mac_cell_sys_info_config make_sys_info_cfg()
  {
    // Note: the packed SIB1 only holds the schedulingInfoList entries, given that the test helper does not build a
    // schedulingInfoList2. The SI epoch layout is derived from the SI scheduling configuration regardless.
    static const std::array<sib_type, 1> sched_info_list_sibs{sib_type::sib2};

    mac_cell_sys_info_config cfg;
    cfg.sib1 = make_sib1_with_si_sched_info(sched_info_list_sibs);
    for (unsigned i = 0; i != 2; ++i) {
      cfg.si_messages.push_back(bcch_dl_sch_payload_type{make_random_pdu()});
    }
    cfg.si_sched_cfg.si_messages.emplace_back().sibs = sib_type_set{sib_type::sib2};
    si_message_scheduling_config& ntn_si_msg         = cfg.si_sched_cfg.si_messages.emplace_back();
    ntn_si_msg.sibs                                  = sib_type_set{sib_type::sib19};
    ntn_si_msg.si_window_position                    = 3;
    add_pws_si_message(cfg, sib_type::sib7);
    return cfg;
  }

  /// Enqueues an SI PDU update for the SIB19 SI message.
  byte_buffer push_ntn_si_pdu_update()
  {
    ntn_segments.clear();
    ntn_segments.push_back(make_random_pdu());

    mac_cell_sys_info_pdu_update req;
    req.sib_idx     = sib_type::sib19;
    req.si_messages = span<byte_buffer>(ntn_segments);
    report_fatal_error_if_not(bench.push_si_pdu_updates(req), "Failed to enqueue the SI PDU update");
    return ntn_segments.front().copy();
  }

  /// Serves a grant for the SIB19 SI message out of a given epoch, at a given position of it.
  byte_buffer serve_ntn_grant(unsigned si_msg_index, si_version_type version, units::bytes tbs)
  {
    sib_information si_info = make_sib_pdu(si_msg_index, version, tbs, sib_type_set{sib_type::sib19});
    return byte_buffer::create(bench.assembler.encode_si_pdu(bench.current_slot, si_info)).value();
  }

  std::vector<byte_buffer> ntn_segments;
  si_bench                 bench;
};

TEST_F(si_message_controller_si_pdu_update_test, when_no_warning_is_on_air_then_the_update_is_served_from_its_position)
{
  const byte_buffer pdu = push_ntn_si_pdu_update();

  // With no warning on air, the epoch holds the SIB2 and the SIB19 SI messages, so the latter sits at position 1.
  const units::bytes tbs{static_cast<unsigned>(pdu.length())};
  ASSERT_EQ(serve_ntn_grant(1, bench.si_mng.last_command().version, tbs), pdu);
}

TEST_F(si_message_controller_si_pdu_update_test, when_no_si_message_carries_the_sib_then_the_update_is_rejected)
{
  std::vector<byte_buffer>     segments{make_random_pdu()};
  mac_cell_sys_info_pdu_update req;
  req.sib_idx     = sib_type::sib4;
  req.si_messages = span<byte_buffer>(segments);

  ASSERT_FALSE(bench.push_si_pdu_updates(req)) << "The cell broadcasts no SI message carrying SIB4";
}

TEST_F(si_message_controller_si_pdu_update_test, when_a_warning_goes_on_air_then_the_update_is_still_served)
{
  // The warning joins the epoch ahead of the SIB19 SI message, pushing it from position 1 to position 2.
  std::vector<byte_buffer>     segments = make_random_segmented_pdu(50, 1);
  mac_cell_sys_info_pdu_update pws_req;
  pws_req.sib_idx       = sib_type::sib7;
  pws_req.si_messages   = span<byte_buffer>(segments);
  pws_req.pws_broadcast = pws_broadcast_indication{std::chrono::seconds{1}, 1};
  ASSERT_TRUE(bench.push_si_pdu_updates(pws_req));
  ASSERT_TRUE(bench.last_pws_cmd.has_value());

  const byte_buffer pdu = push_ntn_si_pdu_update();

  // The SI PDU update is matched against the SIBs the grant carries, so it survives the SI message changing position.
  const units::bytes tbs{static_cast<unsigned>(pdu.length())};
  ASSERT_EQ(serve_ntn_grant(2, bench.last_pws_cmd->version, tbs), pdu);
}

/// Fixture with one SI-message carrying SIB7 (ETWS) and another carrying SIB8 (CMAS), so that two warnings can be
/// broadcast one after the other.
class si_message_controller_two_warnings_test : public si_message_controller_sched_info_test
{
public:
  si_message_controller_two_warnings_test() { bench_two.emplace(make_two_warnings_cfg()); }

  static mac_cell_sys_info_config make_two_warnings_cfg()
  {
    static const std::array<sib_type, 1> si_msg_sibs{sib_type::sib2};

    mac_cell_sys_info_config cfg;
    cfg.sib1 = make_sib1_with_si_sched_info(si_msg_sibs);
    cfg.si_messages.push_back(bcch_dl_sch_payload_type{make_random_pdu()});
    cfg.si_sched_cfg.si_messages.emplace_back().sibs = sib_type_set{sib_type::sib2};
    add_pws_si_message(cfg, sib_type::sib7);
    add_pws_si_message(cfg, sib_type::sib8);
    return cfg;
  }

  /// Starts a warning carried by a given SIB.
  void start_warning(sib_type sib)
  {
    segments = make_random_segmented_pdu(50, 1);

    mac_cell_sys_info_pdu_update pws_req;
    pws_req.sib_idx       = sib;
    pws_req.si_messages   = span<byte_buffer>(segments);
    pws_req.pws_broadcast = pws_broadcast_indication{std::chrono::seconds{1}, 1};
    report_fatal_error_if_not(bench_two->push_si_pdu_updates(pws_req), "Failed to start the warning");
  }

  std::vector<byte_buffer> segments;
  std::optional<si_bench>  bench_two;
};

TEST_F(si_message_controller_two_warnings_test, when_warning_ends_then_a_new_warning_does_not_bring_it_back)
{
  // The ETWS warning is broadcast and finishes.
  start_warning(sib_type::sib7);
  ASSERT_TRUE(bench_two->last_pws_cmd.has_value());
  bench_two->serve_sib1_grant(bench_two->last_pws_cmd->version);
  bench_two->serve_sib1_grant(bench_two->si_mng.last_command().version);

  // A CMAS warning starts afterwards. Its epoch must carry it alone.
  start_warning(sib_type::sib8);
  ASSERT_EQ(bench_two->only_broadcasting_si_message().sib_set, sib_type_set{sib_type::sib8});

  units::bytes    tbs{MAX_BCCH_DL_SCH_PDU_SIZE / 2};
  sib_information si_info = make_sib_pdu(std::nullopt, bench_two->last_pws_cmd->version, tbs);
  auto            payload = bench_two->last_pws_cmd->sib1->encode(bench_two->current_slot, si_info);
  ASSERT_TRUE(payload.has_value());
  ASSERT_EQ(get_listed_sibs(payload.value()), (std::vector<sib_type>{sib_type::sib2, sib_type::sib8}))
      << "SIB1 must not advertise the warning that is over";
}

TEST_F(si_message_controller_two_warnings_test, when_two_warnings_are_on_air_then_they_are_listed_after_the_rest)
{
  // The CMAS warning starts first, so the order the epoch imposes can be told apart from the activation order.
  start_warning(sib_type::sib8);
  start_warning(sib_type::sib7);

  // The SI window of an SI message derives from its position, so the warnings come after the SI message that is
  // always broadcast, and among themselves in the order the cell is provisioned for them.
  const std::vector<sib_type> expected{sib_type::sib2, sib_type::sib7, sib_type::sib8};
  ASSERT_EQ(get_epoch_sibs(*bench_two->last_pws_cmd), expected);
  ASSERT_EQ(get_sib1_listed_sibs(*bench_two->last_pws_cmd, bench_two->current_slot), expected);
}
