// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_up_test_mode_routines.h"
#include "../cu_up_manager_helpers.h"
#include <utility>

using namespace ocudu;
using namespace ocuup;

/// Enable test mode routine.
cu_up_enable_test_mode_routine::cu_up_enable_test_mode_routine(
    const cu_up_enable_test_mode_routine_configuration& cfg,
    const cu_up_enable_test_mode_routine_dependencies&  dependencies) :
  test_mode_cfg(cfg.test_mode_cfg),
  cu_up_mngr(dependencies.cu_up_mngr),
  ue_mngr(dependencies.ue_mngr),
  ngu_demux(dependencies.ngu_demux)
{
}

void cu_up_enable_test_mode_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Setup bearer context(s) and a PDU session for each context.
  for (unsigned i = 0, e = test_mode_cfg.nof_ues; i != e; ++i) {
    bearer_context_setup = fill_test_mode_bearer_context_setup_request(test_mode_cfg);
    setup_resp           = cu_up_mngr.handle_bearer_context_setup_request(bearer_context_setup);
    // Store list of NG-U test TEID(s).
    teids.push_back(setup_resp.pdu_session_resource_setup_list.begin()->ng_dl_up_tnl_info.gtp_teid);
  }

  // Apply test TEID(s) to demux.
  ngu_demux.apply_test_teids(teids);

  // Modify bearer context(s).
  st = ue_mngr.get_up_state();
  for (st_it = st.begin(); st_it != st.end(); ++st_it) {
    bearer_modify = fill_test_mode_bearer_context_modification_request(test_mode_cfg, st_it->first, st_it->second);
    CORO_AWAIT(cu_up_mngr.handle_bearer_context_modification_request(bearer_modify));
  }

  cu_up_mngr.trigger_disable_test_mode();
  cu_up_mngr.trigger_reestablish_test_mode();

  CORO_RETURN();
}

/// Disable test mode routine.
cu_up_disable_test_mode_routine::cu_up_disable_test_mode_routine(
    const cu_up_disable_test_mode_routine_dependencies& dependencies) :
  cu_up_mngr(dependencies.cu_up_mngr), ue_mngr(dependencies.ue_mngr)
{
}

void cu_up_disable_test_mode_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Remove bearer context(s).
  st = ue_mngr.get_up_state();
  for (st_it = st.begin(); st_it != st.end(); ++st_it) {
    release_command.ue_index = st_it->first;
    CORO_AWAIT(cu_up_mngr.handle_bearer_context_release_command(release_command));
  }

  cu_up_mngr.trigger_enable_test_mode();
  CORO_RETURN();
}

/// Reestablish test mode routine.
cu_up_reestablish_test_mode_routine::cu_up_reestablish_test_mode_routine(
    const cu_up_reestablish_test_mode_routine_configuration& cfg,
    const cu_up_reestablish_test_mode_routine_dependencies&  dependencies) :
  test_mode_cfg(std::move(cfg.test_mode_cfg)), cu_up_mngr(dependencies.cu_up_mngr), ue_mngr(dependencies.ue_mngr)
{
}

void cu_up_reestablish_test_mode_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  st = ue_mngr.get_up_state();
  for (st_it = st.begin(); st_it != st.end(); ++st_it) {
    bearer_modify = fill_test_mode_bearer_context_modification_request(test_mode_cfg, st_it->first, st_it->second);
    bearer_modify.security_info = fill_test_mode_security_info(test_mode_cfg);
    CORO_AWAIT(cu_up_mngr.handle_bearer_context_modification_request(bearer_modify));
  }

  cu_up_mngr.trigger_reestablish_test_mode();
  CORO_RETURN();
}
