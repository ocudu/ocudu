// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_high/du_manager/du_configurator.h"
#include "ocudu/ntn/ntn_sib19_update_handler.h"
#include "ocudu/ocudulog/ocudulog.h"

namespace ocudu {
namespace odu {

/// Implementation of the NTN SIB19 msg update handler.
class du_high_ntn_sib19_update_handler_impl : public ocudu_ntn::ntn_sib19_update_handler
{
public:
  explicit du_high_ntn_sib19_update_handler_impl(du_configurator& du_cfgr_);

  /// \brief Handle a SIB19 msg update request.
  ///
  /// This function converts and forwards a SIB19 msg update request to the DU configurator.
  ///
  /// \param req SIB19 msg update request.
  void handle_sib19_msg_update(const ocudu_ntn::ntn_sib19_update_request& req) override;

private:
  ocudulog::basic_logger& logger;
  du_configurator&        du_cfgr;
};

} // namespace odu
} // namespace ocudu
