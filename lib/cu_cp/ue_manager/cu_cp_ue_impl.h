// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../adapters/ngap_adapters.h"
#include "../adapters/nrppa_adapters.h"
#include "../adapters/rrc_ue_adapters.h"
#include "../cell_meas_manager/measurement_context.h"
#include "../ue_location_manager/ue_location_manager.h"
#include "../ue_security_manager/ue_security_manager_impl.h"
#include "../up_resource_manager/up_resource_manager_impl.h"
#include "cu_cp_ue_impl_interface.h"
#include "ue_task_scheduler_impl.h"
#include "ocudu/cu_cp/cu_cp_cho_types.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp_bearer_context_update.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/du_cell_index.h"
#include "ocudu/ran/gnb_du_id.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/xnap/xnap_types.h"
#include <optional>

namespace ocudu::ocucp {

/// \brief Context of a CU-CP UE.
struct cu_cp_ue_context {
  cu_cp_du_index_t             du_idx       = cu_cp_du_index_t::invalid;
  cu_cp_cu_up_index_t          cu_up_idx    = cu_cp_cu_up_index_t::invalid;
  xnc_peer_index_t             xnc_peer_idx = xnc_peer_index_t::invalid;
  plmn_identity                plmn         = plmn_identity::test_value();
  gnb_du_id_t                  du_id        = gnb_du_id_t::invalid;
  cu_cp_ue_index_t             ue_index     = cu_cp_ue_index_t::invalid;
  rnti_t                       crnti        = rnti_t::INVALID_RNTI;
  aggregate_maximum_bit_rate_t ue_ambr;
  /// \brief Flag to disable new UE reconfigurations. This can be used, for instance, to reconfigure UE contexts
  /// that are in the process of handover.
  bool reconfiguration_disabled = false;
  /// \brief Pending NR redirect target set before the NGAP round-trip (NGAP carries no redirectedCarrierInfo).
  /// Consumed by ue_context_release_routine once the AMF responds with UE Context Release Command.
  std::optional<cu_cp_release_redirect_nr_info> pending_redirect_nr_info;
};

struct cu_cp_ue_handover_context {
  cu_cp_ue_index_t target_ue_index = cu_cp_ue_index_t::invalid;
  uint8_t          rrc_reconfig_transaction_id;
};

/// \brief Single CHO candidate cell context.
struct cu_cp_cho_candidate {
  cond_recfg_id_t cond_recfg_id{
      cond_recfg_id_t(bounded_integer_invalid_tag{})};          ///< Conditional reconfiguration ID (1-8 per 3GPP).
  pci_t               target_pci = INVALID_PCI;                 ///< Target cell PCI.
  nr_cell_global_id_t target_cgi;                               ///< Target cell global identity.
  cu_cp_ue_index_t target_ue_index = cu_cp_ue_index_t::invalid; ///< Target UE index; invalid for inter-CU candidates.
  byte_buffer      prepared_rrc_recfg;                          ///< Pre-packed RRCReconfiguration for this target.
  unsigned         rrc_reconfig_transaction_id = 0; ///< RRC transaction ID for this candidate's reconfiguration.

  /// \brief E1AP bearer context modification request for CU-UP tunnel update after CHO completion.
  e1ap_bearer_context_modification_request bearer_context_mod_request;

  /// Xn-C peer index; set when the target is served by a remote CU-CP (inter-CU CHO via Xn).
  std::optional<xnc_peer_index_t> xnc_index;
  /// Target XNAP UE ID returned in HO Request Ack; needed for ConditionalHandoverCancel.
  peer_xnap_ue_id_t peer_xnap_ue_id = peer_xnap_ue_id_t::invalid;

  /// \brief Returns true if this candidate targets a remote CU-CP (Xn-based CHO).
  bool is_inter_cu() const { return xnc_index.has_value(); }
};

/// \brief CHO context for a UE (supports 1-8 candidates per 3GPP).
struct cu_cp_ue_cho_context {
  /// \brief CHO state machine states.
  /// Phases follow the CHO flow: targets preparation -> source RRC reconfiguration -> execution -> completion.
  enum class state_t {
    idle,                ///< No CHO configured.
    targets_preparation, ///< Preparing candidate target contexts.
    rrc_reconfiguration, ///< Sending/waiting source UE CHO reconfiguration.
    execution,           ///< UE executing CHO towards a target.
    completion           ///< Finalizing winner/cleanup before returning to idle.
  };

  /// \brief Role of the UE in the CHO procedure.
  enum class role_t {
    source, ///< This UE is the CHO source.
    target, ///< This UE is a CHO target candidate, awaiting Access Success.
  };

  role_t           role            = role_t::source;            ///< CHO role of this UE.
  cu_cp_ue_index_t source_ue_index = cu_cp_ue_index_t::invalid; ///< For target UEs: the source UE index.
  state_t          state           = state_t::idle;             ///< Current CHO state.
  std::vector<cu_cp_cho_candidate> candidates;                  ///< CHO candidate cells (1-8).
  unique_timer cho_execution_timer; ///< Fires conditional_handover_cancellation_routine if UE never executes CHO.

  /// \brief Find candidate by target UE index.
  /// \param[in] target_ue_idx Target UE index to search for.
  /// \return Pointer to candidate if found, nullptr otherwise.
  cu_cp_cho_candidate* find_candidate(cu_cp_ue_index_t target_ue_idx)
  {
    for (auto& candidate : candidates) {
      if (candidate.target_ue_index == target_ue_idx) {
        return &candidate;
      }
    }
    return nullptr;
  }

  /// \brief Find candidate by target CGI.
  /// \param[in] cgi Target CGI to search for.
  /// \return Pointer to candidate if found, nullptr otherwise.
  cu_cp_cho_candidate* find_candidate(const nr_cell_global_id_t& cgi)
  {
    for (auto& candidate : candidates) {
      if (candidate.target_cgi == cgi) {
        return &candidate;
      }
    }
    return nullptr;
  }

  /// \brief Clear CHO context and reset to idle state.
  /// Stops the execution timer first to prevent stale callbacks after reset.
  void clear()
  {
    cho_execution_timer.stop();
    role            = role_t::source;
    source_ue_index = cu_cp_ue_index_t::invalid;
    state           = state_t::idle;
    candidates.clear();
  }
};

class cu_cp_ue : public cu_cp_ue_impl_interface
{
public:
  cu_cp_ue(cu_cp_ue_index_t               ue_index_,
           cu_cp_du_index_t               du_index_,
           timer_manager&                 timers_,
           task_executor&                 task_exec_,
           const up_resource_manager_cfg& up_cfg,
           const security_manager_config& sec_cfg,
           ue_task_scheduler_impl         task_sched_,
           std::optional<gnb_du_id_t>     du_id_       = std::nullopt,
           std::optional<pci_t>           pci_         = std::nullopt,
           std::optional<rnti_t>          c_rnti_      = std::nullopt,
           std::optional<du_cell_index_t> pcell_index_ = std::nullopt);

  /// \brief Cancel all pending UE tasks.
  void stop();

  /// \brief Get the UE index of the UE.
  cu_cp_ue_index_t get_ue_index() const override { return ue_index; }

  /// \brief Get the PCI of the UE.
  [[nodiscard]] pci_t get_pci() const { return pci; }

  /// \brief Get the C-RNTI of the UE.
  [[nodiscard]] rnti_t get_c_rnti() const { return ue_ctxt.crnti; }

  [[nodiscard]] gnb_du_id_t get_du_id() const { return ue_ctxt.du_id; }

  /// \brief Get the DU index of the UE.
  [[nodiscard]] cu_cp_du_index_t get_du_index() const override { return ue_ctxt.du_idx; }

  /// \brief Get the CU-UP index of the UE.
  [[nodiscard]] cu_cp_cu_up_index_t get_cu_up_index() const override { return ue_ctxt.cu_up_idx; }

  /// \brief Get the XN-C peer index of the UE.
  [[nodiscard]] xnc_peer_index_t get_xnc_peer_index() const override { return ue_ctxt.xnc_peer_idx; }

  /// \brief Get the PCell index of the UE.
  du_cell_index_t get_pcell_index() { return pcell_index; }

  /// \brief Get the UP resource manager of the UE.
  up_resource_manager& get_up_resource_manager() override { return up_mng; }

  /// \brief Get the task scheduler of the UE.
  ue_task_scheduler& get_task_sched() override { return task_sched; }

  /// \brief Get the security manager of the UE.
  ue_security_manager& get_security_manager() override { return sec_mng; }

  cu_cp_ue_context&                     get_ue_context() { return ue_ctxt; }
  [[nodiscard]] const cu_cp_ue_context& get_ue_context() const { return ue_ctxt; }

  /// \brief Get the location manager of the UE.
  ue_location_manager& get_location_manager() { return loc_mng; }

  /// \brief Get the measurement context of the UE.
  cell_meas_manager_ue_context& get_meas_context() { return meas_context; }

  /// \brief Get the RAN paging timer of the UE.
  unique_timer& get_ran_paging_timer() { return ran_paging_timer; }

  /// \brief Get the RNA update timer of the UE.
  unique_timer& get_rna_update_timer() { return rna_update_timer; }

  /// \brief Update a UE with PCI and/or C-RNTI.
  void update_du_ue(gnb_du_id_t     du_id_       = gnb_du_id_t::invalid,
                    pci_t           pci_         = INVALID_PCI,
                    rnti_t          c_rnti_      = rnti_t::INVALID_RNTI,
                    du_cell_index_t pcell_index_ = INVALID_DU_CELL_INDEX);

  /// \brief Set/update the measurement context of the UE.
  void update_meas_context(cell_meas_manager_ue_context meas_ctxt);

  /// \brief Check if the DU UE context is created.
  /// \return True if the DU UE context is created, false otherwise.
  [[nodiscard]] bool du_ue_created() const { return ue_ctxt.du_idx != cu_cp_du_index_t::invalid; }

  /// \brief Set the RRC UE of the UE.
  void set_rrc_ue(rrc_ue_interface& rrc_ue_);

  /// \brief Set the CU-UP index of the UE.
  void set_cu_up_index(cu_cp_cu_up_index_t cu_up_idx) { ue_ctxt.cu_up_idx = cu_up_idx; }

  /// \brief Set the XN-C peer index of the UE.
  void set_xnc_peer_index(xnc_peer_index_t xnc_peer_idx) { ue_ctxt.xnc_peer_idx = xnc_peer_idx; }

  /// \brief Get the NGAP RRC UE notifier of the UE.
  ngap_rrc_ue_notifier& get_ngap_rrc_ue_notifier() override { return ngap_rrc_ue_ev_notifier; }

  /// \brief Get the NGAP CU-CP UE notifier of the UE.
  ngap_cu_cp_ue_notifier& get_ngap_cu_cp_ue_notifier() { return ngap_cu_cp_ue_ev_notifier; }

  /// \brief Get the NRPPA CU-CP UE notifier of the UE.
  nrppa_cu_cp_ue_notifier& get_nrppa_cu_cp_ue_notifier() { return nrppa_cu_cp_ue_ev_notifier; }

  /// \brief Get the RRC UE CU-CP UE notifier of the UE.
  rrc_ue_cu_cp_ue_notifier& get_rrc_ue_cu_cp_ue_notifier() { return rrc_ue_cu_cp_ue_ev_notifier; }

  rrc_ue_context_update_notifier& get_rrc_ue_context_update_notifier() { return rrc_ue_cu_cp_ev_notifier; }

  /// \brief Get the RRC UE measurement notifier of the UE.
  rrc_ue_measurement_notifier& get_rrc_ue_measurement_notifier() { return rrc_ue_cu_cp_ev_notifier; }

  /// \brief Get the NGAP to RRC UE adapter of the UE.
  ngap_rrc_ue_adapter& get_ngap_rrc_ue_adapter() { return ngap_rrc_ue_ev_notifier; }

  /// \brief Get the RRC UE to NGAP adapter of the UE.
  rrc_ue_ngap_adapter& get_rrc_ue_ngap_adapter() { return rrc_ue_ngap_ev_notifier; }

  /// \brief Get the RRC to CU-CP adapter of the UE.
  rrc_ue_cu_cp_adapter& get_rrc_ue_cu_cp_adapter() { return rrc_ue_cu_cp_ev_notifier; }

  /// \brief Get the RRC UE of the UE.
  rrc_ue_interface* get_rrc_ue() const { return rrc_ue; }

  /// \brief Get the measurement results of the UE.
  std::optional<cell_measurement_positioning_info>& get_measurement_results() override
  {
    return meas_context.meas_results;
  }

  /// \brief Set UE AMBR.
  /// \param[in] ue_ambr The AMBR to set for the UE.
  void set_ue_ambr(aggregate_maximum_bit_rate_t ue_ambr) override;

  /// \brief Get UE AMBR.
  aggregate_maximum_bit_rate_t get_ue_ambr() const override { return ue_ctxt.ue_ambr; }

  unique_timer& get_handover_ue_release_timer() { return handover_ue_release_timer; }

  std::optional<cu_cp_ue_handover_context>& get_ho_context() { return ho_context; }

  /// \brief Get the Conditional Handover context of the UE.
  std::optional<cu_cp_ue_cho_context>& get_cho_context() { return cho_context; }

private:
  // Common context.
  cu_cp_ue_index_t       ue_index = cu_cp_ue_index_t::invalid;
  ue_task_scheduler_impl task_sched;
  up_resource_manager    up_mng;
  ue_security_manager    sec_mng;
  ue_location_manager    loc_mng;

  // DU/CU-UP UE context.
  cu_cp_ue_context ue_ctxt;
  du_cell_index_t  pcell_index = INVALID_DU_CELL_INDEX;
  pci_t            pci         = INVALID_PCI;

  // RRC UE context.
  rrc_ue_interface*       rrc_ue = nullptr;
  rrc_ue_ngap_adapter     rrc_ue_ngap_ev_notifier;
  rrc_ue_cu_cp_ue_adapter rrc_ue_cu_cp_ue_ev_notifier;

  // NGAP UE context.
  ngap_cu_cp_ue_adapter ngap_cu_cp_ue_ev_notifier;
  ngap_rrc_ue_adapter   ngap_rrc_ue_ev_notifier;

  // NRPPA UE context.
  nrppa_cu_cp_ue_adapter nrppa_cu_cp_ue_ev_notifier;

  // CU-CP UE context.
  rrc_ue_cu_cp_adapter                     rrc_ue_cu_cp_ev_notifier;
  cell_meas_manager_ue_context             meas_context;
  unique_timer                             handover_ue_release_timer;
  unique_timer                             ran_paging_timer;
  unique_timer                             rna_update_timer;
  std::optional<cu_cp_ue_handover_context> ho_context;
  std::optional<cu_cp_ue_cho_context>      cho_context; ///< Conditional Handover context.
};

} // namespace ocudu::ocucp
