// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../cu_up_manager_impl.h"
#include "../ue_manager.h"
#include "ocudu/cu_up/cu_up_config.h"
#include "ocudu/gtpu/gtpu_demux.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {
namespace ocuup {

class cu_up_enable_test_mode_routine
{
public:
  cu_up_enable_test_mode_routine(cu_up_test_mode_config test_mode_cfg_,
                                 cu_up_manager_impl&    cu_up_mngr_,
                                 ue_manager&            ue_mngr_,
                                 gtpu_demux_ctrl&       ngu_demux_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "CU-UP enable test mode routine"; }

private:
  const cu_up_test_mode_config test_mode_cfg;
  cu_up_manager_impl&          cu_up_mngr;
  ue_manager&                  ue_mngr;
  gtpu_demux_ctrl&             ngu_demux;

  unique_timer test_mode_ue_timer;

  ocudulog::basic_logger& logger;

  e1ap_bearer_context_setup_request        bearer_context_setup;
  e1ap_bearer_context_setup_response       setup_resp;
  std::vector<gtpu_teid_t>                 teids;
  up_state_t                               st;
  up_state_t::iterator                     st_it;
  e1ap_bearer_context_modification_request bearer_modify;
};

class cu_up_disable_test_mode_routine
{
public:
  cu_up_disable_test_mode_routine(cu_up_manager_impl& cu_up_mngr_, ue_manager& ue_mngr_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "CU-UP disable test mode routine"; }

private:
  cu_up_manager_impl&     cu_up_mngr;
  ue_manager&             ue_mngr;
  ocudulog::basic_logger& logger;

  up_state_t                          st;
  up_state_t::iterator                st_it;
  e1ap_bearer_context_release_command release_command;
};

class cu_up_reestablish_test_mode_routine
{
public:
  cu_up_reestablish_test_mode_routine(cu_up_test_mode_config test_mode_cfg_,
                                      cu_up_manager_impl&    cu_up_mngr_,
                                      ue_manager&            ue_mngr_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "CU-UP re-establish test mode routine"; }

private:
  cu_up_test_mode_config test_mode_cfg;
  cu_up_manager_impl&    cu_up_mngr;
  ue_manager&            ue_mngr;

  up_state_t                               st;
  up_state_t::iterator                     st_it;
  e1ap_bearer_context_modification_request bearer_modify;
};

} // namespace ocuup
} // namespace ocudu
