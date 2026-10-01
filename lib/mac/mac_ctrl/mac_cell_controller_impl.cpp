// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "mac_cell_controller_impl.h"
#include "../mac_dl/mac_dl_configurator.h"

using namespace ocudu;

mac_cell_controller_impl::mac_cell_controller_impl(const mac_cell_creation_request& cell_cfg,
                                                   timer_factory                    timers,
                                                   task_executor&                   ctrl_exec,
                                                   mac_dl_cell_controller&          dl_cell_) :
  si_mng(cell_cfg.cell_index, cell_cfg.sys_info, timers, ctrl_exec, dl_cell_), dl_cell(dl_cell_)
{
}

async_task<void> mac_cell_controller_impl::start()
{
  return dl_cell.start();
}

async_task<void> mac_cell_controller_impl::stop()
{
  return dl_cell.stop();
}

async_task<mac_cell_reconfig_response> mac_cell_controller_impl::reconfigure(const mac_cell_reconfig_request& request)
{
  return launch_async(
      [this,
       request,
       resp      = mac_cell_reconfig_response{},
       si_result = si_message_controller::si_update_result{},
       dl_req    = mac_dl_cell_reconfig_request{}](coro_context<async_task<mac_cell_reconfig_response>>& ctx) mutable {
        CORO_BEGIN(ctx);

        // System Information updates, with SI change notifications and/or SIB1 valueTag update, and without.
        si_result             = si_mng.handle_si_change_request(request.new_sys_info, request.new_si_pdu_info);
        resp.si_updated       = si_result.si_updated;
        resp.si_pdus_enqueued = si_result.si_pdus_enqueued;

        dl_req.slice_reconf_req           = request.slice_reconf_req;
        dl_req.ntn_ul_ta_update           = request.ntn_ul_ta_update;
        dl_req.cell_barred_mod            = request.cell_barred_mod;
        dl_req.intra_freq_reselection_mod = request.intra_freq_reselection_mod;

        CORO_AWAIT(dl_cell.reconfigure(dl_req));

        CORO_RETURN(resp);
      });
}
