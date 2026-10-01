// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ue_context/f1ap_du_ue.h"
#include "ocudu/asn1/f1ap/f1ap.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/f1ap/du/f1ap_du_positioning_handler.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {
namespace odu {

class f1ap_du_configurator;
class f1ap_du_ue;

class f1ap_du_positioning_information_exchange_procedure
{
public:
  f1ap_du_positioning_information_exchange_procedure(const asn1::f1ap::positioning_info_request_s& msg_,
                                                     f1ap_du_positioning_handler&                  du_mng_,
                                                     f1ap_du_ue&                                   ue_);

  void operator()(coro_context<async_task<void>>& ctx);

  const char* name() const { return "Positioning Information Exchange Procedure"; }

private:
  async_task<du_positioning_info_response> request_du_for_positioning_info();

  void send_response() const;

  const asn1::f1ap::positioning_info_request_s msg;
  f1ap_du_positioning_handler&                 du_mng;
  f1ap_du_ue&                                  ue;
  ocudulog::basic_logger&                      logger;

  du_positioning_info_response result;
};
} // namespace odu
} // namespace ocudu
