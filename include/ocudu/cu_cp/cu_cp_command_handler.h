// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_cp/cell_meas_manager_config.h"
#include "ocudu/cu_cp/cu_cp_cell_command_handler.h"
#include "ocudu/ran/cu_cp_ue_context_release.h"
#include "ocudu/ran/meas_types.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/tac.h"

namespace ocudu::ocucp {

class cu_cp_mobility_command_handler
{
public:
  virtual ~cu_cp_mobility_command_handler() = default;

  /// \brief Trigger handover of a given UE to a target cell.
  ///
  /// The UE is uniquely identified in the CU-CP through the serving Cell PCI
  /// and RNTI. The target is identified through the Target PCI.
  /// \return True if the handover was started. False if the UE or the target cell is unknown, or the trigger
  /// could not be dispatched to the CU-CP. The outcome of the handover itself is asynchronous.
  virtual bool
  trigger_handover(pci_t source_pci, rnti_t rnti, pci_t target_pci, plmn_identity target_plmn, tac_t target_tac) = 0;

  /// \brief Trigger Conditional Handover (CHO) with one or more target cells.
  ///
  /// Prepares CHO candidate cell configurations and sends them to the UE.
  /// The UE is uniquely identified in the CU-CP through the serving Cell PCI and RNTI.
  ///
  /// \param[in] source_pci Serving cell PCI.
  /// \param[in] rnti UE RNTI on the serving cell.
  /// \param[in] target_pcis Target cell PCIs (1-8 candidates supported per 3GPP).
  /// \param[in] timeout Maximum time to wait for CHO completion.
  /// \param[in] t1_thres_override Optional runtime override for the T1 conditional event threshold.
  /// \return True if the conditional handover was started. False if the trigger could not be dispatched to the
  /// CU-CP. The outcome of the conditional handover itself is asynchronous.
  virtual bool trigger_conditional_handover(
      pci_t                                                source_pci,
      rnti_t                                               rnti,
      span<const pci_t>                                    target_pcis,
      std::chrono::milliseconds                            timeout,
      std::optional<std::chrono::system_clock::time_point> t1_thres_override = std::nullopt) = 0;
};

/// Handler for external UE release commands to the CU-CP.
class cu_cp_ue_release_command_handler
{
public:
  virtual ~cu_cp_ue_release_command_handler() = default;

  /// \brief Trigger RRC Release for a UE, with optional NR redirection.
  ///
  /// The UE is released to RRC_IDLE. If redirect_info is set, the RRCRelease message will
  /// include redirectedCarrierInfo pointing to the given NR carrier (TS 38.331 Sec. 5.3.8.3).
  ///
  /// \param[in] source_pci    Serving cell PCI.
  /// \param[in] rnti          UE RNTI on the serving cell.
  /// \param[in] redirect_info Optional NR carrier to redirect the UE to on entry to RRC_IDLE.
  virtual void trigger_release(pci_t                                         source_pci,
                               rnti_t                                        rnti,
                               std::optional<cu_cp_release_redirect_nr_info> redirect_info = std::nullopt) = 0;
};

/// Handler for external NTN neighbour cell measurement info updates to the CU-CP.
class cu_cp_ntn_meas_update_handler
{
public:
  virtual ~cu_cp_ntn_meas_update_handler() = default;

  /// \brief Update the NTN neighbour cell info used when building measurement configurations.
  ///
  /// The epoch time of each entry is expressed in the SFN timeline of the serving cell, so updates apply per
  /// (serving cell, neighbour cell) pair.
  ///
  /// \param[in] serving_nci Serving cell whose neighbour measurement info is updated.
  /// \param[in] ncells Updated NTN neighbour cell info items.
  virtual void update_ntn_neighbour_info(nr_cell_identity                              serving_nci,
                                         std::vector<rrc_ntn_neighbour_cell_info_item> ncells) = 0;
};

/// Handler for external mobility configuration commands to the CU-CP.
///
/// Mutates the cell measurement configuration at runtime: the cells known for measurement purposes, the
/// neighbor relations between them and the report configurations the relations reference. Safe to call from
/// outside the CU-CP execution context (e.g. a WS/O1 command handler): each operation is marshalled onto the
/// CU-CP executor and blocks until its validation result is known. Changes take effect for a UE at its next
/// measurement config generation (connection, reconfiguration, handover or resume).
class cu_cp_mobility_config_handler
{
public:
  virtual ~cu_cp_mobility_config_handler() = default;

  /// \brief Add a cell to the measurement configuration or replace its measurement parameters as a whole:
  /// optional parameters left unset in \c cell_cfg are cleared, which is also how a parameter is unset. The
  /// neighbor relations and the periodic report of the cell are kept. For cells served by a DU of this CU-CP,
  /// the parameters provided over F1 take precedence on (re)attach.
  /// \return True if the cell was added or updated.
  virtual bool update_mobility_cell(const serving_cell_meas_config& cell_cfg) = 0;

  /// \brief Remove a cell from the measurement configuration, including the neighbor relations of other
  /// cells pointing at it.
  /// \return True if the cell existed and was removed.
  virtual bool remove_mobility_cell(nr_cell_identity nci) = 0;

  /// \brief Add a neighbor relation between two configured cells, or replace the report config ids of an
  /// existing one. Directional: the reverse relation must be added separately.
  /// \return True if the relation was added or updated.
  virtual bool update_neighbor(nr_cell_identity             serving_nci,
                               nr_cell_identity             neighbor_nci,
                               std::vector<report_cfg_id_t> report_cfg_ids) = 0;

  /// \brief Remove the neighbor relation from \c serving_nci to \c neighbor_nci.
  /// \return True if the relation existed and was removed.
  virtual bool remove_neighbor(nr_cell_identity serving_nci, nr_cell_identity neighbor_nci) = 0;

  /// \brief Add a new report configuration or replace an existing one.
  /// \return True if the report configuration was added or updated.
  virtual bool update_report_config(report_cfg_id_t report_cfg_id, const rrc_report_cfg_nr& report_cfg) = 0;

  /// \brief Remove a report configuration. Refused while referenced by a neighbor relation or a
  /// serving-cell periodic report.
  /// \return True if the report configuration existed and was removed.
  virtual bool remove_report_config(report_cfg_id_t report_cfg_id) = 0;

  /// \brief Set or clear the serving-cell periodical report config of a cell.
  /// \return True if the cell exists and the periodic report was updated.
  virtual bool set_periodic_report(nr_cell_identity nci, std::optional<report_cfg_id_t> report_cfg_id) = 0;
};

/// Handler for external commands to the CU-CP.
class cu_cp_command_handler
{
public:
  virtual ~cu_cp_command_handler() = default;

  /// Get handler for mobility commands.
  virtual cu_cp_mobility_command_handler& get_mobility_command_handler() = 0;

  /// Get handler for UE release commands.
  virtual cu_cp_ue_release_command_handler& get_ue_release_command_handler() = 0;

  /// Get handler for NTN neighbour cell measurement info updates.
  virtual cu_cp_ntn_meas_update_handler& get_ntn_meas_update_handler() = 0;

  /// Get handler for cell-level lifecycle commands (activate, deactivate).
  virtual cu_cp_cell_command_handler& get_cell_command_handler() = 0;

  /// Get handler for mobility configuration commands.
  virtual cu_cp_mobility_config_handler& get_mobility_config_handler() = 0;
};

} // namespace ocudu::ocucp
