// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_ue_impl.h"
#include "ocudu/adt/format.h"

using namespace ocudu;
using namespace ocucp;

cu_cp_ue::cu_cp_ue(const cu_cp_ue_configuration& cfg, cu_cp_ue_dependencies dependencies) :
  ue_index(cfg.ue_index),
  task_sched(std::move(dependencies.task_sched)),
  up_mng(cfg.up_cfg),
  sec_mng(cfg.sec_cfg),
  rrc_ue_cu_cp_ev_notifier(ue_index)
{
  if (cfg.du_id.has_value() && *cfg.du_id != gnb_du_id_t::invalid) {
    ue_ctxt.du_id = *cfg.du_id;
  }

  if (cfg.pci.has_value() && *cfg.pci != INVALID_PCI) {
    pci = *cfg.pci;
  }

  if (cfg.c_rnti.has_value() && *cfg.c_rnti != rnti_t::INVALID_RNTI) {
    ue_ctxt.crnti = *cfg.c_rnti;
  }

  if (cfg.pcell_index.has_value() && *cfg.pcell_index != INVALID_DU_CELL_INDEX) {
    pcell_index = *cfg.pcell_index;
  }

  ue_ctxt.du_idx = cfg.du_index;

  rrc_ue_cu_cp_ue_ev_notifier.connect_ue(*this);
  ngap_cu_cp_ue_ev_notifier.connect_ue(*this);
  nrppa_cu_cp_ue_ev_notifier.connect_ue(*this);

  handover_ue_release_timer = dependencies.timers.create_unique_timer(dependencies.task_exec);
  ran_paging_timer          = dependencies.timers.create_unique_timer(dependencies.task_exec);
  rna_update_timer          = dependencies.timers.create_unique_timer(dependencies.task_exec);
}

void cu_cp_ue::update_du_ue(gnb_du_id_t                        du_id_,
                            pci_t                              pci_,
                            rnti_t                             c_rnti_,
                            du_cell_index_t                    pcell_index_,
                            std::optional<nr_cell_global_id_t> cgi_)
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

  if (cgi_.has_value()) {
    serving_cell_id = cgi_.value();
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
