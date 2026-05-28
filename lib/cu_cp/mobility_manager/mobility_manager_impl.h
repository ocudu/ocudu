// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../cell_meas_manager/cell_meas_manager_impl.h"
#include "../ngap_repository.h"
#include "../ue_manager/ue_manager_impl.h"
#include "../xnap_repository.h"
#include "metrics/mobility_manager_metrics_aggregator.h"
#include "ocudu/cu_cp/cu_cp_command_handler.h"
#include "ocudu/cu_cp/mobility_manager_config.h"
#include "ocudu/ran/cu_cp_types.h"

namespace ocudu::ocucp {

class du_processor_repository;

/// Handler for measurement related events.
class mobility_manager_measurement_handler
{
public:
  virtual ~mobility_manager_measurement_handler() = default;

  /// \brief Handle event where neighbor became better than serving cell.
  virtual void handle_neighbor_better_than_spcell(cu_cp_ue_index_t     ue_index,
                                                  gnb_id_t             neighbor_gnb_id,
                                                  nr_cell_identity     neighbor_nci,
                                                  pci_t                neighbor_pci,
                                                  plmn_identity        neighbor_plmn,
                                                  std::optional<tac_t> neighbor_tac) = 0;
};

/// Interface used to capture the mobility management metrics to the CU-CP.
class mobility_manager_metrics_handler
{
public:
  virtual ~mobility_manager_metrics_handler() = default;

  /// \brief Handle new metrics request for the mobility manager of the CU-CP.
  virtual mobility_management_metrics handle_mobility_metrics_report_request() const = 0;
};

/// Basic mobility manager implementation.
class mobility_manager final : public mobility_manager_measurement_handler,
                               public cu_cp_mobility_command_handler,
                               public mobility_manager_metrics_handler
{
public:
  mobility_manager(const mobility_manager_cfg&      cfg,
                   mobility_manager_cu_cp_notifier& cu_cp_notifier_,
                   ngap_repository&                 ngap_db_,
                   du_processor_repository&         du_db_,
                   xnap_repository&                 xnap_db_,
                   ue_manager&                      ue_mng_,
                   cell_meas_manager&               cell_meas_mng_);

  void trigger_handover(pci_t         source_pci,
                        rnti_t        rnti,
                        pci_t         target_pci,
                        plmn_identity target_plmn,
                        tac_t         target_tac) override;

  void trigger_conditional_handover(
      pci_t                                                source_pci,
      rnti_t                                               rnti,
      span<const pci_t>                                    target_pcis,
      std::chrono::milliseconds                            timeout,
      std::optional<std::chrono::system_clock::time_point> t1_thres_override = std::nullopt) override;

  /// \brief Trigger CHO automatically for an already known UE.
  /// This path is only active when enabled in gNB config.
  void trigger_auto_conditional_handover(cu_cp_ue_index_t ue_index);

  void handle_neighbor_better_than_spcell(cu_cp_ue_index_t     ue_index,
                                          gnb_id_t             neighbor_gnb_id,
                                          nr_cell_identity     neighbor_nci,
                                          pci_t                neighbor_pci,
                                          plmn_identity        neighbor_plmn,
                                          std::optional<tac_t> neighbor_tac) override;

  mobility_manager_metrics_aggregator& get_metrics_handler() { return metrics_handler; }

  mobility_management_metrics handle_mobility_metrics_report_request() const override
  {
    return metrics_handler.request_metrics_report();
  }

private:
  void        handle_handover(cu_cp_ue_index_t     ue_index,
                              gnb_id_t             neighbor_gnb_id,
                              nr_cell_identity     neighbor_nci,
                              pci_t                neighbor_pci,
                              plmn_identity        neighbor_plmn,
                              std::optional<tac_t> neighbor_tac);
  void        handle_inter_cu_handover(cu_cp_ue_index_t source_ue_index,
                                       gnb_id_t         target_gnb_id,
                                       plmn_identity    target_plmn,
                                       tac_t            target_tac,
                                       nr_cell_identity target_nci);
  void        handle_intra_cu_handover(cu_cp_ue_index_t source_ue_index,
                                       pci_t            neighbor_pci,
                                       cu_cp_du_index_t source_du_index,
                                       cu_cp_du_index_t target_du_index);
  static void handle_ngap_handover(ngap_interface&  ngap,
                                   cu_cp_ue&        ue,
                                   gnb_id_t         target_gnb_id,
                                   plmn_identity    target_plmn,
                                   tac_t            target_tac,
                                   nr_cell_identity target_nci);
  void        handle_xnap_handover(ngap_interface&  ngap,
                                   xnap_interface&  xnap,
                                   cu_cp_ue&        ue,
                                   plmn_identity    plmn,
                                   nr_cell_identity target_nci);

  void handle_conditional_handover(pci_t                                                source_pci,
                                   rnti_t                                               rnti,
                                   span<const pci_t>                                    target_pcis,
                                   std::chrono::milliseconds                            timeout,
                                   std::optional<std::chrono::system_clock::time_point> t1_thres_override);

  mobility_manager_cfg             cfg;
  mobility_manager_cu_cp_notifier& cu_cp_notifier;
  ngap_repository&                 ngap_db;
  du_processor_repository&         du_db;
  xnap_repository&                 xnap_db;
  ue_manager&                      ue_mng;
  cell_meas_manager&               cell_meas_mng;

  mobility_manager_metrics_aggregator metrics_handler;

  ocudulog::basic_logger& logger;
};

} // namespace ocudu::ocucp
