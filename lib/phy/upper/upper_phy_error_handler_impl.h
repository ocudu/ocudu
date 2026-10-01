// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/uplink_slot_processor.h"
#include "ocudu/phy/upper/upper_phy_error_handler.h"
#include "ocudu/phy/upper/upper_phy_error_notifier.h"
#include <functional>

namespace ocudu {

/// Upper physical layer error handler implementation.
class upper_phy_error_handler_impl : public upper_phy_error_handler
{
public:
  upper_phy_error_handler_impl(uplink_slot_processor_pool& ul_proc_pool_);

  // See interface for documentation.
  void handle_late_downlink_message(slot_point slot) override;

  // See interface for documentation.
  void handle_late_uplink_message(slot_point slot) override;

  // See interface for documentation.
  void handle_late_prach_message(slot_point slot) override;

  /// Sets the given notifier as the error notifier of this handler.
  void set_error_notifier(upper_phy_error_notifier& notifier) { error_notifier = std::ref(notifier); }

private:
  uplink_slot_processor_pool&                      ul_proc_pool;
  std::reference_wrapper<upper_phy_error_notifier> error_notifier;
};

} // namespace ocudu
