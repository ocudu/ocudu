// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_metrics_decorator_impl.h"

using namespace ocudu;

namespace {

/// Radio factory that creates decorated radio sessions.
class radio_metrics_decorator_factory : public radio_factory
{
public:
  /// \brief Constructor that takes ownership of a base radio factory, used to create decorated radio sessions.
  ///
  /// \param[in] radio_factory_base_ Base radio factory to create radio instances.
  /// \param[in] rf_log_level_       RF log level used by the decorator logger.
  radio_metrics_decorator_factory(std::unique_ptr<radio_factory> radio_factory_base_,
                                  ocudulog::basic_levels         rf_log_level_) :
    radio_factory_base(std::move(radio_factory_base_)), rf_log_level(rf_log_level_)
  {
    report_fatal_error_if_not(radio_factory_base != nullptr, "Invalid base radio factory.");
  }

  // See interface for documentation.
  const radio_configuration::validator& get_configuration_validator() const override
  {
    return radio_factory_base->get_configuration_validator();
  }

  // See interface for documentation.
  std::unique_ptr<radio_session> create(const radio_configuration::radio& config,
                                        task_executor&                    async_task_executor,
                                        radio_event_notifier&             notifier) override
  {
    std::unique_ptr<radio_session> radio_session_base =
        radio_factory_base->create(config, async_task_executor, notifier);
    return std::make_unique<radio_metrics_decorator>(
        std::move(radio_session_base), config.tx_streams.size(), rf_log_level);
  }

private:
  /// Base radio factory.
  std::unique_ptr<radio_factory> radio_factory_base;
  /// RF log level.
  ocudulog::basic_levels rf_log_level;
};

} // namespace

std::unique_ptr<radio_factory>
ocudu::create_radio_metrics_decorator_factory(std::unique_ptr<radio_factory> radio_factory_base_,
                                              ocudulog::basic_levels         rf_log_level)
{
  return std::make_unique<radio_metrics_decorator_factory>(std::move(radio_factory_base_), rf_log_level);
}
