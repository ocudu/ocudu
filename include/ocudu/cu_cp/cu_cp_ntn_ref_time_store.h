// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/lockfree_triple_buffer.h"
#include "ocudu/adt/span.h"
#include "ocudu/cu_cp/cu_cp_ref_time_report_notifier.h"
#include "ocudu/ntn/ntn_configuration_manager_config.h"
#include "ocudu/ntn/ntn_time_provider.h"
#include <map>
#include <memory>
#include <optional>

namespace ocudu {

class timer_manager;
class task_executor;

namespace ocucp {
class cu_cp_ntn_meas_update_handler;
} // namespace ocucp

namespace ocudu_ntn {
class ntn_configuration_manager;
class ntn_time_provider;
class ntn_meas_info_update_handler;
} // namespace ocudu_ntn

/// \brief Lock-free store of the time-SFN mappings reported by the DUs.
///
/// Bridges the DU Reference Time Information reports (pushed by the CU-CP on its execution context) to the NTN
/// configuration manager time queries (pulled from its timer execution context). The set of tracked cells is fixed at
/// construction, so both threads only ever look up (never mutate) the map; each cell's latest mapping is exchanged
/// through a single-producer/single-consumer lock-free triple buffer (one DU writer per cell, one timer reader).
class cu_cp_ntn_ref_time_store : public ocucp::cu_cp_ref_time_report_notifier
{
public:
  /// \param cells NCIs of the cells whose reference time is tracked. Fixed for the lifetime of the store.
  explicit cu_cp_ntn_ref_time_store(span<const nr_cell_identity> cells);

  // See interface for documentation.
  void on_ref_time_info_report(span<const nr_cell_global_id_t>     served_cells,
                               const ocucp::cu_cp_ref_time_report& report) override;

  /// \brief Returns the latest reference time mapping reported by the DU for the timeline of the given cell.
  ///
  /// The mapping is returned as reported (no local extrapolation); freshness comes from the periodic DU reference
  /// time reporting. Returns std::nullopt for an untracked cell or one with no report received yet.
  std::optional<ocudu_ntn::ntn_time_slot_mapping> get_last_mapping(const nr_cell_global_id_t& nr_cgi,
                                                                   subcarrier_spacing         scs) const;

private:
  /// A cell's latest time-SFN mapping; empty until the first reference time report is received.
  using cell_time_sfn_mapping = std::optional<ocudu_ntn::ntn_time_slot_mapping>;
  /// Per-cell mapping, exchanged through an SPSC lock-free triple buffer. Empty until the first report.
  mutable std::map<nr_cell_identity, lockfree_triple_buffer<cell_time_sfn_mapping>> mappings;
};

/// Creates an NTN time provider backed by the given reference time store. Used to feed the NTN configuration manager.
std::unique_ptr<ocudu_ntn::ntn_time_provider> create_cu_cp_ntn_time_provider(cu_cp_ntn_ref_time_store& store);

/// Creates an NTN measurement info update handler that forwards refreshed neighbour info to the given CU-CP handler.
std::unique_ptr<ocudu_ntn::ntn_meas_info_update_handler>
create_cu_cp_ntn_meas_info_handler(ocucp::cu_cp_ntn_meas_update_handler& handler);

/// Creates the NTN configuration manager used by the CU-CP to periodically refresh the NTN neighbour cell info of the
/// measurement configuration. The manager is fed by the reference time store and forwards refreshed info to the given
/// CU-CP measurement update handler.
std::unique_ptr<ocudu_ntn::ntn_configuration_manager>
create_cu_cp_ntn_configuration_manager(const ocudu_ntn::ntn_configuration_manager_config& config,
                                       cu_cp_ntn_ref_time_store&                          ref_time_store,
                                       ocucp::cu_cp_ntn_meas_update_handler&              meas_update_handler,
                                       timer_manager&                                     timers,
                                       task_executor&                                     executor);

} // namespace ocudu
