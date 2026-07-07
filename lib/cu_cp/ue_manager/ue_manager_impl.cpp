// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ue_manager_impl.h"
#include "ocudu/cu_cp/security_manager_config.h"

using namespace ocudu;
using namespace ocucp;

void cu_cp_ue::stop()
{
  task_sched.stop();
}

ue_manager::ue_manager(const ue_manager_config& cfg, const ue_manager_dependencies& dependencies) :
  next_i_rntis({short_i_rnti_t{cfg.gnb_id.id, 0, cfg.ue.nof_i_rnti_ue_bits},
                full_i_rnti_t{cfg.gnb_id.id, 0, cfg.ue.nof_i_rnti_ue_bits}}),
  gnb_id(cfg.gnb_id),
  enable_rrc_metrics(cfg.enable_rrc_metrics),
  ue_config(cfg.ue),
  up_config(up_resource_manager_cfg{cfg.drb_config, cfg.max_nof_drbs_per_ue}),
  sec_config(security_manager_config{cfg.int_algo_pref_list, cfg.enc_algo_pref_list}),
  max_nof_ues(cfg.max_nof_ues),
  timers(dependencies.timers),
  cu_cp_executor(dependencies.cu_cp_executor),
  logger(dependencies.logger),
  ue_task_scheds(timers, dependencies.cu_cp_executor, logger)
{
}

void ue_manager::stop()
{
  ue_task_scheds.stop();
}

cu_cp_ue_index_t ue_manager::add_ue(cu_cp_du_index_t du_index)
{
  if (du_index == cu_cp_du_index_t::invalid) {
    logger.warning("Invalid du_index={}", du_index);
    return cu_cp_ue_index_t::invalid;
  }

  cu_cp_ue_index_t ue_index = allocate_ue_index();
  if (ue_index == cu_cp_ue_index_t::invalid) {
    logger.warning("Failed to add UE. Cause: No available UE index");
    return cu_cp_ue_index_t::invalid;
  }

  // Create a dedicated task scheduler for the UE.
  ue_task_scheduler_impl ue_sched = ue_task_scheds.create_ue_task_sched(ue_index);

  // Create UE object.
  ues.emplace(
      std::piecewise_construct,
      std::forward_as_tuple(ue_index),
      std::forward_as_tuple(ue_index, du_index, timers, cu_cp_executor, up_config, sec_config, std::move(ue_sched)));

  logger.info("ue={} du_index={}: Created new CU-CP UE", ue_index, du_index);

  return ue_index;
}

bool ue_manager::ue_admission_limit_reached() const
{
  return ues.size() > max_nof_ues;
}

bool ue_manager::update_ue_context(cu_cp_ue_index_t ue_index,
                                   gnb_du_id_t      du_id,
                                   pci_t            pci,
                                   rnti_t           rnti,
                                   du_cell_index_t  pcell_index)
{
  if (ue_index == cu_cp_ue_index_t::invalid) {
    logger.warning("Can't update UE with invalid UE index");
    return false;
  }

  if (ues.find(ue_index) == ues.end()) {
    logger.warning("ue={}: Update UE called for inexistent UE", ue_index);
    return false;
  }

  if (du_id == gnb_du_id_t::invalid) {
    logger.warning("Invalid du_id={}", fmt::underlying(du_id));
    return false;
  }

  if (pci == INVALID_PCI) {
    logger.warning("Invalid pci={}", pci);
    return false;
  }

  if (rnti == rnti_t::INVALID_RNTI) {
    logger.warning("Invalid rnti={}", rnti);
    return false;
  }

  if (pcell_index == INVALID_DU_CELL_INDEX) {
    logger.warning("Invalid pcell_index={}", fmt::underlying(pcell_index));
    return false;
  }

  // Check if the UE is already present.
  if (get_ue_index(pci, rnti) != cu_cp_ue_index_t::invalid) {
    logger.warning("UE with pci={} and rnti={} already exists", pci, rnti);
    return false;
  }

  auto& ue = ues.at(ue_index);
  ue.update_du_ue(du_id, pci, rnti, pcell_index);

  // Add PCI and RNTI to lookup.
  pci_rnti_to_ue_index.emplace(std::make_tuple(pci, rnti), ue_index);

  logger.debug("ue={}: Updated UE with gnb_du_id={} pci={} rnti={} pcell_index={}",
               ue_index,
               fmt::underlying(du_id),
               pci,
               rnti,
               fmt::underlying(pcell_index));

  return true;
}

void ue_manager::remove_ue(cu_cp_ue_index_t ue_index)
{
  if (ue_index == cu_cp_ue_index_t::invalid) {
    logger.warning("Can't remove UE with invalid UE index");
    return;
  }

  if (ues.find(ue_index) == ues.end()) {
    logger.warning("ue={}: Remove UE called for inexistent UE", ue_index);
    return;
  }

  // Remove UE from lookups.
  pci_t pci = ues.at(ue_index).get_pci();
  if (pci != INVALID_PCI) {
    rnti_t c_rnti = ues.at(ue_index).get_c_rnti();
    if (c_rnti != rnti_t::INVALID_RNTI) {
      pci_rnti_to_ue_index.erase(std::make_tuple(pci, c_rnti));
    } else {
      logger.warning("ue={}: RNTI not found", ue_index);
    }
  } else {
    logger.debug("ue={}: PCI not found", ue_index);
  }

  // Remove Full-I-RNTI and Short-I-RNTI from lookups, in case the UE was RRC_INACTIVE and is being removed without
  // having gone through set_active() first (e.g. RNA update or RAN paging timeout).
  for (auto it = full_i_rnti_to_ue_index.begin(); it != full_i_rnti_to_ue_index.end();) {
    it = (it->second == ue_index) ? full_i_rnti_to_ue_index.erase(it) : std::next(it);
  }
  for (auto it = short_i_rnti_to_ue_index.begin(); it != short_i_rnti_to_ue_index.end();) {
    it = (it->second == ue_index) ? short_i_rnti_to_ue_index.erase(it) : std::next(it);
  }

  // Remove CU-CP UE from database.
  ues.erase(ue_index);

  logger.debug("ue={}: Removed", ue_index);
}

bool ue_manager::set_plmn(cu_cp_ue_index_t ue_index, const plmn_identity& plmn)
{
  if (ue_index == cu_cp_ue_index_t::invalid) {
    logger.warning("Can't set PLMN for UE with invalid UE index");
    return false;
  }

  if (ues.find(ue_index) == ues.end()) {
    logger.warning("ue={}: Set PLMN called for inexistent UE", ue_index);
    return false;
  }

  if (blocked_plmns.find(plmn) != blocked_plmns.end()) {
    logger.warning(
        "ue={}: CU-CP UE creation Failed. Cause: UE connections for PLMN {} are currently not allowed", ue_index, plmn);
    return false;
  }

  ues.at(ue_index).get_ue_context().plmn = plmn;
  logger.debug("ue={}: Set PLMN to {}", ue_index, plmn);
  return true;
}

void ue_manager::add_blocked_plmns(const std::vector<plmn_identity>& plmns)
{
  for (const auto& plmn : plmns) {
    if (blocked_plmns.insert(plmn).second) {
      logger.info("Blocking new UE connections for PLMN {}", plmn);
    }
  }
}

void ue_manager::remove_blocked_plmns(const std::vector<plmn_identity>& plmns)
{
  for (const auto& plmn : plmns) {
    if (blocked_plmns.erase(plmn)) {
      logger.info("Re-allowing new UE connections for PLMN {}", plmn);
    }
  }
}

std::vector<cu_cp_ue*> ue_manager::find_ues(plmn_identity plmn)
{
  std::vector<cu_cp_ue*> found_ues;
  for (auto& ue : ues) {
    if (ue.second.get_ue_context().plmn == plmn) {
      found_ues.push_back(&ue.second);
    }
  }

  return found_ues;
}

cu_cp_ue_index_t ue_manager::get_ue_index(pci_t pci, rnti_t rnti)
{
  if (pci_rnti_to_ue_index.find(std::make_tuple(pci, rnti)) != pci_rnti_to_ue_index.end()) {
    return pci_rnti_to_ue_index.at(std::make_tuple(pci, rnti));
  }
  logger.debug("UE index for pci={} and rnti={} not found", pci, rnti);
  return cu_cp_ue_index_t::invalid;
}

cu_cp_ue_index_t ue_manager::get_ue_index(full_i_rnti_t full_i_rnti)
{
  if (full_i_rnti_to_ue_index.find(full_i_rnti) != full_i_rnti_to_ue_index.end()) {
    return full_i_rnti_to_ue_index.at(full_i_rnti);
  }
  logger.debug("UE index for {} not found", full_i_rnti);
  return cu_cp_ue_index_t::invalid;
}

cu_cp_ue_index_t ue_manager::get_ue_index(short_i_rnti_t short_i_rnti)
{
  if (short_i_rnti_to_ue_index.find(short_i_rnti) != short_i_rnti_to_ue_index.end()) {
    return short_i_rnti_to_ue_index.at(short_i_rnti);
  }
  logger.debug("UE index for {} not found", short_i_rnti);
  return cu_cp_ue_index_t::invalid;
}

std::optional<i_rntis_t> ue_manager::set_inactive(cu_cp_ue_index_t ue_index)
{
  if (ue_index == cu_cp_ue_index_t::invalid) {
    logger.warning("Can't set inactive UE with invalid UE index");
    return std::nullopt;
  }

  if (ues.find(ue_index) == ues.end()) {
    logger.warning("ue={}: Set inactive called for inexistent UE", ue_index);
    return std::nullopt;
  }

  std::optional<i_rntis_t> i_rntis = allocate_i_rntis();
  if (!i_rntis.has_value()) {
    logger.warning("ue={}: Could not allocate I-RNTIs for inactive UE", ue_index);
    return std::nullopt;
  }

  // Set RRC state to RRC_INACTIVE.
  auto& ue = ues.at(ue_index);
  ue.get_rrc_ue()->set_rrc_state(rrc_state::inactive);

  // Add I-RNTIs to lookups.
  full_i_rnti_to_ue_index.emplace(i_rntis->full_i_rnti, ue_index);
  short_i_rnti_to_ue_index.emplace(i_rntis->short_i_rnti, ue_index);

  return i_rntis;
}

void ue_manager::set_active(cu_cp_ue_index_t ue_index)
{
  if (ue_index == cu_cp_ue_index_t::invalid) {
    logger.warning("Can't set active UE with invalid UE index");
    return;
  }

  if (ues.find(ue_index) == ues.end()) {
    logger.warning("ue={}: Set active called for inexistent UE", ue_index);
    return;
  }

  // Remove Full-I-RNTI from lookup.
  for (auto it = full_i_rnti_to_ue_index.begin(); it != full_i_rnti_to_ue_index.end();) {
    if (it->second == ue_index) {
      it = full_i_rnti_to_ue_index.erase(it);
    } else {
      ++it;
    }
  }
  // Remove Short-I-RNTI from lookup.
  for (auto it = short_i_rnti_to_ue_index.begin(); it != short_i_rnti_to_ue_index.end();) {
    if (it->second == ue_index) {
      it = short_i_rnti_to_ue_index.erase(it);
    } else {
      ++it;
    }
  }
}

std::optional<full_i_rnti_t> ue_manager::get_full_i_rnti(cu_cp_ue_index_t ue_index)
{
  if (ue_index == cu_cp_ue_index_t::invalid) {
    logger.warning("Can't get I-RNTIs for UE with invalid UE index");
    return std::nullopt;
  }

  if (ues.find(ue_index) == ues.end()) {
    logger.warning("ue={}: Get I-RNTIs called for inexistent UE", ue_index);
    return std::nullopt;
  }

  auto& ue = ues.at(ue_index);
  if (ue.get_rrc_ue()->get_rrc_state() != rrc_state::inactive) {
    logger.warning("ue={}: Get I-RNTIs called for active UE", ue_index);
    return std::nullopt;
  }

  for (auto it = full_i_rnti_to_ue_index.begin(); it != full_i_rnti_to_ue_index.end();) {
    if (it->second == ue_index) {
      return it->first;
    }
  }
  return std::nullopt;
}

// Common.

cu_cp_ue* ue_manager::find_ue(cu_cp_ue_index_t ue_index)
{
  if (ues.find(ue_index) != ues.end()) {
    return &ues.at(ue_index);
  }
  return nullptr;
}

ue_task_scheduler* ue_manager::find_ue_task_scheduler(cu_cp_ue_index_t ue_index)
{
  if (ues.find(ue_index) != ues.end() && ues.at(ue_index).du_ue_created()) {
    return &ues.at(ue_index).get_task_sched();
  }
  return nullptr;
}

// DU processor.
cu_cp_ue* ue_manager::find_du_ue(cu_cp_ue_index_t ue_index)
{
  if (ues.find(ue_index) != ues.end() && ues.at(ue_index).du_ue_created()) {
    return &ues.at(ue_index);
  }
  return nullptr;
}

size_t ue_manager::get_nof_du_ues(cu_cp_du_index_t du_index)
{
  unsigned ue_count = 0;
  // Count UEs connected to the DU.
  for (const auto& ue : ues) {
    if (ue.second.get_du_index() == du_index) {
      ue_count++;
    }
  }

  return ue_count;
}

std::vector<cu_cp_metrics_report::ue_info> ue_manager::handle_ue_metrics_report_request() const
{
  if (!enable_rrc_metrics) {
    return {};
  }

  std::vector<cu_cp_metrics_report::ue_info> report;
  report.reserve(ues.size());

  for (const auto& ue : ues) {
    report.emplace_back();
    cu_cp_metrics_report::ue_info& ue_report = report.back();

    ue_report.rnti  = ue.second.get_c_rnti();
    ue_report.du_id = ue.second.get_du_id();
    ue_report.pci   = ue.second.get_pci();

    if (ue.second.get_rrc_ue() == nullptr) {
      ue_report.rrc_connection_state = rrc_state::idle;
    } else {
      ue_report.rrc_connection_state = ue.second.get_rrc_ue()->get_rrc_ue_control_message_handler().get_rrc_state();
    }
  }

  return report;
}

// Private functions.

cu_cp_ue_index_t ue_manager::allocate_ue_index()
{
  // Return invalid when no UE index is available.
  if (ues.size() == cu_cp_ue_index_to_uint(cu_cp_ue_index_t::max)) {
    return cu_cp_ue_index_t::invalid;
  }

  // Check if the next_ue_index is available.
  if (ues.find(next_ue_index) == ues.end()) {
    cu_cp_ue_index_t ret = next_ue_index;
    // increase the next_ue_index
    increase_next_ue_index();
    return ret;
  }

  // Find holes in the allocated IDs by iterating over all ids starting with the next_ue_index to find the
  // available id.
  while (true) {
    // Increase the next_ue_index and try again.
    increase_next_ue_index();

    // Return the id if it is not already used.
    if (ues.find(next_ue_index) == ues.end()) {
      cu_cp_ue_index_t ret = next_ue_index;
      // Increase the next_ue_index.
      increase_next_ue_index();
      return ret;
    }
  }

  return cu_cp_ue_index_t::invalid;
}

std::optional<i_rntis_t> ue_manager::allocate_i_rntis()
{
  // Return invalid when no UE index is available.
  if (short_i_rnti_to_ue_index.size() == max_nof_ues or full_i_rnti_to_ue_index.size() == max_nof_ues) {
    return std::nullopt;
  }

  std::optional<full_i_rnti_t>  next_full;
  std::optional<short_i_rnti_t> next_short;

  // Allocate Full-I-RNTI.
  // Check if the Full-I-RNTI stored in next_i_rntis is available.
  if (full_i_rnti_to_ue_index.find(next_i_rntis.full_i_rnti) == full_i_rnti_to_ue_index.end()) {
    next_full = next_i_rntis.full_i_rnti;
  }

  if (!next_full.has_value()) {
    // Find holes in the allocated Full-I-RNTI IDs by iterating over all ids starting with the Full-I-RNTI stored in
    // next_i_rntis to find the available ID.
    while (true) {
      // Increase the Full-I-RNTI stored in the next_i_rnti and try again.
      increase_full_i_rnti(next_i_rntis.full_i_rnti);
      // Return the id if it is not already used
      if (full_i_rnti_to_ue_index.find(next_i_rntis.full_i_rnti) == full_i_rnti_to_ue_index.end()) {
        next_full = next_i_rntis.full_i_rnti;
        break;
      }
    }
  }

  // Allocate Short-I-RNTI.
  // Check if the Short-I-RNTI stored in next_i_rntis is available.
  if (short_i_rnti_to_ue_index.find(next_i_rntis.short_i_rnti) == short_i_rnti_to_ue_index.end()) {
    next_short = next_i_rntis.short_i_rnti;
  }

  if (!next_short.has_value()) {
    // Find holes in the allocated Short-I-RNTI IDs by iterating over all ids starting with the Short-I-RNTI stored in
    // next_i_rntis to find the available ID.
    while (true) {
      // Increase the Short-I-RNTI stored in the next_i_rnti and try again.
      increase_short_i_rnti(next_i_rntis.short_i_rnti);
      // Return the id if it is not already used
      if (short_i_rnti_to_ue_index.find(next_i_rntis.short_i_rnti) == short_i_rnti_to_ue_index.end()) {
        next_short = next_i_rntis.short_i_rnti;
        break;
      }
    }
  }

  if (next_full.has_value() && next_short.has_value()) {
    // Increase the Full-I-RNTI stored in next_i_rntis.
    increase_full_i_rnti(next_i_rntis.full_i_rnti);
    // Increase the Short-I-RNTI stored in next_i_rntis.
    increase_short_i_rnti(next_i_rntis.short_i_rnti);

    return i_rntis_t{next_short.value(), next_full.value()};
  }

  return std::nullopt;
}
