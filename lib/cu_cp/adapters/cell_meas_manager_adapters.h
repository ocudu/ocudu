// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../cell_meas_manager/cell_meas_manager_impl.h"
#include "../mobility_manager/mobility_manager_impl.h"
#include "ocudu/ran/plmn_identity.h"

namespace ocudu::ocucp {

/// Adapter between cell measurement and mobility manager to trigger handover.
class cell_meas_mobility_manager_adapter : public cell_meas_mobility_manager_notifier
{
public:
  cell_meas_mobility_manager_adapter() = default;

  void connect_mobility_manager(mobility_manager_measurement_handler& handler_) { handler = &handler_; }

  void on_neighbor_better_than_spcell(cu_cp_ue_index_t     ue_index,
                                      gnb_id_t             neighbor_gnb_id,
                                      nr_cell_identity     neighbor_nci,
                                      pci_t                neighbor_pci,
                                      plmn_identity        neighbor_plmn,
                                      std::optional<tac_t> neighbor_tac = std::nullopt) override
  {
    ocudu_assert(handler != nullptr, "Mobility manager handler must not be nullptr");
    handler->handle_neighbor_better_than_spcell(
        ue_index, neighbor_gnb_id, neighbor_nci, neighbor_pci, neighbor_plmn, neighbor_tac);
  }

private:
  mobility_manager_measurement_handler* handler = nullptr;
};

} // namespace ocudu::ocucp
