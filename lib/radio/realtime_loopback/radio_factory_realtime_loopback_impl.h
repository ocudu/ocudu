// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "radio_config_realtime_loopback_validator.h"
#include "ocudu/radio/radio_factory.h"

namespace ocudu {

/// Implements a loopback realtime radio factory.
class radio_factory_realtime_loopback_impl : public radio_factory
{
public:
  // See interface for documentation.
  const radio_configuration::validator& get_configuration_validator() const override;

  // See interface for documentation.
  std::unique_ptr<radio_session> create(const radio_configuration::radio& config,
                                        task_executor&                    async_task_executor,
                                        radio_event_notifier&             notifier) override;

  /// Creates a realtime loopback radio with a customizable function to obtain the current RF time.
  std::unique_ptr<radio_session>
  create_with_custom_time(const radio_configuration::radio&                    config,
                          task_executor&                                       async_task_executor,
                          radio_event_notifier&                                notifier,
                          const unique_function<baseband_gateway_timestamp()>& get_current_rf_timestamp_fn);
};

} // namespace ocudu
