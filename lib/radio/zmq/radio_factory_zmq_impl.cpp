// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_factory_zmq_impl.h"
#include "radio_config_zmq_validator.h"
#include "radio_session_zmq_impl.h"

using namespace ocudu;

std::unique_ptr<radio_session> radio_factory_zmq_impl::create(const radio_configuration::radio& config,
                                                              task_executor&                    async_task_executor,
                                                              radio_event_notifier&             notifier)
{
  // Create radio session based on ZMQ.
  std::unique_ptr<radio_session_zmq_impl> session =
      std::make_unique<radio_session_zmq_impl>(config, async_task_executor, notifier);

  // Check session was created successfully.
  if (!session->is_successful()) {
    return nullptr;
  }

  // Otherwise, return the instance.
  return session;
}

const radio_configuration::validator& radio_factory_zmq_impl::get_configuration_validator() const
{
  static radio_config_zmq_config_validator config_validator;
  return config_validator;
}
