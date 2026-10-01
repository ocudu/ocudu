// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ue_context/f1ap_du_ue.h"
#include "../ue_context/f1ap_du_ue_manager.h"
#include "ocudu/asn1/f1ap/f1ap.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents_ue.h"

namespace ocudu {
namespace odu {

struct f1ap_du_context;

class f1ap_du_ue_context_release_procedure
{
public:
  f1ap_du_ue_context_release_procedure(const asn1::f1ap::ue_context_release_cmd_s& msg_,
                                       f1ap_du_ue_manager&                         ues,
                                       const f1ap_du_context&                      ctxt_,
                                       timer_factory                               timers_);

  void operator()(coro_context<async_task<void>>& ctx);

  /// Wait period for RRC container in UE CONTEXT RELEASE COMMAND to be delivered in the lower layers, before giving
  /// up and proceeding with the UE context release regardless.
  /// Note: This timeout should account for the delay for the UE to receive the RRC container, which is
  /// non-deterministic, and the timeout of 60msec specified in TS 38.331, 5.3.8.3 for the UE to ACK the RRC
  /// container.
  static constexpr std::chrono::milliseconds rrc_container_delivery_timeout{120};

  /// Time between successful RRC container delivery and actual UE deletion, to avoid removing the UE RAN
  /// resources while the UE may still be using them (which could cause collisions in PUCCH).
  static constexpr std::chrono::milliseconds ue_full_release_timeout{60};

private:
  const char* name() const { return "UE Context Release"; }

  void send_ue_context_release_complete();

  async_task<bool> handle_rrc_container();

  const asn1::f1ap::ue_context_release_cmd_s msg;
  f1ap_du_ue&                                ue;
  ocudulog::basic_logger&                    logger = ocudulog::fetch_basic_logger("DU-F1");
  f1ap_message_notifier&                     cu_msg_notifier; // used after the UE context as been released.
  const f1ap_du_context&                     du_ctxt;
  timer_factory                              timers;

  std::chrono::milliseconds rem_timeout{0};
  bool                      success = false;
};

} // namespace odu
} // namespace ocudu
