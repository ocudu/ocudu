// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "fapi_adaptor/phy_fapi_p5_sector_operation_request_adaptor.h"
#include "split6_flexible_o_du_low_session_manager.h"
#include "ocudu/du/du_operation_controller.h"
#include "ocudu/fapi_adaptor/mac/p5/mac_fapi_p5_sector_adaptor.h"
#include <memory>

namespace ocudu {

/// Split 6 flexible O-DU low dependencies.
struct split6_flexible_o_du_low_dependencies {
  std::unique_ptr<split6_flexible_o_du_low_session_factory>                   odu_low_session_factory;
  std::unique_ptr<fapi_adaptor::mac_fapi_p5_sector_adaptor>                   mac_p5_adaptor;
  std::unique_ptr<fapi_adaptor::phy_fapi_p5_sector_operation_request_adaptor> phy_p5_adaptor;
};

/// \brief Split 6 flexible O-DU low.
///
/// This is the class returned by the application unit when it creates the O-DU low.
/// This object:
///  - Manages/owns the configuration adaptor and the FAPI configuration related objects.
///  - Controls the cell creation/destruction using the cell operation request handler implementation.
class split6_flexible_o_du_low : public du_operation_controller
{
public:
  explicit split6_flexible_o_du_low(split6_flexible_o_du_low_dependencies dependencies);

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  split6_flexible_o_du_low_session_manager                                    odu_low_session_manager;
  std::unique_ptr<fapi_adaptor::mac_fapi_p5_sector_adaptor>                   mac_p5_adaptor;
  std::unique_ptr<fapi_adaptor::phy_fapi_p5_sector_operation_request_adaptor> phy_p5_adaptor;
};

} // namespace ocudu
