// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_du_ref_time_info_report_procedure.h"
#include "ocudu/asn1/f1ap/common.h"
#include "ocudu/asn1/f1ap/f1ap.h"
#include "ocudu/f1ap/f1ap_message.h"

using namespace ocudu;
using namespace ocudu::odu;
using namespace asn1::f1ap;

f1ap_du_ref_time_info_report_procedure::f1ap_du_ref_time_info_report_procedure(f1ap_du_time_provider& time_provider_,
                                                                               f1ap_message_notifier& cu_notif_,
                                                                               timer_factory          timers_,
                                                                               subcarrier_spacing     scs_) :
  time_provider(time_provider_),
  cu_notif(cu_notif_),
  scs(scs_),
  periodic_timer(timers_.create_timer()),
  logger(ocudulog::fetch_basic_logger("DU-F1"))
{
}

f1ap_du_ref_time_info_report_procedure::~f1ap_du_ref_time_info_report_procedure()
{
  stop_periodic();
}

void f1ap_du_ref_time_info_report_procedure::handle_control(const ref_time_info_report_ctrl_s& msg)
{
  const uint16_t transaction_id = msg->transaction_id;
  const auto&    rrt            = msg->report_request_type;

  switch (rrt.event_type.value) {
    case event_type_opts::on_demand:
      logger.debug("Reference time info: on-demand report requested");
      stop_periodic();
      send_report(transaction_id);
      break;
    case event_type_opts::periodic:
      if (not rrt.report_periodicity_value_present) {
        logger.warning("Reference time info: periodic request missing periodicity value, ignoring");
        return;
      }
      if (rrt.report_periodicity_value == 0) {
        logger.warning("Reference time info: periodic request with periodicity value of 0, ignoring");
        return;
      }
      logger.debug("Reference time info: starting periodic reporting every {} radio frame(s)",
                   rrt.report_periodicity_value);
      start_periodic(transaction_id, rrt.report_periodicity_value);
      break;
    case event_type_opts::stop:
      logger.debug("Reference time info: stopping periodic reporting");
      stop_periodic();
      break;
    default:
      logger.warning("Reference time info: unknown event type {}", rrt.event_type.to_string());
      break;
  }
}

void f1ap_du_ref_time_info_report_procedure::send_report(uint16_t transaction_id)
{
  auto mapping = time_provider.get_last_mapping(scs);
  if (not mapping.has_value()) {
    logger.warning("Reference time info: no slot-to-time mapping available, dropping report");
    return;
  }

  f1ap_message msg{};
  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_REF_TIME_INFO_REPORT);
  auto& report                                 = msg.pdu.init_msg().value.ref_time_info_report();
  report->transaction_id                       = transaction_id;
  report->time_ref_info.ref_time               = std::move(mapping->ref_time_r16);
  report->time_ref_info.ref_sfn                = mapping->ref_slot.sfn();
  report->time_ref_info.time_info_type_present = mapping->is_local_clock;

  cu_notif.on_new_message(msg);
}

void f1ap_du_ref_time_info_report_procedure::start_periodic(uint16_t transaction_id, uint16_t period_rf)
{
  periodic_transaction_id = transaction_id;
  // 1 radio frame = 10 ms.
  const std::chrono::milliseconds period{static_cast<int64_t>(period_rf) * 10};
  periodic_timer.set(period, [this](timer_id_t /*tid*/) {
    send_report(periodic_transaction_id);
    periodic_timer.run();
  });
  periodic_timer.run();
}

void f1ap_du_ref_time_info_report_procedure::stop_periodic()
{
  periodic_timer.stop();
}
