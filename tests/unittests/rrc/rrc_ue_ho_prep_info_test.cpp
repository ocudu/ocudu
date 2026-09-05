// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rrc_ue_test_helpers.h"
#include "rrc_ue_test_messages.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/rrc_nr/rrc_nr.h"
#include "ocudu/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

/// Fixture for the RRC HandoverPreparationInformation generation tests.
class rrc_ue_ho_prep_info_test : public rrc_ue_test_helper, public ::testing::Test
{
protected:
  static void SetUpTestSuite() { ocudulog::init(); }

  void SetUp() override
  {
    init();

    receive_setup_request();
    ASSERT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);
    check_srb1_exists();
    receive_setup_complete();

    ASSERT_TRUE(init_security_context());
    enable_security();

    // Store UE capabilities. get_packed_handover_preparation_message() requires them to generate the message.
    rrc_ue_capability_transfer_request msg;
    async_task<bool>         t = get_rrc_ue_control_message_handler()->handle_rrc_ue_capability_transfer_request(msg);
    lazy_task_launcher<bool> t_launcher(t);
    receive_ue_capability_information(2);
    ASSERT_TRUE(t.ready());
  }

  void TearDown() override { ocudulog::flush(); }

  /// Adds a DRB with the given QoS flows to the UE's UP context, as returned by on_up_context_required().
  void add_drb_to_up_context(pdu_session_id_t psi, drb_id_t drb_id, const std::vector<qos_flow_id_t>& qfis)
  {
    up_context& up_ctxt = rrc_ue_cu_cp_notifier.up_ctxt;
    if (up_ctxt.pdu_sessions.find(psi) == up_ctxt.pdu_sessions.end()) {
      up_ctxt.pdu_sessions.emplace(
          psi,
          up_pdu_session_context{psi,
                                 pdu_session_type_t::ipv4,
                                 up_transport_layer_info{transport_layer_address::create_from_string("127.0.0.1"),
                                                         int_to_gtpu_teid(0x1)}});
    }

    up_drb_context drb_ctxt;
    drb_ctxt.drb_id               = drb_id;
    drb_ctxt.pdu_session_id       = psi;
    drb_ctxt.sdap_cfg.pdu_session = psi;
    drb_ctxt.rlc_mod              = rlc_mode::am;
    // PDCP configuration of a DRB.
    drb_ctxt.pdcp_cfg                    = {};
    drb_ctxt.pdcp_cfg.rb_type            = pdcp_rb_type::drb;
    drb_ctxt.pdcp_cfg.rlc_mode           = pdcp_rlc_mode::am;
    drb_ctxt.pdcp_cfg.ciphering_required = true;
    drb_ctxt.pdcp_cfg.tx.sn_size         = pdcp_sn_size::size18bits;
    drb_ctxt.pdcp_cfg.tx.discard_timer   = pdcp_discard_timer::ms100;
    drb_ctxt.pdcp_cfg.rx.sn_size         = pdcp_sn_size::size18bits;
    drb_ctxt.pdcp_cfg.rx.t_reordering    = pdcp_t_reordering::ms100;
    for (qos_flow_id_t qfi : qfis) {
      drb_ctxt.qos_flows.emplace(qfi, up_qos_flow_context{qfi, {}});
      drb_ctxt.sdap_cfg.mapped_qos_flows_to_add.push_back(qfi);
    }

    up_ctxt.pdu_sessions.at(psi).drbs.emplace(drb_id, drb_ctxt);
  }

  /// Unpacks the AS-Config's embedded RRCReconfiguration from a packed HandoverPreparationInformation.
  static bool unpack_as_config(asn1::rrc_nr::rrc_recfg_s& source_recfg, const byte_buffer& packed_ho_prep)
  {
    asn1::rrc_nr::ho_prep_info_s ho_prep_info;
    asn1::cbit_ref               bref({packed_ho_prep.begin(), packed_ho_prep.end()});
    if (ho_prep_info.unpack(bref) != asn1::OCUDUASN_SUCCESS) {
      return false;
    }
    const auto& ies = ho_prep_info.crit_exts.c1().ho_prep_info();
    if (!ies.source_cfg_present) {
      return false;
    }
    asn1::cbit_ref recfg_bref({ies.source_cfg.rrc_recfg.begin(), ies.source_cfg.rrc_recfg.end()});
    return source_recfg.unpack(recfg_bref) == asn1::OCUDUASN_SUCCESS;
  }
};

/// AS-Config must carry the UE's full current radio bearer configuration, not just the DRBs of the most recent
/// procedure. TS 38.331 Section 11.2.3 requires all fields reflecting the current AS configuration to be included.
TEST_F(rrc_ue_ho_prep_info_test, when_ue_has_multiple_pdu_sessions_then_as_config_contains_all_drbs)
{
  // Two PDU sessions, set up at different points in time, each with its own DRB.
  add_drb_to_up_context(pdu_session_id_t::min, drb_id_t::drb1, {qos_flow_id_t::min});
  add_drb_to_up_context(uint_to_pdu_session_id(2), drb_id_t::drb2, {uint_to_qos_flow_id(2)});

  byte_buffer packed_ho_prep = get_rrc_ue_control_message_handler()->get_packed_handover_preparation_message();
  ASSERT_FALSE(packed_ho_prep.empty());

  asn1::rrc_nr::rrc_recfg_s source_recfg;
  ASSERT_TRUE(unpack_as_config(source_recfg, packed_ho_prep));

  const auto& recfg_ies = source_recfg.crit_exts.rrc_recfg();
  ASSERT_TRUE(recfg_ies.radio_bearer_cfg_present);

  // Both DRBs must be present, each mapped to its own PDU session and QoS flow.
  const auto& drb_list = recfg_ies.radio_bearer_cfg.drb_to_add_mod_list;
  ASSERT_EQ(drb_list.size(), 2U);

  ASSERT_EQ(drb_list[0].drb_id, 1U);
  ASSERT_TRUE(drb_list[0].cn_assoc_present);
  ASSERT_EQ(drb_list[0].cn_assoc.type(), asn1::rrc_nr::drb_to_add_mod_s::cn_assoc_c_::types_opts::sdap_cfg);
  EXPECT_EQ(drb_list[0].cn_assoc.sdap_cfg().pdu_session, to_underlying(pdu_session_id_t::min));
  ASSERT_EQ(drb_list[0].cn_assoc.sdap_cfg().mapped_qos_flows_to_add.size(), 1U);
  EXPECT_EQ(drb_list[0].cn_assoc.sdap_cfg().mapped_qos_flows_to_add[0], 0U);

  ASSERT_EQ(drb_list[1].drb_id, 2U);
  ASSERT_TRUE(drb_list[1].cn_assoc_present);
  ASSERT_EQ(drb_list[1].cn_assoc.type(), asn1::rrc_nr::drb_to_add_mod_s::cn_assoc_c_::types_opts::sdap_cfg);
  EXPECT_EQ(drb_list[1].cn_assoc.sdap_cfg().pdu_session, 2U);
  ASSERT_EQ(drb_list[1].cn_assoc.sdap_cfg().mapped_qos_flows_to_add.size(), 1U);
  EXPECT_EQ(drb_list[1].cn_assoc.sdap_cfg().mapped_qos_flows_to_add[0], 2U);
}

/// A UE without any established DRB has no radio bearer configuration to report, so AS-Config is omitted.
TEST_F(rrc_ue_ho_prep_info_test, when_ue_has_no_drbs_then_as_config_is_absent)
{
  byte_buffer packed_ho_prep = get_rrc_ue_control_message_handler()->get_packed_handover_preparation_message();
  ASSERT_FALSE(packed_ho_prep.empty());

  asn1::rrc_nr::ho_prep_info_s ho_prep_info;
  asn1::cbit_ref               bref({packed_ho_prep.begin(), packed_ho_prep.end()});
  ASSERT_EQ(ho_prep_info.unpack(bref), asn1::OCUDUASN_SUCCESS);
  EXPECT_FALSE(ho_prep_info.crit_exts.c1().ho_prep_info().source_cfg_present);
}
