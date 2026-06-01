// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/rrc/ue/rrc_measurement_types_asn1_converters.h"
#include "rrc_ue_test_helpers.h"
#include "rrc_ue_test_messages.h"
#include "ocudu/asn1/rrc_nr/ul_dcch_msg_ies.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

/// Fixture class RRC Reestablishment tests preparation
class rrc_ue_meas_report : public rrc_ue_test_helper, public ::testing::Test
{
protected:
  static void SetUpTestSuite() { ocudulog::init(); }

  void SetUp() override { init(); }

  void TearDown() override
  {
    // flush logger after each test
    ocudulog::flush();
  }
};

TEST_F(rrc_ue_meas_report, when_dummy_meas_report_received_then_conversion_successful)
{
  byte_buffer meas_report_pdu = generate_measurement_report_pdu();

  // Parse UL-CCCH
  asn1::rrc_nr::ul_dcch_msg_s ul_dcch_msg;
  asn1::cbit_ref              bref({meas_report_pdu.begin(), meas_report_pdu.end()});
  ASSERT_EQ(ul_dcch_msg.unpack(bref), asn1::OCUDUASN_SUCCESS);
  ASSERT_EQ(ul_dcch_msg.msg.type().value, asn1::rrc_nr::ul_dcch_msg_type_c::types_opts::c1);
  ASSERT_EQ(ul_dcch_msg.msg.c1().type().value, asn1::rrc_nr::ul_dcch_msg_type_c::c1_c_::types_opts::meas_report);

  rrc_meas_results meas_results =
      asn1_to_measurement_results(ul_dcch_msg.msg.c1().meas_report().crit_exts.meas_report().meas_results, logger);

  // check if the meas results conversion was successful
  check_meas_results(meas_results);
}

TEST_F(rrc_ue_meas_report, when_invalid_meas_report_received_then_meas_results_are_empty)
{
  std::vector<uint8_t> fuzzed_data{0x01, 0xe2, 0x02, 0xdd, 0x00, 0x5d, 0x5e, 0xaf, 0xe0, 0x51, 0x07, 0x20, 0x04,
                                   0x03, 0xe4, 0x82, 0x07, 0x00, 0x00, 0x02, 0x00, 0x02, 0xbd, 0x60, 0x6c, 0x78,
                                   0x0a, 0xd5, 0x2f, 0x70, 0x70, 0xfb, 0xa3, 0xc0, 0xc1, 0xb9, 0xbf, 0x01, 0x43,
                                   0xe0, 0x01, 0x66, 0x07, 0x08, 0xcf, 0x92, 0x81, 0xc0, 0x40, 0x00, 0x60, 0x00,
                                   0x01, 0x00, 0x81, 0x05, 0x25, 0xb9, 0xb4, 0xad, 0x44, 0x06, 0xa0, 0xd3, 0x0f,
                                   0x4a, 0x6f, 0x10, 0x20, 0xc0, 0xfc, 0x00, 0xfb, 0x85, 0xcd, 0x8e, 0x00, 0x00};
  ocudu::byte_buffer   meas_report_pdu = ocudu::byte_buffer::create(fuzzed_data).value();

  // Parse UL-CCCH
  asn1::rrc_nr::ul_dcch_msg_s ul_dcch_msg;
  asn1::cbit_ref              bref({meas_report_pdu.begin(), meas_report_pdu.end()});
  ASSERT_EQ(ul_dcch_msg.unpack(bref), asn1::OCUDUASN_SUCCESS);
  ASSERT_EQ(ul_dcch_msg.msg.type().value, asn1::rrc_nr::ul_dcch_msg_type_c::types_opts::c1);
  ASSERT_EQ(ul_dcch_msg.msg.c1().type().value, asn1::rrc_nr::ul_dcch_msg_type_c::c1_c_::types_opts::meas_report);

  rrc_meas_results meas_results =
      asn1_to_measurement_results(ul_dcch_msg.msg.c1().meas_report().crit_exts.meas_report().meas_results, logger);

  ASSERT_TRUE(meas_results.meas_result_neigh_cells.has_value() and
              meas_results.meas_result_neigh_cells->meas_result_list_nr.empty());
}

// Fixture with a configurable measurement notifier so we can inject a real rrc_meas_cfg.
class rrc_ue_packed_meas_config : public rrc_ue_test_helper, public ::testing::Test
{
protected:
  static void SetUpTestSuite() { ocudulog::init(); }

  void SetUp() override
  {
    // Configure the mock to return the standard dummy meas config (valid, encodable).
    rrc_ue_cu_cp_notifier.next_meas_cfg = generate_dummy_meas_config();

    init();
  }

  void TearDown() override { ocudulog::flush(); }
};

TEST_F(rrc_ue_packed_meas_config, cond_meas_true_returns_non_empty_without_mutating_context)
{
  // Baseline: pack the regular (non-CHO) config and record its length.
  byte_buffer regular_packed = rrc_ue->get_packed_meas_config();

  // CHO path: must return a non-empty buffer (mock returns a valid meas config).
  const std::vector<pci_t> candidate_pcis = {2, 3};
  byte_buffer              cho_packed     = rrc_ue->get_packed_meas_config(candidate_pcis);
  ASSERT_FALSE(cho_packed.empty());

  // Context must not have been mutated: a subsequent non-CHO call must produce the same result
  // as the baseline, confirming the CHO path did not overwrite the stored meas config.
  byte_buffer regular_packed_after = rrc_ue->get_packed_meas_config();
  ASSERT_EQ(regular_packed.length(), regular_packed_after.length());
}
