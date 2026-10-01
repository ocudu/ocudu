// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "mac_fapi_p5_fastpath_cell_operation_controller_impl.h"
#include "p5_responses_handler.h"
#include "p5_transaction_outcome_manager.h"
#include "ocudu/fapi_adaptor/mac/p5/mac_fapi_p5_sector_fastpath_adaptor.h"

namespace ocudu {
namespace fapi_adaptor {

struct mac_fapi_p5_sector_fastpath_adaptor_config;
struct mac_fapi_p5_sector_fastpath_adaptor_dependencies;

/// MAC-FAPI P5 sector fastpath adaptor implementation.
class mac_fapi_p5_sector_fastpath_adaptor_impl : public mac_fapi_p5_sector_fastpath_adaptor
{
public:
  mac_fapi_p5_sector_fastpath_adaptor_impl(const mac_fapi_p5_sector_fastpath_adaptor_config&       config,
                                           const mac_fapi_p5_sector_fastpath_adaptor_dependencies& dependencies);

  // See interface for documentation.
  fapi::p5_responses_notifier& get_p5_responses_notifier() override;

  // See interface for documentation.
  fapi::error_indication_notifier& get_error_indication_notifier() override;

  // See interface for documentation.
  phy_cell_operation_controller& get_operation_controller() override;

  /// \brief Returns the MAC cell slot handler of this P5 sector adaptor.
  ///
  /// This handler is used to set the outcome of a START.request transaction.
  mac_cell_slot_handler& get_mac_cell_slot_handler() { return responses_handler; }

private:
  p5_transaction_outcome_manager                      transaction_manager;
  p5_responses_handler                                responses_handler;
  mac_fapi_p5_fastpath_cell_operation_controller_impl controller;
};

} // namespace fapi_adaptor
} // namespace ocudu
