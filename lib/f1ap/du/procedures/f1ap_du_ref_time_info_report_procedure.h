// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/f1ap/du/f1ap_du_time_provider.h"
#include "ocudu/f1ap/f1ap_message_notifier.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include "ocudu/support/timers.h"

namespace ocudu {
namespace odu {

/// \brief Handles Reference Time Information Reporting as per TS 38.473 section 8.12.
///
/// Receives REFERENCE TIME INFORMATION REPORTING CONTROL from the CU (on-demand, periodic, stop)
/// and sends REFERENCE TIME INFORMATION REPORT messages to the CU using the MAC subframe time mapper
/// as the time source.
class f1ap_du_ref_time_info_report_procedure
{
public:
  f1ap_du_ref_time_info_report_procedure(f1ap_du_time_provider& time_provider_,
                                         f1ap_message_notifier& cu_notif_,
                                         timer_factory          timers_,
                                         subcarrier_spacing     scs_);
  ~f1ap_du_ref_time_info_report_procedure();

  /// \brief Handle incoming REFERENCE TIME INFORMATION REPORTING CONTROL from the CU.
  void handle_control(const asn1::f1ap::ref_time_info_report_ctrl_s& msg);

private:
  void send_report(uint16_t transaction_id);
  void start_periodic(uint16_t transaction_id, uint16_t period_rf);
  void stop_periodic();

  f1ap_du_time_provider&  time_provider;
  f1ap_message_notifier&  cu_notif;
  subcarrier_spacing      scs;
  unique_timer            periodic_timer;
  uint16_t                periodic_transaction_id = 0;
  ocudulog::basic_logger& logger;
};

} // namespace odu
} // namespace ocudu
