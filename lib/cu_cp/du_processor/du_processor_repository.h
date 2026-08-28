// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../adapters/du_processor_adapters.h"
#include "../cu_cp_impl_interface.h"
#include "../ue_manager/ue_manager_impl.h"
#include "du_configuration_manager.h"
#include "du_metrics_handler.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/support/async/async_task.h"
#include "ocudu/support/async/async_task_scheduler.h"

namespace ocudu::ocucp {

struct cu_cp_configuration;

/// DU processor repository configuration.
struct du_repository_config {
  gnb_id_t                                      gnb_id;
  std::string                                   ran_node_name;
  std::vector<cu_cp_configuration::ngap_config> ngaps;
  unsigned                                      max_nof_dus;
  srb_pdcp_config                               srb2_cfg;
  std::map<five_qi_t, cu_cp_qos_config>         drb_config;
  security::preferred_integrity_algorithms      int_algo_pref_list;
  security::preferred_ciphering_algorithms      enc_algo_pref_list;
  bool                                          force_reestablishment_fallback;
  bool                                          force_resume_fallback;
  std::chrono::milliseconds                     rrc_procedure_guard_time_ms;
  std::optional<std::chrono::seconds>           rrc_reject_wait_time;
  unsigned                                      rrc_version;
  bool                                          enable_rrc_metrics;
  f1ap_configuration                            f1ap;
};

/// DU processor repository dependencies.
struct du_repository_dependencies {
  task_executor&                         cu_cp_executor;
  timer_manager&                         timers;
  cu_cp_du_event_handler&                cu_cp_du_handler;
  cu_cp_measurement_config_handler&      meas_config_handler;
  cu_cp_ue_removal_handler&              ue_removal_handler;
  cu_cp_ue_context_manipulation_handler& ue_context_handler;
  async_task_scheduler&                  common_task_sched;
  ue_manager&                            ue_mng;
  du_connection_notifier&                du_conn_notif;
  cu_cp_ref_time_report_notifier&        ref_time_report_notifier;
  ocudulog::basic_logger&                logger;
};

class du_processor_repository : public du_repository_metrics_handler
{
public:
  du_processor_repository(const du_repository_config& configuration_, const du_repository_dependencies& dependencies);

  /// \brief Checks whether a cell with the specified PCI is served by any of the connected DUs.
  /// \param[in] pci The serving cell PCI.
  /// \return The index of the DU serving the given PCI.
  cu_cp_du_index_t find_du(pci_t pci) const;

  /// \brief Checks whether a cell with the specified CGI is served by any of the connected DUs.
  /// \param[in] cgi The serving cell CGI.
  /// \return The index of the DU serving the given CGI.
  cu_cp_du_index_t find_du(const nr_cell_global_id_t& cgi) const;

  /// \brief Find the DU that hosts the given CGI in any state (served or deactivated).
  ///
  /// Used by the cell lifecycle command path to resolve previously-locked cells when running an
  /// activate command. find_du(cgi) only searches served cells and would miss locked ones.
  cu_cp_du_index_t find_du_any_state(const nr_cell_global_id_t& cgi);

  /// \brief PCI-keyed variant of find_du_any_state(). Lets the mobility path recognize a handover
  /// target that a local DU hosts in deactivated state instead of treating the PCI as foreign.
  cu_cp_du_index_t find_du_any_state(pci_t pci);

  /// \brief Find a DU object.
  /// \param[in] du_index The index of the DU processor object.
  /// \return A pointer to the DU processor object, nullptr if the DU processor object is not found.
  du_processor* find_du_processor(cu_cp_du_index_t du_index);

  /// \brief Find a DU object.
  /// \param[in] du_index The index of the DU processor object.
  /// \return The DU processor object.
  du_processor& get_du_processor(cu_cp_du_index_t du_index);

  std::vector<cu_cp_du_index_t> get_du_processor_indexes() const;

  /// \brief Get the NR cells currently served by the connected DUs.
  std::vector<cu_cp_served_cell_info> get_served_cells();

  std::vector<cu_cp_metrics_report::du_info> handle_du_metrics_report_request() const override;

  /// Gets the number of F1AP UEs.
  size_t get_nof_f1ap_ues() const;

  /// Gets the number of RRC UEs.
  size_t get_nof_rrc_ues() const;

  /// \brief Adds a DU processor object to the CU-CP.
  /// \return The DU index of the added DU processor object.
  cu_cp_du_index_t add_du(std::unique_ptr<f1ap_message_notifier> f1ap_tx_pdu_notifier);

  /// \brief Launches task that removes the specified DU processor object from the CU-CP.
  /// \param[in] du_index The index of the DU processor to delete.
  /// \return asynchronous task for the DU processor removal.
  async_task<void> remove_du(cu_cp_du_index_t du_index);

  /// Number of DUs managed by the CU-CP.
  size_t get_nof_dus() const { return du_db.size(); }

private:
  struct du_context {
    // CU-CP handler of DU processor events.
    du_processor_cu_cp_adapter du_to_cu_cp_notifier;

    std::unique_ptr<du_processor> processor;

    /// Notifier used by the CU-CP to push F1AP Tx messages to the respective DU.
    std::unique_ptr<f1ap_message_notifier> f1ap_tx_pdu_notifier;
  };

  /// \brief Get the next available index from the DU processor database.
  /// \return The DU index.
  cu_cp_du_index_t get_next_du_index();

  du_repository_config cfg;

  task_executor&                         cu_cp_executor;
  timer_manager&                         timers;
  cu_cp_du_event_handler&                cu_cp_du_handler;
  cu_cp_measurement_config_handler&      meas_config_handler;
  cu_cp_ue_removal_handler&              ue_removal_handler;
  cu_cp_ue_context_manipulation_handler& ue_context_handler;
  async_task_scheduler&                  common_task_sched;
  ue_manager&                            ue_mng;
  du_connection_notifier&                du_conn_notif;
  cu_cp_ref_time_report_notifier&        ref_time_report_notifier;
  ocudulog::basic_logger&                logger;

  std::map<cu_cp_du_index_t, du_context> du_db;

  du_configuration_manager du_cfg_mng;
};

} // namespace ocudu::ocucp
