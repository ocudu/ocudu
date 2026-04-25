// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_cp/du_processor_context.h"
#include "ocudu/f1ap/cu_cp/du_setup_notifier.h"
#include "ocudu/f1ap/cu_cp/f1ap_cu_configuration_update.h"
#include "ocudu/ran/gnb_du_id.h"
#include "ocudu/support/ocudu_assert.h"

namespace ocudu {
namespace ocucp {

/// Current configuration of the DU being managed by the CU-CP.
struct du_configuration_context {
  /// gNB-DU ID reported during F1 setup, as per TS 38.473.
  gnb_du_id_t id;
  /// gNB-DU name reported during F1 setup, as per TS 38.473.
  std::string name;
  uint8_t     rrc_version = 2;
  /// Served cells for this DU.
  std::vector<du_cell_configuration> served_cells;
  /// Deactivated cells for this DU.
  std::vector<du_cell_configuration> deactivated_cells;

  const du_cell_configuration* find_cell(pci_t pci) const
  {
    auto it = std::find_if(served_cells.begin(), served_cells.end(), [&pci](const auto& c) { return c.pci == pci; });
    return it != served_cells.end() ? &(*it) : nullptr;
  }
  const du_cell_configuration* find_cell(nr_cell_global_id_t cgi) const
  {
    auto it = std::find_if(served_cells.begin(), served_cells.end(), [&cgi](const auto& c) { return c.cgi == cgi; });
    return it != served_cells.end() ? &(*it) : nullptr;
  }
  /// \brief Find a cell in either served or deactivated state.
  ///
  /// Used by the cell lifecycle command path on CU-CP to locate cells that may currently be
  /// deactivated (e.g. resolving an activate_cell command for a previously-locked cell). The
  /// plain find_cell() above only searches served_cells, which is correct for handover and
  /// admission paths but excludes locked cells.
  const du_cell_configuration* find_cell_any_state(nr_cell_global_id_t cgi) const
  {
    if (auto* c = find_cell(cgi); c != nullptr) {
      return c;
    }
    auto it = std::find_if(
        deactivated_cells.begin(), deactivated_cells.end(), [&cgi](const auto& c) { return c.cgi == cgi; });
    return it != deactivated_cells.end() ? &(*it) : nullptr;
  }
};

class du_configuration_handler
{
public:
  virtual ~du_configuration_handler() = default;

  /// \brief Whether the DU already shared its configuration with the CU-CP.
  bool has_context() const { return ctxt != nullptr; }

  /// Getter for the current DU configuration.
  const du_configuration_context& get_context() const
  {
    ocudu_assert(ctxt != nullptr, "bad access to DU configuration context");
    return *ctxt;
  }

  /// Add a new DU configuration the CU-CP.
  virtual error_type<du_setup_result::rejected> handle_new_du_config(const du_setup_request& req) = 0;

  /// Update the configuration of an existing DU managed by the CU-CP.
  virtual error_type<du_setup_result::rejected> handle_du_config_update(const du_config_update_request& req) = 0;

  /// Update the configuration of an existing DU managed by the CU-CP.
  virtual void handle_gnb_cu_configuration_update(const f1ap_gnb_cu_configuration_update& req) = 0;

protected:
  const du_configuration_context* ctxt = nullptr;
};

} // namespace ocucp
} // namespace ocudu
