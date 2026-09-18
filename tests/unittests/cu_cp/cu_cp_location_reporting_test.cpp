// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_test_environment.h"
#include "lib/cu_cp/ue_location_manager/ue_location_manager.h"
#include "tests/test_doubles/ngap/ngap_test_message_validators.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/ngap/ngap_ies.h"
#include "ocudu/asn1/ngap/ngap_pdu_contents.h"
#include "ocudu/ngap/ngap_message.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

class cu_cp_location_reporting_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_location_reporting_test() : cu_cp_test_environment(cu_cp_test_env_params{})
  {
    // Run NG setup to completion.
    run_ng_setup();

    // Setup DU.
    std::optional<unsigned> ret = connect_new_du();
    EXPECT_TRUE(ret.has_value());
    du_idx = ret.value();
    EXPECT_TRUE(this->run_f1_setup(du_idx));

    // Setup CU-UP.
    ret = connect_new_cu_up();
    EXPECT_TRUE(ret.has_value());
    cu_up_idx = ret.value();
    EXPECT_TRUE(this->run_e1_setup(cu_up_idx));
  }

  [[nodiscard]] bool attach_ue()
  {
    if (!cu_cp_test_environment::attach_ue(
            du_idx, cu_up_idx, du_ue_id, crnti, amf_ue_id, cu_up_e1ap_id, psi, drb_id_t::drb1, qfi)) {
      return false;
    }
    ue_ctx = this->find_ue_context(du_idx, du_ue_id);
    return ue_ctx != nullptr;
  }

  // Runs the UE attach up through ICS with location reporting configured.
  [[nodiscard]] bool attach_ue_with_ics_location_reporting(const location_report_request& loc_req,
                                                           std::optional<ngap_message>&   location_report)
  {
    if (!connect_new_ue(du_idx, du_ue_id, crnti)) {
      return false;
    }
    if (!authenticate_ue(du_idx, du_ue_id, amf_ue_id)) {
      return false;
    }
    ue_ctx = this->find_ue_context(du_idx, du_ue_id);
    if (ue_ctx == nullptr) {
      return false;
    }
    if (!setup_ue_security_and_ue_capabilities(du_idx, du_ue_id, std::nullopt, true, loc_req)) {
      return false;
    }

    location_report = get_location_report_if_required(loc_req);

    return true;
  }

  unsigned du_idx    = 0;
  unsigned cu_up_idx = 0;

  gnb_du_ue_f1ap_id_t    du_ue_id      = gnb_du_ue_f1ap_id_t::min;
  rnti_t                 crnti         = to_rnti(0x4601);
  amf_ue_id_t            amf_ue_id     = amf_ue_id_t::min;
  gnb_cu_up_ue_e1ap_id_t cu_up_e1ap_id = gnb_cu_up_ue_e1ap_id_t::min;

  pdu_session_id_t psi = uint_to_pdu_session_id(1);
  qos_flow_id_t    qfi = uint_to_qos_flow_id(1);

  const ue_context* ue_ctx = nullptr;

  ngap_message ngap_pdu;
};

TEST_F(cu_cp_location_reporting_test,
       when_location_reporting_control_with_direct_type_is_received_then_location_report_is_sent_to_amf)
{
  // Attach UE.
  ASSERT_TRUE(attach_ue());

  // Drain any pending NGAP messages.
  while (get_amf().try_pop_rx_pdu(ngap_pdu)) {
  }

  // Inject Location Reporting Control message from AMF.
  get_amf().push_tx_pdu(
      generate_location_reporting_control_message(ue_ctx->amf_ue_id.value(), ue_ctx->ran_ue_id.value()));

  // Wait for the Location Report to be sent to the AMF.
  ASSERT_TRUE(this->wait_for_ngap_tx_pdu(ngap_pdu));

  // Verify it's a Location Report.
  ASSERT_EQ(ngap_pdu.pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(ngap_pdu.pdu.init_msg().value.type(),
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::location_report);

  const auto& location_report = ngap_pdu.pdu.init_msg().value.location_report();

  // Verify UE IDs.
  ASSERT_EQ(location_report->amf_ue_ngap_id, to_underlying(ue_ctx->amf_ue_id.value()));
  ASSERT_EQ(location_report->ran_ue_ngap_id, to_underlying(ue_ctx->ran_ue_id.value()));

  // Verify location report request type.
  ASSERT_EQ(location_report->location_report_request_type.event_type, asn1::ngap::event_type_opts::options::direct);
  ASSERT_EQ(location_report->location_report_request_type.report_area.value,
            asn1::ngap::report_area_opts::options::cell);

  // Verify user location info is NR.
  ASSERT_EQ(location_report->user_location_info.type(),
            asn1::ngap::user_location_info_c::types_opts::options::user_location_info_nr);

  const auto& user_loc_info = location_report->user_location_info.user_location_info_nr();

  // Verify NR-CGI matches the serving cell configured in the DU (default served_cell_item_info).
  ASSERT_EQ(user_loc_info.nr_cgi.nr_cell_id.to_number(),
            nr_cell_identity::create(gnb_id_t{411, 22}, 0).value().value());
  ASSERT_EQ(user_loc_info.nr_cgi.plmn_id.to_number(), plmn_identity::test_value().to_bcd());

  // Verify TAI matches the serving cell TAC (default tac=7) and PLMN.
  ASSERT_EQ(user_loc_info.tai.plmn_id.to_number(), plmn_identity::test_value().to_bcd());
  ASSERT_EQ(user_loc_info.tai.tac.to_number(), 7);
}

TEST_F(cu_cp_location_reporting_test,
       when_location_reporting_control_with_nulltype_event_type_is_received_then_failure_indication_is_sent)
{
  ASSERT_TRUE(attach_ue());

  // Drain any pending NGAP messages.
  while (get_amf().try_pop_rx_pdu(ngap_pdu)) {
  }

  // Inject malformed Location Reporting Control message with event type = "nulltype".
  auto msg = generate_location_reporting_control_message(ue_ctx->amf_ue_id.value(), ue_ctx->ran_ue_id.value());
  msg.pdu.init_msg().value.location_report_ctrl()->location_report_request_type.event_type =
      asn1::ngap::event_type_opts::options::nulltype;
  get_amf().push_tx_pdu(msg);

  // Expect a Location Reporting Failure Indication to be sent to the AMF.
  ASSERT_TRUE(this->wait_for_ngap_tx_pdu(ngap_pdu));
  ASSERT_TRUE(test_helpers::is_valid_location_reporting_failure_indication(ngap_pdu));

  const auto& fail_ind = ngap_pdu.pdu.init_msg().value.location_report_fail_ind();
  ASSERT_EQ(fail_ind->amf_ue_ngap_id, to_underlying(ue_ctx->amf_ue_id.value()));
  ASSERT_EQ(fail_ind->ran_ue_ngap_id, to_underlying(ue_ctx->ran_ue_id.value()));
  ASSERT_EQ(fail_ind->cause.type(), asn1::ngap::cause_c::types_opts::protocol);
  ASSERT_EQ(fail_ind->cause.protocol().value,
            asn1::ngap::cause_protocol_opts::abstract_syntax_error_falsely_constructed_msg);
}

TEST_F(cu_cp_location_reporting_test,
       when_location_reporting_control_with_duplicate_ref_ids_is_received_then_failure_indication_is_sent)
{
  ASSERT_TRUE(attach_ue());

  // Drain any pending NGAP messages.
  while (get_amf().try_pop_rx_pdu(ngap_pdu)) {
  }

  // Inject Location Reporting Control with duplicate ref_ids (22, 22) in the Area of Interest list.
  get_amf().push_tx_pdu(generate_location_reporting_control_message_with_ue_presence(
      ue_ctx->amf_ue_id.value(), ue_ctx->ran_ue_id.value(), {22, 22}));

  // Expect a Location Reporting Failure Indication to be sent to the AMF.
  ASSERT_TRUE(this->wait_for_ngap_tx_pdu(ngap_pdu));
  ASSERT_TRUE(test_helpers::is_valid_location_reporting_failure_indication(ngap_pdu));

  const auto& fail_ind = ngap_pdu.pdu.init_msg().value.location_report_fail_ind();
  ASSERT_EQ(fail_ind->amf_ue_ngap_id, to_underlying(ue_ctx->amf_ue_id.value()));
  ASSERT_EQ(fail_ind->ran_ue_ngap_id, to_underlying(ue_ctx->ran_ue_id.value()));
  ASSERT_EQ(fail_ind->cause.type(), asn1::ngap::cause_c::types_opts::radio_network);
  ASSERT_EQ(fail_ind->cause.radio_network().value,
            asn1::ngap::cause_radio_network_opts::multiple_location_report_ref_id_instances);
}

TEST_F(cu_cp_location_reporting_test,
       when_location_reporting_control_collides_with_configured_ref_id_then_failure_indication_is_sent)
{
  ASSERT_TRUE(attach_ue());

  // Drain any pending NGAP messages.
  while (get_amf().try_pop_rx_pdu(ngap_pdu)) {
  }

  // Configure ref_id=22 successfully (UE presence type sends no immediate report).
  get_amf().push_tx_pdu(generate_location_reporting_control_message_with_ue_presence(
      ue_ctx->amf_ue_id.value(), ue_ctx->ran_ue_id.value(), {22}));

  // Send another control message with different ref_id=21, which does not collide with the configured one.
  get_amf().push_tx_pdu(generate_location_reporting_control_message_with_ue_presence(
      ue_ctx->amf_ue_id.value(), ue_ctx->ran_ue_id.value(), {21}));

  // Verify that the two valid messages do not cause failure indication.
  ASSERT_FALSE(this->wait_for_ngap_tx_pdu(ngap_pdu, std::chrono::milliseconds{5}));

  // Send another control message with the same ref_id=22, which collides with the first one.
  get_amf().push_tx_pdu(generate_location_reporting_control_message_with_ue_presence(
      ue_ctx->amf_ue_id.value(), ue_ctx->ran_ue_id.value(), {22}));

  // Expect a Location Reporting Failure Indication for the colliding message.
  ASSERT_TRUE(this->wait_for_ngap_tx_pdu(ngap_pdu));
  ASSERT_TRUE(test_helpers::is_valid_location_reporting_failure_indication(ngap_pdu));

  const auto& fail_ind = ngap_pdu.pdu.init_msg().value.location_report_fail_ind();
  ASSERT_EQ(fail_ind->amf_ue_ngap_id, to_underlying(ue_ctx->amf_ue_id.value()));
  ASSERT_EQ(fail_ind->ran_ue_ngap_id, to_underlying(ue_ctx->ran_ue_id.value()));
  ASSERT_EQ(fail_ind->cause.type(), asn1::ngap::cause_c::types_opts::radio_network);
  ASSERT_EQ(fail_ind->cause.radio_network().value,
            asn1::ngap::cause_radio_network_opts::multiple_location_report_ref_id_instances);
}

TEST_F(cu_cp_location_reporting_test,
       when_location_reporting_is_configured_and_ue_reestablishes_then_location_report_is_sent)
{
  // Attach UE.
  ASSERT_TRUE(attach_ue());

  // Configure change_of_serving_cell_and_ue_presence_in_the_area_of_interest reporting, so that the report is not
  // suppressed when the UE reestablishes on the same cell (cell-change-only suppresses same-cell duplicate reports).
  get_amf().push_tx_pdu(generate_location_reporting_control_message_with_cell_change_and_ue_presence(
      ue_ctx->amf_ue_id.value(), ue_ctx->ran_ue_id.value(), {1}));
  ASSERT_TRUE(this->wait_for_ngap_tx_pdu(ngap_pdu));
  ASSERT_TRUE(test_helpers::is_valid_location_report(ngap_pdu));

  // Trigger RRC reestablishment (UE reconnects with new C-RNTI, same DU).
  ASSERT_TRUE(reestablish_ue(du_idx, cu_up_idx, int_to_gnb_du_ue_f1ap_id(1), to_rnti(0x4602), crnti, pci_t{0}));

  // Expect a Location Report to be sent to the AMF after the reestablishment.
  ASSERT_TRUE(this->wait_for_ngap_tx_pdu(ngap_pdu));
  ASSERT_TRUE(test_helpers::is_valid_location_report(ngap_pdu));

  const auto& location_report = ngap_pdu.pdu.init_msg().value.location_report();
  ASSERT_EQ(location_report->amf_ue_ngap_id, to_underlying(ue_ctx->amf_ue_id.value()));
  ASSERT_EQ(location_report->ran_ue_ngap_id, to_underlying(ue_ctx->ran_ue_id.value()));
  ASSERT_EQ(location_report->location_report_request_type.event_type,
            asn1::ngap::event_type_opts::options::change_of_serving_cell_and_ue_presence_in_the_area_of_interest);
}

TEST_F(cu_cp_location_reporting_test,
       when_only_cell_change_reporting_is_configured_and_ue_reestablishes_to_same_cell_then_no_report_is_sent)
{
  // Attach UE.
  ASSERT_TRUE(attach_ue());

  // Configure change_of_serve_cell reporting. An immediate report is sent upon configuration.
  get_amf().push_tx_pdu(generate_location_reporting_control_message_with_cell_change(ue_ctx->amf_ue_id.value(),
                                                                                     ue_ctx->ran_ue_id.value()));
  ASSERT_TRUE(this->wait_for_ngap_tx_pdu(ngap_pdu));
  ASSERT_TRUE(test_helpers::is_valid_location_report(ngap_pdu));

  // Trigger RRC reestablishment on the same cell — no cell change occurred.
  ASSERT_TRUE(reestablish_ue(du_idx, cu_up_idx, int_to_gnb_du_ue_f1ap_id(1), to_rnti(0x4602), crnti, pci_t{0}));

  // No Location Report should be sent since the serving cell did not change.
  ASSERT_FALSE(this->wait_for_ngap_tx_pdu(ngap_pdu, std::chrono::milliseconds{5}));
}

TEST_F(cu_cp_location_reporting_test, when_ics_with_direct_type_is_received_then_location_report_is_sent_to_amf)
{
  location_report_request loc_req;
  loc_req.location_reporting_type = location_report_request::event_type::direct;
  loc_req.location_report_area    = location_report_request::report_area::cell;

  std::optional<ngap_message> loc_report;
  ASSERT_TRUE(attach_ue_with_ics_location_reporting(loc_req, loc_report));

  ASSERT_TRUE(loc_report.has_value());
  ngap_pdu = *loc_report;

  // Verify the captured Location Report.
  ASSERT_EQ(ngap_pdu.pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(ngap_pdu.pdu.init_msg().value.type(),
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::location_report);

  const auto& location_report = ngap_pdu.pdu.init_msg().value.location_report();

  ASSERT_EQ(location_report->amf_ue_ngap_id, to_underlying(ue_ctx->amf_ue_id.value()));
  ASSERT_EQ(location_report->ran_ue_ngap_id, to_underlying(ue_ctx->ran_ue_id.value()));

  ASSERT_EQ(location_report->location_report_request_type.event_type, asn1::ngap::event_type_opts::direct);
  ASSERT_EQ(location_report->location_report_request_type.report_area.value, asn1::ngap::report_area_opts::cell);

  ASSERT_EQ(location_report->user_location_info.type(),
            asn1::ngap::user_location_info_c::types_opts::user_location_info_nr);

  const auto& user_loc_info = location_report->user_location_info.user_location_info_nr();
  ASSERT_EQ(user_loc_info.nr_cgi.nr_cell_id.to_number(),
            nr_cell_identity::create(gnb_id_t{411, 22}, 0).value().value());
  ASSERT_EQ(user_loc_info.nr_cgi.plmn_id.to_number(), plmn_identity::test_value().to_bcd());
  ASSERT_EQ(user_loc_info.tai.plmn_id.to_number(), plmn_identity::test_value().to_bcd());
  ASSERT_EQ(user_loc_info.tai.tac.to_number(), 7);
}

TEST_F(cu_cp_location_reporting_test,
       when_ics_with_change_of_serving_cell_type_is_received_then_location_report_is_sent_to_amf)
{
  location_report_request loc_req;
  loc_req.location_reporting_type = location_report_request::event_type::change_of_serve_cell;
  loc_req.location_report_area    = location_report_request::report_area::cell;

  std::optional<ngap_message> loc_report;
  ASSERT_TRUE(attach_ue_with_ics_location_reporting(loc_req, loc_report));

  ASSERT_TRUE(loc_report.has_value());
  ngap_pdu = *loc_report;

  ASSERT_EQ(ngap_pdu.pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(ngap_pdu.pdu.init_msg().value.type(),
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::location_report);

  const auto& location_report = ngap_pdu.pdu.init_msg().value.location_report();

  ASSERT_EQ(location_report->amf_ue_ngap_id, to_underlying(ue_ctx->amf_ue_id.value()));
  ASSERT_EQ(location_report->ran_ue_ngap_id, to_underlying(ue_ctx->ran_ue_id.value()));
  ASSERT_EQ(location_report->location_report_request_type.event_type,
            asn1::ngap::event_type_opts::change_of_serve_cell);
  ASSERT_EQ(location_report->user_location_info.type(),
            asn1::ngap::user_location_info_c::types_opts::user_location_info_nr);
}

namespace {

/// Builds a User Location Information for a cell, optionally reporting a Mapped Cell ID and a derived TAC for the UE.
cu_cp_user_location_info_nr make_uli(uint64_t                uu_nci,
                                     std::optional<uint64_t> mapped_nci  = std::nullopt,
                                     std::optional<tac_t>    derived_tac = std::nullopt)
{
  cu_cp_user_location_info_nr uli;
  uli.nr_cgi.plmn_id          = plmn_identity::test_value();
  uli.nr_cgi.nci              = nr_cell_identity::create(uu_nci).value();
  uli.tai                     = {plmn_identity::test_value(), 7};
  uli.ue_location_derived_tac = derived_tac;
  if (mapped_nci.has_value()) {
    uli.mapped_nci = nr_cell_identity::create(mapped_nci.value()).value();
  }
  return uli;
}

/// Reports the UE presence in \c aoi, the only Area of Interest configured.
ue_presence presence_in(const area_of_interest& aoi, const cu_cp_user_location_info_nr& uli)
{
  ue_location_manager_cfg cfg;
  cfg.report_ue_presence_in_aoi = true;
  cfg.area_of_interest_list.emplace(1, aoi);

  ue_location_manager mng;
  mng.set_config(cfg);

  const location_report report = mng.get_direct_location_report(cu_cp_ue_index_t::min, uli, location_report_request{});
  EXPECT_TRUE(report.ue_presence_in_area_of_interest_list.has_value());
  EXPECT_EQ(report.ue_presence_in_area_of_interest_list->size(), 1);
  return report.ue_presence_in_area_of_interest_list->front().ue_presence_in_aio;
}

/// Reports the UE presence in an Area of Interest holding the cell \c aoi_nci alone.
ue_presence presence_in_area_of(uint64_t aoi_nci, const cu_cp_user_location_info_nr& uli)
{
  area_of_interest aoi;
  aoi.cell_list.push_back({plmn_identity::test_value(), nr_cell_identity::create(aoi_nci).value()});
  return presence_in(aoi, uli);
}

/// Reports the UE presence in an Area of Interest holding the tracking area \c aoi_tac alone.
ue_presence presence_in_tracking_area_of(tac_t aoi_tac, const cu_cp_user_location_info_nr& uli)
{
  area_of_interest aoi;
  aoi.tai_list.push_back({plmn_identity::test_value(), aoi_tac});
  return presence_in(aoi, uli);
}

/// Reports the UE presence in an Area of Interest holding the RAN node \c aoi_gnb_id alone.
ue_presence presence_in_ran_node_of(gnb_id_t aoi_gnb_id, const cu_cp_user_location_info_nr& uli)
{
  area_of_interest aoi;
  aoi.ran_node_list.push_back({plmn_identity::test_value(), aoi_gnb_id});
  return presence_in(aoi, uli);
}

/// Reports the second of two consecutive locations, with change_of_serve_cell reporting alone active.
std::optional<location_report> report_after(const cu_cp_user_location_info_nr& first,
                                            const cu_cp_user_location_info_nr& second)
{
  ue_location_manager_cfg cfg;
  cfg.report_on_cell_change = true;

  ue_location_manager mng;
  mng.set_config(cfg);

  mng.get_location_report(cu_cp_ue_index_t::min, first);
  return mng.get_location_report(cu_cp_ue_index_t::min, second);
}

} // namespace

TEST(cu_cp_location_change_test, an_unchanged_location_is_not_reported_again)
{
  const cu_cp_user_location_info_nr uli = make_uli(0x66c000, std::nullopt, 9);

  EXPECT_FALSE(report_after(uli, uli).has_value());
}

TEST(cu_cp_location_change_test, a_changed_derived_tac_is_reported_without_a_change_of_cell)
{
  // The UE moved into another area of the same cell, so the TAC reported to the AMF changed while the serving cell
  // did not.
  const std::optional<location_report> report =
      report_after(make_uli(0x66c000, std::nullopt, 9), make_uli(0x66c000, std::nullopt, 8));
  ASSERT_TRUE(report.has_value());
  ASSERT_TRUE(report->user_location_info.ue_location_derived_tac.has_value());
  EXPECT_EQ(report->user_location_info.ue_location_derived_tac.value(), 8);
}

TEST(cu_cp_area_of_interest_test, ue_is_inside_an_area_naming_the_mapped_cell_id_it_reports)
{
  // TS 38.300 sec. 16.14.5: the AMF names the cells of an Area of Interest by the Mapped Cell ID the gNB reports, so
  // matching the Uu Cell ID of the serving cell would place the UE outside every area.
  EXPECT_EQ(presence_in_area_of(0x66c0ff, make_uli(0x66c000, 0x66c0ff)), ue_presence::in);
}

TEST(cu_cp_area_of_interest_test, ue_is_outside_an_area_naming_the_uu_cell_id_it_no_longer_reports)
{
  // Once a Mapped Cell ID applies, it is the identity the core knows the UE by.
  EXPECT_EQ(presence_in_area_of(0x66c000, make_uli(0x66c000, 0x66c0ff)), ue_presence::out);
}

TEST(cu_cp_area_of_interest_test, ue_is_inside_an_area_naming_the_uu_cell_id_without_a_mapped_cell_id)
{
  // A terrestrial cell, or an NTN cell whose UE location is unknown, keeps reporting its Uu Cell ID.
  EXPECT_EQ(presence_in_area_of(0x66c000, make_uli(0x66c000)), ue_presence::in);
}

TEST(cu_cp_area_of_interest_test, ue_presence_is_unknown_while_the_position_naming_the_area_is_missing)
{
  // The cell names its areas by Mapped Cell ID, TS 38.300 sec. 16.14.5, but the UE position that picks one never
  // arrived. Whether the UE is in the area is not something this gNB can answer, and TS 38.413 sec. 9.3.1.67 keeps a
  // third value for exactly that.
  cu_cp_user_location_info_nr uli = make_uli(0x66c000);
  uli.mapped_nci_unknown          = true;

  EXPECT_EQ(presence_in_area_of(0x66c0ff, uli), ue_presence::unknown);
}

TEST(cu_cp_area_of_interest_test, a_missing_position_does_not_hide_an_area_naming_the_uu_cell_id)
{
  // The serving cell is named outright, so no Mapped Cell ID is needed to place the UE.
  cu_cp_user_location_info_nr uli = make_uli(0x66c000);
  uli.mapped_nci_unknown          = true;

  EXPECT_EQ(presence_in_area_of(0x66c000, uli), ue_presence::in);
}

TEST(cu_cp_area_of_interest_test, a_missing_position_does_not_hide_a_tracking_area_the_cell_broadcasts)
{
  // Only the cells of an Area of Interest are named by Mapped Cell ID, TS 38.300 sec. 16.14.5. A tracking area is
  // answered from what the cell broadcasts, so it is decided without a position and TS 23.502 Annex D.2 leaves no
  // room for reporting it unknown.
  cu_cp_user_location_info_nr uli = make_uli(0x66c000);
  uli.mapped_nci_unknown          = true;

  EXPECT_EQ(presence_in_tracking_area_of(7, uli), ue_presence::in);
  EXPECT_EQ(presence_in_tracking_area_of(99, uli), ue_presence::out);
}

TEST(cu_cp_area_of_interest_test, a_missing_position_does_not_hide_a_ran_node_area)
{
  // The RAN node of an Area of Interest is matched against the gNB ID of the serving cell, which no position is
  // needed to read.
  cu_cp_user_location_info_nr uli = make_uli(0x66c000);
  uli.mapped_nci_unknown          = true;

  EXPECT_EQ(presence_in_ran_node_of(gnb_id_t{411, 22}, uli), ue_presence::in);
  EXPECT_EQ(presence_in_ran_node_of(gnb_id_t{412, 22}, uli), ue_presence::out);
}

TEST(cu_cp_area_of_interest_test, ue_is_outside_an_area_naming_an_unrelated_cell)
{
  EXPECT_EQ(presence_in_area_of(0x66c0fe, make_uli(0x66c000, 0x66c0ff)), ue_presence::out);
}

TEST(cu_cp_location_change_test, a_changed_mapped_cell_id_is_reported_without_a_change_of_cell)
{
  // TS 38.300 sec. 16.14.5 NOTE 2 lets areas of different Mapped Cell IDs differ from the tracking areas, so a UE can
  // cross into an area naming another Mapped Cell ID while the serving cell and the derived TAC stay the same. The
  // identity reported to the core changed, so the report must be sent.
  const std::optional<location_report> report =
      report_after(make_uli(0x66c000, 0x66c0ff), make_uli(0x66c000, 0x66c0fe));
  ASSERT_TRUE(report.has_value());
  ASSERT_TRUE(report->user_location_info.mapped_nci.has_value());
  EXPECT_EQ(report->user_location_info.mapped_nci->value(), 0x66c0fe);
}

TEST(cu_cp_location_change_test, a_mapped_cell_id_that_appears_is_reported_without_a_change_of_cell)
{
  // The UE reported no position at first, so the core knew the cell by its Uu Cell ID alone.
  const std::optional<location_report> report = report_after(make_uli(0x66c000), make_uli(0x66c000, 0x66c0ff));
  ASSERT_TRUE(report.has_value());
  ASSERT_TRUE(report->user_location_info.mapped_nci.has_value());
  EXPECT_EQ(report->user_location_info.mapped_nci->value(), 0x66c0ff);
}
