// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "fapi_to_phy_fastpath_translator.h"
#include "phy_to_fapi_error_event_fastpath_translator.h"
#include "phy_to_fapi_results_event_fastpath_translator.h"
#include "phy_to_fapi_time_event_fastpath_translator.h"
#include "ocudu/fapi_adaptor/phy/p7/phy_fapi_p7_sector_fastpath_adaptor.h"
#include "ocudu/fapi_adaptor/phy/p7/phy_fapi_p7_sector_fastpath_adaptor_config.h"

namespace ocudu {
class uplink_request_processor;

namespace fapi_adaptor {

/// \brief PHY-FAPI P7 bidirectional sector fastpath adaptor implementation.
class phy_fapi_p7_sector_fastpath_adaptor_impl : public phy_fapi_p7_sector_fastpath_adaptor
{
public:
  /// Constructor for the PHY-FAPI bidirectional sector adaptor.
  phy_fapi_p7_sector_fastpath_adaptor_impl(const phy_fapi_p7_sector_fastpath_adaptor_config& config,
                                           phy_fapi_p7_sector_fastpath_adaptor_dependencies  dependencies);

  upper_phy_error_notifier& get_error_notifier() override;

  // See interface for documentation.
  upper_phy_timing_notifier& get_timing_notifier() override;

  // See interface for documentation.
  fapi::p7_requests_gateway& get_p7_requests_gateway() override;

  // See interface for documentation.
  upper_phy_rx_results_notifier& get_rx_results_notifier() override;

  // See interface for documentation.
  void set_p7_slot_indication_notifier(fapi::p7_slot_indication_notifier& notifier) override;

  // See interface for documentation.
  void set_error_indication_notifier(fapi::error_indication_notifier& fapi_error_notifier) override;

  // See interface for documentation.
  void set_p7_indications_notifier(fapi::p7_indications_notifier& fapi_notifier) override;

  // See interface for documentation.
  fapi::p7_last_request_notifier& get_p7_last_request_notifier() override;

private:
  /// PHY-to-FAPI uplink results events translator.
  phy_to_fapi_results_event_fastpath_translator results_translator;
  /// FAPI-to-PHY message translator.
  fapi_to_phy_fastpath_translator fapi_translator;
  /// PHY-to-FAPI time events translator.
  phy_to_fapi_time_event_fastpath_translator time_translator;
  /// PHY-to-FAPI error events translator.
  phy_to_fapi_error_event_fastpath_translator error_translator;
};

} // namespace fapi_adaptor
} // namespace ocudu
