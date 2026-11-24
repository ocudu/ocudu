// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../mac_config_interfaces.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {

struct mac_control_config;
struct mac_ue_context;
class mac_scheduler_configurator;
class rnti_manager;

class mac_ue_reconfiguration_procedure
{
public:
  explicit mac_ue_reconfiguration_procedure(const mac_ue_reconfiguration_request& req_,
                                            mac_control_config&                   cfg_,
                                            mac_ue_context&                       ue_ctx_,
                                            rnti_manager&                         rnti_table_,
                                            mac_ul_configurator&                  mac_ul_,
                                            mac_dl_configurator&                  mac_dl_,
                                            mac_scheduler_configurator&           sched_cfg_);

  void operator()(coro_context<async_task<mac_ue_reconfiguration_response>>& ctx);

  static const char* name() { return "MAC UE Reconfiguration"; }

private:
  mac_ue_reconfiguration_response handle_result(bool result);

  /// Allocates the CS-RNTI of a UE that gets configured grants and writes it into the request.
  /// \return false if the UE gets configured grants but no CS-RNTI could be allocated.
  bool prepare_cs_rnti();

  /// Frees the CS-RNTI that the new UE configuration no longer holds, depending on whether the scheduler applied it.
  void update_cs_rnti(bool sched_applied);

  /// Removes the CS-RNTI the UE holds from the RNTI table.
  void release_cs_rnti() const;

  mac_ue_reconfiguration_request req;
  mac_control_config&            cfg;
  ocudulog::basic_logger&        logger;
  mac_ue_context&                ue_ctx;
  rnti_manager&                  rnti_table;
  mac_ul_configurator&           ul_unit;
  mac_dl_configurator&           dl_unit;
  mac_scheduler_configurator&    sched_cfg;

  /// CS-RNTI the UE holds before the reconfiguration, or INVALID_RNTI if none.
  rnti_t old_cs_rnti = rnti_t::INVALID_RNTI;
  /// CS-RNTI the UE holds with the new configuration, or INVALID_RNTI if none.
  rnti_t new_cs_rnti = rnti_t::INVALID_RNTI;

  bool add_ue_result  = false;
  bool sched_conf_res = true;
};

} // namespace ocudu
