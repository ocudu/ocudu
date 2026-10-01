// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "split6_flexible_o_du_low_session.h"
#include "split6_flexible_o_du_low_session_factory.h"
#include "ocudu/fapi/p5/p5_operational_change_request_notifier.h"
#include <memory>

namespace ocudu {
class split6_flexible_o_du_low_metrics_notifier;

/// \brief Split 6 flexible O-DU low session manager.
///
/// This class listens to on start/stop requests from FAPI and creates the flexible O-DU low session.
class split6_flexible_o_du_low_session_manager : public fapi::p5_operational_change_request_notifier
{
public:
  explicit split6_flexible_o_du_low_session_manager(
      std::unique_ptr<split6_flexible_o_du_low_session_factory> odu_low_session_factory_) :
    odu_low_session_factory(std::move(odu_low_session_factory_))
  {
    ocudu_assert(odu_low_session_factory, "Invalid O-DU low session factory");
  }

  // See interface for documentation.
  bool on_start_request(const fapi::cell_configuration& config) override;

  // See interface for documentation.
  void on_stop_request() override { flexible_odu_low.reset(); }

private:
  std::unique_ptr<split6_flexible_o_du_low_session_factory> odu_low_session_factory;
  std::unique_ptr<split6_flexible_o_du_low_session>         flexible_odu_low;
};

} // namespace ocudu
