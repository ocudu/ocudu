// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "f1ap_ue_context.h"
#include "ocudu/adt/slotted_array.h"
#include "ocudu/f1ap/du/f1c_bearer.h"
#include "ocudu/f1ap/du/f1c_rx_sdu_notifier.h"
#include "ocudu/f1ap/f1ap_message_notifier.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/support/executors/task_executor.h"

namespace ocudu {

class timer_manager;

namespace odu {

class f1ap_event_manager;
class f1ap_du_configurator;

class ue_bearer_manager
{
public:
  ue_bearer_manager(f1ap_ue_context&       ue_ctx_,
                    f1ap_message_notifier& f1ap_notif_,
                    f1ap_du_configurator&  du_configurator_,
                    task_executor&         ctrl_exec_,
                    task_executor&         ue_exec_,
                    timer_manager&         timers_) :
    ue_ctx(ue_ctx_),
    f1ap_notifier(f1ap_notif_),
    du_configurator(du_configurator_),
    ctrl_exec(ctrl_exec_),
    ue_exec(ue_exec_),
    timers(timers_)
  {
  }

  void add_srb0_f1c_bearer(f1c_rx_sdu_notifier&       f1c_rx_sdu_notif,
                           const nr_cell_global_id_t& pcell_cgi,
                           const byte_buffer&         du_cu_rrc_container,
                           f1ap_event_manager&        ev_mng);

  void add_f1c_bearer(srb_id_t srb_id, f1c_rx_sdu_notifier& rx_sdu_notif);

  f1c_bearer* find_srb(srb_id_t srb_id)
  {
    return f1c_bearers.contains(srb_id_to_uint(srb_id)) ? f1c_bearers[srb_id_to_uint(srb_id)].get() : nullptr;
  }
  const f1c_bearer* find_srb(srb_id_t srb_id) const
  {
    return f1c_bearers.contains(srb_id_to_uint(srb_id)) ? f1c_bearers[srb_id_to_uint(srb_id)].get() : nullptr;
  }

private:
  f1ap_ue_context&       ue_ctx;
  f1ap_message_notifier& f1ap_notifier;
  f1ap_du_configurator&  du_configurator;
  task_executor&         ctrl_exec;
  task_executor&         ue_exec;
  timer_manager&         timers;

  slotted_array<std::unique_ptr<f1c_bearer>, MAX_NOF_SRBS> f1c_bearers;
};

} // namespace odu
} // namespace ocudu
