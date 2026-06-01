// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "up_resource_manager_impl.h"
#include "up_resource_manager_helpers.h"
#include "ocudu/ocudulog/ocudulog.h"

using namespace ocudu;
using namespace ocucp;

up_resource_manager::up_resource_manager(const up_resource_manager_cfg& cfg_) :
  cfg(cfg_), logger(ocudulog::fetch_basic_logger("CU-CP"))
{
}

bool up_resource_manager::validate_request(
    const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>& setup_items) const
{
  return is_valid(setup_items, context, cfg, logger);
}

bool up_resource_manager::validate_request(const ngap_pdu_session_resource_modify_request& pdu) const
{
  return is_valid(pdu, context, cfg, logger);
}

bool up_resource_manager::validate_request(const ngap_pdu_session_resource_release_command& pdu) const
{
  return is_valid(pdu, context, cfg, logger);
}

up_config_update up_resource_manager::calculate_update(
    const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>& setup_items)
{
  ocudu_assert(is_valid(setup_items, context, cfg, logger), "Invalid PDU Session Resource Setup items.");
  return ocudu::ocucp::calculate_update(setup_items, context, cfg, logger);
}

up_config_update up_resource_manager::calculate_update(const ngap_pdu_session_resource_modify_request& pdu)
{
  ocudu_assert(is_valid(pdu, context, cfg, logger), "Invalid PDU Session Resource Modify request.");
  return ocudu::ocucp::calculate_update(pdu, context, cfg, logger);
}

up_config_update up_resource_manager::calculate_update(const ngap_pdu_session_resource_release_command& pdu)
{
  ocudu_assert(is_valid(pdu, context, cfg, logger), "Invalid PDU Session Resource Release command.");
  return ocudu::ocucp::calculate_update(pdu, context, cfg, logger);
}

static void apply_update_for_new_drbs(up_pdu_session_context&                   pdu_session_context,
                                      up_context&                               context,
                                      const std::map<drb_id_t, up_drb_context>& drb_to_add)
{
  for (const auto& drb : drb_to_add) {
    pdu_session_context.drbs.emplace(drb.first, drb.second);

    // Add to DRB-to-PDU session look-up table.
    context.drb_map.emplace(drb.first, pdu_session_context.id);

    // Mark DRB ID as used, until keys are refreshed.
    context.used_drb_ids[get_used_drb_index(drb.first)] = true;

    // add QoS flows of the DRB to the map.
    for (const auto& flow : drb.second.qos_flows) {
      context.qos_flow_map.insert({flow.first, drb.first});
    }
  }
}

static void apply_update_for_removed_drbs(up_pdu_session_context&      pdu_session_context,
                                          up_context&                  context,
                                          const std::vector<drb_id_t>& drb_to_remove)
{
  // Remove DRB and all mapped flows.
  for (const auto& drb_id : drb_to_remove) {
    // First remove all QoS flows from map.
    for (const auto& flow : pdu_session_context.drbs.at(drb_id).qos_flows) {
      context.qos_flow_map.erase(flow.first);
    }

    // Now remove DRB from PDU session and DRB map.
    pdu_session_context.drbs.erase(drb_id);
    context.drb_map.erase(drb_id);
  }
}

void up_resource_manager::apply_config_update(const up_config_update_result& result)
{
  // Apply config update in an additive way.
  for (const auto& session : result.pdu_sessions_added_list) {
    // Create new PDU session context.
    up_pdu_session_context new_ctxt(session.id, session.type, session.ul_ngu_up_tnl_info);

    // Make sure that DRB integrity/ciphering settings are saved.
    new_ctxt.integrity_protection_result       = session.integrity_protection_result;
    new_ctxt.confidentiality_protection_result = session.confidentiality_protection_result;

    // Add new DRBs.
    apply_update_for_new_drbs(new_ctxt, context, session.drb_to_add);

    // Add PDU session to list of active sessions.
    context.pdu_sessions.emplace(new_ctxt.id, new_ctxt);
  }

  for (const auto& mod_session : result.pdu_sessions_modified_list) {
    ocudu_assert(
        context.pdu_sessions.find(mod_session.id) != context.pdu_sessions.end(), "{} not allocated", mod_session.id);
    auto& session_context = context.pdu_sessions.at(mod_session.id);

    // Add new DRBs.
    apply_update_for_new_drbs(session_context, context, mod_session.drb_to_add);

    // Remove DRBs.
    apply_update_for_removed_drbs(session_context, context, mod_session.drb_to_remove);
  }

  for (const auto& rem_session : result.pdu_sessions_removed_list) {
    ocudu_assert(context.pdu_sessions.find(rem_session) != context.pdu_sessions.end(), "{} not allocated", rem_session);

    auto& session_context = context.pdu_sessions.at(rem_session);

    // Remove allocated DRBs from map.
    for (const auto& drb : session_context.drbs) {
      // Remove all QoS flows.
      for (const auto& qos_flow : drb.second.qos_flows) {
        context.qos_flow_map.erase(qos_flow.first);
      }
      context.drb_map.erase(drb.first);
    }

    context.pdu_sessions.erase(rem_session);
  }
}

const up_pdu_session_context& up_resource_manager::get_pdu_session_context(pdu_session_id_t psi) const
{
  ocudu_assert(context.pdu_sessions.find(psi) != context.pdu_sessions.end(), "{} not allocated", psi);
  return context.pdu_sessions.at(psi);
}

bool up_resource_manager::has_pdu_session(pdu_session_id_t pdu_session_id) const
{
  return context.pdu_sessions.find(pdu_session_id) != context.pdu_sessions.end();
}

const up_drb_context& up_resource_manager::get_drb_context(drb_id_t drb_id) const
{
  ocudu_assert(context.drb_map.find(drb_id) != context.drb_map.end(), "{} not allocated", drb_id);
  const auto& psi = context.drb_map.at(drb_id);
  ocudu_assert(context.pdu_sessions.find(psi) != context.pdu_sessions.end(), "Couldn't find {}", psi);
  ocudu_assert(context.pdu_sessions.at(psi).drbs.find(drb_id) != context.pdu_sessions.at(psi).drbs.end(),
               "Couldn't find {} in {}",
               drb_id,
               psi);
  return context.pdu_sessions.at(psi).drbs.at(drb_id);
}

size_t up_resource_manager::get_nof_drbs() const
{
  return context.drb_map.size();
}

size_t up_resource_manager::get_nof_pdu_sessions() const
{
  return context.pdu_sessions.size();
}

size_t up_resource_manager::get_nof_qos_flows(pdu_session_id_t psi) const
{
  size_t nof_qos_flows = 0;

  // Look up specified session
  if (!has_pdu_session(psi)) {
    logger.warning("{} not allocated", psi);
  } else {
    // Return number of active flows for specified session only.
    const auto& session = get_pdu_session_context(psi);
    for (const auto& drb : session.drbs) {
      nof_qos_flows += drb.second.qos_flows.size();
    }
  }

  return nof_qos_flows;
}

size_t up_resource_manager::get_total_nof_qos_flows() const
{
  // Return number of active QoS flows in all active sessions.
  return context.qos_flow_map.size();
}

size_t up_resource_manager::get_nof_used_drb_ids() const
{
  // Get number of DRB IDs that were already used with current KgNB key
  return context.used_drb_ids.count();
}

size_t up_resource_manager::get_nof_stale_drb_ids() const
{
  // Get number of DRB IDs that were already used with current KgNB key and are not in use by any DRB
  // Those DRB IDs can be moved to available DRB IDs pool
  return get_nof_used_drb_ids() - get_nof_drbs();
}

size_t up_resource_manager::get_nof_available_drb_ids() const
{
  // Get number of DRB IDs that were not yet used by any DRB with current KgNB key
  return MAX_NOF_DRBS - get_nof_used_drb_ids();
}

std::vector<drb_id_t> up_resource_manager::get_drbs() const
{
  std::vector<drb_id_t> drb_ids;
  drb_ids.reserve(context.drb_map.size());
  for (const auto& drb : context.drb_map) {
    drb_ids.push_back(drb.first);
  }

  return drb_ids;
}

const std::map<pdu_session_id_t, up_pdu_session_context>& up_resource_manager::get_pdu_sessions_map() const
{
  return context.pdu_sessions;
}

std::vector<pdu_session_id_t> up_resource_manager::get_pdu_sessions() const
{
  std::vector<pdu_session_id_t> pdu_session_ids;
  pdu_session_ids.reserve(context.pdu_sessions.size());
  for (const auto& session : context.pdu_sessions) {
    pdu_session_ids.push_back(session.first);
  }

  return pdu_session_ids;
}

void up_resource_manager::refresh_drb_id_after_key_change()
{
  // Clear all used DRBs.
  context.used_drb_ids.reset();

  // Mark all DRB IDs of current DRBs as used.
  for (auto& drb : context.drb_map) {
    context.used_drb_ids[get_used_drb_index(drb.first)] = true;
  }
}

bool up_resource_manager::key_refresh_useful()
{
  // This check can be used to proactively trigger intra-cell handover.
  // Number of DRB IDs needed is calculated as smaller of default max_nof_drbs_per_ue (8) and the amount of
  // addditional DRBs that UE can still add (max_nof_drbs_per_ue - number of active DRBs).
  // Trigger intra-cell handover if:
  // - if number of available DRB IDs is smaller than number of DRBs needed
  // - if stale DRB IDs would be refreshed, there would be enough available DRB IDs.
  constexpr size_t default_max_nof_drbs_per_ue = 8;
  size_t           nof_drbs_needed = std::min(cfg.max_nof_drbs_per_ue - get_nof_drbs(), default_max_nof_drbs_per_ue);
  if (get_nof_available_drb_ids() < nof_drbs_needed) {
    if (get_nof_stale_drb_ids() + get_nof_available_drb_ids() >= nof_drbs_needed) {
      return true;
    }
  }
  return false;
}

bool up_resource_manager::key_refresh_required()
{
  // This check triggers intra-cell handover when last available DRB ID was used.
  // Check if KgNB key refresh via intra-cell handover is required:
  // - there is zero available unused DRB IDs left for new DRBs,
  // - there are some stale DRB IDs that could be refreshed.
  // This situation should not happen - key_refresh_useful() should always trigger not later that this check.
  if (get_nof_stale_drb_ids() > 0 && get_nof_available_drb_ids() == 0) {
    logger.debug("KgNB key refresh required, no DRB IDs available");
    return true;
  }
  return false;
}

up_context up_resource_manager::get_up_context() const
{
  return context;
}

void up_resource_manager::set_up_context(const up_context& ctx)
{
  context = ctx;
}
