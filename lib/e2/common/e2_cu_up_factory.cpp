// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/e2/e2_cu_up_factory.h"
#include "e2_entity.h"
#include "e2_impl.h"
#include "e2sm/e2sm_kpm/e2sm_kpm_asn1_packer.h"
#include "e2sm/e2sm_kpm/e2sm_kpm_cu_meas_provider_impl.h"
#include "e2sm/e2sm_kpm/e2sm_kpm_du_meas_provider_impl.h"
#include "e2sm/e2sm_kpm/e2sm_kpm_impl.h"
#include "e2sm/e2sm_rc/e2sm_rc_asn1_packer.h"
#include "e2sm/e2sm_rc/e2sm_rc_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/e2/e2_agent_dependencies.h"

using namespace ocudu;

std::unique_ptr<e2_agent>
ocudu::create_e2_cu_up_agent(const e2ap_config&                                 e2ap_cfg_,
                             e2_connection_client&                              e2_client_,
                             e2_cu_metrics_interface*                           e2_metrics_,
                             timer_factory                                      timers_,
                             task_executor&                                     e2_exec_,
                             std::unique_ptr<e2_node_component_config_provider> node_component_config_provider_)
{
  ocudulog::basic_logger& logger = ocudulog::fetch_basic_logger("E2-CU-UP");
  e2_agent_dependencies   dependencies{.logger                         = logger,
                                       .e2_client                      = e2_client_,
                                       .timers                         = timers_,
                                       .task_exec                      = e2_exec_,
                                       .node_component_config_provider = std::move(node_component_config_provider_),
                                       .e2sm_modules                   = {}};

  // E2SM-KPM
  if (e2ap_cfg_.e2sm_kpm_enabled) {
    auto e2sm_kpm_meas_provider = std::make_unique<e2sm_kpm_cu_up_meas_provider_impl>();
    std::unique_ptr<e2sm_kpm_asn1_packer> e2sm_kpm_packer =
        std::make_unique<e2sm_kpm_asn1_packer>(*e2sm_kpm_meas_provider);
    std::unique_ptr<e2sm_kpm_impl> e2sm_kpm_iface =
        std::make_unique<e2sm_kpm_impl>(logger, *e2sm_kpm_packer, *e2sm_kpm_meas_provider);
    e2_metrics_->connect_e2_cu_meas_provider(std::move(e2sm_kpm_meas_provider));
    dependencies.e2sm_modules.emplace_back(e2sm_module{e2sm_kpm_asn1_packer::ran_func_id,
                                                       e2sm_kpm_asn1_packer::oid,
                                                       std::move(e2sm_kpm_packer),
                                                       std::move(e2sm_kpm_iface)});
  }

  // E2SM-RC
  if (e2ap_cfg_.e2sm_rc_enabled) {
    auto e2sm_rc_packer = std::make_unique<e2sm_rc_asn1_packer>();
    auto e2sm_rc_iface  = std::make_unique<e2sm_rc_impl>(logger, *e2sm_rc_packer);
    dependencies.e2sm_modules.emplace_back(e2sm_module{e2sm_rc_asn1_packer::ran_func_id,
                                                       e2sm_rc_asn1_packer::oid,
                                                       std::move(e2sm_rc_packer),
                                                       std::move(e2sm_rc_iface)});
  }

  return std::make_unique<e2_entity>(e2ap_cfg_, std::move(dependencies));
}
