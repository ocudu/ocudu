// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ofh_integration_test_non_rt_ru_factory.h"
#include "ru_ofh_impl.h"
#include "ocudu/ofh/ofh_factories.h"
#include "ocudu/ofh/timing/ofh_timing_manager.h"
#include "ocudu/ofh/timing/ofh_timing_metrics.h"
#include "ocudu/support/error_handling.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/synchronization/stop_event.h"
#include "ocudu/support/synchronization/sync_event.h"
#include <thread>

using namespace ocudu;
using namespace ofh;

namespace {

/// \brief Non-realtime timing manager.
///
/// Keeps an OTA symbol counter that is incremented after sleeping for one symbol duration, notifying every symbol to
/// the subscribers. The OS may oversleep, which only slows down the emulated time without skipping any symbol.
class non_rt_timing_manager : public timing_manager,
                              private operation_controller,
                              private ota_symbol_boundary_notifier_manager,
                              private timing_metrics_collector
{
public:
  non_rt_timing_manager(ocudulog::basic_logger& logger_,
                        task_executor&          executor_,
                        subcarrier_spacing      scs,
                        cyclic_prefix           cp) :
    logger(logger_),
    executor(executor_),
    numerology(to_numerology_value(scs)),
    nof_symbols_per_slot(get_nsymb_per_slot(cp)),
    nof_symbols_per_hyper_sfn(NOF_SFNS * NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(scs) *
                              nof_symbols_per_slot),
    symbol_duration(std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::duration<double, std::nano>(1e6 / (nof_symbols_per_slot * get_nof_slots_per_subframe(scs)))))
  {
  }

  // See interface for documentation.
  operation_controller& get_controller() override { return *this; }

  // See interface for documentation.
  ota_symbol_boundary_notifier_manager& get_ota_symbol_boundary_notifier_manager() override { return *this; }

  // See interface for documentation.
  timing_metrics_collector& get_metrics_collector() override { return *this; }

private:
  // See interface for documentation.
  void start() override
  {
    logger.info("Starting the non-realtime timing manager");
    stop_manager.reset();

    sync_event wait_event;
    if (!executor.defer([this, start_token = wait_event.get_token(), stop_token = stop_manager.get_token()]() mutable {
          // Signal start() caller thread that the operation is complete.
          start_token.reset();
          timing_loop(stop_token);
        })) {
      report_fatal_error("Unable to start the non-realtime timing manager");
    }

    // Block waiting for timing executor to start.
    wait_event.wait();
  }

  // See interface for documentation.
  void stop() override
  {
    logger.info("Requesting stop of the non-realtime timing manager");
    // Blocks until the timing loop releases its token, so the notifiers are no longer accessed afterwards.
    stop_manager.stop();
    ota_notifiers.clear();
  }

  // See interface for documentation.
  void subscribe(span<ota_symbol_boundary_notifier*> notifiers) override
  {
    // The defer() call in start() synchronizes the contents of ota_notifiers with the timing thread.
    ota_notifiers.assign(notifiers.begin(), notifiers.end());
  }

  // See interface for documentation.
  void collect_metrics(timing_metrics& metrics) override { metrics = {}; }

  /// Notifies the current symbol and advances the symbol counter until a stop is requested.
  void timing_loop(const stop_event_token& token)
  {
    while (!token.is_stop_requested()) {
      slot_symbol_point_context context{
          .symbol_point = slot_symbol_point(numerology, symbol_count % nof_symbols_per_hyper_sfn, nof_symbols_per_slot),
          .hfn          = static_cast<unsigned>((symbol_count / nof_symbols_per_hyper_sfn) % NOF_HYPER_SFNS),
          .time_point   = std::chrono::system_clock::now()};
      for (auto* notifier : ota_notifiers) {
        notifier->on_new_symbol(context);
      }

      std::this_thread::sleep_for(symbol_duration);
      ++symbol_count;
    }
  }

  ocudulog::basic_logger&                    logger;
  task_executor&                             executor;
  const unsigned                             numerology;
  const unsigned                             nof_symbols_per_slot;
  const unsigned                             nof_symbols_per_hyper_sfn;
  const std::chrono::nanoseconds             symbol_duration;
  uint64_t                                   symbol_count = 0;
  std::vector<ota_symbol_boundary_notifier*> ota_notifiers;
  stop_event_source                          stop_manager;
};

} // namespace

std::unique_ptr<radio_unit> test::create_non_rt_ofh_ru(const ru_ofh_configuration& config,
                                                       ru_ofh_dependencies&&       dependencies)
{
  report_fatal_error_if_not(dependencies.timing_notifier, "Invalid timing notifier");

  const sector_configuration& last_sector_cfg = config.sector_configs.back();

  ru_ofh_impl_dependencies ofh_dependencies;
  ofh_dependencies.logger             = dependencies.logger;
  ofh_dependencies.timing_notifier    = dependencies.timing_notifier;
  ofh_dependencies.error_notifier     = dependencies.error_notifier;
  ofh_dependencies.rx_symbol_notifier = dependencies.rx_symbol_notifier;
  ofh_dependencies.timing_mngr        = std::make_unique<non_rt_timing_manager>(
      *dependencies.logger, *dependencies.rt_timing_executor, last_sector_cfg.scs, last_sector_cfg.cp);

  ru_ofh_impl_config ru_config;
  ru_config.nof_slot_offset_du_ru = last_sector_cfg.max_processing_delay_slots;
  ru_config.nof_symbols_per_slot  = get_nsymb_per_slot(last_sector_cfg.cp);
  ru_config.scs                   = last_sector_cfg.scs;

  auto ru_ofh = std::make_unique<ru_ofh_impl>(ru_config, std::move(ofh_dependencies));

  std::vector<std::unique_ptr<sector>> sectors;
  for (unsigned i = 0, e = config.sector_configs.size(); i != e; ++i) {
    dependencies.sector_dependencies[i].notifier     = &ru_ofh->get_uplane_rx_symbol_notifier();
    dependencies.sector_dependencies[i].err_notifier = &ru_ofh->get_error_notifier();

    auto ofh_sector = create_ofh_sector(config.sector_configs[i], std::move(dependencies.sector_dependencies[i]));
    report_fatal_error_if_not(ofh_sector, "Unable to create OFH sector");
    sectors.emplace_back(std::move(ofh_sector));
  }

  ru_ofh->set_ofh_sectors(std::move(sectors));

  return ru_ofh;
}
