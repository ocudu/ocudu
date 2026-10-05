// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_bearer_resource_manager.h"
#include "ocudu/adt/format.h"
#include "ocudu/mac/config/mac_config_helpers.h"
#include "ocudu/ran/qos/five_qi_qos_mapping.h"
#include "ocudu/rlc/rlc_srb_config_factory.h"

using namespace ocudu;
using namespace odu;

/// \brief Finds an unused LCID for DRBs given a list of UE configured RLC bearers.
static lcid_t find_empty_lcid(const slotted_id_vector<drb_id_t, du_ue_drb_config>& drbs)
{
  static_vector<lcid_t, MAX_NOF_DRBS> used_lcids;
  for (const auto& drb : drbs) {
    used_lcids.push_back(drb.lcid);
  }
  std::sort(used_lcids.begin(), used_lcids.end());
  if (used_lcids.empty() or used_lcids[0] > LCID_MIN_DRB) {
    return LCID_MIN_DRB;
  }
  auto it = std::adjacent_find(used_lcids.begin(), used_lcids.end(), [](lcid_t l, lcid_t r) { return l + 1 < r; });
  if (it == used_lcids.end()) {
    // no gaps found. Use the last value + 1.
    --it;
  }
  // beginning of the gap + 1.
  lcid_t lcid = uint_to_lcid(static_cast<unsigned>(*it) + 1U);
  if (lcid > LCID_MAX_DRB) {
    return INVALID_LCID;
  }
  return lcid;
}

static error_type<std::string> validate_drb_setup_request(const f1ap_drb_to_setup&                             drb,
                                                          const slotted_id_vector<drb_id_t, du_ue_drb_config>& drb_list,
                                                          const std::map<five_qi_t, du_qos_config>& qos_config)
{
  // Validate QOS config.
  five_qi_t fiveqi = drb.qos_info.drb_qos.qos_desc.get_5qi();
  auto      qos_it = qos_config.find(fiveqi);
  if (qos_it == qos_config.end()) {
    return make_unexpected(fmt::format("No {} 5QI configured", fiveqi));
  }
  const du_qos_config& qos = qos_it->second;
  if (qos.rlc.mode != drb.mode) {
    return make_unexpected(
        fmt::format("RLC mode mismatch for {}. QoS config for {} configures {} but CU-CP requested {}",
                    drb.drb_id,
                    fiveqi,
                    qos.rlc.mode,
                    drb.mode));
  }

  // The GBR QoS Flow Information IE is required for a GBR QoS DRB, as per TS 38.473 sections 8.3.1.4 and 8.3.4.4.
  if (is_gbr_five_qi(fiveqi) and not drb.qos_info.drb_qos.gbr_qos_info.has_value()) {
    return make_unexpected(fmt::format("No GBR QoS Flow Information provided for GBR {}", fiveqi));
  }

  // Validate UL UP TNL INFO.
  if (drb.uluptnl_info_list.empty()) {
    return make_unexpected("No UL UP TNL Info List provided");
  }

  // Search for established DRB with matching DRB-Id.
  if (drb_list.contains(drb.drb_id)) {
    return make_unexpected("DRB-Id already exists");
  }

  return {};
}

static error_type<std::string>
validate_drb_modification_request(const f1ap_drb_to_modify&                            drb,
                                  const slotted_id_vector<drb_id_t, du_ue_drb_config>& drb_list)
{
  // Search for established DRB with matching DRB-Id.
  if (not drb_list.contains(drb.drb_id)) {
    return make_unexpected("DRB-Id not found");
  }

  // Validate UL UP TNL INFO.
  if (drb.uluptnl_info_list.empty()) {
    return make_unexpected("No UL UP TNL Info List provided");
  }

  return {};
}

static void reestablish_context(du_ue_resource_config& new_ue_cfg, const du_ue_resource_config& old_ue_cfg)
{
  for (const du_ue_srb_config& old_srb : old_ue_cfg.srbs) {
    new_ue_cfg.srbs.emplace(old_srb.srb_id, old_srb);
  }
  for (const du_ue_drb_config& old_drb : old_ue_cfg.drbs) {
    new_ue_cfg.drbs.emplace(old_drb.drb_id, old_drb);
  }
}

// du_bearer_resource_manager

du_bearer_resource_manager::du_bearer_resource_manager(const std::map<srb_id_t, du_srb_config>&  srbs_,
                                                       const std::map<five_qi_t, du_qos_config>& qos_,
                                                       ocudulog::basic_logger&                   logger_) :
  srb_config(srbs_), qos_config(qos_), logger(logger_)
{
}

std::optional<ul_harq_mode>
du_bearer_resource_manager::supported_allowed_harq_mode(const du_ue_resource_config& ue_cfg,
                                                        std::optional<ul_harq_mode>  allowed_harq_mode,
                                                        lcid_t                       lcid) const
{
  if (not allowed_harq_mode.has_value()) {
    return std::nullopt;
  }

  // [Implementation-defined] TS 38.331 gives the mask a meaning only when uplinkHARQ-mode is present, so an absent
  // PUSCH serving cell config is taken to leave every process in mode A.
  harq_ul_mode_mask mode_mask = ~harq_ul_mode_mask(MAX_NOF_HARQS);
  unsigned          nof_harqs = static_cast<unsigned>(pusch_serving_cell_config::nof_harq_proc_for_pusch::n16);
  const auto&       serv_cell = ue_cfg.cell_group.cells.at(SERVING_PCELL_IDX).serv_cell_cfg;
  if (serv_cell.ul_config.has_value() and serv_cell.ul_config->pusch_serv_cell_cfg.has_value()) {
    mode_mask = serv_cell.ul_config->pusch_serv_cell_cfg->ul_harq_mode;
    nof_harqs = static_cast<unsigned>(serv_cell.ul_config->pusch_serv_cell_cfg->nof_harq_proc);
  }

  if (not is_ul_harq_mode_available(mode_mask, nof_harqs, *allowed_harq_mode)) {
    logger.warning("lcid={}: Dropping the allowed HARQ mode {}. Cause: no UL HARQ process of this UE operates in that "
                   "mode",
                   lcid,
                   *allowed_harq_mode);
    return std::nullopt;
  }

  // The restriction selects nothing when every process already operates in the allowed mode. Such a UE does not report
  // the UL HARQ mode B capability the restriction belongs to either, as per TS 38.306, Section 4.2.6.1.
  const ul_harq_mode other_mode =
      *allowed_harq_mode == ul_harq_mode::mode_a ? ul_harq_mode::mode_b : ul_harq_mode::mode_a;
  if (not is_ul_harq_mode_available(mode_mask, nof_harqs, other_mode)) {
    logger.debug("lcid={}: Dropping the allowed HARQ mode {}. Cause: every UL HARQ process of this UE operates in that "
                 "mode",
                 lcid,
                 *allowed_harq_mode);
    return std::nullopt;
  }

  return allowed_harq_mode;
}

du_ue_bearer_resource_update_response
du_bearer_resource_manager::update(du_ue_resource_config&                      ue_cfg,
                                   const du_ue_bearer_resource_update_request& upd_req,
                                   const du_ue_resource_config*                reestablished_context)
{
  du_ue_bearer_resource_update_response resp;

  // > In case of RRC Reestablishment, retrieve old DRB context, to be considered in the config update.
  if (reestablished_context != nullptr) {
    reestablish_context(ue_cfg, *reestablished_context);
  }

  // Remove DRBs.
  rem_drbs(ue_cfg, upd_req);

  // Setup SRBs.
  setup_srbs(ue_cfg, upd_req);

  // Setup DRBs.
  resp.drbs_failed_to_setup = setup_drbs(ue_cfg, upd_req);

  // Modify DRBs.
  resp.drbs_failed_to_mod = modify_drbs(ue_cfg, upd_req);

  // Keep the signalling of the UE out of the mode B grants its data is served by.
  restrict_srbs_to_mode_a(ue_cfg);

  return resp;
}

void du_bearer_resource_manager::restrict_srbs_to_mode_a(du_ue_resource_config& ue_cfg) const
{
  for (du_ue_srb_config& srb : ue_cfg.srbs) {
    srb.mac_cfg.allowed_harq_mode =
        supported_allowed_harq_mode(ue_cfg, ul_harq_mode::mode_a, srb_id_to_lcid(srb.srb_id));
  }
}

void du_bearer_resource_manager::setup_srbs(du_ue_resource_config&                      ue_cfg,
                                            const du_ue_bearer_resource_update_request& upd_req)
{
  for (srb_id_t srb_id : upd_req.srbs_to_setup) {
    if (ue_cfg.srbs.contains(srb_id)) {
      // The SRB is already setup (e.g. SRB1 gets setup automatically).
      continue;
    }

    ue_cfg.srbs.emplace(srb_id);
    du_ue_srb_config& new_srb = ue_cfg.srbs[srb_id];
    new_srb.srb_id            = srb_id;
    new_srb.mac_cfg           = make_default_srb_mac_lc_config(srb_id_to_lcid(srb_id));
    auto srb_config_it        = srb_config.find(srb_id);
    if (srb_config_it != srb_config.end()) {
      new_srb.rlc_cfg = srb_config_it->second.rlc;
      // Apply the optional proactive UL grant configuration (currently used for SRB1).
      new_srb.mac_cfg.triggered_ul_grant = srb_config_it->second.triggered_ul_grant;
    } else {
      new_srb.rlc_cfg = make_default_srb_rlc_config();
    }
  }
}

std::vector<drb_id_t> du_bearer_resource_manager::setup_drbs(du_ue_resource_config&                      ue_cfg,
                                                             const du_ue_bearer_resource_update_request& upd_req)
{
  std::vector<drb_id_t> failed_drbs;

  for (const f1ap_drb_to_setup& drb_to_setup : upd_req.drbs_to_setup) {
    auto res = validate_drb_setup_request(drb_to_setup, ue_cfg.drbs, qos_config);
    if (not res.has_value()) {
      failed_drbs.push_back(drb_to_setup.drb_id);
      logger.warning("Failed to allocate {}. Cause: {}", drb_to_setup.drb_id, res.error());
      continue;
    }

    // Allocate LCID.
    lcid_t lcid = find_empty_lcid(ue_cfg.drbs);
    if (lcid > LCID_MAX_DRB) {
      logger.warning("Failed to allocate {}. Cause: No available LCIDs", drb_to_setup.drb_id);
      failed_drbs.push_back(drb_to_setup.drb_id);
      continue;
    }

    // Get QoS Config from 5QI
    five_qi_t            fiveqi = drb_to_setup.qos_info.drb_qos.qos_desc.get_5qi();
    const du_qos_config& qos    = qos_config.at(fiveqi);

    // Create new DRB QoS Flow.
    du_ue_drb_config& new_drb = ue_cfg.drbs.emplace(drb_to_setup.drb_id);
    new_drb.drb_id            = drb_to_setup.drb_id;
    new_drb.lcid              = lcid;
    new_drb.pdcp_sn_len       = drb_to_setup.pdcp_sn_len;
    new_drb.s_nssai           = drb_to_setup.qos_info.s_nssai;
    new_drb.qos               = drb_to_setup.qos_info.drb_qos;
    new_drb.f1u               = qos.f1u;
    new_drb.rlc_cfg           = qos.rlc;
    const std::optional<ul_harq_mode> allowed_harq_mode =
        supported_allowed_harq_mode(ue_cfg, qos.allowed_harq_mode, lcid);
    new_drb.mac_cfg = make_non_gbr_drb_mac_lc_config(allowed_harq_mode);
    if (drb_to_setup.qos_info.drb_qos.gbr_qos_info.has_value()) {
      // Populate MAC LC configuration for GBR DRB if GBR QoS information is present.
      new_drb.mac_cfg = make_gbr_drb_mac_lc_config(*drb_to_setup.qos_info.drb_qos.gbr_qos_info, allowed_harq_mode);
    }
    new_drb.mac_cfg.triggered_ul_grant = qos.triggered_ul_grant;

    // Update pdcp_sn_len in RLC config
    auto& rlc_cfg = new_drb.rlc_cfg;
    switch (rlc_cfg.mode) {
      case rlc_mode::am:
        rlc_cfg.am.tx.pdcp_sn_len = drb_to_setup.pdcp_sn_len;
        break;
      case rlc_mode::um_bidir:
      case rlc_mode::um_unidir_dl:
        rlc_cfg.um.tx.pdcp_sn_len = drb_to_setup.pdcp_sn_len;
        break;
      default:
        break;
    }
  }

  return failed_drbs;
}

std::vector<drb_id_t> du_bearer_resource_manager::modify_drbs(du_ue_resource_config&                      ue_cfg,
                                                              const du_ue_bearer_resource_update_request& upd_req)
{
  std::vector<drb_id_t> failed_drbs;

  for (const f1ap_drb_to_modify& drb_to_modify : upd_req.drbs_to_mod) {
    auto res = validate_drb_modification_request(drb_to_modify, ue_cfg.drbs);
    if (not res.has_value()) {
      logger.warning("Failed to modify {}. Cause: {}", drb_to_modify.drb_id, res.error());
      failed_drbs.push_back(drb_to_modify.drb_id);
      continue;
    }
  }

  return failed_drbs;
}

void du_bearer_resource_manager::rem_drbs(du_ue_resource_config&                      ue_cfg,
                                          const du_ue_bearer_resource_update_request& upd_req)
{
  for (drb_id_t drb_id : upd_req.drbs_to_rem) {
    if (not ue_cfg.drbs.contains(drb_id)) {
      logger.warning("Failed to release {}. Cause: DRB not found", drb_id);
      continue;
    }

    // Remove DRB
    ue_cfg.drbs.erase(drb_id);
  }
}
