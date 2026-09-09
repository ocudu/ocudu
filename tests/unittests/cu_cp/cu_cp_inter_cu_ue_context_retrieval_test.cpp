// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "tests/test_doubles/ngap/ngap_test_message_validators.h"
#include "tests/test_doubles/rrc/rrc_test_message_validators.h"
#include "tests/test_doubles/rrc/rrc_test_messages.h"
#include "tests/test_doubles/xnap/xnap_test_message_validators.h"
#include "tests/unittests/cu_cp/cu_cp_test_environment.h"
#include "tests/unittests/e1ap/common/e1ap_cu_cp_test_messages.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "tests/unittests/xnap/xnap_test_messages.h"
#include "ocudu/asn1/e1ap/e1ap_pdu_contents.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "ocudu/asn1/ngap/ngap_pdu_contents.h"
#include "ocudu/asn1/xnap/xnap_pdu_contents.h"
#include "ocudu/security/integrity.h"
#include "ocudu/security/security_asn1_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

/// Fixture for the target side of the UE context retrieval procedure (TS 38.423 section 8.2.4), i.e. this node
/// fetching a UE context from the peer that still holds it after the UE reestablished here.
class cu_cp_inter_cu_ue_context_retrieval_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_inter_cu_ue_context_retrieval_test() : cu_cp_inter_cu_ue_context_retrieval_test(false) {}

  explicit cu_cp_inter_cu_ue_context_retrieval_test(bool enable_rrc_inactive) :
    cu_cp_test_environment({/* max nof cu-ups */ 8,
                            /* max nof dus */ 8,
                            /* max nof ues */ 8192,
                            /* max nof drbs per ue */ 8,
                            /* amf config */ {{default_supported_tracking_area}},
                            /* trigger ho from measurements */ true,
                            enable_rrc_inactive,
                            /* enable xnc peer */ true})
  {
    run_ng_setup();

    // Run XN setup to completion.
    run_xn_setup();

    std::optional<unsigned> ret = connect_new_du();
    EXPECT_TRUE(ret.has_value());
    du_idx = ret.value();
    EXPECT_TRUE(this->run_f1_setup(du_idx));

    ret = connect_new_cu_up();
    EXPECT_TRUE(ret.has_value());
    cu_up_idx = ret.value();
    EXPECT_TRUE(this->run_e1_setup(cu_up_idx));
  }

  /// Injects an RRCReestablishmentRequest for a UE this node has no context for.
  void ue_sends_rrc_reest_request(pci_t failure_pci)
  {
    byte_buffer rrc_container = test_helpers::pack_ul_ccch_msg(
        test_helpers::create_rrc_reestablishment_request(old_crnti, failure_pci, "1111010001000010"));

    get_du(du_idx).push_ul_pdu(test_helpers::generate_init_ul_rrc_message_transfer(
        du_ue_id, crnti, plmn_identity::test_value(), {}, std::move(rrc_container)));
  }

  /// Waits for Msg4 and reports whether it was an RRCReestablishment on SRB1, the RRC Setup fallback arriving on
  /// SRB0.
  [[nodiscard]] bool wait_for_msg4_and_check_reestablishment_accepted()
  {
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu), "Msg4 was not sent to the DU");
    report_fatal_error_if_not(test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu), "Invalid DL RRC message");

    auto& dl_rrc_msg = *f1ap_pdu.pdu.init_msg().value.dl_rrc_msg_transfer();
    cu_ue_id         = int_to_gnb_cu_ue_f1ap_id(dl_rrc_msg.gnb_cu_ue_f1ap_id);

    // The RRC Setup fallback goes over SRB0.
    if (int_to_srb_id(dl_rrc_msg.srb_id) == srb_id_t::srb0) {
      report_fatal_error_if_not(test_helpers::is_valid_rrc_setup(dl_rrc_msg.rrc_container), "Invalid RRC Setup");
      return false;
    }

    // The RRCReestablishment goes over SRB1, behind a PDCP header.
    byte_buffer dl_dcch = dl_rrc_msg.rrc_container.deep_copy().value();
    dl_dcch.trim_head(2);
    report_fatal_error_if_not(test_helpers::is_valid_rrc_reestablishment(dl_dcch), "Invalid RRC Reestablishment");

    // The old F1AP UE ID identifies a UE context at this DU for the DU to release, which applies to a UE that was
    // already served here.
    report_fatal_error_if_not(not dl_rrc_msg.old_gnb_du_ue_f1ap_id_present,
                              "Unexpected old gNB-DU-UE-F1AP-ID for a context retrieved from a peer");

    return true;
  }

  /// Rebuilds the AS keys the CU-CP derives from the retrieved KgNB*, so the test can integrity protect the UL
  /// messages it injects. The peer transfers only KgNB* and the UE's capabilities; the algorithms are selected locally
  /// from the CU-CP's preference lists, so both sides are reproduced here the same way.
  security::sec_128_as_config make_retrieved_rrc_as_config()
  {
    security::security_context sec_ctxt;
    security::asn1_to_key(sec_ctxt.k,
                          generate_retrieve_ue_context_response(local_xnap_ue_id, peer_xnap_ue_id)
                              .pdu.successful_outcome()
                              .value.retrieve_ue_context_resp()
                              ->ue_context_info_retr_ue_ctxt_resp.security_info.key_ng_ran_star);

    // Capabilities as the generated response advertises them.
    sec_ctxt.supported_int_algos = {true, true, false};
    sec_ctxt.supported_enc_algos = {true, true, false};

    const cu_cp_configuration& cfg = get_cu_cp_cfg();
    report_error_if_not(sec_ctxt.select_algorithms(cfg.security.int_algo_pref_list, cfg.security.enc_algo_pref_list),
                        "Could not select security algorithms");
    sec_ctxt.generate_as_keys();

    return sec_ctxt.get_128_as_config(security::sec_domain::rrc);
  }

  /// Sends an UL DCCH message on SRB1, integrity protected with the keys derived from the retrieved KgNB*. Without a
  /// matching MAC-I the PDCP discards it and releases the UE.
  [[nodiscard]] bool ue_sends_ul_dcch(byte_buffer pdu, uint32_t count)
  {
    if (!pdu.prepend(std::array<uint8_t, 2>{0x00U, static_cast<uint8_t>(count)})) {
      return false;
    }

    const security::sec_128_as_config as_cfg = make_retrieved_rrc_as_config();
    report_error_if_not(as_cfg.k_128_int.has_value(), "No integrity key was derived");

    security::sec_mac mac  = {};
    byte_buffer_view  view = {pdu};
    security::security_nia2(mac,
                            as_cfg.k_128_int.value(),
                            count,
                            srb_id_to_uint(srb_id_t::srb1) - 1,
                            security::security_direction::uplink,
                            view);

    if (!pdu.append(mac)) {
      return false;
    }

    get_du(du_idx).push_ul_pdu(
        test_helpers::generate_ul_rrc_message_transfer(du_ue_id, cu_ue_id, srb_id_t::srb1, std::move(pdu)));
    return true;
  }

  [[nodiscard]] bool ue_sends_rrc_reest_complete()
  {
    return ue_sends_ul_dcch(test_helpers::pack_ul_dcch_msg(test_helpers::create_rrc_reestablishment_complete()), 0);
  }

  [[nodiscard]] bool ue_sends_rrc_reconfiguration_complete()
  {
    return ue_sends_ul_dcch(test_helpers::pack_ul_dcch_msg(test_helpers::create_rrc_reconfiguration_complete(1)), 1);
  }

  /// Retrieves the UE context from the peer and answers the UE with an RRCReestablishment.
  [[nodiscard]] bool retrieve_ue_context()
  {
    ue_sends_rrc_reest_request(xnc_peer_served_pci);
    report_fatal_error_if_not(this->wait_for_xnap_tx_pdu(xnc_peer_idx, xnap_pdu),
                              "No Retrieve UE Context Request was sent");
    get_xnc_cu_cp(xnc_peer_idx).push_tx_pdu(generate_retrieve_ue_context_response(local_xnap_ue_id, peer_xnap_ue_id));
    report_fatal_error_if_not(wait_for_msg4_and_check_reestablishment_accepted(), "Reestablishment was not accepted");
    return true;
  }

  unsigned du_idx       = 0;
  unsigned cu_up_idx    = 0;
  unsigned xnc_peer_idx = 0;

  gnb_du_ue_f1ap_id_t du_ue_id = gnb_du_ue_f1ap_id_t::min;
  gnb_cu_ue_f1ap_id_t cu_ue_id = gnb_cu_ue_f1ap_id_t::invalid;
  rnti_t              crnti    = to_rnti(0x4601);
  /// C-RNTI the UE had at the peer. No local UE holds it, which is what forces the retrieval.
  rnti_t old_crnti = to_rnti(0x4602);

  local_xnap_ue_id_t local_xnap_ue_id = local_xnap_ue_id_t::min;
  peer_xnap_ue_id_t  peer_xnap_ue_id  = peer_xnap_ue_id_t::min;

  gnb_cu_cp_ue_e1ap_id_t cu_cp_e1ap_id = gnb_cu_cp_ue_e1ap_id_t::invalid;
  gnb_cu_up_ue_e1ap_id_t cu_up_e1ap_id = gnb_cu_up_ue_e1ap_id_t::min;
  amf_ue_id_t            amf_ue_id     = amf_ue_id_t::min;
  /// AMF UE ID the generated Retrieve UE Context Response reports for the UE.
  static constexpr uint64_t peer_reported_amf_ue_id = 1;
  /// DL UE AMBR the generated Retrieve UE Context Response reports for the UE.
  static constexpr uint64_t peer_reported_ue_ambr = 1000000000;
  ran_ue_id_t               ran_ue_id             = ran_ue_id_t::min;

  xnap_message xnap_pdu;
  e1ap_message e1ap_pdu;
  f1ap_message f1ap_pdu;
  ngap_message ngap_pdu;
};

TEST_F(cu_cp_inter_cu_ue_context_retrieval_test, when_peer_serves_failure_cell_then_ue_context_is_retrieved_over_xn)
{
  ue_sends_rrc_reest_request(xnc_peer_served_pci);

  // No local context matches, so the peer serving that PCI must be asked for it.
  ASSERT_TRUE(this->wait_for_xnap_tx_pdu(xnc_peer_idx, xnap_pdu)) << "No Retrieve UE Context Request was sent";
  ASSERT_TRUE(test_helpers::is_pdu_type(xnap_pdu,
                                        asn1::xnap::xnap_elem_procs_o::init_msg_c::types::retrieve_ue_context_request))
      << "Unexpected XNAP message sent to the peer";

  get_xnc_cu_cp(xnc_peer_idx).push_tx_pdu(generate_retrieve_ue_context_response(local_xnap_ue_id, peer_xnap_ue_id));

  // With the context in hand, the UE is answered with an RRCReestablishment.
  ASSERT_TRUE(wait_for_msg4_and_check_reestablishment_accepted())
      << "Fell back to RRC Setup despite a successful retrieval";
}

TEST_F(cu_cp_inter_cu_ue_context_retrieval_test, when_context_is_retrieved_then_bearers_are_setup_and_path_is_switched)
{
  ASSERT_TRUE(retrieve_ue_context());
  ASSERT_TRUE(ue_sends_rrc_reest_complete());

  // The retrieved PDU sessions are established at the CU-UP, which holds nothing for this UE.
  ASSERT_TRUE(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu)) << "No Bearer Context Setup Request was sent";
  ASSERT_EQ(e1ap_pdu.pdu.init_msg().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::bearer_context_setup_request)
      << "Expected a Bearer Context Setup Request for a retrieved context";
  const auto& bearer_setup = *e1ap_pdu.pdu.init_msg().value.bearer_context_setup_request();
  cu_cp_e1ap_id            = int_to_gnb_cu_cp_ue_e1ap_id(bearer_setup.gnb_cu_cp_ue_e1ap_id);

  // The AMBR is learned from the AMF at Initial Context Setup, which this UE went through at the peer, so the value
  // the peer reported is the one to use.
  ASSERT_EQ(bearer_setup.ue_dl_aggr_max_bit_rate, peer_reported_ue_ambr)
      << "The CU-UP was not given the AMBR the peer reported";

  // The DU holds a UE context from Msg3, which is modified to carry the retrieved bearers.
  get_cu_up(cu_up_idx).push_tx_pdu(ocucp::generate_bearer_context_setup_response(cu_cp_e1ap_id, cu_up_e1ap_id));
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu)) << "No UE Context Modification Request was sent";
  ASSERT_EQ(f1ap_pdu.pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::ue_context_mod_request)
      << "Expected a UE Context Modification Request for a UE the DU already knows";

  // SRB1 survived the reestablishment, so only SRB2 and the DRBs are added.
  const auto& ue_ctxt_mod = *f1ap_pdu.pdu.init_msg().value.ue_context_mod_request();
  ASSERT_TRUE(ue_ctxt_mod.srbs_to_be_setup_mod_list_present);
  ASSERT_EQ(ue_ctxt_mod.srbs_to_be_setup_mod_list.size(), 1);
  ASSERT_TRUE(ue_ctxt_mod.drbs_to_be_setup_mod_list_present);

  // The UE holds the configuration of the peer, which this node cannot signal a delta on top of, so the DU is asked
  // for a full configuration (TS 38.401 section 8.4.1.1).
  ASSERT_TRUE(ue_ctxt_mod.full_cfg_present) << "The DU was asked for a delta on the configuration of the peer";

  get_du(du_idx).push_ul_pdu(test_helpers::generate_ue_context_modification_response(du_ue_id, cu_ue_id, crnti));

  // The CU-UP is told about the DL F1-U tunnels the DU allocated.
  ASSERT_TRUE(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu)) << "No Bearer Context Modification Request was sent";
  ASSERT_EQ(e1ap_pdu.pdu.init_msg().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::bearer_context_mod_request);
  get_cu_up(cu_up_idx).push_tx_pdu(ocucp::generate_bearer_context_modification_response(cu_cp_e1ap_id, cu_up_e1ap_id));

  // The UE is reconfigured with SRB2 and its DRBs.
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu)) << "No RRC Reconfiguration was sent";
  ASSERT_TRUE(test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu));

  const byte_buffer packed_dl_dcch = test_helpers::extract_dl_dcch_msg(test_helpers::get_rrc_container(f1ap_pdu));
  asn1::cbit_ref    bref{packed_dl_dcch};
  asn1::rrc_nr::dl_dcch_msg_s dl_dcch_msg;
  ASSERT_EQ(dl_dcch_msg.unpack(bref), asn1::OCUDUASN_SUCCESS);

  // The UE releases the configuration it holds from the peer before applying this one (TS 38.331 section 5.3.5.11),
  // so the bearers carry the configuration to set them up anew.
  const auto& recfg_ies = dl_dcch_msg.msg.c1().rrc_recfg().crit_exts.rrc_recfg();
  ASSERT_TRUE(recfg_ies.non_crit_ext_present);
  ASSERT_TRUE(recfg_ies.non_crit_ext.full_cfg_present)
      << "The UE was told to apply the configuration of the peer as a delta";
  ASSERT_TRUE(recfg_ies.radio_bearer_cfg_present);
  ASSERT_EQ(recfg_ies.radio_bearer_cfg.drb_to_add_mod_list.size(), 1);
  const auto& asn1_drb = recfg_ies.radio_bearer_cfg.drb_to_add_mod_list[0];
  ASSERT_FALSE(asn1_drb.reestablish_pdcp_present) << "A released DRB cannot re-establish its PDCP entity";
  ASSERT_TRUE(asn1_drb.pdcp_cfg_present) << "The DRB was set up without a PDCP configuration";

  ASSERT_TRUE(ue_sends_rrc_reconfiguration_complete());

  // Only once the UE is served here is the AMF asked to move the DL path over (TS 38.413 section 8.4.4).
  ASSERT_TRUE(this->wait_for_ngap_tx_pdu(ngap_pdu)) << "No Path Switch Request was sent";
  ASSERT_TRUE(test_helpers::is_valid_path_switch_request(ngap_pdu)) << "Invalid Path Switch Request";

  // The AMF identifies the UE by the ID it has there, which is why the retrieval reports it.
  const auto& path_switch = *ngap_pdu.pdu.init_msg().value.path_switch_request();
  ASSERT_EQ(path_switch.source_amf_ue_ngap_id, peer_reported_amf_ue_id)
      << "Path Switch did not carry the AMF UE ID the peer reported";

  // The ack has to echo the RAN UE ID the CU-CP allocated for this UE, which it learns only from the request.
  get_amf().push_tx_pdu(generate_path_switch_request_ack(uint_to_amf_ue_id(peer_reported_amf_ue_id),
                                                         uint_to_ran_ue_id(path_switch.ran_ue_ngap_id)));

  // Finally the peer is told to drop its context, which signals the takeover.
  ASSERT_TRUE(this->wait_for_xnap_tx_pdu(xnc_peer_idx, xnap_pdu)) << "No UE Context Release was sent to the peer";
  ASSERT_TRUE(test_helpers::is_pdu_type(xnap_pdu, asn1::xnap::xnap_elem_procs_o::init_msg_c::types::ue_context_release))
      << "Expected a UE Context Release towards the peer";
}

TEST_F(cu_cp_inter_cu_ue_context_retrieval_test, when_peer_rejects_retrieval_then_fallback_to_rrc_setup)
{
  ue_sends_rrc_reest_request(xnc_peer_served_pci);
  ASSERT_TRUE(this->wait_for_xnap_tx_pdu(xnc_peer_idx, xnap_pdu));

  // The peer refuses, e.g. because the ShortMAC-I did not verify against the UE's source keys.
  get_xnc_cu_cp(xnc_peer_idx).push_tx_pdu(generate_retrieve_ue_context_failure(local_xnap_ue_id));

  ASSERT_FALSE(wait_for_msg4_and_check_reestablishment_accepted())
      << "Reestablishment was accepted although the peer rejected the retrieval";
}

TEST_F(cu_cp_inter_cu_ue_context_retrieval_test, when_no_peer_serves_failure_cell_then_no_retrieval_is_attempted)
{
  // The UE reports a cell outside the peer's served cell list, which leaves the peer unresolved.

  ue_sends_rrc_reest_request(xnc_peer_served_pci + 1);

  // Fall back to RRC Setup as before, without bothering the peer.
  ASSERT_FALSE(wait_for_msg4_and_check_reestablishment_accepted()) << "Unexpected reestablishment";
  ASSERT_FALSE(this->get_xnc_cu_cp(xnc_peer_idx).try_pop_rx_pdu(xnap_pdu))
      << "A retrieval was attempted for a cell no peer serves";
}

/// Fixture for a UE resuming at this node while the peer that suspended it still holds its context
/// (TS 38.300 section 9.2.2.4.1). The peer is resolved from the gNB ID inside the I-RNTI, and the bearers are up
/// before the RRCResume, because the UE resumes its DRBs on receiving it.
class cu_cp_inter_cu_ue_context_retrieval_resume_test : public cu_cp_inter_cu_ue_context_retrieval_test
{
public:
  cu_cp_inter_cu_ue_context_retrieval_resume_test() : cu_cp_inter_cu_ue_context_retrieval_test(true) {}

  /// Builds a Short-I-RNTI the way the peer would have allocated it, from the node identifier its gNB ID carries.
  uint64_t make_peer_i_rnti(uint32_t ue_ref = 0) const
  {
    const short_i_rnti_profile profile = get_cu_cp_cfg().ue.short_i_rnti_prof;
    return short_i_rnti_t{profile, short_i_rnti_t::to_local_node_id(profile, get_xnc_peer_gnb_id().id), ue_ref}.value();
  }

  /// Builds a Short-I-RNTI carrying a node identifier no connected peer has.
  uint64_t make_unknown_i_rnti() const
  {
    const short_i_rnti_profile profile = get_cu_cp_cfg().ue.short_i_rnti_prof;
    const uint32_t             node_id = short_i_rnti_t::to_local_node_id(profile, get_xnc_peer_gnb_id().id + 1);
    return short_i_rnti_t{profile, node_id, 0}.value();
  }

  /// Injects an RRCResumeRequest for a UE this node has no context for. The ResumeMAC-I is only verifiable by the peer
  /// holding the UE's source keys, so any value gets as far as the retrieval.
  void ue_sends_rrc_resume_request(uint64_t                     i_rnti,
                                   asn1::rrc_nr::resume_cause_e resume_cause = asn1::rrc_nr::resume_cause_opts::mo_data)
  {
    byte_buffer rrc_container = test_helpers::pack_ul_ccch_msg(
        test_helpers::create_rrc_resume_request(i_rnti, "1111010001000010", resume_cause));

    get_du(du_idx).push_ul_pdu(test_helpers::generate_init_ul_rrc_message_transfer(
        du_ue_id, crnti, plmn_identity::test_value(), {}, std::move(rrc_container)));
  }

  /// Retrieves the UE context from the peer the I-RNTI points at.
  [[nodiscard]] bool retrieve_ue_context_for_resume()
  {
    ue_sends_rrc_resume_request(make_peer_i_rnti());
    report_fatal_error_if_not(this->wait_for_xnap_tx_pdu(xnc_peer_idx, xnap_pdu),
                              "No Retrieve UE Context Request was sent");
    report_fatal_error_if_not(
        test_helpers::is_pdu_type(xnap_pdu,
                                  asn1::xnap::xnap_elem_procs_o::init_msg_c::types::retrieve_ue_context_request),
        "Unexpected XNAP message sent to the peer");

    // The UE is identified towards the peer by the I-RNTI it allocated, not by a failure cell.
    const auto& request = *xnap_pdu.pdu.init_msg().value.retrieve_ue_context_request();
    report_fatal_error_if_not(request.ue_context_id.type() == asn1::xnap::ue_context_id_c::types_opts::rrc_resume,
                              "The retrieval did not carry the RRC Resume UE Context ID");
    report_fatal_error_if_not(request.ue_context_id.rrc_resume().i_rnti.i_rnti_short().to_number() ==
                                  make_peer_i_rnti(),
                              "The retrieval did not carry the I-RNTI the UE presented");

    get_xnc_cu_cp(xnc_peer_idx).push_tx_pdu(generate_retrieve_ue_context_response(local_xnap_ue_id, peer_xnap_ue_id));
    return true;
  }

  /// Establishes the bearers the RRCResume needs and returns once Msg4 was sent to the UE.
  [[nodiscard]] bool setup_bearers_and_await_rrc_resume()
  {
    // The retrieved PDU sessions are established at the CU-UP, which holds nothing for this UE.
    report_fatal_error_if_not(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu), "No Bearer Context Setup Request");
    report_fatal_error_if_not(e1ap_pdu.pdu.init_msg().value.type().value ==
                                  asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::bearer_context_setup_request,
                              "Expected a Bearer Context Setup Request for a retrieved context");
    cu_cp_e1ap_id =
        int_to_gnb_cu_cp_ue_e1ap_id(e1ap_pdu.pdu.init_msg().value.bearer_context_setup_request()->gnb_cu_cp_ue_e1ap_id);
    get_cu_up(cu_up_idx).push_tx_pdu(ocucp::generate_bearer_context_setup_response(cu_cp_e1ap_id, cu_up_e1ap_id));

    // The UE performed a fresh random access, so the DU holds no SRBs or DRBs for it and the context is set up.
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu), "No UE Context Setup Request");
    report_fatal_error_if_not(f1ap_pdu.pdu.init_msg().value.type().value ==
                                  asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::ue_context_setup_request,
                              "Expected a UE Context Setup Request for a resuming UE");

    const auto& ue_ctxt_setup = *f1ap_pdu.pdu.init_msg().value.ue_context_setup_request();
    cu_ue_id                  = int_to_gnb_cu_ue_f1ap_id(ue_ctxt_setup.gnb_cu_ue_f1ap_id);
    report_fatal_error_if_not(ue_ctxt_setup.srbs_to_be_setup_list_present, "No SRBs were set up at the DU");
    report_fatal_error_if_not(ue_ctxt_setup.srbs_to_be_setup_list.size() == 2,
                              "Expected SRB1 and SRB2 to be set up at the DU");
    report_fatal_error_if_not(ue_ctxt_setup.drbs_to_be_setup_list_present, "No DRBs were set up at the DU");

    get_du(du_idx).push_ul_pdu(test_helpers::generate_ue_context_setup_response(
        cu_ue_id, du_ue_id, crnti, test_helpers::create_cell_group_config(), {drb_id_t::drb1}));

    // The CU-UP is told about the DL F1-U tunnels the DU allocated.
    report_fatal_error_if_not(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu),
                              "No Bearer Context Modification Request");
    report_fatal_error_if_not(e1ap_pdu.pdu.init_msg().value.type().value ==
                                  asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::bearer_context_mod_request,
                              "Expected a Bearer Context Modification Request");
    get_cu_up(cu_up_idx).push_tx_pdu(
        ocucp::generate_bearer_context_modification_response(cu_cp_e1ap_id, cu_up_e1ap_id));

    // Only now can Msg4 be sent, as the UE resumes its DRBs upon receiving it.
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu), "Msg4 was not sent to the DU");
    report_fatal_error_if_not(test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu), "Invalid DL RRC message");

    const auto& dl_rrc_msg = *f1ap_pdu.pdu.init_msg().value.dl_rrc_msg_transfer();
    report_fatal_error_if_not(int_to_srb_id(dl_rrc_msg.srb_id) == srb_id_t::srb1,
                              "Msg4 was not an RRCResume on SRB1 but an RRC Setup fallback on SRB0");

    return true;
  }

  [[nodiscard]] bool ue_sends_rrc_resume_complete()
  {
    return ue_sends_ul_dcch(test_helpers::pack_ul_dcch_msg(test_helpers::create_rrc_resume_complete()), 0);
  }
};

TEST_F(cu_cp_inter_cu_ue_context_retrieval_resume_test, when_i_rnti_points_at_peer_then_ue_context_is_retrieved_over_xn)
{
  ASSERT_TRUE(retrieve_ue_context_for_resume());
  ASSERT_TRUE(setup_bearers_and_await_rrc_resume()) << "The UE was not resumed with the retrieved context";
}

TEST_F(cu_cp_inter_cu_ue_context_retrieval_resume_test, when_context_is_retrieved_then_rrc_resume_carries_a_full_config)
{
  ASSERT_TRUE(retrieve_ue_context_for_resume());
  ASSERT_TRUE(setup_bearers_and_await_rrc_resume());

  const byte_buffer packed_dl_dcch = test_helpers::extract_dl_dcch_msg(test_helpers::get_rrc_container(f1ap_pdu));
  asn1::cbit_ref    bref{packed_dl_dcch};
  asn1::rrc_nr::dl_dcch_msg_s dl_dcch_msg;
  ASSERT_EQ(dl_dcch_msg.unpack(bref), asn1::OCUDUASN_SUCCESS);
  ASSERT_TRUE(test_helpers::is_valid_rrc_resume(dl_dcch_msg));

  // The configuration the UE stored while suspended is the one of the peer, which this node cannot signal a delta on
  // top of (TS 38.331 section 5.3.13.4).
  const auto& resume_ies = dl_dcch_msg.msg.c1().rrc_resume().crit_exts.rrc_resume();
  ASSERT_TRUE(resume_ies.full_cfg_present) << "The UE was told to apply the configuration of the peer as a delta";

  // The UE releases the bearers it stored, so they carry the configuration to set them up anew.
  ASSERT_TRUE(resume_ies.radio_bearer_cfg_present);
  ASSERT_EQ(resume_ies.radio_bearer_cfg.drb_to_add_mod_list.size(), 1);
  const auto& asn1_drb = resume_ies.radio_bearer_cfg.drb_to_add_mod_list[0];
  ASSERT_FALSE(asn1_drb.reestablish_pdcp_present) << "A released DRB cannot re-establish its PDCP entity";
  ASSERT_TRUE(asn1_drb.pdcp_cfg_present) << "The DRB was set up without a PDCP configuration";
  ASSERT_TRUE(asn1_drb.cn_assoc_present) << "The DRB was set up without an SDAP configuration";
}

TEST_F(cu_cp_inter_cu_ue_context_retrieval_resume_test, when_context_is_retrieved_on_resume_then_path_is_switched)
{
  ASSERT_TRUE(retrieve_ue_context_for_resume());
  ASSERT_TRUE(setup_bearers_and_await_rrc_resume());

  // The path is only switched once the UE confirms the RRCResume (TS 38.300 section 9.2.2.4.1, steps 7 and 8).
  ASSERT_FALSE(this->wait_for_ngap_tx_pdu(ngap_pdu, std::chrono::milliseconds{5}))
      << "The path was switched before the UE confirmed the RRCResume";

  ASSERT_TRUE(ue_sends_rrc_resume_complete());

  ASSERT_TRUE(this->wait_for_ngap_tx_pdu(ngap_pdu)) << "No Path Switch Request was sent";
  ASSERT_TRUE(test_helpers::is_valid_path_switch_request(ngap_pdu)) << "Invalid Path Switch Request";

  const auto& path_switch = *ngap_pdu.pdu.init_msg().value.path_switch_request();
  ASSERT_EQ(path_switch.source_amf_ue_ngap_id, peer_reported_amf_ue_id)
      << "Path Switch did not carry the AMF UE ID the peer reported";

  get_amf().push_tx_pdu(generate_path_switch_request_ack(uint_to_amf_ue_id(peer_reported_amf_ue_id),
                                                         uint_to_ran_ue_id(path_switch.ran_ue_ngap_id)));

  // The peer is told to drop its context, which is what signals the takeover.
  ASSERT_TRUE(this->wait_for_xnap_tx_pdu(xnc_peer_idx, xnap_pdu)) << "No UE Context Release was sent to the peer";
  ASSERT_TRUE(test_helpers::is_pdu_type(xnap_pdu, asn1::xnap::xnap_elem_procs_o::init_msg_c::types::ue_context_release))
      << "Expected a UE Context Release towards the peer";
}

TEST_F(cu_cp_inter_cu_ue_context_retrieval_resume_test, when_context_is_retrieved_on_resume_then_nas_is_carried_on_srb2)
{
  ASSERT_TRUE(retrieve_ue_context_for_resume());
  ASSERT_TRUE(setup_bearers_and_await_rrc_resume());
  ASSERT_TRUE(ue_sends_rrc_resume_complete());

  // The path switch is what gives the AMF the RAN UE ID it addresses the UE with.
  ASSERT_TRUE(this->wait_for_ngap_tx_pdu(ngap_pdu)) << "No Path Switch Request was sent";
  const auto& path_switch = *ngap_pdu.pdu.init_msg().value.path_switch_request();
  get_amf().push_tx_pdu(generate_path_switch_request_ack(uint_to_amf_ue_id(peer_reported_amf_ue_id),
                                                         uint_to_ran_ue_id(path_switch.ran_ue_ngap_id)));
  ASSERT_TRUE(this->wait_for_xnap_tx_pdu(xnc_peer_idx, xnap_pdu)) << "No UE Context Release was sent to the peer";

  // A UE that resumed with a retrieved context reaches an RRC UE this node created, which carries NAS on the SRB2 it
  // set up for it.
  get_amf().push_tx_pdu(ocucp::generate_downlink_nas_transport_message(uint_to_amf_ue_id(peer_reported_amf_ue_id),
                                                                       uint_to_ran_ue_id(path_switch.ran_ue_ngap_id)));

  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu)) << "The NAS message was not forwarded to the UE";
  ASSERT_TRUE(test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu));
  ASSERT_EQ(int_to_srb_id(f1ap_pdu.pdu.init_msg().value.dl_rrc_msg_transfer()->srb_id), srb_id_t::srb2)
      << "The NAS message was not carried on SRB2";
}

TEST_F(cu_cp_inter_cu_ue_context_retrieval_resume_test, when_i_rnti_points_at_no_peer_then_no_retrieval_is_attempted)
{
  // An I-RNTI allocated by a node this one has no Xn-C link to cannot be resolved, so the UE gets a new connection
  // instead (TS 38.300 section 9.2.2.6 step 2).
  ue_sends_rrc_resume_request(make_unknown_i_rnti());

  ASSERT_FALSE(this->get_xnc_cu_cp(xnc_peer_idx).try_pop_rx_pdu(xnap_pdu))
      << "A retrieval was attempted for an I-RNTI no peer allocated";

  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu)) << "Msg4 was not sent to the DU";
  ASSERT_TRUE(test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu));
  ASSERT_EQ(int_to_srb_id(f1ap_pdu.pdu.init_msg().value.dl_rrc_msg_transfer()->srb_id), srb_id_t::srb0)
      << "Expected an RRC Setup fallback on SRB0";
}

TEST_F(cu_cp_inter_cu_ue_context_retrieval_resume_test,
       when_peer_rejects_retrieval_on_resume_then_fallback_to_rrc_setup)
{
  ue_sends_rrc_resume_request(make_peer_i_rnti());
  ASSERT_TRUE(this->wait_for_xnap_tx_pdu(xnc_peer_idx, xnap_pdu));

  // The peer refuses, e.g. because the ResumeMAC-I did not verify against the UE's source keys.
  get_xnc_cu_cp(xnc_peer_idx).push_tx_pdu(generate_retrieve_ue_context_failure(local_xnap_ue_id));

  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu)) << "Msg4 was not sent to the DU";
  ASSERT_TRUE(test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu));
  ASSERT_EQ(int_to_srb_id(f1ap_pdu.pdu.init_msg().value.dl_rrc_msg_transfer()->srb_id), srb_id_t::srb0)
      << "Expected an RRC Setup fallback on SRB0 after the peer rejected the retrieval";
}

TEST_F(cu_cp_inter_cu_ue_context_retrieval_resume_test, when_resume_cause_is_rna_update_then_no_retrieval_is_attempted)
{
  // An RNA update does not move the UE to RRC_CONNECTED, so it would need an anchor relocation rather than a resume
  // (TS 38.300 section 9.2.2.5). Until that exists the UE gets a new connection instead of a retrieval, so no bearers
  // are set up for a UE that is never answered with an RRCResume.
  ue_sends_rrc_resume_request(make_peer_i_rnti(), asn1::rrc_nr::resume_cause_opts::rna_upd);

  ASSERT_FALSE(this->get_xnc_cu_cp(xnc_peer_idx).try_pop_rx_pdu(xnap_pdu))
      << "A retrieval was attempted for an RNA update";

  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu)) << "Msg4 was not sent to the DU";
  ASSERT_TRUE(test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu));
  ASSERT_EQ(int_to_srb_id(f1ap_pdu.pdu.init_msg().value.dl_rrc_msg_transfer()->srb_id), srb_id_t::srb0)
      << "Expected an RRC Setup fallback on SRB0 for an RNA update from a peer's I-RNTI";
}
