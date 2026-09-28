// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../du_cell_manager.h"
#include "../du_ue/du_ue_manager_repository.h"
#include "ocudu/f1ap/du/f1ap_du_positioning_handler.h"
#include "ocudu/mac/mac_positioning_measurement_handler.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::odu {

/// \brief Measures the E-CID quantities of one UE, as per TS 38.473, Section 8.13.12.
///
/// The UE transmits SRS in its serving cell. The procedure asks the MAC to measure the next SRS of the UE and
/// converts the result into the F1AP units.
class du_ue_e_cid_measurement_procedure
{
public:
  du_ue_e_cid_measurement_procedure(const du_e_cid_meas_request& req_,
                                    du_cell_manager&             du_cells_,
                                    du_ue_manager_repository&    ue_mng_,
                                    const du_manager_params&     du_params_);

  void operator()(coro_context<async_task<du_e_cid_meas_response>>& ctx);

private:
  /// Prepares the MAC request. Returns false if the DU cannot measure the requested quantities.
  bool prepare_mac_request();

  du_e_cid_meas_response create_response();

  const du_e_cid_meas_request req;
  du_cell_manager&            du_cells;
  du_ue_manager_repository&   ue_mng;
  const du_manager_params&    du_params;
  ocudulog::basic_logger&     logger;

  /// Set when the CU-CP requested the UL Angle of Arrival.
  bool aoa_requested = false;
  /// Geographical coordinates of the serving cell, if the cell configuration provides them.
  std::optional<geographical_coordinates_t> geo_coords;

  mac_positioning_measurement_request  mac_req;
  mac_positioning_measurement_response mac_resp;
};

} // namespace ocudu::odu
