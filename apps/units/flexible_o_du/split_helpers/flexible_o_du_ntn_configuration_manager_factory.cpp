// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "flexible_o_du_ntn_configuration_manager_factory.h"
#include "ocudu/du/du_high/du_high_ntn_sib19_update_handler_factory.h"
#include "ocudu/du/du_high/du_high_ntn_time_provider_factory.h"
#include "ocudu/ntn/ntn_configuration_manager.h"
#include "ocudu/ntn/ntn_configuration_manager_dependencies.h"
#include "ocudu/ntn/ntn_configuration_manager_factory.h"
#include "ocudu/ru/sdr/ru_sdr_ntn_doppler_compensation_handler_factory.h"

using namespace ocudu;
using namespace ocudu_ntn;

std::unique_ptr<ocudu_ntn::ntn_configuration_manager>
ocudu::create_ntn_configuration_manager(const ntn_configuration_manager_config& ntn_config,
                                        odu::du_configurator&                   du_cfgtr,
                                        mac_subframe_time_mapper&               du_time_mapper,
                                        ru_controller&                          ru_ctrl,
                                        timer_manager&                          timers,
                                        task_executor&                          executor)
{
  auto doppler_handler          = create_ru_sdr_ntn_doppler_compensation_handler(ru_ctrl);
  auto sib19_msg_update_handler = create_ntn_sib19_update_handler(du_cfgtr);
  auto time_provider            = create_du_high_ntn_time_provider(du_time_mapper);

  ntn_configuration_manager_dependencies dependencies{std::move(sib19_msg_update_handler),
                                                      std::move(time_provider),
                                                      std::move(doppler_handler),
                                                      /*meas_info_update_handler=*/nullptr,
                                                      timers,
                                                      executor};

  return ocudu::ocudu_ntn::create_ntn_configuration_manager(ntn_config, std::move(dependencies));
}
