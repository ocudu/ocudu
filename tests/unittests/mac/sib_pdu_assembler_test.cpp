// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "si_test_helpers.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocudu::test_helpers;

class sib_pdu_assembler_test : public ::testing::Test
{
public:
  sib_pdu_assembler_test() : bench(make_sys_info_cfg()) {}

  static mac_cell_sys_info_config make_sys_info_cfg()
  {
    mac_cell_sys_info_config cfg;
    cfg.sib1 = make_random_pdu();
    return cfg;
  }

  byte_buffer update_si_pdus(const byte_buffer& sib1, span<const bcch_dl_sch_payload_type> si_msgs = {})
  {
    mac_cell_sys_info_config req;
    // Note: sib1 may alias the payload being moved out, so it is copied before the move.
    req.sib1                = sib1.copy();
    byte_buffer old_pdu     = std::move(bench.sys_info_cfg.sib1);
    bench.sys_info_cfg.sib1 = req.sib1.copy();
    for (const auto& si_msg : si_msgs) {
      req.si_messages.push_back(si_msg);
      req.si_sched_cfg.si_messages.emplace_back().sibs = sib_type_set{sib_type::sib2};
    }
    std::optional<si_update_command> cmd = bench.update_si(req);
    last_version = cmd.has_value() ? std::optional<si_version_type>{cmd->version} : std::nullopt;
    return old_pdu;
  }

  si_bench                       bench;
  std::optional<si_version_type> last_version;
};

TEST_F(sib_pdu_assembler_test, when_sib1_is_scheduled_then_the_correct_payload_is_generated)
{
  units::bytes        padding_len{test_rng::uniform_int<unsigned>(0, 20)};
  units::bytes        tbs     = units::bytes{(unsigned)bench.sys_info_cfg.sib1.length()} + padding_len;
  sib_information     si_info = make_sib_pdu(std::nullopt, 0, tbs);
  span<const uint8_t> pdu     = bench.assembler.encode_si_pdu(bench.current_slot, si_info);

  byte_buffer expected = make_pdu_with_padding(bench.sys_info_cfg.sib1, tbs);
  ASSERT_EQ(expected, pdu) << fmt::format("Incorrect SIB1 payload returned.\n> expected=[{}]\n> result = [{}])",
                                          expected,
                                          byte_buffer::create(pdu).value());
}

TEST_F(sib_pdu_assembler_test, when_invalid_si_msg_index_is_scheduled_then_a_pdu_of_zeros_is_generated)
{
  units::bytes        padding_len{test_rng::uniform_int<unsigned>(0, 20)};
  units::bytes        tbs     = units::bytes{(unsigned)bench.sys_info_cfg.sib1.length()} + padding_len;
  sib_information     si_info = make_sib_pdu(2, 0, tbs);
  span<const uint8_t> pdu     = bench.assembler.encode_si_pdu(bench.current_slot, si_info);

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
    bench.current_slot++;

    // Old SIB1 version is scheduled, old SIB1 PDU is encoded.
    units::bytes        padding_len{test_rng::uniform_int<unsigned>(0, 20)};
    units::bytes        tbs         = units::bytes{(unsigned)old_msg.length()} + padding_len;
    sib_information     old_si_info = make_sib_pdu(std::nullopt, 0, tbs);
    span<const uint8_t> pdu         = bench.assembler.encode_si_pdu(bench.current_slot, old_si_info);
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
    bench.current_slot++;

    // Encoding new PDU.
    units::bytes        padding_len{test_rng::uniform_int<unsigned>(0, 20)};
    units::bytes        tbs         = units::bytes{(unsigned)new_msg.length()} + padding_len;
    sib_information     new_si_info = make_sib_pdu(std::nullopt, 1, tbs);
    span<const uint8_t> pdu         = bench.assembler.encode_si_pdu(bench.current_slot, new_si_info);

    auto expected = make_pdu_with_padding(new_msg, tbs);

    ASSERT_EQ(expected, pdu) << fmt::format("Incorrect SIB1 payload returned.\n> expected=[{}]\n> result = [{}])",
                                            expected,
                                            byte_buffer::create(pdu).value());
  }
}

TEST_F(sib_pdu_assembler_test, when_si_message_is_added_then_encoding_matched_added_si_message)
{
  auto new_msg = make_random_pdu();
  this->update_si_pdus(bench.sys_info_cfg.sib1, std::vector<bcch_dl_sch_payload_type>{{new_msg.copy()}});
  ASSERT_EQ(last_version, 1);

  units::bytes        padding_len{test_rng::uniform_int<unsigned>(0, 20)};
  units::bytes        tbs         = units::bytes{(unsigned)new_msg.length()} + padding_len;
  sib_information     new_si_info = make_sib_pdu(0, 1, tbs);
  span<const uint8_t> pdu         = bench.assembler.encode_si_pdu(bench.current_slot, new_si_info);

  auto expected = make_pdu_with_padding(new_msg, tbs);
  ASSERT_EQ(expected, pdu) << fmt::format("Incorrect SI-message payload returned.\n> expected=[{}]\n> result = [{}])",
                                          expected,
                                          byte_buffer::create(pdu).value());
}

TEST_F(sib_pdu_assembler_test, when_two_si_epochs_coexist_then_each_grant_is_encoded_with_its_own)
{
  // The ETWS/CMAS epoch coexists with the normal-operation one, and the scheduler picks between them per grant. The
  // assembler must resolve each grant through the epoch it was scheduled with, in either direction and repeatedly.
  const byte_buffer baseline_sib1 = bench.sys_info_cfg.sib1.copy();

  mac_cell_sys_info_config pws_cfg;
  pws_cfg.sib1 = make_random_pdu();
  ASSERT_NE(pws_cfg.sib1, baseline_sib1);
  const si_update_command pws_cmd = bench.apply_pws_si(pws_cfg, bench.si_mng.last_command().version + 1);

  auto encode_sib1_of = [this](si_version_type version, const byte_buffer& expected) {
    units::bytes        tbs{static_cast<unsigned>(expected.length())};
    sib_information     si_info = make_sib_pdu(std::nullopt, version, tbs);
    span<const uint8_t> pdu     = bench.assembler.encode_si_pdu(bench.current_slot, si_info);
    return byte_buffer::create(pdu).value() == expected;
  };

  for (unsigned i = 0; i != 3; ++i) {
    bench.current_slot++;
    ASSERT_TRUE(encode_sib1_of(pws_cmd.version, pws_cfg.sib1)) << "ETWS/CMAS epoch not served, iteration " << i;
    bench.current_slot++;
    ASSERT_TRUE(encode_sib1_of(bench.si_mng.last_command().version, baseline_sib1))
        << "Normal-operation epoch not served after the ETWS/CMAS one, iteration " << i;
  }
}
