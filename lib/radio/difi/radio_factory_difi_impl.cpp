// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_factory_difi_impl.h"
#include "radio_session_difi_impl.h"

using namespace ocudu;

const radio_configuration::validator& radio_factory_difi_impl::get_configuration_validator() const
{
  static const radio_config_difi_validator validator;
  return validator;
}

std::unique_ptr<radio_session> radio_factory_difi_impl::create(const radio_configuration::radio& config,
                                                               task_executor&                    async_task_executor,
                                                               radio_event_notifier&             notifier)
{
  auto session = std::make_unique<radio_session_difi_impl>(config, async_task_executor, notifier);
  if (!session->is_successful()) {
    return nullptr;
  }
  return session;
}
