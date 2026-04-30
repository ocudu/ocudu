// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1c_du_bearer_impl.h"
#include "du/procedures/f1ap_du_event_manager.h"
#include "ocudu/asn1/f1ap/common.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/ran/pdcp/pdcp_sn_util.h"
#include "ocudu/support/async/async_no_op_task.h"
#include "ocudu/support/async/async_timer.h"
#include "ocudu/support/async/execute_on_blocking.h"
#include "ocudu/support/executors/execute_until_success.h"

using namespace ocudu;
using namespace ocudu::odu;

f1c_srb0_du_bearer::f1c_srb0_du_bearer(f1ap_ue_context&           ue_ctxt_,
                                       const nr_cell_global_id_t& pcell_cgi,
                                       const byte_buffer&         du_cu_rrc_container_,
                                       f1ap_message_notifier&     f1ap_notifier_,
                                       f1c_rx_sdu_notifier&       f1c_rx_sdu_notifier_,
                                       f1ap_event_manager&        ev_manager_,
                                       f1ap_du_configurator&      du_configurator_,
                                       task_executor&             ctrl_exec_,
                                       task_executor&             ue_exec_,
                                       timer_manager&             timers_) :
  ue_ctxt(ue_ctxt_),
  nr_cgi(pcell_cgi),
  du_cu_rrc_container(du_cu_rrc_container_.copy()),
  f1ap_notifier(f1ap_notifier_),
  sdu_notifier(f1c_rx_sdu_notifier_),
  ev_manager(ev_manager_),
  du_configurator(du_configurator_),
  ctrl_exec(ctrl_exec_),
  ue_exec(ue_exec_),
  timers(timers_),
  logger(ocudulog::fetch_basic_logger("DU-F1"))
{
}

void f1c_srb0_du_bearer::handle_sdu(byte_buffer_chain sdu)
{
  // Convert into byte_buffer.
  byte_buffer pdu{byte_buffer::fallback_allocation_tag{}};
  report_fatal_error_if_not(pdu.append(sdu.begin(), sdu.end()), "byte buffer with fallback allocator should not fail");

  // Ensure SRB tasks are handled within the control executor.
  execute_until_success(ctrl_exec, timers, [this, pdu = std::move(pdu)]() mutable {
    const protocol_transaction<f1ap_transaction_response> transaction = ev_manager.transactions.create_transaction();

    // Pack Initial UL RRC Message Transfer as per TS38.473, Section 8.4.1.
    f1ap_message msg;
    msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_INIT_UL_RRC_MSG_TRANSFER);
    asn1::f1ap::init_ul_rrc_msg_transfer_s& init_msg = msg.pdu.init_msg().value.init_ul_rrc_msg_transfer();
    init_msg->gnb_du_ue_f1ap_id                      = gnb_du_ue_f1ap_id_to_uint(ue_ctxt.gnb_du_ue_f1ap_id);
    init_msg->nr_cgi.plmn_id                         = nr_cgi.plmn_id.to_bytes();
    init_msg->nr_cgi.nr_cell_id.from_number(nr_cgi.nci.value());
    init_msg->c_rnti                         = to_value(ue_ctxt.rnti);
    init_msg->rrc_container                  = std::move(pdu);
    init_msg->du_to_cu_rrc_container_present = not du_cu_rrc_container.empty();
    if (init_msg->du_to_cu_rrc_container_present) {
      init_msg->du_to_cu_rrc_container = std::move(du_cu_rrc_container);
    }
    init_msg->sul_access_ind_present                   = false;
    init_msg->transaction_id                           = transaction.id();
    init_msg->ran_ue_id_present                        = false;
    init_msg->rrc_container_rrc_setup_complete_present = false;

    // Notify upper layers of the initial UL RRC Message Transfer.
    f1ap_notifier.on_new_message(msg);
  });
}

void f1c_srb0_du_bearer::handle_transmit_notification(uint32_t highest_pdcp_sn)
{
  report_fatal_error("Transmission notifications do not exist for SRB0");
}

void f1c_srb0_du_bearer::handle_delivery_notification(uint32_t highest_pdcp_sn)
{
  report_fatal_error("Delivery notifications do not exist for SRB0");
}

void f1c_srb0_du_bearer::handle_pdu(byte_buffer pdu, bool /* unused */)
{
  // Change to UE execution context before forwarding the SDU to lower layers.
  execute_until_success(
      ue_exec, timers, [this, sdu = std::move(pdu)]() mutable { sdu_notifier.on_new_sdu(std::move(sdu)); });
}

async_task<bool> f1c_srb0_du_bearer::handle_pdu_and_await_delivery(byte_buffer               sdu,
                                                                   bool                      report_rrc_delivery_status,
                                                                   std::chrono::milliseconds time_to_wait)
{
  // Forward task to lower layers.
  handle_pdu(std::move(sdu), report_rrc_delivery_status);

  // For SRB0, there is no delivery notification mechanism, so we just let the timeout trigger.
  return launch_async([this, time_to_wait](coro_context<async_task<bool>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_AWAIT(async_wait_for(du_configurator.get_timer_factory().create_timer(), time_to_wait));
    CORO_RETURN(false);
  });
}

async_task<bool> f1c_srb0_du_bearer::handle_pdu_and_await_transmission(byte_buffer pdu,
                                                                       bool /* unused */,
                                                                       std::chrono::milliseconds /*unused*/)
{
  // Forward task to lower layers.
  handle_pdu(std::move(pdu));

  // For SRB0, there is no transmission notification.
  return launch_no_op_task(true);
}

// Max number of concurrent PDCP SN deliveries/transmissions being awaited on.
static constexpr size_t MAX_CONCURRENT_PDU_EVENTS = 8;

f1c_other_srb_du_bearer::f1c_other_srb_du_bearer(f1ap_ue_context&       ue_ctxt_,
                                                 srb_id_t               srb_id_,
                                                 f1ap_message_notifier& f1ap_notifier_,
                                                 f1c_rx_sdu_notifier&   f1c_sdu_notifier_,
                                                 f1ap_du_configurator&  du_configurator_,
                                                 task_executor&         ctrl_exec_,
                                                 task_executor&         ue_exec_,
                                                 timer_manager&         timers_) :
  ue_ctxt(ue_ctxt_),
  srb_id(srb_id_),
  f1ap_notifier(f1ap_notifier_),
  sdu_notifier(f1c_sdu_notifier_),
  du_configurator(du_configurator_),
  ctrl_exec(ctrl_exec_),
  ue_exec(ue_exec_),
  timers(timers_),
  logger(ocudulog::fetch_basic_logger("DU-F1")),
  event_pool(MAX_CONCURRENT_PDU_EVENTS)
{
  // set up the always set event
  event_source_type source{du_configurator.get_timer_factory()};
  always_set_event.subscribe_to(source, std::chrono::milliseconds{0});
  source.set(true);
}

void f1c_other_srb_du_bearer::handle_sdu(byte_buffer_chain sdu)
{
  // Convert into byte_buffer.
  byte_buffer pdu{byte_buffer::fallback_allocation_tag{}};
  report_fatal_error_if_not(pdu.append(sdu.begin(), sdu.end()), "byte buffer with fallback allocator should not fail");

  // Ensure SRB tasks are handled within the control executor as they involve access to the UE context.
  execute_until_success(ctrl_exec, timers, [this, pdu = std::move(pdu)]() mutable {
    gnb_cu_ue_f1ap_id_t cu_ue_id = ue_ctxt.gnb_cu_ue_f1ap_id;
    if (cu_ue_id >= gnb_cu_ue_f1ap_id_t::max) {
      logger.warning("ue={} rnti={} du_ue={} SRB={}: Discarding F1AP RX SDU. Cause: GNB-CU-UE-F1AP-ID is invalid.",
                     ue_ctxt.ue_index,
                     ue_ctxt.rnti,
                     fmt::underlying(ue_ctxt.gnb_du_ue_f1ap_id),
                     fmt::underlying(srb_id));
      return;
    }

    f1ap_message msg;

    // Fill F1AP UL RRC Message Transfer.
    msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_UL_RRC_MSG_TRANSFER);
    asn1::f1ap::ul_rrc_msg_transfer_s& ul_msg = msg.pdu.init_msg().value.ul_rrc_msg_transfer();
    ul_msg->gnb_du_ue_f1ap_id                 = gnb_du_ue_f1ap_id_to_uint(ue_ctxt.gnb_du_ue_f1ap_id);
    ul_msg->gnb_cu_ue_f1ap_id                 = gnb_cu_ue_f1ap_id_to_uint(ue_ctxt.gnb_cu_ue_f1ap_id);
    ul_msg->srb_id                            = srb_id_to_uint(srb_id);
    ul_msg->rrc_container                     = std::move(pdu);
    ul_msg->sel_plmn_id_present               = false;
    ul_msg->new_gnb_du_ue_f1ap_id_present     = false;

    f1ap_notifier.on_new_message(msg);

    // If a UE RRC config is pending, we consider the reception of a UL RRC message as the confirmation that the
    // RRC config has been applied by the UE.
    if (ue_ctxt.rrc_state == f1ap_ue_context::ue_rrc_state::config_pending) {
      ue_ctxt.rrc_state = f1ap_ue_context::ue_rrc_state::config_applied;
      du_configurator.on_ue_config_applied(ue_ctxt.ue_index);
    }
  });
}

void f1c_other_srb_du_bearer::handle_pdu(byte_buffer pdu, bool rrc_delivery_status_request)
{
  if (pdu.length() < 3) {
    logger.warning(
        "DL {} Rx SRB{} PDU: Dropping PDU. Cause: Invalid length={}.", ue_ctxt, srb_id_to_uint(srb_id), pdu.length());
    return;
  }

  // Handle RRC delivery status request.
  if (rrc_delivery_status_request) {
    // Extract PDCP SN.
    std::optional<uint32_t> pdcp_sn = get_pdcp_sn(pdu, pdcp_sn_size::size12bits, true, logger);
    if (pdcp_sn.has_value()) {
      // Add status report request to list of requests.
      pending_rrc_delivery_status_reports.push_back(pdcp_sn.value());
    } else {
      logger.warning("Rx PDU {}: Ignoring SRB{} Rx PDU RRC Delivery Status Request. Cause: Failed to extract PDCP SN.",
                     ue_ctxt,
                     srb_id_to_uint(srb_id));
    }
  }

  // Change to UE execution context before forwarding the SDU to lower layers.
  execute_until_success(ue_exec, timers, [this, sdu = std::move(pdu)]() mutable {
    // Forward SDU to lower layers.
    sdu_notifier.on_new_sdu(std::move(sdu));
  });
}

async_task<bool> f1c_other_srb_du_bearer::handle_pdu_and_await_transmission(byte_buffer pdu,
                                                                            bool        report_rrc_delivery_status,
                                                                            std::chrono::milliseconds time_to_wait)
{
  return handle_pdu_and_await(std::move(pdu), true, report_rrc_delivery_status, time_to_wait);
}

async_task<bool> f1c_other_srb_du_bearer::handle_pdu_and_await_delivery(byte_buffer pdu,
                                                                        bool        report_rrc_delivery_status,
                                                                        std::chrono::milliseconds time_to_wait)
{
  return handle_pdu_and_await(std::move(pdu), false, report_rrc_delivery_status, time_to_wait);
}

void f1c_other_srb_du_bearer::handle_transmit_notification(uint32_t highest_pdcp_sn)
{
  defer_until_success(ctrl_exec, timers, [this, highest_pdcp_sn]() { handle_notification(highest_pdcp_sn, true); });
}

void f1c_other_srb_du_bearer::handle_delivery_notification(uint32_t highest_pdcp_sn)
{
  defer_until_success(ctrl_exec, timers, [this, highest_pdcp_sn]() { handle_notification(highest_pdcp_sn, false); });
}

async_task<bool> f1c_other_srb_du_bearer::handle_pdu_and_await(byte_buffer               pdu,
                                                               bool                      tx_or_delivery,
                                                               bool                      report_rrc_delivery_status,
                                                               std::chrono::milliseconds time_to_wait)
{
  // Extract PDCP SN.
  std::optional<uint32_t> pdcp_sn = get_pdcp_sn(pdu, pdcp_sn_size::size12bits, true, logger);

  // Check if PDCP SN extraction was successful.
  if (not pdcp_sn) {
    // If not, forward PDU to lower layers anyway. An RLF may be triggered.
    handle_pdu(std::move(pdu), false);

    // Return right-away, instead of waiting for notification.
    logger.warning("Rx PDU {}: Ignoring SRB{} Rx PDU {} notification. Cause: Failed to extract PDCP SN.",
                   ue_ctxt,
                   srb_id_to_uint(srb_id),
                   tx_or_delivery ? "Tx" : "delivery");
    return launch_no_op_task(false);
  }

  // Store RRC delivery status request for later processing.
  if (report_rrc_delivery_status) {
    pending_rrc_delivery_status_reports.push_back(pdcp_sn.value());
  }

  // Forward SDU to lower layers via UE executor.
  execute_until_success(
      ue_exec, timers, [this, sdu = std::move(pdu)]() mutable { sdu_notifier.on_new_sdu(std::move(sdu)); });

  return wait_for_notification(pdcp_sn.value(), tx_or_delivery, time_to_wait);
}

void f1c_other_srb_du_bearer::handle_notification(uint32_t highest_pdcp_sn, bool tx_or_delivery)
{
  std::optional<uint32_t>& last_sn        = tx_or_delivery ? last_pdcp_sn_transmitted : last_pdcp_sn_delivered;
  auto&                    pending_events = tx_or_delivery ? pending_transmissions : pending_deliveries;

  // Register the highest PDCP SN.
  last_sn = highest_pdcp_sn;

  // When the gNB-DU has successfully delivered an RRC message to the UE for which the gNB-CU has requested a
  // delivery report, the gNB-DU shall send the RRC DELIVERY REPORT message to the gNB-CU containing the RRC
  // Delivery Status IE and the SRB ID IE.
  if (not tx_or_delivery) {
    while (not pending_rrc_delivery_status_reports.empty() and
           highest_pdcp_sn >= pending_rrc_delivery_status_reports.front()) {
      uint32_t trigger_sn = pending_rrc_delivery_status_reports.front();
      pending_rrc_delivery_status_reports.pop_front();
      // TODO: Save in-order PDCP SN delivery.
      handle_rrc_delivery_report(trigger_sn, highest_pdcp_sn);
    }
  }

  // Trigger awaiters of delivery notifications.
  pending_events.erase(std::remove_if(pending_events.begin(),
                                      pending_events.end(),
                                      [highest_pdcp_sn](const auto& event) {
                                        if (event->pdcp_sn <= highest_pdcp_sn) {
                                          // Trigger awaiter and remove it from the list of pending events.
                                          event->source.set(true);
                                          return true;
                                        }
                                        return false;
                                      }),
                       pending_events.end());
}

async_task<bool> f1c_other_srb_du_bearer::wait_for_notification(uint32_t                  pdcp_sn,
                                                                bool                      tx_or_delivery,
                                                                std::chrono::milliseconds time_to_wait)
{
  std::optional<uint32_t>& last_sn        = tx_or_delivery ? last_pdcp_sn_transmitted : last_pdcp_sn_delivered;
  auto&                    pending_events = tx_or_delivery ? pending_transmissions : pending_deliveries;

  // Check if the PDU has been already delivered.
  if (last_sn.has_value() and last_sn.value() >= pdcp_sn) {
    // SN already notified.
    return launch_no_op_task(true);
  }

  // Allocate a slot in the event pool.
  auto event_slot = event_pool.create(pdcp_sn, du_configurator.get_timer_factory(), time_to_wait);
  if (event_slot == nullptr) {
    logger.warning("Rx PDU {}: Ignoring {} Rx PDU {} notification. Cause: Failed to allocate notification handler.",
                   ue_ctxt,
                   srb_id,
                   tx_or_delivery ? "Tx" : "delivery");
    return launch_no_op_task(true);
  }

  // Allocation successful. Return awaitable event.
  auto& ev = pending_events.emplace_back(std::move(event_slot));
  return launch_async([&ev](coro_context<async_task<bool>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_AWAIT_VALUE(auto res, ev->observer);
    // True if there was no cancellation or timeout.
    CORO_RETURN(res.index() == 0);
  });
}

void f1c_other_srb_du_bearer::handle_rrc_delivery_report(uint32_t trigger_pdcp_sn,
                                                         uint32_t highest_in_order_pdcp_sn) const
{
  f1ap_message msg;
  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_RRC_DELIVERY_REPORT);
  asn1::f1ap::rrc_delivery_report_ies_container& report = *msg.pdu.init_msg().value.rrc_delivery_report();

  report.gnb_cu_ue_f1ap_id                   = gnb_cu_ue_f1ap_id_to_uint(ue_ctxt.gnb_cu_ue_f1ap_id);
  report.gnb_du_ue_f1ap_id                   = gnb_du_ue_f1ap_id_to_uint(ue_ctxt.gnb_du_ue_f1ap_id);
  report.srb_id                              = srb_id_to_uint(srb_id);
  report.rrc_delivery_status.trigger_msg     = trigger_pdcp_sn;
  report.rrc_delivery_status.delivery_status = highest_in_order_pdcp_sn;

  // Forward message to F1-C interface.
  f1ap_notifier.on_new_message(msg);
}
