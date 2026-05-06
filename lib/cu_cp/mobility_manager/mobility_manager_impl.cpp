// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "mobility_manager_impl.h"
#include "../du_processor/du_processor_repository.h"
#include "mobility_manager_helpers.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/ran/tac.h"
#include <optional>
#include <set>
#include <vector>

using namespace ocudu;
using namespace ocucp;

mobility_manager::mobility_manager(const mobility_manager_cfg&      cfg_,
                                   mobility_manager_cu_cp_notifier& cu_cp_notifier_,
                                   ngap_repository&                 ngap_db_,
                                   du_processor_repository&         du_db_,
                                   xnap_repository&                 xnap_db_,
                                   ue_manager&                      ue_mng_,
                                   cell_meas_manager&               cell_meas_mng_) :
  cfg(cfg_),
  cu_cp_notifier(cu_cp_notifier_),
  ngap_db(ngap_db_),
  du_db(du_db_),
  xnap_db(xnap_db_),
  ue_mng(ue_mng_),
  cell_meas_mng(cell_meas_mng_),
  logger(ocudulog::fetch_basic_logger("CU-CP"))
{
}

void mobility_manager::trigger_handover(pci_t         source_pci,
                                        rnti_t        rnti,
                                        pci_t         target_pci,
                                        plmn_identity target_plmn,
                                        tac_t         target_tac)
{
  cu_cp_ue_index_t ue_index = ue_mng.get_ue_index(source_pci, rnti);
  if (ue_index == cu_cp_ue_index_t::invalid) {
    logger.warning("Could not trigger handover, UE is invalid. rnti={} pci={}", rnti, source_pci);
    return;
  }
  expected<std::pair<unsigned, nr_cell_identity>> target = cell_meas_mng.find_neighbour_nci(target_pci);
  if (not target) {
    logger.warning("Could not trigger handover, unknown target cell. pci={}", target_pci);
    return;
  }

  handle_handover(ue_index,
                  target.value().second.gnb_id(target->first),
                  target.value().second,
                  target_pci,
                  target_plmn,
                  target_tac);
}

void mobility_manager::trigger_conditional_handover(
    pci_t                                                source_pci,
    rnti_t                                               rnti,
    span<const pci_t>                                    target_pcis,
    std::chrono::milliseconds                            timeout,
    std::optional<std::chrono::system_clock::time_point> t1_thres_override)
{
  // Trigger CHO by preparing and then automatically executing.
  handle_conditional_handover(source_pci, rnti, target_pcis, timeout, t1_thres_override);
}

void mobility_manager::trigger_auto_conditional_handover(cu_cp_ue_index_t ue_index)
{
  if (!cfg.trigger_cho_on_ue_setup) {
    return;
  }

  cu_cp_ue* u = ue_mng.find_du_ue(ue_index);
  if (u == nullptr) {
    logger.debug("ue={}: Skipping auto-CHO: UE not found", ue_index);
    return;
  }

  if (u->get_rrc_ue() == nullptr) {
    logger.debug("ue={}: Skipping auto-CHO: RRC UE missing", ue_index);
    return;
  }

  if (u->get_rrc_ue()->get_rrc_state() != rrc_state::connected) {
    logger.debug("ue={}: Skipping auto-CHO: UE is not in RRC Connected state", ue_index);
    return;
  }

  if (!u->get_rrc_ue()->is_conditional_handover_supported()) {
    logger.debug("ue={}: Skipping auto-CHO: UE does not support CHO", ue_index);
    return;
  }

  if (u->get_cho_context().has_value()) {
    logger.debug("ue={}: Skipping auto-CHO: CHO context already initialized", ue_index);
    return;
  }

  if (u->get_cu_up_index() == cu_cp_cu_up_index_t::invalid || u->get_up_resource_manager().get_nof_drbs() == 0) {
    logger.info("ue={}: Auto-CHO deferred: bearer/user-plane context not ready yet", ue_index);
    return;
  }

  const pci_t  source_pci  = u->get_pci();
  const rnti_t source_rnti = u->get_c_rnti();
  if (source_pci == INVALID_PCI || source_rnti == rnti_t::INVALID_RNTI) {
    logger.debug(
        "ue={}: Skipping auto-CHO: source PCI or C-RNTI invalid (pci={}, rnti={})", ue_index, source_pci, source_rnti);
    return;
  }

  logger.info("ue={}: Auto-CHO trigger enabled. Starting CHO with default neighbor candidate set, timeout={}ms",
              ue_index,
              cfg.cho_timeout.count());
  handle_conditional_handover(source_pci, source_rnti, span<const pci_t>{}, cfg.cho_timeout, std::nullopt);
}

void mobility_manager::handle_conditional_handover(
    pci_t                                                source_pci,
    rnti_t                                               rnti,
    span<const pci_t>                                    target_pcis,
    std::chrono::milliseconds                            timeout,
    std::optional<std::chrono::system_clock::time_point> t1_thres_override)
{
  // Find UE by PCI and RNTI.
  cu_cp_ue_index_t ue_index = ue_mng.get_ue_index(source_pci, rnti);
  if (ue_index == cu_cp_ue_index_t::invalid) {
    logger.warning("Could not prepare CHO, UE is invalid. rnti={} pci={}", rnti, source_pci);
    return;
  }

  // Find the UE context.
  cu_cp_ue* u = ue_mng.find_du_ue(ue_index);
  if (u == nullptr) {
    logger.error("ue={}: Couldn't find UE for CHO preparation", ue_index);
    return;
  }

  // Check UE supports CHO before doing any candidate work.
  if (u->get_rrc_ue() == nullptr || !u->get_rrc_ue()->is_conditional_handover_supported()) {
    logger.warning("ue={}: UE does not support CHO (Rel-16); aborting preparation", ue_index);
    return;
  }

  std::vector<pci_t> requested_target_pcis;
  if (target_pcis.empty()) {
    const nr_cell_identity serving_nci = u->get_rrc_ue()->get_cell_context().cgi.nci;
    requested_target_pcis              = cell_meas_mng.get_neighbor_pcis(serving_nci);
    if (requested_target_pcis.empty()) {
      logger.warning(
          "ue={}: CHO preparation failed. No neighbor targets configured for serving nci={:#x}", ue_index, serving_nci);
      return;
    }
    logger.debug("ue={}: No CHO targets provided. Using all {} neighbor cells as candidates",
                 ue_index,
                 requested_target_pcis.size());
  } else {
    requested_target_pcis.assign(target_pcis.begin(), target_pcis.end());
  }

  std::set<pci_t>    seen_target_pcis;
  std::vector<pci_t> unique_target_pcis;
  unique_target_pcis.reserve(requested_target_pcis.size());
  for (pci_t target_pci : requested_target_pcis) {
    if (seen_target_pcis.insert(target_pci).second) {
      unique_target_pcis.push_back(target_pci);
    }
  }
  if (unique_target_pcis.size() != requested_target_pcis.size()) {
    logger.warning("ue={}: CHO request contains duplicate target PCIs. Duplicates were ignored", ue_index);
  }

  if (unique_target_pcis.size() > 8) {
    logger.warning(
        "ue={}: CHO preparation failed. Too many unique target PCIs ({} > 8)", ue_index, unique_target_pcis.size());
    return;
  }

  // Validate all targets are intra-CU.
  cu_cp_du_index_t source_du = u->get_du_index();
  if (source_du == cu_cp_du_index_t::invalid) {
    logger.warning("ue={}: CHO preparation failed. Source DU index is invalid", ue_index);
    return;
  }

  std::vector<cu_cp_cho_target_candidate> targets;
  for (pci_t target_pci : unique_target_pcis) {
    cu_cp_du_index_t target_du = du_db.find_du(target_pci);
    if (target_du != cu_cp_du_index_t::invalid) {
      // Intra-CU candidate: target cell served by a local DU.
      std::optional<nr_cell_global_id_t> cgi =
          du_db.get_du_processor(target_du).get_mobility_handler().get_cgi(target_pci);
      if (!cgi.has_value()) {
        logger.warning("ue={}: CHO candidate skipped. Could not find CGI for PCI {}", ue_index, target_pci);
        continue;
      }
      targets.push_back({target_pci, cgi.value(), target_du, std::nullopt});
    } else {
      // Inter-CU candidate: try to find a remote CU-CP via Xn.
      expected<std::pair<unsigned, nr_cell_identity>> nbr = cell_meas_mng.find_neighbour_nci(target_pci);
      if (!nbr.has_value()) {
        logger.warning("ue={}: CHO candidate skipped. PCI {} not found in neighbour NCI list", ue_index, target_pci);
        continue;
      }
      gnb_id_t                        target_gnb_id = nbr->second.gnb_id(nbr->first);
      std::optional<xnc_peer_index_t> xnc_index     = xnap_db.find_xnap_index(target_gnb_id);
      if (!xnc_index.has_value()) {
        logger.warning(
            "ue={}: CHO candidate skipped. No Xn peer found for gNB-ID derived from PCI {}", ue_index, target_pci);
        continue;
      }
      nr_cell_global_id_t cgi{u->get_ue_context().plmn, nbr->second};
      targets.push_back({target_pci, cgi, cu_cp_du_index_t::invalid, xnc_index});
    }
  }
  if (targets.empty()) {
    logger.warning("ue={}: CHO preparation failed. No valid candidates after PCI lookup", ue_index);
    return;
  }

  // Check if CHO is already pending.
  auto& cho_ctx = u->get_cho_context();
  if (cho_ctx.has_value() && cho_ctx->state != cu_cp_ue_cho_context::state_t::idle) {
    logger.warning(
        "ue={}: CHO preparation failed. CHO already pending (state={})", ue_index, static_cast<int>(cho_ctx->state));
    return;
  }

  // Initialize CHO context.
  if (!cho_ctx.has_value()) {
    cho_ctx.emplace();
  }
  cho_ctx->clear();

  logger.info("ue={}: Starting CHO with {} candidate(s)", ue_index, targets.size());

  cu_cp_intra_cu_cho_request cho_request{};
  cho_request.source_ue_index   = ue_index;
  cho_request.source_du_index   = source_du;
  cho_request.targets           = std::move(targets);
  cho_request.timeout           = timeout;
  cho_request.t1_thres_override = t1_thres_override;

  auto cho_trigger = [this, cho_request = std::move(cho_request), cho_response = cu_cp_intra_cu_cho_response{}](
                         coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);
    CORO_AWAIT_VALUE(cho_response, cu_cp_notifier.on_intra_cu_cho_required(cho_request));
    if (!cho_response.success) {
      logger.warning("ue={}: Intra-CU CHO failed", cho_request.source_ue_index);
    }
    CORO_RETURN();
  };

  u->get_task_sched().schedule_async_task(launch_async(std::move(cho_trigger)));
}

void mobility_manager::handle_neighbor_better_than_spcell(cu_cp_ue_index_t     ue_index,
                                                          gnb_id_t             neighbor_gnb_id,
                                                          nr_cell_identity     neighbor_nci,
                                                          pci_t                neighbor_pci,
                                                          plmn_identity        neighbor_plmn,
                                                          std::optional<tac_t> neighbor_tac)
{
  if (!cfg.trigger_handover_from_measurements) {
    logger.debug("ue={}: Ignoring better neighbor pci={}", ue_index, neighbor_pci);
    return;
  }
  handle_handover(ue_index, neighbor_gnb_id, neighbor_nci, neighbor_pci, neighbor_plmn, neighbor_tac);
}

void mobility_manager::handle_handover(cu_cp_ue_index_t     ue_index,
                                       gnb_id_t             neighbor_gnb_id,
                                       nr_cell_identity     neighbor_nci,
                                       pci_t                neighbor_pci,
                                       plmn_identity        neighbor_plmn,
                                       std::optional<tac_t> neighbor_tac)
{
  // Find the UE context.
  cu_cp_ue* u = ue_mng.find_du_ue(ue_index);
  if (u == nullptr) {
    logger.error("ue={}: Couldn't find UE", ue_index);
    return;
  }
  cu_cp_ue_context& ue_ctxt = u->get_ue_context();
  if (ue_ctxt.reconfiguration_disabled) {
    logger.debug("ue={}: MeasurementReport ignored. Cause: UE cannot be reconfigured", ue_index);
    return;
  }
  if (neighbor_pci == INVALID_PCI) {
    logger.error("ue={}: Ignoring Handover Request. Cause: Invalid target PCI {} received", ue_index, neighbor_pci);
    return;
  }

  // Handover is going ahead.

  // Disable new reconfigurations from now on (except for the Handover Command).
  ue_ctxt.reconfiguration_disabled = true;

  // Try to find target DU. If it is not found, it means that the target cell is not managed by this CU-CP and
  // an inter-CU handover is required.
  cu_cp_du_index_t target_du = du_db.find_du(neighbor_pci);
  if (target_du == cu_cp_du_index_t::invalid) {
    logger.debug("ue={}: Requesting inter CU handover. No local DU/cell with pci={} found", ue_index, neighbor_pci);
    if (!neighbor_tac.has_value()) {
      logger.error("ue={}: Cannot trigger inter-CU handover. Target TAC is required but not set", ue_index);
      return;
    }
    handle_inter_cu_handover(ue_index, neighbor_gnb_id, neighbor_plmn, neighbor_tac.value(), neighbor_nci);
    return;
  }

  cu_cp_du_index_t source_du = ue_mng.find_du_ue(ue_index)->get_du_index();

  if (source_du == target_du) {
    logger.info("ue={}: Trigger intra-CU (intra-DU) handover on du={}", ue_index, source_du);
  } else {
    logger.info("ue={}: Trigger intra-CU (inter-DU) handover from source_du={} to target_du={}",
                ue_index,
                source_du,
                target_du);
  }
  handle_intra_cu_handover(ue_index, neighbor_pci, source_du, target_du);
}

void mobility_manager::handle_intra_cu_handover(cu_cp_ue_index_t source_ue_index,
                                                pci_t            neighbor_pci,
                                                cu_cp_du_index_t source_du_index,
                                                cu_cp_du_index_t target_du_index)
{
  // Lookup CGI at target DU.
  std::optional<nr_cell_global_id_t> cgi =
      du_db.get_du_processor(target_du_index).get_mobility_handler().get_cgi(neighbor_pci);
  if (!cgi.has_value()) {
    logger.warning(
        "ue={}: Couldn't retrieve CGI for pci={} at du_index={}", source_ue_index, neighbor_pci, target_du_index);
    return;
  }

  cu_cp_intra_cu_handover_request request = {};
  request.source_ue_index                 = source_ue_index;
  request.target_pci                      = neighbor_pci;
  request.cgi                             = cgi.value();
  request.target_du_index                 = target_du_index;

  cu_cp_ue* u = ue_mng.find_du_ue(source_ue_index);
  if (u == nullptr) {
    logger.error("ue={}: Couldn't find UE", source_ue_index);
    return;
  }

  // Trigger Intra CU handover routine on the DU processor of the source DU.
  auto ho_trigger = [this, request, response = cu_cp_intra_cu_handover_response{}, &source_du_index, &target_du_index](
                        coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);
    CORO_AWAIT_VALUE(response, cu_cp_notifier.on_intra_cu_handover_required(request, source_du_index, target_du_index));
    CORO_RETURN();
  };
  u->get_task_sched().schedule_async_task(launch_async(std::move(ho_trigger)));
}

void mobility_manager::handle_inter_cu_handover(cu_cp_ue_index_t source_ue_index,
                                                gnb_id_t         target_gnb_id,
                                                plmn_identity    target_plmn,
                                                tac_t            target_tac,
                                                nr_cell_identity target_nci)
{
  cu_cp_ue* ue = ue_mng.find_du_ue(source_ue_index);
  if (ue == nullptr) {
    logger.error("ue={}: Couldn't find UE", source_ue_index);
    return;
  }

  cu_cp_ue_context& ue_ctxt = ue->get_ue_context();

  auto* ngap = ngap_db.find_ngap(ue_ctxt.plmn);
  if (ngap == nullptr) {
    logger.error("ue={}: Couldn't find NGAP", source_ue_index);
    return;
  }

  // Try to find target XN-C CU-CP peer. If it is not found, it means an NG handover is required.
  auto* xnap = xnap_db.find_xnap(target_gnb_id);
  if (xnap == nullptr) {
    logger.debug("ue={}: Requesting NG handover. No XN-C peer CU-CP peer with gnb_id={} found",
                 source_ue_index,
                 target_gnb_id.id);
    handle_ngap_handover(*ngap, *ue, target_gnb_id, target_plmn, target_tac, target_nci);
    return;
  }

  logger.debug("ue={}: Requesting XN handover for gnb_id={}", source_ue_index, target_gnb_id.id);
  handle_xnap_handover(*ngap, *xnap, *ue, ue_ctxt.plmn, target_nci);
}

void mobility_manager::handle_ngap_handover(ngap_interface&  ngap,
                                            cu_cp_ue&        ue,
                                            gnb_id_t         target_gnb_id,
                                            plmn_identity    target_plmn,
                                            tac_t            target_tac,
                                            nr_cell_identity target_nci)
{
  ngap_handover_preparation_request request =
      generate_ngap_handover_preparation_request(ue.get_ue_index(),
                                                 target_gnb_id,
                                                 target_plmn,
                                                 target_tac,
                                                 target_nci,
                                                 ue.get_up_resource_manager().get_pdu_sessions_map());

  // Send handover preparation request to the NGAP handler.
  auto ho_trigger = [&ngap, request, response = ngap_handover_preparation_response{}](
                        coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);
    CORO_AWAIT_VALUE(response, ngap.get_ngap_control_message_handler().handle_handover_preparation_request(request));
    CORO_RETURN();
  };
  ue.get_task_sched().schedule_async_task(launch_async(std::move(ho_trigger)));
}

void mobility_manager::handle_xnap_handover(ngap_interface&  ngap,
                                            xnap_interface&  xnap,
                                            cu_cp_ue&        ue,
                                            plmn_identity    plmn,
                                            nr_cell_identity target_nci)
{
  const ngap_context_t& ngap_ctxt = ngap.get_ngap_context();

  std::optional<guami_t> served_guami;
  for (const auto& guami : ngap_ctxt.served_guami_list) {
    if (guami.plmn == plmn) {
      served_guami = guami;
    }
  }
  if (!served_guami.has_value()) {
    logger.error("ue={}: Couldn't find GUAMI for {}", ue.get_ue_index(), plmn);
    return;
  }

  amf_ue_id_t source_amf_ue_id = ngap.get_amf_ue_id(ue.get_ue_index());
  if (source_amf_ue_id == amf_ue_id_t::invalid) {
    logger.error("ue={}: UE has invalid AMF UE ID", ue.get_ue_index());
    return;
  }

  xnap_handover_request request = generate_xnap_handover_request(
      ue.get_ue_index(),
      nr_cell_global_id_t{served_guami->plmn, target_nci},
      served_guami.value(),
      source_amf_ue_id,
      ue.get_ue_ambr(),
      ue.get_security_manager().get_security_context(),
      ue.get_up_resource_manager().get_pdu_sessions_map(),
      ue.get_rrc_ue()->get_rrc_ue_control_message_handler().get_packed_handover_preparation_message(),
      ue.get_location_manager().get_location_reporting_request());

  // Send handover preparation request to the XN-C handler.
  auto ho_trigger =
      [&xnap, request, response = xnap_handover_preparation_response{}](coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);
        CORO_AWAIT_VALUE(response, xnap.handle_handover_request_required(request));
        CORO_RETURN();
      };
  ue.get_task_sched().schedule_async_task(launch_async(std::move(ho_trigger)));
}
