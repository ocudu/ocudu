// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_ue_impl.h"

using namespace ocudu;
using namespace ocucp;

cu_cp_ue::cu_cp_ue(cu_cp_ue_index_t               ue_index_,
                   cu_cp_du_index_t               du_index_,
                   timer_manager&                 timers_,
                   task_executor&                 task_exec_,
                   const up_resource_manager_cfg& up_cfg,
                   const security_manager_config& sec_cfg,
                   ue_task_scheduler_impl         task_sched_,
                   std::optional<gnb_du_id_t>     du_id_,
                   std::optional<pci_t>           pci_,
                   std::optional<rnti_t>          c_rnti_,
                   std::optional<du_cell_index_t> pcell_index_) :
  ue_index(ue_index_),
  task_sched(std::move(task_sched_)),
  up_mng(up_cfg),
  sec_mng(sec_cfg),
  rrc_ue_cu_cp_ev_notifier(ue_index)
{
  if (du_id_.has_value() && du_id_.value() != gnb_du_id_t::invalid) {
    ue_ctxt.du_id = du_id_.value();
  }

  if (pci_.has_value() && pci_.value() != INVALID_PCI) {
    pci = pci_.value();
  }

  if (c_rnti_.has_value() && c_rnti_.value() != rnti_t::INVALID_RNTI) {
    ue_ctxt.crnti = c_rnti_.value();
  }

  if (pcell_index_.has_value() && pcell_index_.value() != INVALID_DU_CELL_INDEX) {
    pcell_index = pcell_index_.value();
  }

  ue_ctxt.du_idx = du_index_;

  rrc_ue_cu_cp_ue_ev_notifier.connect_ue(*this);
  ngap_cu_cp_ue_ev_notifier.connect_ue(*this);
  nrppa_cu_cp_ue_ev_notifier.connect_ue(*this);

  handover_ue_release_timer = timers_.create_unique_timer(task_exec_);
  ran_paging_timer          = timers_.create_unique_timer(task_exec_);
  rna_update_timer          = timers_.create_unique_timer(task_exec_);
}

void cu_cp_ue::update_du_ue(gnb_du_id_t du_id_, pci_t pci_, rnti_t c_rnti_, du_cell_index_t pcell_index_)
{
  if (du_id_ != gnb_du_id_t::invalid) {
    ue_ctxt.du_id = du_id_;
  }

  if (pci_ != INVALID_PCI) {
    pci = pci_;
  }

  if (c_rnti_ != rnti_t::INVALID_RNTI) {
    ue_ctxt.crnti = c_rnti_;
  }

  if (pcell_index_ != INVALID_DU_CELL_INDEX) {
    pcell_index = pcell_index_;
  }
}

void cu_cp_ue::update_meas_context(cell_meas_manager_ue_context meas_ctxt)
{
  meas_context = std::move(meas_ctxt);
}

/// \brief Set the RRC UE of the UE.
/// \param[in] rrc_ue_ RRC UE of the UE.
void cu_cp_ue::set_rrc_ue(rrc_ue_interface& rrc_ue_)
{
  rrc_ue = &rrc_ue_;
}

void cu_cp_ue::set_ue_ambr(aggregate_maximum_bit_rate_t ue_ambr)
{
  ue_ctxt.ue_ambr = ue_ambr;
}
