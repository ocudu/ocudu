// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ofh/ofh_error_notifier.h"
#include "ocudu/ru/ru_error_notifier.h"

namespace ocudu {

/// Radio Unit error handler for the Open Fronthaul interface.
class ru_ofh_error_handler_impl : public ofh::error_notifier
{
public:
  explicit ru_ofh_error_handler_impl(ru_error_notifier& notifier_) : error_notifier(notifier_) {}

  // See interface for documentation.
  void on_late_downlink_message(const ofh::error_context& context) override;

  // See interface for documentation.
  void on_late_uplink_message(const ofh::error_context& context) override;

  // See interface for documentation.
  void on_late_prach_message(const ofh::error_context& context) override;

private:
  ru_error_notifier& error_notifier;
};

} // namespace ocudu
