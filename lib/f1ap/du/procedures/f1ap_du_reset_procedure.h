// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/f1ap/f1ap.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/f1ap/du/f1ap_du.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {

class f1ap_message_notifier;

namespace odu {

class f1ap_du_ue_manager;

/// Implementation of the F1AP RESET procedure as per TS 38.473, section 8.2.1.
class reset_procedure
{
public:
  reset_procedure(const asn1::f1ap::reset_s& msg,
                  f1ap_du_configurator&      du_mng,
                  f1ap_du_ue_manager&        ue_mng,
                  f1ap_message_notifier&     msg_notifier);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  const char* name() const { return "Reset"; }

  async_task<void>           handle_reset();
  std::vector<du_ue_index_t> create_ues_to_reset() const;
  void                       send_ack() const;

  const asn1::f1ap::reset_s msg;
  f1ap_du_configurator&     du_mng;
  f1ap_du_ue_manager&       ue_mng;
  f1ap_message_notifier&    msg_notifier;
  ocudulog::basic_logger&   logger;

  std::vector<asn1::f1ap::ue_associated_lc_f1_conn_item_s> ues_reset;
};

} // namespace odu
} // namespace ocudu
