// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rrm_policy_remote_command_helpers.h"
#include "apps/units/flexible_o_du/o_du_high/du_high/commands/du_high_remote_commands.h"
#include "nlohmann/json.hpp"

using namespace ocudu;

error_type<std::string> test_helpers::apply_rrm_policy_reconfiguration(odu::du_configurator& configurator,
                                                                       plmn_identity         plmn_id,
                                                                       s_nssai_t             s_nssai,
                                                                       unsigned              min_rbs,
                                                                       unsigned              max_rbs)
{
  nlohmann::json req;

  /*
    {
      "cmd": "rrm_policy_ratio_set",
      "policies": {
        "resourceType": "PRB",
        "rRMPolicyMemberList": [
          {
            "plmn": "00101",
            "sst": 1,
            "sd": 0xffffff
          }
        ],
        "min_prb_policy_ratio": min_rbs,
        "max_prb_policy_ratio": max_rbs,
        "dedicated_ratio": 0
      }
    }
  */

  req["cmd"]                             = "rrm_policy_ratio_set";
  req["policies"]["resourceType"]        = "PRB";
  req["policies"]["rRMPolicyMemberList"] = nlohmann::json::array();
  nlohmann::json policy;
  policy["plmn"] = plmn_id.to_string();
  policy["sst"]  = s_nssai.sst.value();
  policy["sd"]   = s_nssai.sd.value();
  req["policies"]["rRMPolicyMemberList"].push_back(policy);
  req["policies"]["min_prb_policy_ratio"] = min_rbs;
  req["policies"]["max_prb_policy_ratio"] = max_rbs;
  req["policies"]["dedicated_ratio"]      = 0;

  std::unique_ptr<app_services::remote_command> remote =
      std::make_unique<rrm_policy_ratio_remote_command>(configurator);

  auto result = remote->execute(req);
  if (not result.has_value()) {
    return make_unexpected(result.error());
  }
  return {};
}
