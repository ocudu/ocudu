// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ngap_repository.h"
#include "ocudu/adt/format.h"
#include "ocudu/ngap/ngap_context.h"
#include "ocudu/ngap/ngap_factory.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;
using namespace ocucp;

ngap_repository::ngap_repository(const ngap_repository_config& cfg_, const ngap_repository_dependencies& dependencies) :
  cfg(cfg_),
  cu_cp_executor(dependencies.cu_cp_executor),
  timers(dependencies.timers),
  n2_gws(dependencies.n2_gws),
  cu_cp_notifier(dependencies.cu_cp_notifier),
  paging_handler(dependencies.paging_handler),
  logger(dependencies.logger),
  amf_task_sched(ngap_task_scheduler_config{.max_nof_amfs = static_cast<uint16_t>(n2_gws.size())},
                 ngap_task_scheduler_dependencies{.timers = dependencies.timers,
                                                  .exec   = dependencies.cu_cp_executor,
                                                  .logger = dependencies.logger})
{
  ocudu_assert(n2_gws.size() == cfg.ngaps.size(), "N2 gateways must have the same sice as NGAPs");

  for (uint16_t amf_idx = 0; amf_idx < n2_gws.size(); amf_idx++) {
    auto* ngap_entity = add_ngap(uint_to_cu_cp_amf_index(amf_idx));
    ocudu_assert(ngap_entity != nullptr, "Failed to add NGAP for gateway");
  }
}

ngap_interface* ngap_repository::add_ngap(cu_cp_amf_index_t amf_index)
{
  n2_connection_client&             n2_gw  = *n2_gws[static_cast<uint16_t>(amf_index)];
  cu_cp_configuration::ngap_config& config = cfg.ngaps[static_cast<uint16_t>(amf_index)];

  // Create NGAP object
  auto it = ngap_db.insert(std::make_pair(amf_index, ngap_context{}));
  ocudu_assert(it.second, "Unable to insert NGAP in map");
  ngap_context& ngap_ctxt = it.first->second;
  ngap_ctxt.ngap_to_cu_cp_notifier.connect_cu_cp(cu_cp_notifier, paging_handler, amf_index, amf_task_sched);

  ngap_configuration              ngap_cfg = {cfg.gnb_id,
                                              cfg.ran_node_name,
                                              amf_index,
                                              config.supported_tas,
                                              config.amf_addr,
                                              cfg.procedure_timeout,
                                              cfg.request_pdu_session_timeout};
  std::unique_ptr<ngap_interface> ngap_entity =
      create_ngap(ngap_cfg, ngap_ctxt.ngap_to_cu_cp_notifier, n2_gw, timers, cu_cp_executor);

  if (ngap_entity == nullptr) {
    logger.error("Failed to create NGAP");
    return nullptr;
  }

  ngap_ctxt.ngap = std::move(ngap_entity);

  return ngap_ctxt.ngap.get();
}

void ngap_repository::update_plmn_lookup(cu_cp_amf_index_t amf_index)
{
  auto ngap = ngap_db.find(amf_index);
  if (ngap == ngap_db.end()) {
    logger.error("NGAP not found for AMF index {}", amf_index);
    return;
  }

  std::vector<plmn_identity> supported_plmns = ngap->second.ngap->get_ngap_context().get_supported_plmns();
  for (auto plmn : supported_plmns) {
    plmn_to_amf_index.emplace(plmn, amf_index);
  }
}

ngap_interface* ngap_repository::find_ngap(const plmn_identity& plmn)
{
  if (plmn_to_amf_index.find(plmn) == plmn_to_amf_index.end()) {
    return nullptr;
  }

  return ngap_db.at(plmn_to_amf_index.at(plmn)).ngap.get();
}

ngap_interface* ngap_repository::find_ngap(const cu_cp_amf_index_t& amf_index)
{
  if (ngap_db.find(amf_index) == ngap_db.end()) {
    return nullptr;
  }

  return ngap_db.at(amf_index).ngap.get();
}

std::map<cu_cp_amf_index_t, ngap_interface*> ngap_repository::get_ngaps() const
{
  std::map<cu_cp_amf_index_t, ngap_interface*> calculated_ngaps;
  for (const auto& amf : ngap_db) {
    calculated_ngaps.emplace(amf.first, amf.second.ngap.get());
  }
  return calculated_ngaps;
}

std::vector<ngap_info> ngap_repository::handle_ngap_metrics_report_request() const
{
  if (!cfg.enable_ngap_metrics) {
    return {};
  }
  std::vector<ngap_info> ngap_reports;
  ngap_reports.reserve(ngap_db.size());
  for (const auto& ngap : ngap_db) {
    ngap_reports.emplace_back(ngap.second.ngap->get_metrics_handler().handle_ngap_metrics_report_request());
  }
  return ngap_reports;
}

size_t ngap_repository::get_nof_ngap_ues() const
{
  size_t nof_ues = 0;
  for (const auto& du : ngap_db) {
    nof_ues += du.second.ngap->get_nof_ues();
  }
  return nof_ues;
}

std::vector<supported_tracking_area> ngap_repository::get_supported_tracking_areas() const
{
  std::vector<supported_tracking_area> supported_tas;
  for (const auto& ngap : ngap_db) {
    const auto& ngap_supported_tas = ngap.second.ngap->get_ngap_context().supported_tas;
    supported_tas.insert(supported_tas.end(), ngap_supported_tas.begin(), ngap_supported_tas.end());
  }
  return supported_tas;
}

std::vector<guami_t> ngap_repository::get_served_guamis() const
{
  std::vector<guami_t> served_guamis;
  for (const auto& ngap : ngap_db) {
    const auto& ngap_served_guamis = ngap.second.ngap->get_ngap_context().served_guami_list;
    served_guamis.insert(served_guamis.end(), ngap_served_guamis.begin(), ngap_served_guamis.end());
  }
  return served_guamis;
}
