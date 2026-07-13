// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/f1ap/du/f1ap_du_factory.h"
#include "f1ap_du_impl.h"

using namespace ocudu;
using namespace odu;

std::unique_ptr<f1ap_du> odu::create_f1ap(f1c_connection_client&   f1c_client_handler,
                                          f1ap_du_configurator&    du_mng,
                                          task_executor&           ctrl_exec,
                                          f1ap_ue_executor_mapper& ue_exec_mapper,
                                          f1ap_du_paging_notifier& paging_notifier,
                                          f1ap_du_pws_notifier&    pws_notifier,
                                          timer_manager&           timers)
{
  auto f1ap_du = std::make_unique<f1ap_du_impl>(
      f1c_client_handler, du_mng, ctrl_exec, ue_exec_mapper, paging_notifier, pws_notifier, timers);
  return f1ap_du;
}
