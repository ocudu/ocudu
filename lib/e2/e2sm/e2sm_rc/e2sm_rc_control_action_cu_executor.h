// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/asn1_utils.h"
#include "ocudu/cu_cp/cu_configurator.h"
#include "ocudu/e2/e2.h"
#include "ocudu/e2/e2sm/e2sm.h"
#include <map>

namespace ocudu {

class e2sm_rc_control_action_cu_executor_base : public e2sm_control_action_executor
{
public:
  e2sm_rc_control_action_cu_executor_base() = delete;
  e2sm_rc_control_action_cu_executor_base(cu_configurator& cu_configurator_, uint32_t action_id_);
  virtual ~e2sm_rc_control_action_cu_executor_base() = default;

  asn1::e2sm::ran_function_definition_ctrl_action_item_s get_control_action_definition() override;

  /// e2sm_control_request_executor functions.
  uint32_t                              get_action_id() override;
  std::string                           get_action_name() override;
  bool                                  ric_control_action_supported(const e2sm_ric_control_request& req) override = 0;
  async_task<e2sm_ric_control_response> execute_ric_control_action(const e2sm_ric_control_request& req) override   = 0;
  async_task<e2sm_ric_control_response> return_ctrl_failure(const e2sm_ric_control_request& req);

protected:
  ocudulog::basic_logger& logger;
  uint32_t                action_id;
  std::string             action_name;
  cu_configurator&        cu_param_configurator;
};

class e2sm_rc_control_action_3_1_cu_executor : public e2sm_rc_control_action_cu_executor_base
{
public:
  e2sm_rc_control_action_3_1_cu_executor(cu_configurator& du_configurator_);
  virtual ~e2sm_rc_control_action_3_1_cu_executor() = default;

  /// e2sm_control_request_executor functions.
  bool                                  ric_control_action_supported(const e2sm_ric_control_request& req) override;
  async_task<e2sm_ric_control_response> execute_ric_control_action(const e2sm_ric_control_request& req) override;
  void parse_action_ran_parameter_value(const asn1::e2sm::ran_param_value_type_c& ran_p,
                                        uint64_t                                  ran_param_id,
                                        uint64_t                                  ue_id,
                                        cu_handover_control_config&               ctrl_cfg);
  using T = cu_handover_control_config;

private:
};

} // namespace ocudu
