// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ue_reconfiguration_procedure.h"
#include "../rnti_manager.h"
#include "mac_config.h"
#include "mac_controller.h"
#include "mac_scheduler_configurator.h"
#include "proc_logger.h"
#include "ocudu/adt/format.h"

using namespace ocudu;

mac_ue_reconfiguration_procedure::mac_ue_reconfiguration_procedure(const mac_ue_reconfiguration_request& req_,
                                                                   mac_control_config&                   cfg_,
                                                                   mac_ue_context&                       ue_ctx_,
                                                                   rnti_manager&                         rnti_table_,
                                                                   mac_ul_configurator&                  mac_ul_,
                                                                   mac_dl_configurator&                  mac_dl_,
                                                                   mac_scheduler_configurator&           sched_cfg_) :
  req(req_),
  cfg(cfg_),
  logger(cfg.logger),
  ue_ctx(ue_ctx_),
  rnti_table(rnti_table_),
  ul_unit(mac_ul_),
  dl_unit(mac_dl_),
  sched_cfg(sched_cfg_)
{
}

void mac_ue_reconfiguration_procedure::operator()(coro_context<async_task<mac_ue_reconfiguration_response>>& ctx)
{
  CORO_BEGIN(ctx);
  logger.debug("{}: started...", mac_log_prefix(req.ue_index, req.crnti, name()));

  // > Allocate the CS-RNTI, if the UE gets configured grants.
  if (not prepare_cs_rnti()) {
    // Nothing was changed yet, so there is nothing to roll back.
    CORO_EARLY_RETURN(handle_result(false));
  }

  // If there are bearers to add or modify.
  if (not req.bearers_to_addmod.empty()) {
    // > Add/Mod logical channels in the DEMUX.
    CORO_AWAIT_VALUE(add_ue_result, ul_unit.addmod_bearers(req.ue_index, req.bearers_to_addmod));
    if (not add_ue_result) {
      // The scheduler did not receive the new configuration.
      update_cs_rnti(false);
      CORO_EARLY_RETURN(handle_result(false));
    }

    // > Add/Mod logical channels in the MUX.
    CORO_AWAIT_VALUE(add_ue_result, dl_unit.addmod_bearers(req.ue_index, req.pcell_index, req.bearers_to_addmod));
    if (not add_ue_result) {
      update_cs_rnti(false);
      CORO_EARLY_RETURN(handle_result(false));
    }
  }

  // > Reconfigure UE in Scheduler.
  logger.debug("{}: started...", mac_log_prefix(req.ue_index, req.crnti, "Sched UE Config"));
  CORO_AWAIT_VALUE(sched_conf_res, sched_cfg.handle_ue_reconfiguration_request(req));
  if (sched_conf_res) {
    logger.info("{}: successfully finished", mac_log_prefix(req.ue_index, req.crnti, "Sched UE Config"));
  } else {
    logger.warning("{}: failed. Cause: The scheduler kept the previous UE configuration",
                   mac_log_prefix(req.ue_index, req.crnti, "Sched UE Config"));
  }
  update_cs_rnti(sched_conf_res);
  if (not sched_conf_res) {
    CORO_EARLY_RETURN(handle_result(false));
  }

  // If there are bearers to remove.
  if (not req.bearers_to_rem.empty()) {
    // > Remove logical channels in the DEMUX.
    CORO_AWAIT_VALUE(add_ue_result, ul_unit.remove_bearers(req.ue_index, req.bearers_to_rem));
    if (not add_ue_result) {
      CORO_EARLY_RETURN(handle_result(false));
    }

    // > Remove logical channels in the MUX.
    CORO_AWAIT_VALUE(add_ue_result, dl_unit.remove_bearers(req.ue_index, req.pcell_index, req.bearers_to_rem));
    if (not add_ue_result) {
      CORO_EARLY_RETURN(handle_result(false));
    }
  }

  // > After UE insertion, send response to DU manager.
  CORO_RETURN(handle_result(true));
}

bool mac_ue_reconfiguration_procedure::prepare_cs_rnti()
{
  old_cs_rnti = ue_ctx.cs_rnti;
  if (not req.cs_rnti_requested) {
    // CG deactivation, if the UE holds a CS-RNTI: free it once the scheduler applies the configuration without CG.
    return true;
  }

  if (old_cs_rnti != rnti_t::INVALID_RNTI) {
    // CG kept: report the CS-RNTI the UE already holds, so that the DU keeps it in the UE configuration.
    new_cs_rnti = old_cs_rnti;
  } else {
    // CG activation: allocate a new CS-RNTI now, as the new configuration carries it to the scheduler.
    new_cs_rnti = rnti_table.allocate(/* is_cs_rnti */ true);
    if (new_cs_rnti == rnti_t::INVALID_RNTI) {
      // Without a CS-RNTI, neither the scheduler nor the UE can use the configured grant.
      logger.warning("{}: Failed to allocate CS-RNTI for configured grant",
                     mac_log_prefix(req.ue_index, req.crnti, name()));
      return false;
    }
    if (not rnti_table.add_ue(new_cs_rnti, req.ue_index, /* is_cs_rnti */ true)) {
      logger.warning(
          "{}: CS-RNTI={} collided in RNTI table", mac_log_prefix(req.ue_index, req.crnti, name()), new_cs_rnti);
      new_cs_rnti = rnti_t::INVALID_RNTI;
      return false;
    }
    ue_ctx.cs_rnti = new_cs_rnti;
    logger.debug(
        "{}: Allocated CS-RNTI={} for configured grant", mac_log_prefix(req.ue_index, req.crnti, name()), new_cs_rnti);
  }

  // Pass the CS-RNTI to the scheduler and to the PHY configuration.
  req.assigned_cs_rnti = new_cs_rnti;
  if (req.phy_cell_group_cfg.has_value()) {
    req.phy_cell_group_cfg->cs_rnti = new_cs_rnti;
  }
  return true;
}

void mac_ue_reconfiguration_procedure::update_cs_rnti(bool sched_applied)
{
  // The scheduler keeps the old UE configuration when it does not apply the new one, so a CS-RNTI change only holds if
  // the scheduler applies it.
  if (sched_applied and old_cs_rnti != rnti_t::INVALID_RNTI and new_cs_rnti == rnti_t::INVALID_RNTI) {
    // The scheduler no longer uses the CS-RNTI.
    logger.debug("{}: Deallocating CS-RNTI={} (configured grant deactivated)",
                 mac_log_prefix(req.ue_index, req.crnti, name()),
                 old_cs_rnti);
    release_cs_rnti();
  } else if (not sched_applied and new_cs_rnti != rnti_t::INVALID_RNTI and new_cs_rnti != old_cs_rnti) {
    // The new CS-RNTI was reserved for a configuration that the scheduler did not apply.
    logger.debug("{}: Deallocating CS-RNTI={} (configuration not applied by the scheduler)",
                 mac_log_prefix(req.ue_index, req.crnti, name()),
                 new_cs_rnti);
    release_cs_rnti();
  }
}

void mac_ue_reconfiguration_procedure::release_cs_rnti() const
{
  rnti_table.rem_ue(ue_ctx.cs_rnti, /* is_cs_rnti */ true);
  ue_ctx.cs_rnti = rnti_t::INVALID_RNTI;
}

mac_ue_reconfiguration_response mac_ue_reconfiguration_procedure::handle_result(bool result)
{
  if (result) {
    logger.info("{}: finished successfully", mac_log_prefix(req.ue_index, req.crnti, name()));
  } else {
    logger.warning("{}: failed", mac_log_prefix(req.ue_index, req.crnti, name()));
  }

  // Respond back to DU manager with result
  mac_ue_reconfiguration_response resp{};
  resp.ue_index = req.ue_index;
  resp.result   = result;
  resp.cs_rnti_allocated =
      result and new_cs_rnti != rnti_t::INVALID_RNTI ? std::optional<rnti_t>{new_cs_rnti} : std::nullopt;
  return resp;
}
