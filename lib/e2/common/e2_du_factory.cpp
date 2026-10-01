// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/e2/e2_du_factory.h"
#include "e2_entity.h"
#include "e2_impl.h"
#include "e2sm/e2sm_ccc/e2sm_ccc_asn1_packer.h"
#include "e2sm/e2sm_ccc/e2sm_ccc_control_action_du_executor.h"
#include "e2sm/e2sm_ccc/e2sm_ccc_control_service_impl.h"
#include "e2sm/e2sm_ccc/e2sm_ccc_impl.h"
#include "e2sm/e2sm_kpm/e2sm_kpm_asn1_packer.h"
#include "e2sm/e2sm_kpm/e2sm_kpm_du_meas_provider_impl.h"
#include "e2sm/e2sm_kpm/e2sm_kpm_impl.h"
#include "e2sm/e2sm_rc/e2sm_rc_asn1_packer.h"
#include "e2sm/e2sm_rc/e2sm_rc_control_action_du_executor.h"
#include "e2sm/e2sm_rc/e2sm_rc_control_service_impl.h"
#include "e2sm/e2sm_rc/e2sm_rc_impl.h"
#include "ocudu/e2/e2_agent_dependencies.h"

using namespace ocudu;

std::unique_ptr<e2_agent> ocudu::create_e2_du_agent(const e2ap_config& e2ap_cfg_, e2ap_dependencies dependencies)
{
  e2_agent_dependencies agent_dependencies{.logger    = dependencies.logger,
                                           .e2_client = dependencies.e2_client,
                                           .timers    = dependencies.timers,
                                           .task_exec = dependencies.e2_exec,
                                           .node_component_config_provider =
                                               std::move(dependencies.node_component_config_provider),
                                           .e2sm_modules = {}};

  // E2SM-KPM
  if (e2ap_cfg_.e2sm_kpm_enabled) {
    ocudu_assert(dependencies.f1ap_ue_id_translator, "Invalid F1AP UE id translator");
    ocudu_assert(dependencies.e2_metrics_var, "Invalid E2 metrics");

    auto e2sm_kpm_meas_provider = std::make_unique<e2sm_kpm_du_meas_provider_impl>(*dependencies.f1ap_ue_id_translator);
    auto e2sm_kpm_packer        = std::make_unique<e2sm_kpm_asn1_packer>(*e2sm_kpm_meas_provider);
    auto e2sm_kpm_iface =
        std::make_unique<e2sm_kpm_impl>(dependencies.logger, *e2sm_kpm_packer, *e2sm_kpm_meas_provider);

    dependencies.e2_metrics_var->connect_e2_du_meas_provider(std::move(e2sm_kpm_meas_provider));

    agent_dependencies.e2sm_modules.emplace_back(e2sm_module{e2sm_kpm_asn1_packer::ran_func_id,
                                                             e2sm_kpm_asn1_packer::oid,
                                                             std::move(e2sm_kpm_packer),
                                                             std::move(e2sm_kpm_iface)});
  }

  // E2SM-RC
  if (e2ap_cfg_.e2sm_rc_enabled) {
    ocudu_assert(dependencies.du_configurator, "Invalid DU configurator");
    ocudu_assert(dependencies.f1ap_ue_id_translator, "Invalid F1AP UE id translator");

    auto e2sm_rc_packer             = std::make_unique<e2sm_rc_asn1_packer>();
    auto e2sm_rc_iface              = std::make_unique<e2sm_rc_impl>(dependencies.logger, *e2sm_rc_packer);
    int  control_service_style_id   = 2;
    auto rc_control_service_style   = std::make_unique<e2sm_rc_control_service>(control_service_style_id);
    auto rc_control_action_executor = std::make_unique<e2sm_rc_control_action_2_6_du_executor>(
        *dependencies.du_configurator, *dependencies.f1ap_ue_id_translator);

    rc_control_service_style->add_e2sm_rc_control_action_executor(std::move(rc_control_action_executor));
    e2sm_rc_packer->add_e2sm_control_service(rc_control_service_style.get());
    e2sm_rc_iface->add_e2sm_control_service(std::move(rc_control_service_style));

    agent_dependencies.e2sm_modules.emplace_back(e2sm_module{e2sm_rc_asn1_packer::ran_func_id,
                                                             e2sm_rc_asn1_packer::oid,
                                                             std::move(e2sm_rc_packer),
                                                             std::move(e2sm_rc_iface)});
  }

  // E2SM-CCC
  if (e2ap_cfg_.e2sm_ccc_enabled) {
    auto e2sm_ccc_packer = std::make_unique<e2sm_ccc_asn1_packer>();
    auto e2sm_ccc_iface  = std::make_unique<e2sm_ccc_impl>(dependencies.logger, *e2sm_ccc_packer);
    std::unique_ptr<e2sm_control_service> ccc_control_service_style =
        std::make_unique<e2sm_ccc_control_service_style_2>();
    std::unique_ptr<e2sm_control_action_executor> ccc_control_action_executor =
        std::make_unique<e2sm_ccc_control_o_rrm_policy_ratio_executor>(*dependencies.du_configurator,
                                                                       dependencies.e2_exec);
    ccc_control_service_style->add_e2sm_rc_control_action_executor(std::move(ccc_control_action_executor));
    e2sm_ccc_packer->add_e2sm_control_service(ccc_control_service_style.get());
    e2sm_ccc_iface->add_e2sm_control_service(std::move(ccc_control_service_style));

    agent_dependencies.e2sm_modules.emplace_back(e2sm_module{e2sm_ccc_asn1_packer::ran_func_id,
                                                             e2sm_ccc_asn1_packer::oid,
                                                             std::move(e2sm_ccc_packer),
                                                             std::move(e2sm_ccc_iface)});
  }

  return std::make_unique<e2_entity>(e2ap_cfg_, std::move(agent_dependencies));
}
