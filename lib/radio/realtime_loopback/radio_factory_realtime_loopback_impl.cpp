// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_factory_realtime_loopback_impl.h"
#include "radio_config_realtime_loopback_validator.h"
#include "radio_session_realtime_loopback_impl.h"

using namespace ocudu;

std::unique_ptr<radio_session> radio_factory_realtime_loopback_impl::create(const radio_configuration::radio& config,
                                                                            task_executor&        async_task_executor,
                                                                            radio_event_notifier& notifier)
{
  // Create a loopback realtime radio session.
  std::unique_ptr<radio_session_realtime_loopback_impl> session =
      std::make_unique<radio_session_realtime_loopback_impl>(config, async_task_executor, notifier);

  return session;
}

std::unique_ptr<radio_session> radio_factory_realtime_loopback_impl::create_with_custom_time(
    const radio_configuration::radio&                    config,
    task_executor&                                       async_task_executor,
    radio_event_notifier&                                notifier,
    const unique_function<baseband_gateway_timestamp()>& get_current_rf_timestamp_fn)
{
  // Create a loopback realtime radio session.
  std::unique_ptr<radio_session_realtime_loopback_impl> session =
      std::make_unique<radio_session_realtime_loopback_impl>(
          config, async_task_executor, notifier, get_current_rf_timestamp_fn);

  return session;
}

const radio_configuration::validator& radio_factory_realtime_loopback_impl::get_configuration_validator() const
{
  static radio_config_realtime_loopback_validator config_validator;
  return config_validator;
}
