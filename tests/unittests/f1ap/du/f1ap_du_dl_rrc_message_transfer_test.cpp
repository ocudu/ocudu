// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_du_test_helpers.h"
#include "test_doubles/f1ap/f1ap_test_messages.h"
#include "ocudu/adt/format.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocudu::odu;

static f1ap_message generate_dl_rrc_message_transfer(srb_id_t srb_id, const byte_buffer& container)
{
  return test_helpers::generate_dl_rrc_message_transfer(
      int_to_gnb_du_ue_f1ap_id(0), int_to_gnb_cu_ue_f1ap_id(0), srb_id, container.copy());
}

class f1ap_du_dl_rrc_message_transfer_test : public f1ap_du_test
{
protected:
  f1ap_du_dl_rrc_message_transfer_test()
  {
    // Run Test Preamble.
    run_f1_setup_procedure();
    test_ue = run_f1ap_ue_create(to_du_ue_index(0));

    f1c_gw.clear_tx_pdus();
  }

  ue_test_context* test_ue = nullptr;
};

TEST_F(f1ap_du_dl_rrc_message_transfer_test,
       when_dl_rrc_message_transfer_is_received_lower_layer_srb_notifier_receives_rrc_pdu)
{
  // Forward DL RRC Message Transfer
  byte_buffer  test_rrc_msg = test_helpers::create_dl_dcch_rrc_container(0, {0x1, 0x2, 0x3});
  f1ap_message msg          = generate_dl_rrc_message_transfer(srb_id_t::srb1, test_rrc_msg);
  ASSERT_TRUE(test_ue->f1c_bearers[1].rx_sdu_notifier.last_pdu.empty());
  f1ap->handle_message(msg);

  // The RRC container should be forwarded to lower layers.
  ASSERT_FALSE(test_ue->f1c_bearers[1].rx_sdu_notifier.last_pdu.empty());
  ASSERT_EQ(test_ue->f1c_bearers[1].rx_sdu_notifier.last_pdu, test_rrc_msg);
}

TEST_F(f1ap_du_dl_rrc_message_transfer_test, when_dl_rrc_message_transfer_is_received_gnb_cu_ue_f1ap_id_is_updated)
{
  // Forward DL RRC Message Transfer
  byte_buffer  test_rrc_msg = test_helpers::create_dl_dcch_rrc_container(0, {0x1, 0x2, 0x3});
  f1ap_message msg          = generate_dl_rrc_message_transfer(srb_id_t::srb1, test_rrc_msg);
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_cu_ue_f1ap_id = 5;
  // The DL RRC Message Transfer should update the gNB-CU UE F1AP ID.
  f1ap->handle_message(msg);

  // Forward SRB1 Rx SDU, and observe that the new gNB-CU UE F1AP ID is used.
  test_ue->f1c_bearers[1].bearer->handle_sdu(byte_buffer_chain::create(test_rrc_msg.copy()).value());
  ASSERT_EQ(f1c_gw.last_tx_pdu().pdu.init_msg().value.ul_rrc_msg_transfer()->gnb_cu_ue_f1ap_id,
            msg.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_cu_ue_f1ap_id)
      << "Invalid gNB-CU UE F1AP ID";
}

TEST_F(f1ap_du_dl_rrc_message_transfer_test, when_awaited_pdu_is_delivered_then_the_event_resources_are_released)
{
  // The events used to await the delivery of a PDU are taken from a pool, and each of them holds a timer. Run several
  // await/delivery cycles and verify that the events do not accumulate resources as they are recycled.
  auto run_delivery_cycle = [this](uint32_t pdcp_sn) {
    byte_buffer      test_rrc_msg = test_helpers::create_dl_dcch_rrc_container(pdcp_sn, {0x1, 0x2, 0x3});
    async_task<bool> t            = test_ue->f1c_bearers[1].bearer->handle_pdu_and_await_delivery(
        std::move(test_rrc_msg), false, std::chrono::milliseconds{1000});
    lazy_task_launcher<bool> launcher{t};
    EXPECT_FALSE(t.ready()) << "The delivery of the PDU should not be reported yet";

    test_ue->f1c_bearers[1].bearer->handle_delivery_notification(pdcp_sn);
    this->tick();
    EXPECT_TRUE(t.ready()) << "The delivery of the PDU should have been reported";
  };

  run_delivery_cycle(1);
  const size_t nof_timers_after_first_cycle = timer_service.nof_timers();

  for (uint32_t pdcp_sn = 2; pdcp_sn != 12; ++pdcp_sn) {
    run_delivery_cycle(pdcp_sn);
  }

  ASSERT_EQ(timer_service.nof_timers(), nof_timers_after_first_cycle)
      << "The resources of the events are not released when the events are recycled";
}

TEST_F(
    f1ap_du_dl_rrc_message_transfer_test,
    when_dl_rrc_message_transfer_contains_rrc_delivery_status_request_then_rrc_delivery_status_is_reported_after_container_delivery)
{
  uint32_t pdcp_sn = 5;

  // Forward DL RRC Message Transfer
  byte_buffer         test_rrc_msg = test_helpers::create_dl_dcch_rrc_container(pdcp_sn, {0x1, 0x2, 0x3});
  f1ap_message        msg          = generate_dl_rrc_message_transfer(srb_id_t::srb1, test_rrc_msg);
  gnb_du_ue_f1ap_id_t du_ue_id =
      int_to_gnb_du_ue_f1ap_id(msg.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_du_ue_f1ap_id);
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->rrc_delivery_status_request_present = true;
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->rrc_delivery_status_request.value =
      asn1::f1ap::rrc_delivery_status_request_opts::true_value;
  f1ap->handle_message(msg);

  // When lower layers report delivery for PDUs with PDCP SN lower than the SN of the DLRRCMessageTransfer, nothing
  // happens.
  test_ue->f1c_bearers[1].bearer->handle_transmit_notification(0);
  test_ue->f1c_bearers[1].bearer->handle_delivery_notification(0);
  test_ue->f1c_bearers[1].bearer->handle_transmit_notification(pdcp_sn);
  this->tick();
  ASSERT_FALSE(f1c_gw.pop_tx_pdu().has_value());
  // PDCP SN matching the one of the DLRRCMessageTransfer was delivered. The F1AP-DU should send back to the CU-CP an
  // RRCDeliveryReport
  test_ue->f1c_bearers[1].bearer->handle_delivery_notification(pdcp_sn);
  this->tick();
  auto ret = f1c_gw.pop_tx_pdu();
  ASSERT_TRUE(ret.has_value());
  ASSERT_EQ(ret.value().pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::rrc_delivery_report);
  const asn1::f1ap::rrc_delivery_report_s& report = ret.value().pdu.init_msg().value.rrc_delivery_report();
  ASSERT_EQ(report->gnb_du_ue_f1ap_id, to_underlying(du_ue_id));
  ASSERT_EQ(report->srb_id, 1);
  ASSERT_EQ(report->rrc_delivery_status.trigger_msg, pdcp_sn);
  ASSERT_EQ(report->rrc_delivery_status.delivery_status, pdcp_sn);
}

TEST_F(f1ap_du_test,
       when_dl_rrc_message_transfer_contains_cu_ue_id_that_does_not_match_then_error_indication_is_sent_to_cu)
{
  // Run Test Preamble.
  run_f1_setup_procedure();
  // > create UE1
  ue_test_context* ue1 = run_f1ap_ue_create(to_du_ue_index(0));
  ASSERT_NE(ue1, nullptr);
  // > run DL rrc message for UE1 that will set the gnb-cu-ue-f1ap-id.
  byte_buffer  test_rrc_msg = byte_buffer::create({0x1, 0x2, 0x3}).value();
  f1ap_message msg          = generate_dl_rrc_message_transfer(srb_id_t::srb1, test_rrc_msg);
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_cu_ue_f1ap_id = 0;
  ASSERT_TRUE(ue1->f1c_bearers[1].rx_sdu_notifier.last_pdu.empty());
  f1ap->handle_message(msg);
  ASSERT_FALSE(ue1->f1c_bearers[1].rx_sdu_notifier.last_pdu.empty());
  // > send DL RRC Message with invalid gNB CU UE F1AP ID.
  ue1->f1c_bearers[1].rx_sdu_notifier.last_pdu.clear();
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_cu_ue_f1ap_id = 1;
  f1ap->handle_message(msg);

  // Test Section

  // > No RRC container is forwarded.
  ASSERT_TRUE(ue1->f1c_bearers[1].rx_sdu_notifier.last_pdu.empty());
  // > Error Indication is sent to CU.
  ASSERT_EQ(f1c_gw.last_tx_pdu().pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::error_ind);
  const auto& err_ind = f1c_gw.last_tx_pdu().pdu.init_msg().value.error_ind();
  ASSERT_TRUE(err_ind->gnb_du_ue_f1ap_id_present);
  ASSERT_EQ(err_ind->gnb_du_ue_f1ap_id, 0);
  ASSERT_TRUE(err_ind->cause_present);
  ASSERT_EQ(err_ind->cause.radio_network().value,
            asn1::f1ap::cause_radio_network_opts::unknown_or_inconsistent_pair_of_ue_f1ap_id);
}

TEST_F(f1ap_du_test,
       when_dl_rrc_message_transfer_with_old_gnb_du_ue_id_then_du_is_notified_and_rrc_container_is_forwarded)
{
  // Run Test Preamble.
  run_f1_setup_procedure();
  // > create UE1
  ue_test_context* ue1 = run_f1ap_ue_create(to_du_ue_index(0));
  ASSERT_NE(ue1, nullptr);
  // > run DL rrc message for UE1 that will set the gnb-cu-ue-f1ap-id.
  byte_buffer  test_rrc_msg = byte_buffer::create({0x1, 0x2, 0x3}).value();
  f1ap_message msg          = generate_dl_rrc_message_transfer(srb_id_t::srb0, test_rrc_msg);
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_cu_ue_f1ap_id = 0;
  f1ap->handle_message(msg);
  // > create UE2
  ue_test_context* ue2 = run_f1ap_ue_create(to_du_ue_index(1));
  ASSERT_NE(ue2, nullptr);
  // > Send DL RRC Message Transfer for UE2 (containing old gNB DU UE ID).
  msg = generate_dl_rrc_message_transfer(srb_id_t::srb1, test_rrc_msg);
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_du_ue_f1ap_id             = 1;
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_cu_ue_f1ap_id             = 1;
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->old_gnb_du_ue_f1ap_id_present = true;
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->old_gnb_du_ue_f1ap_id         = 0;
  ASSERT_FALSE(this->f1ap_du_cfg_handler.last_reestablishment_ue_indexes.has_value());
  f1ap->handle_message(msg);

  // Test Section.

  // > DU was notified of reestablishment.
  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_reestablishment_ue_indexes.has_value());
  ASSERT_EQ(this->f1ap_du_cfg_handler.last_reestablishment_ue_indexes.value().first, to_du_ue_index(1));
  ASSERT_EQ(this->f1ap_du_cfg_handler.last_reestablishment_ue_indexes.value().second, to_du_ue_index(0));

  // > RRC container is forwarded.
  ASSERT_FALSE(ue2->f1c_bearers[1].rx_sdu_notifier.last_pdu.empty());
  ASSERT_EQ(ue2->f1c_bearers[1].rx_sdu_notifier.last_pdu, test_rrc_msg);

  // > gNB-CU-UE-F1AP-Id is updated.
  ue2->f1c_bearers[1].bearer->handle_sdu(byte_buffer_chain::create(test_rrc_msg.copy()).value());
  ASSERT_EQ(f1c_gw.last_tx_pdu().pdu.init_msg().value.ul_rrc_msg_transfer()->gnb_cu_ue_f1ap_id,
            msg.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_cu_ue_f1ap_id)
      << "Invalid gNB-CU UE F1AP ID";
}

TEST_F(f1ap_du_test, when_dl_rrc_message_transfer_has_duplicate_cu_ue_id_and_old_gnb_du_ue_id_then_cu_ue_id_is_updated)
{
  // Run Test Preamble.
  run_f1_setup_procedure();
  // > create UE1
  ue_test_context* ue1 = run_f1ap_ue_create(to_du_ue_index(0));
  ASSERT_NE(ue1, nullptr);
  // > run DL rrc message for UE1 that will set the gnb-cu-ue-f1ap-id.
  byte_buffer  test_rrc_msg = byte_buffer::create({0x1, 0x2, 0x3}).value();
  f1ap_message msg          = generate_dl_rrc_message_transfer(srb_id_t::srb0, test_rrc_msg);
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_cu_ue_f1ap_id = 5;
  f1ap->handle_message(msg);
  // > create UE2
  ue_test_context* ue2 = run_f1ap_ue_create(to_du_ue_index(1));
  ASSERT_NE(ue2, nullptr);
  // > Send DL RRC Message Transfer for UE2 (containing old gNB DU UE ID).
  msg = generate_dl_rrc_message_transfer(srb_id_t::srb1, test_rrc_msg);
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_du_ue_f1ap_id             = 1;
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_cu_ue_f1ap_id             = 5;
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->old_gnb_du_ue_f1ap_id_present = true;
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->old_gnb_du_ue_f1ap_id         = 0;
  ASSERT_FALSE(this->f1ap_du_cfg_handler.last_reestablishment_ue_indexes.has_value());
  f1ap->handle_message(msg);

  // Test Section.

  // > DU was notified of reestablishment.
  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_reestablishment_ue_indexes.has_value());
  ASSERT_EQ(this->f1ap_du_cfg_handler.last_reestablishment_ue_indexes.value().first, to_du_ue_index(1));
  ASSERT_EQ(this->f1ap_du_cfg_handler.last_reestablishment_ue_indexes.value().second, to_du_ue_index(0));
  // > RRC container is forwarded.
  ASSERT_FALSE(ue2->f1c_bearers[1].rx_sdu_notifier.last_pdu.empty());
  ASSERT_EQ(ue2->f1c_bearers[1].rx_sdu_notifier.last_pdu, test_rrc_msg);
  // > gNB-CU-UE-F1AP-Id is updated.
  ue2->f1c_bearers[1].bearer->handle_sdu(byte_buffer_chain::create(test_rrc_msg.copy()).value());
  ASSERT_EQ(f1c_gw.last_tx_pdu().pdu.init_msg().value.ul_rrc_msg_transfer()->gnb_cu_ue_f1ap_id,
            msg.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_cu_ue_f1ap_id)
      << "Invalid gNB-CU UE F1AP ID";
}
