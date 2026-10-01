// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_high/du_manager/du_manager_params.h"
#include "ocudu/mac/cell_configuration.h"
#include "ocudu/ran/du_types.h"

namespace ocudu {
namespace odu {

struct du_cell_param_config_request;

/// Current DU cell context.
struct du_cell_context {
  enum class state_t { active, inactive, deactivating };

  /// Current configuration.
  du_cell_config cfg;
  /// Encoded System Information being currently sent by the DU cell.
  mac_cell_sys_info_config si_cfg;
  /// Current cell state.
  state_t state;
  /// Current live MIB cellBarred state of the cell. Tracks runtime bar commands (CU-initiated or applied
  /// autonomously by the graceful cell stop) so an already-barred cell is not barred again and so the
  /// configured value is only restored on cell start when a runtime bar actually changed it.
  bool live_barred = false;
};

/// Result of the reconfiguration of a DU cell.
struct du_cell_reconfig_result {
  /// Cell configured.
  du_cell_index_t cell_index;
  /// Whether the CU needs to be notified of the gNB-DU cell configuration update.
  bool cu_notif_required;
  /// Whether the Scheduler needs to be notified about an update in the SI scheduling.
  bool sched_notif_required;
  /// Request for the MAC to change the slice configuration of the cell.
  std::optional<du_cell_slice_reconfig_request> slice_reconf_req;
};

class du_cell_manager
{
public:
  explicit du_cell_manager(const du_manager_params& params_);

  size_t nof_cells() const { return cells.size(); }

  bool has_cell(du_cell_index_t cell_index) const { return cell_index < cells.size(); }

  /// Determine whether cell is activated.
  bool is_cell_active(du_cell_index_t cell_index) const
  {
    assert_cell_exists(cell_index);
    return cells[cell_index]->state == du_cell_context::state_t::active;
  }

  /// Determine whether the cell's live MIB currently advertises cellBarred=barred.
  bool is_cell_barred(du_cell_index_t cell_index) const
  {
    assert_cell_exists(cell_index);
    return cells[cell_index]->live_barred;
  }

  du_cell_index_t get_cell_index(nr_cell_global_id_t nr_cgi) const;

  du_cell_index_t get_cell_index(pci_t pci) const;

  void add_cell(const du_cell_config& cfg);

  const du_cell_config& get_cell_cfg(du_cell_index_t cell_index) const
  {
    assert_cell_exists(cell_index);
    return cells[cell_index]->cfg;
  }
  du_cell_config& get_cell_cfg(du_cell_index_t cell_index)
  {
    assert_cell_exists(cell_index);
    return cells[cell_index]->cfg;
  }

  /// Stop accepting new UE creations in the given cell.
  void stop_accepting_ues(du_cell_index_t cell_index) const
  {
    cells[cell_index]->state = du_cell_context::state_t::deactivating;
  }

  /// Handle request to update a cell configuration.
  /// \return true if a change was detected and applied.
  expected<du_cell_reconfig_result> handle_cell_reconf_request(const du_cell_param_config_request& req) const;

  /// Retrieve current cell system information configuration.
  const mac_cell_sys_info_config& get_sys_info(du_cell_index_t cell_index) const
  {
    assert_cell_exists(cell_index);
    return cells[cell_index]->si_cfg;
  }

  /// Start a specific cell in the DU.
  async_task<bool> start(du_cell_index_t cell_index) const;

  /// Stop a specific cell in the DU.
  async_task<void> stop(du_cell_index_t cell_index) const;

  /// \brief Update the MIB cellBarred flag of a cell at runtime.
  ///
  /// Used to apply a CU-commanded bar (TS 38.473 Cells to be Barred List), by the cell stop procedure to bar
  /// the cell before draining UEs (so idle UEs reselect away before connected UE drain begins) and by the
  /// cell start path to restore the configured cellBarred state after a prior bar-first stop. The new value
  /// takes effect on the next SSB build (one SSB period).
  ///
  /// \remark This only modifies the live MAC MIB state; it intentionally does not modify
  /// \c du_cell_config::cell_barred (the operator-configured intent, restored on cell restart) nor trigger a
  /// gNB-DU Configuration Update. If this is to be used outside of the cell bar/shutdown procedures, the
  /// F1AP/DU-manager configuration state handling needs to be extended accordingly.
  async_task<void> set_cell_barred(du_cell_index_t cell_index, bool barred) const;

  /// \brief Bar a cell and wait for the change to settle on air.
  ///
  /// Bars the cell (cellBarred=true) and then waits a settling window derived from the cell's configured SSB
  /// period, so the barred MIB is transmitted at least once. If the cell is already barred (e.g. by a CU
  /// command), the re-bar is skipped but the settling window is still held, since the tracked state does not
  /// prove a barred SSB has aired yet. Intended to run concurrently with the UE drain in the graceful cell
  /// stop path, so the wait adds no latency in the common case.
  async_task<void> set_cell_barred_and_wait(du_cell_index_t cell_index) const;

  /// Stop all cells in the DU.
  async_task<void> stop_all() const;

  /// Remove all cell configurations.
  void remove_all_cells();

private:
  void assert_cell_exists(du_cell_index_t cell_index) const
  {
    ocudu_assert(has_cell(cell_index), "cell_index={} does not exist", cell_index);
  }

  const du_manager_params& cfg;
  ocudulog::basic_logger&  logger;

  std::vector<std::unique_ptr<du_cell_context>> cells;
};

} // namespace odu
} // namespace ocudu
