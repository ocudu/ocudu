// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ue_creation_procedure.h"
#include "../converters/asn1_rrc_config_helpers.h"
#include "../converters/rlc_config_helpers.h"
#include "../converters/scheduler_configuration_helpers.h"
#include "../metrics/du_procedure_metrics_collector.h"
#include "ocudu/du/du_high/du_manager/du_manager_params.h"
#include "ocudu/rlc/rlc_factory.h"
#include "ocudu/rlc/rlc_srb_config_factory.h"

using namespace ocudu;
using namespace odu;

ue_creation_procedure::ue_creation_procedure(const du_ue_creation_request&   req_,
                                             du_ue_manager_repository&       ue_mng_,
                                             const du_manager_params&        du_params_,
                                             du_ran_resource_manager&        cell_res_alloc_,
                                             du_procedure_metrics_collector& metrics_) :
  req(req_),
  ue_mng(ue_mng_),
  du_params(du_params_),
  du_res_alloc(cell_res_alloc_),
  metrics(metrics_),
  proc_logger(ocudulog::fetch_basic_logger("DU-MNG"), name(), req.ue_index, req.tc_rnti)
{
  if (metrics.enabled()) {
    start_tp = std::chrono::steady_clock::now();
  }
}

void ue_creation_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  proc_logger.log_proc_started();

  // > Check if UE context was created in the DU manager.
  ue_ctx_creation_outcome = create_du_ue_context();
  if (not ue_ctx_creation_outcome.has_value()) {
    proc_logger.log_proc_failure("Failed to create DU UE context. Cause: {}", ue_ctx_creation_outcome.error().data());
    CORO_AWAIT(clear_ue());
    CORO_EARLY_RETURN();
  }

  ue_ctx = ue_ctx_creation_outcome.value();

  // > Initialize bearers and PHY/MAC PCell resources of the DU UE.
  if (not setup_du_ue_resources()) {
    CORO_AWAIT(clear_ue());
    CORO_EARLY_RETURN();
  }

  // > Create F1AP UE context.
  f1ap_resp = create_f1ap_ue();
  if (not f1ap_resp.result) {
    proc_logger.log_proc_failure("Failed to create F1AP UE context");
    CORO_AWAIT(clear_ue());
    CORO_EARLY_RETURN();
  }

  // > Initiate creation of RLC SRB0.
  create_rlc_srbs();

  // > Connect bearers between layers F1AP<->RLC<->MAC.
  connect_layer_bearers();

  // > Initiate MAC UE creation and await result.
  CORO_AWAIT_VALUE(mac_resp, create_mac_ue());
  if (mac_resp.allocated_crnti == rnti_t::INVALID_RNTI) {
    proc_logger.log_proc_failure("Failed to create MAC UE context");
    CORO_AWAIT(clear_ue());
    CORO_EARLY_RETURN();
  }

  // > Assign C-RNTI allocated by MAC.
  ue_mng.update_crnti(req.ue_index, mac_resp.allocated_crnti);

  // > Start Initial UL RRC Message Transfer by signalling MAC to notify CCCH to upper layers.
  if (not req.ul_ccch_msg.empty()) {
    if (not du_params.mac.mgr.get_ue_configurator().handle_ul_ccch_msg(ue_ctx->ue_index, req.ul_ccch_msg.copy())) {
      proc_logger.log_proc_failure("Failed to notify CCCH message to upper layers");
      CORO_AWAIT(clear_ue());
      CORO_EARLY_RETURN();
    }
  }

  proc_logger.log_proc_completed();
  if (metrics.enabled()) {
    metrics.on_ue_creation_completion(std::chrono::steady_clock::now() - start_tp);
  }
  CORO_RETURN();
}

expected<du_ue*, std::string> ue_creation_procedure::create_du_ue_context() const
{
  // Fetch the DU cell configuration of the primary cell UE is connected to.
  // TODO: Use cell manager
  const auto& cell_cfg = du_params.ran.cells[req.pcell_index];
  if (not cell_cfg.enabled) {
    return make_unexpected("UE created in inactive cell");
  }

  // Create a DU UE resource manager, which will be responsible for managing bearer and PUCCH resources.
  auto alloc_result =
      du_res_alloc.create_ue_resource_configurator(req.ue_index, req.pcell_index, req.tc_rnti != rnti_t::INVALID_RNTI);
  if (not alloc_result.has_value()) {
    // The UE resource manager could not create a new UE entry.
    return make_unexpected(alloc_result.error());
  }

  // Create the DU UE context.
  return ue_mng.add_ue(du_ue_context(req.ue_index, req.pcell_index, req.tc_rnti, cell_cfg.nr_cgi),
                       std::move(alloc_result.value()));
}

async_task<void> ue_creation_procedure::clear_ue()
{
  return launch_async([this](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    if (ue_ctx == nullptr) {
      // No UE context to clear.
      CORO_EARLY_RETURN();
    }

    CORO_AWAIT(ue_ctx->handle_activity_stop_request(true));

    if (f1ap_resp.result) {
      du_params.f1ap.ue_mng.handle_ue_deletion_request(req.ue_index);
    }

    if (mac_resp.allocated_crnti != rnti_t::INVALID_RNTI) {
      CORO_AWAIT(du_params.mac.mgr.get_ue_configurator().handle_ue_delete_request(
          mac_ue_delete_request{req.pcell_index, req.ue_index, mac_resp.allocated_crnti}));
    }

    // Clear UE from DU Manager UE repository.
    ue_mng.remove_ue(ue_ctx->ue_index);

    CORO_RETURN();
  });
}

bool ue_creation_procedure::setup_du_ue_resources()
{
  // SRB0 is always created.
  ue_ctx->bearers.add_srb(srb_id_t::srb0);

  if (ue_ctx->resources.resource_alloc_failed()) {
    // In special case, where the RAN resources were not allocated successfully for the UE (e.g. lack of PUCCH space),
    // only SRB0 is added, which will be used for RRC Reject.
    return true;
  }

  // Allocate PCell PHY/MAC resources for DU UE.
  f1ap_ue_context_update_request f1_req;
  f1_req.ue_index = ue_ctx->ue_index;
  f1_req.srbs_to_setup.resize(1);
  f1_req.srbs_to_setup[0]                   = srb_id_t::srb1;
  const du_ue_resource_update_response resp = ue_ctx->resources.update(req.pcell_index, f1_req);
  if (resp.failed()) {
    proc_logger.log_proc_failure("Unable to setup DU UE PCell and SRB resources. Cause: {}",
                                 resp.procedure_error.error().data());
    return false;
  }

  // Create DU UE SRB1.
  ue_ctx->bearers.add_srb(srb_id_t::srb1);

  return true;
}

void ue_creation_procedure::create_rlc_srbs()
{
  // Create SRB0 RLC entity.
  du_ue_srb& srb0 = ue_ctx->bearers.srbs()[srb_id_t::srb0];
  srb0.rlc_bearer = create_rlc_entity(make_rlc_entity_creation_message(du_params.ran.gnb_du_id,
                                                                       ue_ctx->ue_index,
                                                                       ue_ctx->pcell_index,
                                                                       srb0,
                                                                       make_default_srb0_rlc_config(),
                                                                       du_params.services,
                                                                       ue_ctx->get_rlc_rlf_notifier(),
                                                                       du_params.rlc.pcap_writer));

  // Create SRB1 RLC entity.
  if (ue_ctx->bearers.srbs().contains(srb_id_t::srb1)) {
    du_ue_srb& srb1 = ue_ctx->bearers.srbs()[srb_id_t::srb1];
    srb1.rlc_bearer =
        create_rlc_entity(make_rlc_entity_creation_message(du_params.ran.gnb_du_id,
                                                           ue_ctx->ue_index,
                                                           ue_ctx->pcell_index,
                                                           srb1,
                                                           ue_ctx->resources->srbs[srb_id_t::srb1].rlc_cfg,
                                                           du_params.services,
                                                           ue_ctx->get_rlc_rlf_notifier(),
                                                           du_params.rlc.pcap_writer));
  }
}

async_task<mac_ue_create_response> ue_creation_procedure::create_mac_ue()
{
  // Create Request to MAC to create new UE.
  mac_ue_create_request mac_ue_create_msg{};
  mac_ue_create_msg.ue_index           = ue_ctx->ue_index;
  mac_ue_create_msg.crnti              = req.tc_rnti;
  mac_ue_create_msg.cell_index         = req.pcell_index;
  mac_ue_create_msg.mac_cell_group_cfg = ue_ctx->resources->cell_group.mcg_cfg;
  mac_ue_create_msg.phy_cell_group_cfg = ue_ctx->resources->cell_group.pcg_cfg;
  mac_ue_create_msg.rlf_notifier       = &ue_ctx->get_mac_rlf_notifier();
  for (du_ue_srb& bearer : ue_ctx->bearers.srbs()) {
    mac_ue_create_msg.bearers.emplace_back();
    mac_logical_channel_config& lc = mac_ue_create_msg.bearers.back();
    lc.lcid                        = bearer.lcid();
    lc.ul_bearer                   = &bearer.connector.mac_rx_sdu_notifier;
    lc.dl_bearer                   = &bearer.connector.mac_tx_sdu_notifier;
  }
  for (auto& bearer : ue_ctx->bearers.drbs()) {
    mac_ue_create_msg.bearers.emplace_back();
    mac_logical_channel_config& lc = mac_ue_create_msg.bearers.back();
    lc.lcid                        = bearer.second->lcid;
    lc.ul_bearer                   = &bearer.second->connector.mac_rx_sdu_notifier;
    lc.dl_bearer                   = &bearer.second->connector.mac_tx_sdu_notifier;
  }
  mac_ue_create_msg.ul_ccch_msg     = not req.ul_ccch_msg.empty() ? &req.ul_ccch_msg : nullptr;
  mac_ue_create_msg.ul_ccch_slot_rx = req.slot_rx;
  if (ue_ctx->resources->cfra.has_value()) {
    mac_ue_create_msg.cfra_preamble_index = ue_ctx->resources->cfra->preamble_id;
  }

  // Derive the number of DL HARQ processes configured for the UE on its PCell.
  const auto& ue_pcell = ue_ctx->resources->cell_group.cells.at(SERVING_PCELL_IDX);
  const auto& cell_ran = du_params.ran.cells[req.pcell_index].ran;
  if (not ue_ctx->resources.resource_alloc_failed() and ue_pcell.serv_cell_cfg.pdsch_serv_cell_cfg.has_value()) {
    // For NTN cells, pre-size the MAC DL HARQ buffer pool to the cell maximum. UE capabilities
    // are unknown at creation time, so nof_harq_proc is capped at 16; after capability exchange
    // the scheduler HARQ entity is extended up to max_harq_procs (e.g. 32). The MAC buffer must
    // already be large enough at that point to avoid an out-of-bounds assertion.
    mac_ue_create_msg.pdsch_harqs_per_cell =
        cell_ran.ntn_params.has_value() and cell_ran.ntn_params->is_enabled()
            ? cell_ran.init_bwp.pdsch.max_harq_procs
            : static_cast<unsigned>(ue_pcell.serv_cell_cfg.pdsch_serv_cell_cfg->nof_harq_proc);
  } else {
    // UE is going to be RRC Rejected. We only need one HARQ in such scenario.
    mac_ue_create_msg.pdsch_harqs_per_cell = 1;
  }

  // Create Scheduler UE Config Request that will be embedded in the mac UE creation request.
  mac_ue_create_msg.sched_cfg = create_scheduler_ue_config_request(*ue_ctx, *ue_ctx->resources);

  // Request MAC to create new UE.
  return du_params.mac.mgr.get_ue_configurator().handle_ue_create_request(mac_ue_create_msg);
}

f1ap_ue_creation_response ue_creation_procedure::create_f1ap_ue()
{
  using namespace asn1::rrc_nr;

  f1ap_ue_creation_request f1ap_msg{};
  f1ap_msg.ue_index    = ue_ctx->ue_index;
  f1ap_msg.c_rnti      = ue_ctx->rnti;
  f1ap_msg.pcell_index = ue_ctx->pcell_index;

  // Create SRBs.
  f1ap_msg.f1c_bearers_to_add.reserve(ue_ctx->bearers.srbs().size());
  for (du_ue_srb& srb : ue_ctx->bearers.srbs()) {
    f1c_bearer_to_addmod& f1c_bearer = f1ap_msg.f1c_bearers_to_add.emplace_back();
    f1c_bearer.srb_id                = srb.srb_id;
    f1c_bearer.rx_sdu_notifier       = &srb.connector.f1c_rx_sdu_notif;
  }

  cell_group_cfg_s cell_group;
  if (ue_ctx->bearers.srbs().contains(srb_id_t::srb1)) {
    // If allocation of UE RAN resources was successful, the cellGroupConfig is not empty.
    // Pack SRB1 configuration that is going to be passed in the F1AP DU-to-CU-RRC-Container IE to the CU as per
    // TS38.473, Section 8.4.1.2.
    calculate_cell_group_config_diff(cell_group, {}, ue_ctx->resources.value());

    asn1::bit_ref       bref{f1ap_msg.du_cu_rrc_container};
    asn1::OCUDUASN_CODE result = cell_group.pack(bref);
    ocudu_assert(result == asn1::OCUDUASN_SUCCESS, "Failed to generate CellConfigGroup");
  }

  return du_params.f1ap.ue_mng.handle_ue_creation_request(f1ap_msg);
}

void ue_creation_procedure::connect_layer_bearers()
{
  // Connect SRB0 bearer layers.
  du_ue_srb& srb0 = ue_ctx->bearers.srbs()[srb_id_t::srb0];
  srb0.connector.connect(ue_ctx->ue_index,
                         srb_id_t::srb0,
                         *f1ap_resp.f1c_bearers_added[0],
                         *srb0.rlc_bearer,
                         du_params.rlc.mac_ue_info_handler);

  // Connect SRB1 bearer layers.
  if (ue_ctx->bearers.srbs().contains(srb_id_t::srb1)) {
    du_ue_srb& srb1 = ue_ctx->bearers.srbs()[srb_id_t::srb1];
    srb1.connector.connect(ue_ctx->ue_index,
                           srb_id_t::srb1,
                           *f1ap_resp.f1c_bearers_added[1],
                           *srb1.rlc_bearer,
                           du_params.rlc.mac_ue_info_handler);
  }
}
