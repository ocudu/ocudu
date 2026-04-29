// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_factory_realtime_dummy_impl.h"
#include "radio_config_realtime_dummy_validator.h"
#include "radio_session_realtime_dummy_impl.h"

using namespace ocudu;

std::unique_ptr<radio_session> radio_factory_realtime_dummy_impl::create(const radio_configuration::radio& config,
                                                                         task_executor&        async_task_executor,
                                                                         radio_event_notifier& notifier)
{
  // Create a dummy realtime radio session.
  std::unique_ptr<radio_session_realtime_dummy_impl> session =
      std::make_unique<radio_session_realtime_dummy_impl>(config, async_task_executor, notifier);

  return session;
}

const radio_configuration::validator& radio_factory_realtime_dummy_impl::get_configuration_validator() const
{
  static radio_config_realtime_dummy_validator config_validator;
  return config_validator;
}
