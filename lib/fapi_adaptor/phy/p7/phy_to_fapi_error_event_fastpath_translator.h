// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/common/error_indication_notifier.h"
#include "ocudu/phy/upper/upper_phy_error_notifier.h"

namespace ocudu {
namespace fapi_adaptor {

/// \brief PHY-to-FAPI error event fastpath translator.
///
/// This class listens to upper PHY error events and translates them into FAPI ERROR.indication messages that are sent
/// through the FAPI error-specific message notifier.
class phy_to_fapi_error_event_fastpath_translator : public upper_phy_error_notifier
{
public:
  phy_to_fapi_error_event_fastpath_translator();

  // See interface for documentation.
  void on_late_downlink_message(slot_point dl_frame_slot) override;

  // See interface for documentation.
  void on_late_uplink_message(slot_point ul_frame_slot) override;

  // See interface for documentation.
  void on_late_prach_message(slot_point prach_msg_slot) override;

  /// Configures the FAPI error-specific notifier to the given one.
  void set_error_indication_notifier(fapi::error_indication_notifier& fapi_error_notifier)
  {
    error_notifier = &fapi_error_notifier;
  }

private:
  /// Error indication notifier.
  fapi::error_indication_notifier* error_notifier;
};

} // namespace fapi_adaptor
} // namespace ocudu
