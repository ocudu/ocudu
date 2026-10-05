// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_metrics_decorator_impl.h"
#include "ocudu/radio/radio_factory.h"

using namespace ocudu;

/// Internal factory that creates decorated radio sessions from a base radio factory.
class radio_metrics_decorator_factory : public radio_factory
{
public:
  /// \brief Constructor that takes ownership of a base radio factory, used to create decorated radio sessions.
  ///
  /// \param radio_factory_base_  Base radio factory to create radio instances.
  /// \param metric_notifiers_    Optional radio baseband metric notifiers (empty = no notification or IQ metrics).
  /// \param rf_log_level_        RF log level used by the decorator logger.
  radio_metrics_decorator_factory(
      std::unique_ptr<radio_factory>                                              radio_factory_base_,
      const std::vector<std::reference_wrapper<radio_baseband_metrics_notifier>>& metric_notifiers_,
      ocudulog::basic_levels                                                      rf_log_level_) :
    radio_factory_base(std::move(radio_factory_base_)),
    metric_notifiers(std::move(metric_notifiers_)),
    rf_log_level(rf_log_level_)
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

    // Convert the metric notifiers to pointers. Note the notifiers are optional and set to nu
    std::vector<radio_baseband_metrics_notifier*> metric_notifiers_ptrs(
        std::max(config.tx_streams.size(), config.rx_streams.size()));
    ocudu_assert(metric_notifiers.size() == metric_notifiers_ptrs.size(),
                 "Metric notifiers count ({}) must be equal to the TX/RX streams count ({})",
                 metric_notifiers.size(),
                 metric_notifiers_ptrs.size());
    std::transform(metric_notifiers.begin(),
                   metric_notifiers.end(),
                   metric_notifiers_ptrs.begin(),
                   [](radio_baseband_metrics_notifier& item) { return &item; });

    return std::make_unique<radio_metrics_decorator>(
        std::move(radio_session_base), metric_notifiers_ptrs, rf_log_level);
  }

private:
  /// Base radio factory.
  std::unique_ptr<radio_factory> radio_factory_base;
  /// Optional list of metric notifiers.
  std::vector<std::reference_wrapper<radio_baseband_metrics_notifier>> metric_notifiers;
  /// RF log level.
  ocudulog::basic_levels rf_log_level;
};

std::unique_ptr<radio_factory> ocudu::create_radio_metrics_decorator_factory(
    std::unique_ptr<radio_factory>                                              radio_factory_base_,
    const std::vector<std::reference_wrapper<radio_baseband_metrics_notifier>>& metric_notifiers_,
    ocudulog::basic_levels                                                      rf_log_level)
{
  return std::make_unique<radio_metrics_decorator_factory>(
      std::move(radio_factory_base_), metric_notifiers_, rf_log_level);
}
