// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ru/sdr/ru_sdr_executor_mapper.h"
#include "ocudu/support/executors/executor_decoration_factory.h"
#include "ocudu/support/executors/inline_task_executor.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/ocudu_assert.h"
#include <optional>

using namespace ocudu;

namespace {

class ru_sdr_sector_executor_mapper_impl : public ru_sdr_sector_executor_mapper
{
public:
  ru_sdr_sector_executor_mapper_impl(task_executor& dl_exec_,
                                     task_executor& ul_exec_,
                                     task_executor& prach_exec_,
                                     task_executor& tx_exec_,
                                     task_executor& rx_exec_) :
    dl_exec(dl_exec_), ul_exec(ul_exec_), prach_exec(prach_exec_), tx_exec(tx_exec_), rx_exec(rx_exec_)
  {
  }

  task_executor& downlink_executor() override { return dl_exec; }
  task_executor& uplink_executor() override { return ul_exec; }
  task_executor& prach_executor() override { return prach_exec; }
  task_executor& transmitter_executor() override { return tx_exec; }
  task_executor& receiver_executor() override { return rx_exec; }

private:
  task_executor& dl_exec;
  task_executor& ul_exec;
  task_executor& prach_exec;
  task_executor& tx_exec;
  task_executor& rx_exec;
};

/// Helper class to decorate executors with extra functionalities.
struct executor_decorator {
  /// \brief Decorates a task executor with a execution metrics and/or tracing instrumentation.
  /// \tparam Exec                        Task executor class type.
  /// \param[in] exec                     Reference to the executor to decorate.
  /// \param[in] tracing_enabled          Set to true for enabling the executor tracing.
  /// \param[in] metrics_channel_registry Executor metrics backend. Set to \c nullptr to disable execution metrics.
  /// \param[in] exec_name                Executor name.
  /// \return A reference to the task executor decorator.
  template <typename Exec>
  task_executor& decorate(Exec&&                             exec,
                          bool                               tracing_enabled,
                          executor_metrics_channel_registry* metrics_channel_registry,
                          const std::string&                 exec_name = "")
  {
    // No decoration needed, return the original executor.
    if (!tracing_enabled && !metrics_channel_registry) {
      return exec;
    }

    execution_decoration_config cfg;
    if (metrics_channel_registry != nullptr) {
      cfg.metrics.emplace(exec_name, *metrics_channel_registry, tracing_enabled);
    } else if (tracing_enabled) {
      cfg.trace = execution_decoration_config::trace_option{exec_name};
    }
    decorators.push_back(decorate_executor(std::forward<Exec>(exec), cfg));

    return *decorators.back();
  }

private:
  std::vector<std::unique_ptr<task_executor>> decorators;
};

class ru_sdr_executor_mapper_impl : public ru_sdr_executor_mapper
{
public:
  explicit ru_sdr_executor_mapper_impl(const ru_sdr_executor_mapper_sequential_configuration& config)
  {
    ocudu_assert(config.asynchronous_exec != nullptr, "Invalid asynchronous executor.");
    ocudu_assert(config.common_exec != nullptr, "Invalid common executor.");

    async_exec = config.asynchronous_exec;

    std::reference_wrapper<task_executor> dl_exec    = inline_executor;
    std::reference_wrapper<task_executor> ul_exec    = inline_executor;
    std::reference_wrapper<task_executor> prach_exec = inline_executor;
    std::reference_wrapper<task_executor> tx_exec    = *config.common_exec;
    std::reference_wrapper<task_executor> rx_exec    = *config.common_exec;

    if (config.exec_metrics_channel_registry) {
      dl_exec = decorator.decorate(
          dl_exec, config.executor_tracing_enable, config.exec_metrics_channel_registry, "ru_dl_exec");
      ul_exec = decorator.decorate(
          ul_exec, config.executor_tracing_enable, config.exec_metrics_channel_registry, "ru_ul_exec");
      prach_exec = decorator.decorate(
          prach_exec, config.executor_tracing_enable, config.exec_metrics_channel_registry, "ru_prach_exec");
      tx_exec = decorator.decorate(
          tx_exec, config.executor_tracing_enable, config.exec_metrics_channel_registry, "ru_tx_exec");
      rx_exec = decorator.decorate(
          rx_exec, config.executor_tracing_enable, config.exec_metrics_channel_registry, "ru_rx_exec");
    }

    for (unsigned i_sector = 0; i_sector != config.nof_sectors; ++i_sector) {
      sectors.emplace_back(dl_exec, ul_exec, prach_exec, tx_exec, rx_exec);
    }
  }

  explicit ru_sdr_executor_mapper_impl(const ru_sdr_executor_mapper_single_configuration& config)
  {
    ocudu_assert(config.radio_exec != nullptr, "Invalid radio executor.");
    ocudu_assert(config.high_prio_executor != nullptr, "Invalid high priority executor.");

    async_exec = config.radio_exec;

    for (const auto& baseband_exec : config.baseband_exec) {
      ocudu_assert(baseband_exec != nullptr, "Invalid baseband cell executor.");

      std::reference_wrapper<task_executor> dl_exec    = *config.high_prio_executor;
      std::reference_wrapper<task_executor> ul_exec    = inline_executor;
      std::reference_wrapper<task_executor> prach_exec = *config.high_prio_executor;
      std::reference_wrapper<task_executor> tx_exec    = *baseband_exec;
      std::reference_wrapper<task_executor> rx_exec    = *baseband_exec;

      if (config.exec_metrics_channel_registry) {
        dl_exec = decorator.decorate(
            dl_exec, config.executor_tracing_enable, config.exec_metrics_channel_registry, "ru_dl_exec");
        ul_exec = decorator.decorate(
            ul_exec, config.executor_tracing_enable, config.exec_metrics_channel_registry, "ru_ul_exec");
        prach_exec = decorator.decorate(
            prach_exec, config.executor_tracing_enable, config.exec_metrics_channel_registry, "ru_prach_exec");
        tx_exec = decorator.decorate(
            tx_exec, config.executor_tracing_enable, config.exec_metrics_channel_registry, "ru_tx_exec");
        rx_exec = decorator.decorate(
            rx_exec, config.executor_tracing_enable, config.exec_metrics_channel_registry, "ru_rx_exec");
      }

      sectors.emplace_back(dl_exec, ul_exec, prach_exec, tx_exec, rx_exec);
    }
  }

  explicit ru_sdr_executor_mapper_impl(const ru_sdr_executor_mapper_dual_configuration& config)
  {
    ocudu_assert(config.radio_exec != nullptr, "Invalid radio executor.");
    ocudu_assert(config.high_prio_executor != nullptr, "Invalid high priority executor.");

    async_exec = config.radio_exec;

    for (const auto& baseband_exec : config.baseband_exec) {
      ocudu_assert(baseband_exec.rx_exec != nullptr, "Invalid receive baseband cell executor.");
      ocudu_assert(baseband_exec.tx_exec != nullptr, "Invalid transmit baseband cell executor.");

      std::reference_wrapper<task_executor> dl_exec    = *config.high_prio_executor;
      std::reference_wrapper<task_executor> ul_exec    = inline_executor;
      std::reference_wrapper<task_executor> prach_exec = *config.high_prio_executor;
      std::reference_wrapper<task_executor> tx_exec    = *baseband_exec.tx_exec;
      std::reference_wrapper<task_executor> rx_exec    = *baseband_exec.rx_exec;

      if (config.exec_metrics_channel_registry) {
        dl_exec = decorator.decorate(
            dl_exec, config.executor_tracing_enable, config.exec_metrics_channel_registry, "ru_dl_exec");
        ul_exec = decorator.decorate(
            ul_exec, config.executor_tracing_enable, config.exec_metrics_channel_registry, "ru_ul_exec");
        prach_exec = decorator.decorate(
            prach_exec, config.executor_tracing_enable, config.exec_metrics_channel_registry, "ru_prach_exec");
        tx_exec = decorator.decorate(
            tx_exec, config.executor_tracing_enable, config.exec_metrics_channel_registry, "ru_tx_exec");
        rx_exec = decorator.decorate(
            rx_exec, config.executor_tracing_enable, config.exec_metrics_channel_registry, "ru_rx_exec");
      }

      sectors.emplace_back(dl_exec, ul_exec, prach_exec, tx_exec, rx_exec);
    }
  }

  explicit ru_sdr_executor_mapper_impl(const ru_sdr_executor_mapper_triple_configuration& config)
  {
    ocudu_assert(config.radio_exec != nullptr, "Invalid radio executor.");
    ocudu_assert(config.high_prio_executor != nullptr, "Invalid high priority executor.");

    async_exec = config.radio_exec;

    for (const auto& baseband_exec : config.baseband_exec) {
      ocudu_assert(baseband_exec.rx_exec != nullptr, "Invalid receive baseband cell executor.");
      ocudu_assert(baseband_exec.tx_exec != nullptr, "Invalid transmit baseband cell executor.");
      ocudu_assert(baseband_exec.ul_exec != nullptr, "Invalid baseband demodulator cell executor.");

      std::reference_wrapper<task_executor> dl_exec    = *config.high_prio_executor;
      std::reference_wrapper<task_executor> ul_exec    = *baseband_exec.ul_exec;
      std::reference_wrapper<task_executor> prach_exec = *config.high_prio_executor;
      std::reference_wrapper<task_executor> tx_exec    = *baseband_exec.tx_exec;
      std::reference_wrapper<task_executor> rx_exec    = *baseband_exec.rx_exec;

      if (config.exec_metrics_channel_registry) {
        dl_exec = decorator.decorate(
            dl_exec, config.executor_tracing_enable, config.exec_metrics_channel_registry, "ru_dl_exec");
        ul_exec = decorator.decorate(
            ul_exec, config.executor_tracing_enable, config.exec_metrics_channel_registry, "ru_ul_exec");
        prach_exec = decorator.decorate(
            prach_exec, config.executor_tracing_enable, config.exec_metrics_channel_registry, "ru_prach_exec");
        tx_exec = decorator.decorate(
            tx_exec, config.executor_tracing_enable, config.exec_metrics_channel_registry, "ru_tx_exec");
        rx_exec = decorator.decorate(
            rx_exec, config.executor_tracing_enable, config.exec_metrics_channel_registry, "ru_rx_exec");
      }

      sectors.emplace_back(dl_exec, ul_exec, prach_exec, tx_exec, rx_exec);
    }
  }

  ru_sdr_sector_executor_mapper& get_sector_mapper(unsigned index) override { return sectors[index]; }

  task_executor& asynchronous_radio_executor() override { return *async_exec; }

private:
  std::vector<ru_sdr_sector_executor_mapper_impl> sectors;
  task_executor*                                  async_exec;
  executor_decorator                              decorator;
  inline_task_executor                            inline_executor;
};

} // namespace

std::unique_ptr<ru_sdr_executor_mapper>
ocudu::create_ru_sdr_executor_mapper(const ru_sdr_executor_mapper_sequential_configuration& config)
{
  return std::make_unique<ru_sdr_executor_mapper_impl>(config);
}

std::unique_ptr<ru_sdr_executor_mapper>
ocudu::create_ru_sdr_executor_mapper(const ocudu::ru_sdr_executor_mapper_single_configuration& config)
{
  return std::make_unique<ru_sdr_executor_mapper_impl>(config);
}

std::unique_ptr<ru_sdr_executor_mapper>
ocudu::create_ru_sdr_executor_mapper(const ocudu::ru_sdr_executor_mapper_dual_configuration& config)
{
  return std::make_unique<ru_sdr_executor_mapper_impl>(config);
}

std::unique_ptr<ru_sdr_executor_mapper>
ocudu::create_ru_sdr_executor_mapper(const ocudu::ru_sdr_executor_mapper_triple_configuration& config)
{
  return std::make_unique<ru_sdr_executor_mapper_impl>(config);
}
