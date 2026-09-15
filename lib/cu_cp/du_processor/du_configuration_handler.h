// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/cu_cp/du_processor_context.h"
#include "ocudu/f1ap/cu_cp/du_setup_notifier.h"
#include "ocudu/f1ap/cu_cp/f1ap_cu_configuration_update.h"

namespace ocudu::ocucp {

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
  /// \brief Find every served cell the core network names by \c cgi.
  ///
  /// The same \c cgi can match more than one cell, so the search does not stop at the first:
  /// - a Mapped Cell ID names a geographical area, TS 38.300 sec. 16.14.5, which several cells may cover;
  /// - nothing rejects a Mapped Cell ID equal to the Uu Cell ID of a different cell.
  std::vector<const du_cell_configuration*> find_cells_by_reported_cgi(nr_cell_global_id_t cgi) const
  {
    std::vector<const du_cell_configuration*> cells;
    for (const du_cell_configuration& c : served_cells) {
      // A cell answers to its own NR CGI, and, within the same PLMN, to a Mapped Cell ID any of its areas names,
      // which TS 38.300 sec. 16.14.5 reports in place of the Uu Cell ID in an NTN cell.
      if (c.cgi == cgi or (c.cgi.plmn_id == cgi.plmn_id and c.location_mapping.reports_mapped_cell_id(cgi.nci))) {
        cells.push_back(&c);
      }
    }
    return cells;
  }
  /// \brief Find a cell in either served or deactivated state.
  ///
  /// Used by the cell lifecycle command path on CU-CP to locate cells that may currently be
  /// deactivated (e.g. resolving an activate_cell command for a previously-locked cell). The
  /// plain find_cell() above only searches served_cells, which is correct for handover and
  /// admission paths but excludes locked cells.
  const du_cell_configuration* find_cell_any_state(nr_cell_global_id_t cgi) const
  {
    if (const auto* c = find_cell(cgi)) {
      return c;
    }
    auto it = std::find_if(
        deactivated_cells.begin(), deactivated_cells.end(), [&cgi](const auto& c) { return c.cgi == cgi; });
    return it != deactivated_cells.end() ? &(*it) : nullptr;
  }
  /// PCI-keyed variant of the above. Used by the mobility path to distinguish a locally-owned but
  /// administratively deactivated cell (handover target must be rejected) from a PCI this CU-CP does
  /// not know at all (inter-CU handover candidate).
  const du_cell_configuration* find_cell_any_state(pci_t pci) const
  {
    if (const auto* c = find_cell(pci)) {
      return c;
    }
    auto it = std::find_if(
        deactivated_cells.begin(), deactivated_cells.end(), [&pci](const auto& c) { return c.pci == pci; });
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

  /// \brief Add a new DU configuration to the CU-CP.
  ///
  /// A served cell that the CU-CP cannot serve is left out of the configuration: the DU keeps the cell
  /// configured, and the CU-CP never activates it. The request is rejected only when no served cell is left,
  /// or when the DU itself cannot be added.
  /// \param[in] req The DU setup request.
  /// \param[in] readable_cells The cells whose RRC containers the CU-CP could read. Cells outside this set are
  /// left out of the configuration.
  virtual error_type<du_setup_result::rejected>
  handle_new_du_config(const du_setup_request& req, span<const nr_cell_global_id_t> readable_cells) = 0;

  /// \brief Update the configuration of an existing DU managed by the CU-CP.
  ///
  /// A cell that the CU-CP cannot serve is left out of the configuration, as in \ref handle_new_du_config.
  /// \param[in] req The gNB-DU Configuration Update.
  /// \param[in] readable_cells The cells whose RRC containers the CU-CP could read.
  virtual error_type<du_config_update_result::rejected>
  handle_du_config_update(const du_config_update_request& req, span<const nr_cell_global_id_t> readable_cells) = 0;

  /// Update the configuration of an existing DU managed by the CU-CP.
  virtual void handle_gnb_cu_configuration_update(const f1ap_gnb_cu_configuration_update& req) = 0;

protected:
  const du_configuration_context* ctxt = nullptr;
};

} // namespace ocudu::ocucp
