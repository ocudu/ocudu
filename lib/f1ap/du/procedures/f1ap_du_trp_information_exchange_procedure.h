// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/f1ap/f1ap.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/f1ap/du/f1ap_du_positioning_handler.h"
#include "ocudu/f1ap/f1ap_message_notifier.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {
namespace odu {

class f1ap_du_configurator;
class f1ap_du_ue;

class f1ap_du_trp_information_exchange_procedure
{
public:
  f1ap_du_trp_information_exchange_procedure(const asn1::f1ap::trp_info_request_s& msg_,
                                             f1ap_du_positioning_handler&          du_mng_,
                                             f1ap_message_notifier&                cu_notifier_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  bool validate_request() const;
  void send_response(const du_trp_info_response& du_resp) const;
  void send_failure() const;

  const asn1::f1ap::trp_info_request_s msg;
  f1ap_du_positioning_handler&         du_mng;
  f1ap_message_notifier&               cu_notifier;
  ocudulog::basic_logger&              logger;
};

} // namespace odu
} // namespace ocudu
