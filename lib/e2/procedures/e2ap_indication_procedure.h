// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../common/e2ap_asn1_utils.h"
#include "ocudu/asn1/e2ap/e2ap.h"
#include "ocudu/asn1/e2sm/e2sm_kpm_ies.h"
#include "ocudu/e2/e2.h"
#include "ocudu/e2/e2_event_manager.h"
#include "ocudu/e2/e2sm/e2sm.h"
#include "ocudu/e2/subscription/e2_subscription.h"

namespace ocudu {

/// E2 INDICATION procedure will be used to send the measurement data to the RIC in response to the E2 SUBSCRIPTION
/// REQUEST, it will execute periodically until the E2 SUBSCRIPTION DELETE REQUEST is received.
class e2ap_indication_procedure
{
public:
  e2ap_indication_procedure(e2_message_notifier&    notif_,
                            e2_event_manager&       ev_mng_,
                            e2_subscription_info_t& subscription_info_,
                            ocudulog::basic_logger& logger_);

  void operator()(coro_context<eager_async_task<void>>& ctx);

  static const char* name() { return "E2AP Indication Procedure"; }

private:
  /// Send E2 INDICATION message to the RIC.
  void send_e2_indication(e2_indication_message& e2_ind);

  e2_message_notifier&               notifier;
  e2_event_manager&                  ev_mng;
  e2_subscription_info_t&            subscription;
  ocudulog::basic_logger&            logger;
  std::vector<e2_indication_message> e2_ind_vec;
  asn1::e2sm::e2sm_kpm_ind_hdr_s     ric_ind_header;
  asn1::e2sm::e2sm_kpm_ind_msg_s     ric_ind_message;

  protocol_transaction_outcome_observer<asn1::e2ap::ric_sub_delete_request_s> transaction_sink;

  std::chrono::seconds time_to_wait{1};
  bool                 running = true;
};

} // namespace ocudu
