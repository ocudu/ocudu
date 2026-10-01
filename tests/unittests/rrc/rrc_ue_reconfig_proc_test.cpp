// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rrc_ue_test_helpers.h"
#include "rrc_ue_test_messages.h"
#include "tests/ocudu_test_requirements.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/rrc_nr/dl_dcch_msg.h"
#include "ocudu/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

/// Fixture class RRC Reconfiguration tests preparation (to bring UE in RRC connected state)
class rrc_ue_reconfig : public rrc_ue_test_helper, public ::testing::Test
{
protected:
  static void SetUpTestSuite() { ocudulog::init(); }

  void SetUp() override
  {
    init();

    ocudulog::basic_logger& rrc_logger = ocudulog::fetch_basic_logger("RRC", false);
    rrc_logger.set_level(ocudulog::basic_levels::debug);
    rrc_logger.set_hex_dump_max_size(32);

    receive_setup_request();

    // check if the RRC setup message was generated
    ASSERT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);

    // check if SRB1 was created
    check_srb1_exists();

    receive_setup_complete();
  }

  void TearDown() override
  {
    // flush logger after each test
    ocudulog::flush();
  }
};

/// Test the RRC reconfig with connected AMF
TEST_F(rrc_ue_reconfig, when_reconfig_complete_received_proc_successful)
{
  // Prepare args
  rrc_reconfiguration_procedure_request args = generate_rrc_reconfiguration_procedure_request();

  // Trigger Reconfig
  async_task<bool>         t = get_rrc_ue_control_message_handler()->handle_rrc_reconfiguration_request(args);
  lazy_task_launcher<bool> t_launcher(t);

  ASSERT_FALSE(t.ready());

  check_rrc_reconfig_pdu();

  // Receive Reconfig complete
  receive_reconfig_complete();

  ASSERT_TRUE(t.ready());
}

// Test that get_rrc_ue_cond_reconfiguration_context() encodes meas_gap_cfg into the outer CHO
// RRCReconfiguration measConfig when the request carries one.
TEST_F(rrc_ue_reconfig, when_meas_gap_cfg_in_request_then_outer_cho_rrc_message_includes_it)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-MOB-15");

  // Build a packed measGapConfig (gapFR2 setup, offset=10, mgl=ms6, mgrp=ms20, mgta=ms0).
  asn1::rrc_nr::meas_gap_cfg_s asn1_gap;
  asn1_gap.gap_fr2_present = true;
  asn1_gap.gap_fr2.set_setup();
  asn1_gap.gap_fr2.setup().gap_offset = 10;
  asn1_gap.gap_fr2.setup().mgl.value  = asn1::rrc_nr::gap_cfg_s::mgl_opts::ms6;
  asn1_gap.gap_fr2.setup().mgrp.value = asn1::rrc_nr::gap_cfg_s::mgrp_opts::ms20;
  asn1_gap.gap_fr2.setup().mgta.value = asn1::rrc_nr::gap_cfg_s::mgta_opts::ms0;

  byte_buffer packed_gap;
  {
    asn1::bit_ref bref(packed_gap);
    ASSERT_EQ(asn1_gap.pack(bref), asn1::OCUDUASN_SUCCESS);
  }

  // Build a CHO RRC Reconfiguration request with the packed gap.
  // meas_cfg must be present so the measConfig IE block (which injects the gap) runs.
  rrc_reconfiguration_procedure_request req;
  req.cho_candidates = std::vector<cu_cp_ue_cho_candidate>{};
  req.meas_cfg       = generate_dummy_meas_config();
  req.meas_gap_cfg   = packed_gap.copy();

  rrc_ue_cond_reconfiguration_context ctx = rrc_ue->get_rrc_ue_cond_reconfiguration_context(req);
  ASSERT_FALSE(ctx.rrc_ue_cond_reconfiguration_pdu.empty());

  // Strip PDCP header (2 B) and MAC-I (4 B) the same way existing HO tests do it.
  byte_buffer                 raw_pdu = test_helpers::extract_dl_dcch_msg(ctx.rrc_ue_cond_reconfiguration_pdu);
  asn1::rrc_nr::dl_dcch_msg_s dl_dcch;
  {
    asn1::cbit_ref bref(raw_pdu);
    ASSERT_EQ(dl_dcch.unpack(bref), asn1::OCUDUASN_SUCCESS);
  }
  const auto& recfg = dl_dcch.msg.c1().rrc_recfg().crit_exts.rrc_recfg();

  // measGapConfig must be present with the correct gapOffset.
  ASSERT_TRUE(recfg.meas_cfg_present);
  ASSERT_TRUE(recfg.meas_cfg.meas_gap_cfg_present);
  ASSERT_TRUE(recfg.meas_cfg.meas_gap_cfg.gap_fr2_present);
  ASSERT_EQ(recfg.meas_cfg.meas_gap_cfg.gap_fr2.setup().gap_offset, 10);
}

TEST_F(rrc_ue_reconfig, when_no_meas_gap_cfg_in_request_then_outer_cho_rrc_message_omits_it)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-MOB-15");

  rrc_reconfiguration_procedure_request req;
  req.cho_candidates = std::vector<cu_cp_ue_cho_candidate>{};
  req.meas_cfg       = generate_dummy_meas_config();
  // meas_gap_cfg intentionally left empty

  rrc_ue_cond_reconfiguration_context ctx = rrc_ue->get_rrc_ue_cond_reconfiguration_context(req);
  ASSERT_FALSE(ctx.rrc_ue_cond_reconfiguration_pdu.empty());

  byte_buffer                 raw_pdu = test_helpers::extract_dl_dcch_msg(ctx.rrc_ue_cond_reconfiguration_pdu);
  asn1::rrc_nr::dl_dcch_msg_s dl_dcch;
  {
    asn1::cbit_ref bref(raw_pdu);
    ASSERT_EQ(dl_dcch.unpack(bref), asn1::OCUDUASN_SUCCESS);
  }
  const auto& recfg = dl_dcch.msg.c1().rrc_recfg().crit_exts.rrc_recfg();

  // No measGapConfig should be present.
  ASSERT_TRUE(recfg.meas_cfg_present);
  ASSERT_FALSE(recfg.meas_cfg.meas_gap_cfg_present);
}
