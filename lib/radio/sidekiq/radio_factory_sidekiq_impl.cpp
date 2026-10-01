// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_factory_sidekiq_impl.h"
#include "radio_session_sidekiq_impl.h"

using namespace ocudu;

std::unique_ptr<radio_session> radio_factory_sidekiq_impl::create(const radio_configuration::radio& config,
                                                                  task_executor&                    async_task_executor,
                                                                  radio_event_notifier&             notifier)
{
  // Create radio session based on Sidekiq.
  std::unique_ptr<radio_session_sidekiq_impl> session =
      std::make_unique<radio_session_sidekiq_impl>(config, async_task_executor, notifier);

  // Check session was created successfully.
  if (!session->is_successful()) {
    return nullptr;
  }

  // Otherwise, return the instance.
  return session;
}

radio_config_sidekiq_config_validator radio_factory_sidekiq_impl::config_validator;

std::unique_ptr<radio_factory> ocudu::create_dynamic_radio_factory()
{
  return std::make_unique<radio_factory_sidekiq_impl>();
}
