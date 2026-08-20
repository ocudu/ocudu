// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/remote_control/remote_command.h"
#include "ocudu/ntn/ntn_configuration_manager.h"
#include "ocudu/ocudulog/ocudulog.h"

namespace ocudu {
namespace ocudu_ntn {

/// Remote command that updates NTN-config parameters.
class ntn_config_update_remote_command : public app_services::remote_command
{
  ocudulog::basic_logger&    logger;
  ntn_configuration_manager& ntn_cfgr;

public:
  ntn_config_update_remote_command(ntn_configuration_manager& ntn_cfgr_) :
    logger(ocudulog::fetch_basic_logger("NTN")), ntn_cfgr(ntn_cfgr_)
  {
  }

  // See interface for documentation.
  std::string_view get_name() const override { return "ntn_config_update"; }

  // See interface for documentation.
  std::string_view get_description() const override { return "Updates the NTN parameters"; }

  // See interface for documentation.
  expected<nlohmann::json, std::string> execute(const nlohmann::json& json) override;
};

} // namespace ocudu_ntn
} // namespace ocudu
