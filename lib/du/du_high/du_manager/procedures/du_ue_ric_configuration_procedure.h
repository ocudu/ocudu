// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../du_ue/du_ue_manager_repository.h"
#include "ocudu/du/du_high/du_manager/du_configurator.h"
#include "ocudu/mac/mac_ue_configurator.h"
#include "ocudu/support/async/manual_event.h"

namespace ocudu {
namespace odu {

class du_ue_ric_configuration_procedure
{
public:
  du_ue_ric_configuration_procedure(const du_mac_sched_control_config& request_,
                                    du_ue_manager_repository&          ue_mng_,
                                    const du_manager_params&           du_params_);

  void operator()(coro_context<async_task<du_mac_sched_control_config_response>>& ctx);

  const char* name() const { return "UE RIC configuration"; }

private:
  manual_event<du_mac_sched_control_config_response>& dispatch_ue_config_task();

  // Task run from within the UE task loop.
  async_task<mac_ue_reconfiguration_response> handle_mac_config();

  /// \brief Checks whether the RRM Policy Member(s) indicated in a RRM Policy match a slice actually in use by the UE.
  /// \remark A RRM Policy Member identifies a slice by PLMN and S-NSSAI, so a member only matches if its PLMN is the
  /// one of the UE serving cell and its S-NSSAI is the one of a DRB of the UE.
  /// \remark If the policy does not indicate any RRM Policy Member, there is nothing to validate against, and the
  /// check passes.
  bool ue_uses_requested_slice(const rrm_policy_ratio_group& policy) const;

  const du_mac_sched_control_config request;
  du_ue_manager_repository&         ue_mng;
  const du_manager_params&          du_params;

  du_ue* ue = nullptr;

  manual_event<du_mac_sched_control_config_response> ue_config_completed;
};

} // namespace odu
} // namespace ocudu
