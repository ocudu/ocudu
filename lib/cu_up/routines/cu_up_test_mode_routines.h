// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../cu_up_manager_impl.h"

namespace ocudu::ocuup {

/// Holds the CU-CP enable test mode routine configuration parameters.
struct cu_up_enable_test_mode_routine_configuration {
  cu_up_test_mode_config test_mode_cfg;
};

/// Holds the CU-CP enable test mode routine dependencies.
struct cu_up_enable_test_mode_routine_dependencies {
  cu_up_manager_impl& cu_up_mngr;
  ue_manager&         ue_mngr;
  gtpu_demux_ctrl&    ngu_demux;
};

class cu_up_enable_test_mode_routine
{
public:
  cu_up_enable_test_mode_routine(const cu_up_enable_test_mode_routine_configuration& cfg,
                                 const cu_up_enable_test_mode_routine_dependencies&  dependencies);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "CU-UP enable test mode routine"; }

private:
  const cu_up_test_mode_config test_mode_cfg;
  cu_up_manager_impl&          cu_up_mngr;
  ue_manager&                  ue_mngr;
  gtpu_demux_ctrl&             ngu_demux;

  unique_timer test_mode_ue_timer;

  e1ap_bearer_context_setup_request        bearer_context_setup;
  e1ap_bearer_context_setup_response       setup_resp;
  std::vector<gtpu_teid_t>                 teids;
  up_state_t                               st;
  up_state_t::iterator                     st_it;
  e1ap_bearer_context_modification_request bearer_modify;
};

/// Holds the CU-UP disable test mode routine dependencies.
struct cu_up_disable_test_mode_routine_dependencies {
  cu_up_manager_impl& cu_up_mngr;
  ue_manager&         ue_mngr;
};

class cu_up_disable_test_mode_routine
{
public:
  explicit cu_up_disable_test_mode_routine(const cu_up_disable_test_mode_routine_dependencies& dependencies);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "CU-UP disable test mode routine"; }

private:
  cu_up_manager_impl& cu_up_mngr;
  ue_manager&         ue_mngr;

  up_state_t                          st;
  up_state_t::iterator                st_it;
  e1ap_bearer_context_release_command release_command;
};

/// Holds the CU-UP reestablish test mode routine configuration parameters.
struct cu_up_reestablish_test_mode_routine_configuration {
  cu_up_test_mode_config test_mode_cfg;
};

/// Holds the CU-UP reestablish test mode routine dependencies.
struct cu_up_reestablish_test_mode_routine_dependencies {
  cu_up_manager_impl& cu_up_mngr;
  ue_manager&         ue_mngr;
};

class cu_up_reestablish_test_mode_routine
{
public:
  cu_up_reestablish_test_mode_routine(const cu_up_reestablish_test_mode_routine_configuration& cfg,
                                      const cu_up_reestablish_test_mode_routine_dependencies&  dependencies);

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

} // namespace ocudu::ocuup
