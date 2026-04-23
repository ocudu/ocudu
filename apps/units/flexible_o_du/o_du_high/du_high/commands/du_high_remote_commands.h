// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/remote_control/remote_command.h"
#include "ocudu/du/du_high/du_manager/du_configurator.h"

namespace ocudu {

/// Remote command that modifies the SSB parameters.
class ssb_modify_remote_command : public app_services::remote_command
{
  odu::du_configurator& configurator;

public:
  explicit ssb_modify_remote_command(odu::du_configurator& configurator_) : configurator(configurator_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "ssb_set"; }

  // See interface for documentation.
  std::string_view get_description() const override { return "Modifies the SSB parameters"; }

  // See interface for documentation.
  error_type<std::string> execute(const nlohmann::json& json) override;
};

/// Remote command that modifies the RRM policy ratio.
class rrm_policy_ratio_remote_command : public app_services::remote_command
{
  odu::du_configurator& configurator;

public:
  explicit rrm_policy_ratio_remote_command(odu::du_configurator& configurator_) : configurator(configurator_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "rrm_policy_ratio_set"; }

  // See interface for documentation.
  std::string_view get_description() const override { return "Modifies the RRM policy ratio"; }

  // See interface for documentation.
  error_type<std::string> execute(const nlohmann::json& json) override;
};

/// \brief Remote command that updates SIB content without a cell restart.
///
/// Supported SIBs: SIB2 (RACH/reselection), SIB3 (intra-frequency neighbors),
/// SIB4 (inter-frequency neighbors). Each update swaps the SIB content in the
/// live cell config, increments that SIB's value_tag, repacks SIB1 and all SI
/// messages, and notifies MAC and CU-CP. Idle UEs re-read the updated SIB on
/// the next SI period.
class sib_update_remote_command : public app_services::remote_command
{
  odu::du_configurator& configurator;

public:
  explicit sib_update_remote_command(odu::du_configurator& configurator_) : configurator(configurator_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "sib_update"; }

  // See interface for documentation.
  std::string_view get_description() const override
  {
    return "Updates SIB2/SIB3/SIB4 content with per-SIB value_tag increment";
  }

  // See interface for documentation.
  error_type<std::string> execute(const nlohmann::json& json) override;
};

} // namespace ocudu
