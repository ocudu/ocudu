// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_processor_impl.h"
#include "ocudu/adt/expected.h"
#include "ocudu/adt/format.h"
#include "ocudu/cu_cp/cu_cp_ref_time_report_notifier.h"
#include "ocudu/f1ap/cu_cp/f1ap_cu_factory.h"
#include "ocudu/ran/cause/f1ap_cause.h"
#include "ocudu/ran/cause/f1ap_cause_converters.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/rrc/rrc_du_factory.h"
#include "ocudu/support/async/coroutine.h"
#include "ocudu/support/cpu_architecture_info.h"

using namespace ocudu;
using namespace ocucp;

class du_processor_impl::f1ap_du_processor_adapter : public f1ap_du_processor_notifier
{
public:
  f1ap_du_processor_adapter(du_processor_impl& parent_, async_task_scheduler& common_task_sched_) :
    parent(parent_), common_task_sched(&common_task_sched_)
  {
  }

  du_setup_result on_new_du_setup_request(const du_setup_request& msg) override
  {
    return parent.handle_du_setup_request(msg);
  }

  cu_cp_ue_index_t request_new_ue_creation() override { return parent.ue_mng.add_ue(parent.cfg.du_index); }

  ue_rrc_context_creation_outcome
  on_ue_rrc_context_creation_request(const ue_rrc_context_creation_request& req) override
  {
    return parent.handle_ue_rrc_context_creation_request(req);
  }

  void on_du_initiated_ue_context_release_request(const f1ap_ue_context_release_request& req) override
  {
    parent.handle_du_initiated_ue_context_release_request(req);
  }

  void on_access_success(const f1ap_access_success& msg) override { parent.handle_access_success(msg); }

  bool schedule_async_task(async_task<void> task) override { return common_task_sched->schedule(std::move(task)); }

  async_task<void> on_transaction_info_loss(const ue_transaction_info_loss_event& ev) override
  {
    return parent.cu_cp_notifier.on_transaction_info_loss(ev);
  }

  void on_ref_time_info_report(const f1ap_time_ref_info& info) override
  {
    std::optional<std::chrono::system_clock::time_point> time_point =
        parent.rrc->get_ref_time_r16(info.ref_time_r16, info.is_local_clock);
    if (not time_point) {
      parent.logger.warning("du={}: Failed to unpack Reference Time Information Report", parent.cfg.du_index);
      return;
    }

    parent.logger.debug("du={}: Received Reference Time Information Report: sfn={} time={:%T} is_local_clock={}",
                        parent.cfg.du_index,
                        info.ref_slot.sfn(),
                        *time_point,
                        info.is_local_clock);

    cu_cp_ref_time_report_notifier& notifier = parent.ref_time_report_notifier;
    const du_configuration_context* du_ctx   = parent.get_context();
    if (du_ctx == nullptr) {
      return;
    }
    std::vector<nr_cell_global_id_t> served_cells;
    served_cells.reserve(du_ctx->served_cells.size());
    for (const du_cell_configuration& cell : du_ctx->served_cells) {
      served_cells.push_back(cell.cgi);
    }
    notifier.on_ref_time_info_report(
        served_cells, cu_cp_ref_time_report{info.ref_slot, *time_point, info.uncertainty, info.is_local_clock});
  }

private:
  du_processor_impl&    parent;
  async_task_scheduler* common_task_sched = nullptr;
};

// du_processor_impl

du_processor_impl::du_processor_impl(const du_processor_config& cfg_, du_processor_dependencies dependencies) :
  cfg(cfg_),
  du_setup_notif(dependencies.du_setup_notif),
  du_cfg_hdlr(std::move(dependencies.du_cfg_hdlr)),
  cu_cp_notifier(dependencies.cu_cp_notifier),
  f1ap_pdu_notifier(dependencies.f1ap_pdu_notifier),
  ue_mng(dependencies.ue_mng),
  ref_time_report_notifier(dependencies.ref_time_report_notifier),
  logger(dependencies.logger),
  f1ap_ev_notifier(std::make_unique<f1ap_du_processor_adapter>(*this, dependencies.common_task_sched))
{
  // Create F1AP.
  f1ap = create_f1ap(cfg.f1ap, f1ap_pdu_notifier, *f1ap_ev_notifier, dependencies.timers, dependencies.cu_cp_executor);

  // Create RRC DU.
  rrc = create_rrc_du(rrc_cfg_t{.gnb_id                         = cfg.gnb_id,
                                .srb2_cfg                       = cfg.srb2_cfg,
                                .drb_config                     = cfg.drb_config,
                                .int_algo_pref_list             = cfg.int_algo_pref_list,
                                .enc_algo_pref_list             = cfg.enc_algo_pref_list,
                                .force_reestablishment_fallback = cfg.force_reestablishment_fallback,
                                .force_resume_fallback          = cfg.force_resume_fallback,
                                .rrc_procedure_guard_time_ms    = cfg.rrc_procedure_guard_time_ms,
                                .rrc_reject_wait_time           = cfg.rrc_reject_wait_time});
}

du_setup_result du_processor_impl::handle_du_setup_request(const du_setup_request& request)
{
  du_setup_result res;

  // Extract cell info from served cell list.
  // TODO: How to handle missing optional freq and timing in meas timing config?
  std::map<nr_cell_global_id_t, rrc_cell_info> cell_info_db = rrc->get_cell_info(request.gnb_du_served_cells_list);
  if (cell_info_db.empty()) {
    res.result = du_setup_result::rejected{f1ap_cause_transport_t::unspecified, "Could not extract cell info from DU"};
    return res;
  }

  // Collect PLMN IDs and cell meas config of all served cells.
  std::set<plmn_identity>                              plmn_ids;
  std::map<nr_cell_identity, serving_cell_meas_config> meas_config_db;
  for (const auto& [cgi, cell_info] : cell_info_db) {
    for (const auto& plmn : cell_info.plmn_identity_list) {
      plmn_ids.insert(plmn);
    }

    // Fill cell meas config.
    serving_cell_meas_config meas_cfg;
    meas_cfg.nci               = cgi.nci;
    meas_cfg.gnb_id_bit_length = cfg.gnb_id.bit_length;
    meas_cfg.plmn              = cgi.plmn_id;
    meas_cfg.pci               = cell_info.nr_pci;
    meas_cfg.band              = cell_info.band;
    if (!cell_info.meas_timings.empty() && cell_info.meas_timings.begin()->freq_and_timing.has_value()) {
      // TODO: which meas timing to use when multiple are present?
      const auto& freq_timing = cell_info.meas_timings.begin()->freq_and_timing.value();
      meas_cfg.ssb_mtc        = freq_timing.ssb_meas_timing_cfg;
      meas_cfg.ssb_arfcn      = freq_timing.carrier_freq;
      meas_cfg.ssb_scs        = freq_timing.ssb_subcarrier_spacing;
    }

    meas_config_db.emplace(cgi.nci, meas_cfg);
  }

  // Check if CU-CP can accept a new DU connection.
  if (!du_setup_notif.on_du_setup_request(cfg.du_index, plmn_ids)) {
    res.result = du_setup_result::rejected{f1ap_cause_radio_network_t::plmn_not_served_by_the_gnb_cu,
                                           "One or more PLMNs are not served by the GNB CU-CP"};
    return res;
  }

  // Validate and update DU configuration.
  auto cfg_res = du_cfg_hdlr->handle_new_du_config(request);
  if (!cfg_res.has_value()) {
    res.result = cfg_res.error();
    return res;
  }

  // Update cell config in cell measurement manager.
  for (const auto& [nci, meas_config] : meas_config_db) {
    if (!cu_cp_notifier.on_cell_config_update_request(nci, meas_config)) {
      res.result =
          du_setup_result::rejected{f1ap_cause_transport_t::unspecified, "Could not update cell measurement config"};
      return res;
    }
  }

  // Store cell info in RRC DU.
  rrc->store_cell_info_db(cell_info_db);

  // Notify the CU-CP that the cells served by this DU changed.
  cu_cp_notifier.on_served_cells_updated();

  // Realize the reported cells as CU-CP logical cells and let the CU-CP decide, per cell, whether it may be
  // activated (admin-locked cells stay dormant).
  std::vector<du_reported_cell> reported_cells;
  reported_cells.reserve(request.gnb_du_served_cells_list.size());
  for (const auto& served_cell : request.gnb_du_served_cells_list) {
    reported_cells.push_back({served_cell.served_cell_info.nr_cgi, served_cell.served_cell_info.nr_pci});
  }
  std::vector<bool> cells_to_activate = cu_cp_notifier.on_du_cells_reported(cfg.du_index, reported_cells);
  ocudu_assert(cells_to_activate.size() == reported_cells.size(),
               "One activation decision per reported cell is required");

  // Record the dormant (admin-locked) cells as deactivated in the DU configuration records, reusing the
  // same bookkeeping as a command deactivation, so lifecycle lookups treat them exactly like
  // command-deactivated cells (e.g. the unlock command finds them via the any-state lookup). No F1AP
  // message is sent here: the update struct is only the vehicle for the configuration handler's records.
  {
    f1ap_gnb_cu_configuration_update dormant_cells_update;
    for (unsigned i = 0; i != reported_cells.size(); ++i) {
      if (!cells_to_activate[i]) {
        dormant_cells_update.cells_to_be_deactivated_list.push_back({reported_cells[i].cgi});
      }
    }
    if (!dormant_cells_update.cells_to_be_deactivated_list.empty()) {
      du_cfg_hdlr->handle_gnb_cu_configuration_update(dormant_cells_update);
    }
  }

  // Prepare DU response with accepted setup.
  auto& accepted              = res.result.emplace<du_setup_result::accepted>();
  accepted.gnb_cu_name        = cfg.ran_node_name;
  accepted.gnb_cu_rrc_version = cfg.rrc_version;

  // Accept all cells; activate the ones not administratively locked. Cells omitted from the Cells to be
  // Activated List remain configured-but-dormant at the DU, and can be activated later via the gNB-CU
  // Configuration Update procedure (unlock command).
  accepted.cells_to_be_activ_list.reserve(request.gnb_du_served_cells_list.size());
  for (unsigned i = 0; i != request.gnb_du_served_cells_list.size(); ++i) {
    if (!cells_to_activate[i]) {
      continue;
    }
    auto& activ_item  = accepted.cells_to_be_activ_list.emplace_back();
    activ_item.nr_cgi = request.gnb_du_served_cells_list[i].served_cell_info.nr_cgi;
    activ_item.nr_pci = request.gnb_du_served_cells_list[i].served_cell_info.nr_pci;
  }

  return res;
}

bool du_processor_impl::create_rrc_ue(cu_cp_ue&                              ue,
                                      rnti_t                                 c_rnti,
                                      const nr_cell_global_id_t&             cgi,
                                      byte_buffer                            du_to_cu_rrc_container,
                                      std::optional<rrc_ue_transfer_context> rrc_context,
                                      std::optional<rrc_resume_context_t>    remote_resume_context)
{
  const cu_cp_ue_index_t ue_index = ue.get_ue_index();

  // Create RRC UE to F1AP adapter (DL path).
  rrc_ue_f1ap_adapters.emplace(std::piecewise_construct,
                               std::forward_as_tuple(ue_index),
                               std::forward_as_tuple(f1ap->get_f1ap_rrc_message_handler(), ue_index));

  // Create per-UE SRB PDCP context.
  uint32_t nof_cores = cpu_architecture_info::get().get_host_nof_available_cpus();
  srb_pdcp_contexts.emplace(std::piecewise_construct,
                            std::forward_as_tuple(ue_index),
                            std::forward_as_tuple(ue_index,
                                                  ue.get_rrc_ue_cu_cp_ue_notifier().get_timer_factory(),
                                                  ue.get_rrc_ue_cu_cp_ue_notifier().get_executor(),
                                                  nof_cores));

  const du_cell_configuration& cell = *du_cfg_hdlr->get_context().find_cell(cgi);

  // Create new RRC UE entity.
  rrc_ue_creation_message rrc_ue_create_msg{};
  rrc_ue_create_msg.ue_index              = ue_index;
  rrc_ue_create_msg.c_rnti                = c_rnti;
  rrc_ue_create_msg.cell.cgi              = cgi;
  rrc_ue_create_msg.cell.tac              = cell.tac;
  rrc_ue_create_msg.cell.tac_list         = cell.tac_list;
  rrc_ue_create_msg.cell.pci              = cell.pci;
  rrc_ue_create_msg.cell.bands            = cell.bands;
  rrc_ue_create_msg.f1ap_pdu_notifier     = &rrc_ue_f1ap_adapters.at(ue_index);
  rrc_ue_create_msg.ngap_notifier         = &ue.get_rrc_ue_ngap_adapter();
  rrc_ue_create_msg.rrc_ue_cu_cp_notifier = &ue.get_rrc_ue_context_update_notifier();
  rrc_ue_create_msg.measurement_notifier  = &ue.get_rrc_ue_measurement_notifier();
  rrc_ue_create_msg.cu_cp_ue_notifier     = &ue.get_rrc_ue_cu_cp_ue_notifier();
  rrc_ue_create_msg.pdcp_manager          = &srb_pdcp_contexts.at(ue_index);
  rrc_ue_create_msg.du_to_cu_container    = std::move(du_to_cu_rrc_container);
  rrc_ue_create_msg.rrc_context           = std::move(rrc_context);
  rrc_ue_create_msg.remote_resume_context = std::move(remote_resume_context);
  auto* rrc_ue                            = rrc->add_ue(rrc_ue_create_msg);
  if (rrc_ue == nullptr) {
    logger.warning("Could not create RRC UE");
    pdcp_removal.remove_ue_context(ue_index);
    remove_ue_context(ue_index);
    return false;
  }

  // Connect PDCP to RRC for the UL SDU delivery path.
  srb_pdcp_contexts.at(ue_index).connect_rrc_ue(rrc_ue->get_ul_pdu_handler(), [rrc_ue](ngap_cause_t cause) {
    rrc_ue->get_controller().on_ue_release_required(cause);
  });

  // Connect F1AP to PDCP (SRB1/SRB2) and F1AP to RRC (SRB0) adapters.
  f1ap_pdcp_dcch_adapters[ue_index] = {};
  f1ap_rrc_ccch_adapters[ue_index]  = {};
  f1ap_rrc_ccch_adapters.at(ue_index).connect_rrc_ue(rrc_ue->get_ul_pdu_handler());
  f1ap_pdcp_dcch_adapters.at(ue_index).connect_pdcp(srb_pdcp_contexts.at(ue_index));

  // Notify CU-CP about the creation of the RRC UE.
  cu_cp_notifier.on_rrc_ue_created(ue_index, *rrc_ue);

  return true;
}

ue_rrc_context_creation_outcome
du_processor_impl::handle_ue_rrc_context_creation_request(const ue_rrc_context_creation_request& req)
{
  ocudu_assert(req.c_rnti != rnti_t::INVALID_RNTI, "ue={} c-rnti={}: Invalid C-RNTI", req.ue_index, req.c_rnti);

  // Lambda to release the UE context in case of any failure during the creation procedure.
  auto release_ue = [this](cu_cp_ue_index_t ue_index) {
    cu_cp_ue_context_release_request release_request;
    release_request.ue_index = ue_index;
    release_request.cause    = ngap_cause_radio_network_t::radio_res_not_available;

    cu_cp_ue* ue = ue_mng.find_ue(ue_index);
    if (ue == nullptr) {
      logger.warning("ue={}: UE to release not found", ue_index);
      return;
    }

    // Schedule on UE task scheduler.
    ue->get_task_sched().schedule_async_task(
        launch_async([this, release_request](coro_context<async_task<void>>& ctx) mutable {
          CORO_BEGIN(ctx);
          CORO_AWAIT(cu_cp_notifier.on_ue_release_required(release_request));
          CORO_RETURN();
        }));
  };

  bool is_resume_request = false;

  cu_cp_ue* ue = nullptr;

  // Resume identity whose I-RNTI matched no local UE. Handed to the RRC UE, which asks the peer that allocated it
  // for the context.
  std::optional<rrc_resume_context_t> remote_resume_context;

  // Check if this is a RRC Resume request for an existing UE.
  if (not req.rrc_container.empty()) {
    std::optional<rrc_resume_context_t> resume_context = rrc->get_rrc_resume_context(req.rrc_container.copy());
    if (!resume_context.has_value()) {
      logger.warning("ue={}: Could not extract RRC Resume context from UL CCCH Message", req.ue_index);
      // Schedule UE context release and return error response.
      release_ue(req.ue_index);
      return make_unexpected(default_error_t{});
    }

    if (resume_context->is_resume && resume_context->rrc_resume_id.has_value()) {
      cu_cp_ue_index_t resume_ue_index;
      if (std::holds_alternative<short_i_rnti_t>(resume_context->rrc_resume_id.value())) {
        resume_ue_index = ue_mng.get_ue_index(std::get<short_i_rnti_t>(resume_context->rrc_resume_id.value()));
        logger.debug("ue={}: RRC Resume Request with {}",
                     resume_ue_index,
                     std::get<short_i_rnti_t>(resume_context->rrc_resume_id.value()));
      } else {
        resume_ue_index = ue_mng.get_ue_index(std::get<full_i_rnti_t>(resume_context->rrc_resume_id.value()));
        logger.debug("ue={}: RRC Resume Request with {}",
                     resume_ue_index,
                     std::get<full_i_rnti_t>(resume_context->rrc_resume_id.value()));
      }

      if (resume_ue_index == cu_cp_ue_index_t::invalid) {
        // The I-RNTI matched no local UE, so the node that suspended it holds the context.
        remote_resume_context = resume_context;
      } else {
        if (cfg.force_resume_fallback) {
          // RRC Resume fallback forced - do not resume. The DU doesn't have a F1AP UE context, so we also remove it
          // here.
          logger.info("ue={}: RRC Resume fallback forced. Removing F1AP UE context", resume_ue_index);
          f1ap->get_f1ap_ue_context_removal_handler().remove_ue_context(resume_ue_index);
        } else {
          ue = ue_mng.find_du_ue(resume_ue_index);
          ue_mng.set_active(resume_ue_index);
          is_resume_request = true;

          // Remove the new UE context that was created for this request since it's a resume request, and we should
          // reuse the existing context.
          logger.debug("ue={}: RRC Resume detected, removing newly created UE context", req.ue_index);
          ue_mng.remove_ue(req.ue_index);
          f1ap->get_f1ap_ue_context_removal_handler().remove_ue_context(req.ue_index);
        }
      }
    }
  }

  if (ue == nullptr) {
    // RRC Resume not requested or failed - update UE context.

    // Check that UE can be served by this CU.
    if (ue_mng.ue_admission_limit_reached()) {
      logger.warning("ue={}: UE admission limit reached", req.ue_index);
      // Update the RRC connection establishment attempt cause as unknown, since the real cause isn't known at this
      // stage
      rrc->handle_attempted_rrc_setup(establishment_cause_t::unknown);
      // Update the RRC connection establishment fail cause
      rrc->handle_failed_rrc_connection_establishment(establishment_fail_cause_t::network_reject);
      // Schedule UE context release and return error response.
      release_ue(req.ue_index);
      return make_unexpected(default_error_t{});
    }

    // Check that creation message is valid.
    const du_cell_configuration* pcell = du_cfg_hdlr->get_context().find_cell(req.cgi);
    if (pcell == nullptr) {
      logger.warning("ue={} c-rnti={}: Could not find cell with nci={}", req.ue_index, req.c_rnti, req.cgi.nci);
      // Schedule UE context release and return error response.
      release_ue(req.ue_index);
      return make_unexpected(default_error_t{});
    }
    const pci_t pci = pcell->pci;

    if (!ue_mng.update_ue_context(req.ue_index, du_cfg_hdlr->get_context().id, pci, req.c_rnti, pcell->cell_index)) {
      logger.warning("ue={}: Could not update UE context", req.ue_index);
      // Schedule UE context release and return error response.
      release_ue(req.ue_index);
      return make_unexpected(default_error_t{});
    }

    ue = ue_mng.find_ue(req.ue_index);
  }

  // If this is not a RRCResume, create an RRC UE. If the DU-to-CU-RRC-Container is empty, the UE will be rejected.
  if (not is_resume_request) {
    if (ue == nullptr) {
      logger.warning("ue={}: Could not find UE after updating context", req.ue_index);
      return make_unexpected(default_error_t{});
    }

    if (not create_rrc_ue(
            *ue, req.c_rnti, req.cgi, req.du_to_cu_rrc_container.copy(), req.prev_context, remote_resume_context)) {
      logger.warning("ue={}: Could not create RRC UE object", ue->get_ue_index());
      // Schedule UE context release and return error response.
      release_ue(ue->get_ue_index());
      return make_unexpected(default_error_t{});
    }
  }

  // Signal back that the UE was successfully created.
  logger.info(
      "ue={} c-rnti={}: UE created{}", ue->get_ue_index(), req.c_rnti, is_resume_request ? " (RRC Resume)" : "");

  return ue_rrc_context_creation_response{ue->get_ue_index(),
                                          &f1ap_rrc_ccch_adapters.at(ue->get_ue_index()),
                                          &f1ap_pdcp_dcch_adapters.at(ue->get_ue_index()).get_srb1_notifier(),
                                          &f1ap_pdcp_dcch_adapters.at(ue->get_ue_index()).get_srb2_notifier()};
}

void du_processor_impl::handle_du_initiated_ue_context_release_request(const f1ap_ue_context_release_request& request)
{
  ocudu_assert(request.ue_index != cu_cp_ue_index_t::invalid, "Invalid UE index", request.ue_index);

  cu_cp_ue* ue = ue_mng.find_du_ue(request.ue_index);
  if (ue == nullptr) {
    logger.warning("ue={}: Dropping DU initiated UE context release request. UE does not exist", request.ue_index);
    return;
  }

  logger.debug("ue={}: Handling DU initiated UE context release request", request.ue_index);

  // The DU requested a UE release, so we cancel all ongoing RRC transactions for the UE.
  auto* rrc_ue = ue->get_rrc_ue();
  if (rrc_ue == nullptr) {
    logger.warning("ue={}: Dropping DU initiated UE context release request. RRC UE does not exist", request.ue_index);
    return;
  }
  rrc_ue->cancel_all_transactions();

  // Schedule on UE task scheduler.
  ue->get_task_sched().schedule_async_task(
      launch_async([this, request, ue](coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);

        CORO_AWAIT(cu_cp_notifier.on_ue_release_required(
            {request.ue_index, ue->get_up_resource_manager().get_pdu_sessions(), f1ap_to_ngap_cause(request.cause)}));
        CORO_RETURN();
      }));
}

void du_processor_impl::handle_access_success(const f1ap_access_success& msg)
{
  logger.debug("ue={}: Received Access Success notification from DU for cell plmn={} nci={}",
               msg.ue_index,
               msg.cgi.plmn_id,
               msg.cgi.nci);

  cu_cp_ue* ue = ue_mng.find_du_ue(msg.ue_index);
  if (ue == nullptr) {
    logger.warning("ue={}: Dropping Access Success notification. UE does not exist", msg.ue_index);
    return;
  }

  cu_cp_access_success_indication ind;
  ind.ue_index = msg.ue_index;
  ind.cgi      = msg.cgi;

  // Resolve source UE via CHO backlink so the caller can schedule the source routine on the source UE's scheduler.
  if (ue->get_cho_context().has_value() && ue->get_cho_context()->role == cu_cp_ue_cho_context::role_t::target &&
      ue->get_cho_context()->source_ue_index != cu_cp_ue_index_t::invalid) {
    ind.source_ue_index = ue->get_cho_context()->source_ue_index;
  }

  cu_cp_ue* source_ue = ue_mng.find_du_ue(ind.source_ue_index);
  if (source_ue == nullptr) {
    // For inter-CU CHO the source UE lives on a remote CU-CP; Access Success is not needed locally since
    // the target execution routine awaits RRCReconfigurationComplete instead.
    if (ind.source_ue_index == cu_cp_ue_index_t::invalid) {
      logger.debug("ue={}: Ignoring Access Success notification. No local source UE (inter-CU CHO target)",
                   msg.ue_index);
      return;
    }
    logger.warning("ue={}: Dropping Access Success notification. Source UE does not exist", msg.ue_index);
    return;
  }

  source_ue->get_task_sched().schedule_async_task(
      launch_async([this, ind](coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);
        CORO_AWAIT(cu_cp_notifier.on_access_success(ind));
        CORO_RETURN();
      }));
}

bool du_processor_impl::has_cell(pci_t pci)
{
  return du_cfg_hdlr->get_context().find_cell(pci) != nullptr;
}

bool du_processor_impl::has_cell(nr_cell_global_id_t cgi)
{
  return du_cfg_hdlr->get_context().find_cell(cgi) != nullptr;
}

bool du_processor_impl::has_cell_any_state(nr_cell_global_id_t cgi)
{
  return du_cfg_hdlr->get_context().find_cell_any_state(cgi) != nullptr;
}

async_task<f1ap_gnb_cu_configuration_update_response>
du_processor_impl::handle_configuration_update(const f1ap_gnb_cu_configuration_update& request)
{
  // Update the DU configuration.
  du_cfg_hdlr->handle_gnb_cu_configuration_update(request);

  // Notify the CU-CP that the cells served by this DU changed.
  cu_cp_notifier.on_served_cells_updated();

  return f1ap->handle_gnb_cu_configuration_update(request);
}

std::optional<nr_cell_global_id_t> du_processor_impl::get_cgi(pci_t pci)
{
  const du_cell_configuration* cell = du_cfg_hdlr->get_context().find_cell(pci);
  if (cell != nullptr) {
    return cell->cgi;
  }
  return std::nullopt;
}

byte_buffer du_processor_impl::get_packed_sib1(nr_cell_global_id_t cgi)
{
  const auto& cells = du_cfg_hdlr->get_context().served_cells;
  for (const auto& cell : cells) {
    if (cell.cgi == cgi) {
      return cell.sys_info.packed_sib1.copy();
    }
  }
  return byte_buffer{};
}

void du_processor_impl::pdcp_removal_handler_impl::remove_ue_context(cu_cp_ue_index_t ue_index)
{
  parent->srb_pdcp_contexts.erase(ue_index);
}

void du_processor_impl::remove_ue_context(cu_cp_ue_index_t ue_index)
{
  f1ap_pdcp_dcch_adapters.erase(ue_index);
  f1ap_rrc_ccch_adapters.erase(ue_index);
  rrc_ue_f1ap_adapters.erase(ue_index);
}

cu_cp_metrics_report::du_info du_processor_impl::handle_du_metrics_report_request() const
{
  cu_cp_metrics_report::du_info report;
  report.id = gnb_du_id_t::invalid;
  if (du_cfg_hdlr->has_context()) {
    report.id         = du_cfg_hdlr->get_context().id;
    const auto& cells = du_cfg_hdlr->get_context().served_cells;
    for (const auto& cell : cells) {
      report.cells.emplace_back();
      report.cells.back().cgi = cell.cgi;
      report.cells.back().pci = cell.pci;
    }
  }
  // Get RRC metrics.
  rrc->get_rrc_du_metrics_collector().collect_metrics(report.rrc_metrics);

  return report;
}
