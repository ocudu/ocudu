// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "pdxch_processor_modulator_notifier.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_reader_view.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_writer_view.h"
#include "ocudu/instrumentation/traces/ru_traces.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ocuduvec/compare.h"
#include "ocudu/ocuduvec/conversion.h"
#include "ocudu/ocuduvec/dot_prod.h"
#include "ocudu/ocuduvec/zero.h"
#include "ocudu/phy/antenna_ports.h"
#include "ocudu/phy/lower/amplitude_controller/amplitude_controller.h"
#include "ocudu/phy/lower/lower_phy_baseband_metrics.h"
#include "ocudu/phy/lower/modulation/ofdm_modulator.h"
#include "ocudu/phy/lower/processors/downlink/pdxch/pdxch_processor_baseband.h"
#include "ocudu/phy/lower/sampling_rate.h"
#include "ocudu/phy/support/resource_grid_reader.h"
#include "ocudu/phy/support/shared_resource_grid.h"
#include "ocudu/ran/beamforming/beam_weights_codebook.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/math/stats.h"
#include "ocudu/support/memory_pool/bounded_object_pool.h"
#include <thread>

namespace ocudu {

/// \brief Physical downlink channels modulator.
///
/// This class asynchronously modulates transmission requests for downlink channels from a resource grid and a baseband
/// buffer. The modulation process is split in tasks per OFDM symbol and port.
///
/// This class contains a finite state machine with the following states:
/// - Idle: it is the initial state and the modulator accepts the handling of new transmissions.
/// - Modulating: The modulator has forked several tasks and it is waiting for their completion.
class pdxch_baseband_modulator
{
public:
  /// \brief Creates a physical downlink channel modulator from all the required parameters.
  ///
  /// \param[in] scs                    Subcarrier spacing.
  /// \param[in] cp                     Cyclic prefix.
  /// \param[in] srate                  Sampling rate.
  /// \param[in] executor_              Task executor for processing baseband modulation.
  /// \param[in] modulator_             OFDM Modulator, called from the executor - it must be thread safe.
  /// \param[in] amplitude_control_     Amplitude controller, called from the executor - it must be thread safe.
  /// \param[in] beamforming_codebook_  Beamfoming coefficient codebook. It determines the number of antenna ports and
  ///                                   beams.
  /// \param[in] notifier_              Reference to the interface for notifying the completion of the processing.
  pdxch_baseband_modulator(subcarrier_spacing                  scs,
                           cyclic_prefix                       cp,
                           sampling_rate                       srate,
                           task_executor&                      executor_,
                           ofdm_symbol_modulator&              modulator_,
                           amplitude_controller&               amplitude_control_,
                           const beam_weights_codebook&        beamforming_codebook_,
                           pdxch_processor_modulator_notifier& notifier_) :
    logger(ocudulog::fetch_basic_logger("PHY")),
    nof_ports(beamforming_codebook_.get_nof_antennas()),
    nof_beams(beamforming_codebook_.get_nof_beams()),
    nof_symbols_per_slot(get_nsymb_per_slot(cp)),
    executor(executor_),
    modulator(modulator_),
    amplitude_control(amplitude_control_),
    beamforming_codebook(beamforming_codebook_),
    notifier(notifier_),
    cf_buffer({srate.to_kHz(), nof_ports})
  {
    unsigned nof_slots_per_subframe = get_nof_slots_per_subframe(scs);

    // Calculate symbol sizes in a subframe.
    for (unsigned i_symbol_sf = 0; i_symbol_sf != nof_symbols_per_slot * nof_slots_per_subframe; ++i_symbol_sf) {
      symbol_sizes_sf[i_symbol_sf] =
          srate.get_dft_size(scs) + cp.get_length(i_symbol_sf, scs).to_samples(srate.to_Hz());
    }
  }

  /// \brief Waits for the modulator to complete tasks.
  ///
  /// This method is necessary for ensuring that no pending task is left while stopping the downlink baseband processor.
  void wait_for_completion() const
  {
    while (current_state.load(std::memory_order_relaxed)) {
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
  }

  /// \brief Handles a transmission request.
  ///
  /// The handling of a transmission request is not successful if:
  /// - the buffer pointer is invalid; or
  /// - the modulator is busy with a previous transmission.
  ///
  /// The method logs an error if the asynchronous task executor cannot defer a modulation task. In this case, the
  /// portion of the baseband buffer is set to zero.
  ///
  /// The handling of a transmission request keeps the modulator busy until the completion of the asynchronous tasks by
  /// setting the internal state to \ref state_modulate_mask plus the total number of tasks. The modulator becomes
  /// available again after notifying the completion through the \ref pdxch_processor_modulator_notifier interface.
  ///
  /// The shared resource grid is temporally hold in the class and released upon the completion of the modulation.
  ///
  /// \param[in] buffer   Baseband buffer - stores the resultant modulated signal.
  /// \param[in] grid     Resource grid - source of the frequency domain data.
  /// \param[in] context  Processing slot context.
  /// \return True if the request is handled successfully, otherwise false.
  /// \remark An assertion is triggered if the number of ports contained in the buffer differs from the number of
  ///         antenna ports.
  /// \remark An assertion is triggered if the number of ports contained in the resource grid is
  ///         different from the number of beams.
  bool
  handle_request(baseband_gateway_buffer_ptr buffer, const shared_resource_grid& grid, resource_grid_context context)
  {
    // Skip if the given buffer is not valid.
    if (!buffer) {
      return false;
    }

    // Verify the number of ports match.
    ocudu_assert(buffer->get_nof_channels() == nof_ports, "The buffer number of ports do not match.");
    ocudu_assert(grid.get_reader().get_nof_ports() == nof_beams,
                 "The resource grid number of ports (i.e., {}) do not match with the number of beams (i.e., {})",
                 grid.get_reader().get_nof_ports(),
                 nof_beams);

    // Try transitioning to modulate plus all modulation tasks.
    uint32_t expected_state = state_idle;
    if (!current_state.compare_exchange_strong(expected_state,
                                               state_modulate_mask + nof_symbols_per_slot * nof_ports)) {
      return false;
    }

    // Prepare current context, grid and result for the asynchronous OFDM modulation.
    current_context = context;
    current_grid    = grid.copy();
    current_result  = {{}, std::move(buffer)};

    // Calculate the OFDM symbol indexes within the subframe.
    unsigned i_symbol_sf_begin = current_context.slot.subframe_slot_index() * nof_symbols_per_slot;
    unsigned i_symbol_sf_end   = i_symbol_sf_begin + nof_symbols_per_slot;

    // Calculate number of samples within the current_context.slot.
    unsigned nof_samples_slot =
        std::accumulate(symbol_sizes_sf.begin() + i_symbol_sf_begin, symbol_sizes_sf.begin() + i_symbol_sf_end, 0);

    // Prepare baseband buffer.
    current_result.buffer->resize(nof_samples_slot);

    // Prepare metrics.
    metrics_collection.resize(nof_symbols_per_slot * nof_ports);

    unsigned i_symbol_start = 0;
    for (unsigned i_symbol = 0, i_metric = 0; i_symbol != nof_symbols_per_slot; ++i_symbol) {
      // Calculate OFDM symbol within the subframe.
      unsigned i_symbol_sf = i_symbol + i_symbol_sf_begin;

      // Process each port individually.
      for (unsigned i_port = 0; i_port != nof_ports; ++i_port, ++i_metric) {
        bool success = executor.defer([this, i_symbol_sf, i_port, i_metric, i_symbol_start]() {
          unsigned symbol_size = symbol_sizes_sf[i_symbol_sf];

          // Create view to the writer offset.
          span<ci16_t> ci16_buf = current_result.buffer->get_writer()[i_port].subspan(i_symbol_start, symbol_size);
          // Get a view over the temporary buffer holding float-based complex samples.
          span<cf_t> cf_buf = cf_buffer.get_view({i_port}).subspan(i_symbol_start, symbol_size);

          // Start tracing.
          trace_point tp = ru_tracer.now();

          // Obtain the beamforming coefficients for this antenna port.
          span<const cf_t> port_weights = beamforming_codebook.get_antenna_coefficients(i_port);

          // OFDM modulation.
          modulator.modulate(cf_buf, current_grid.get_reader(), port_weights, i_symbol_sf);

          // Apply amplitude control.
          amplitude_control.process(cf_buf, cf_buf);

          // Perform signal measurements.
          metrics_collection[i_metric].avg_power  = ocuduvec::average_power(cf_buf);
          metrics_collection[i_metric].peak_power = ocuduvec::max_abs_element(cf_buf).second;
          metrics_collection[i_metric].clipping   = {ocuduvec::count_if_part_abs_greater_than(cf_buf, 0.95),
                                                     cf_buf.size()};

          // Convert complex floating-point buffer to complex integer-based.
          ocuduvec::convert(ci16_buf, cf_buf, ocuduvec::scaling_factor_cf_to_ci16);

          ru_tracer << trace_event("PDxCH modulation", tp);

          // Indicate completion of the task.
          complete_symbol_task();
        });

        if (!success) {
          logger.error(current_context.slot.sfn(),
                       current_context.slot.slot_index(),
                       "Failed to enqueue modulation task for symbol {}.",
                       i_symbol_sf);
          span<ci16_t> ci16_buf =
              current_result.buffer->get_writer()[i_port].subspan(i_symbol_start, symbol_sizes_sf[i_symbol_sf]);
          ocuduvec::zero(ci16_buf);
          complete_symbol_task();
        }
      }

      // Increment OFDM symbol sample index.
      i_symbol_start += symbol_sizes_sf[i_symbol_sf];
    }

    // The handling of the downlink transmission is successful.
    return true;
  }

private:
  /// Helper method for performing the tasks upon the completion of the modulation.
  void complete_symbol_task()
  {
    // Decrement counter.
    uint32_t prev = current_state.fetch_sub(1);
    ocudu_assert((prev & state_modulate_mask) == state_modulate_mask, "Unexpected state 0x{:08x}.", prev);

    // Skip completion actions if the modulation tasks are not completed.
    if (prev != state_completed) {
      return;
    }

    // The resource grid is no longer necessary.
    current_grid = {};

    // Gather metrics.
    sample_statistics<float> peak_power;
    sample_statistics<float> avg_power;
    uint64_t                 nof_clipped_samples     = 0;
    uint64_t                 total_processed_samples = 0;
    for (const auto& metrics : metrics_collection) {
      if (std::isnormal(metrics.peak_power) && std::isnormal(metrics.avg_power)) {
        peak_power.update(metrics.peak_power);
        avg_power.update(metrics.avg_power);
        nof_clipped_samples += metrics.clipping->nof_clipped_samples;
        total_processed_samples += metrics.clipping->nof_processed_samples;
      }
    }
    current_result.metrics.peak_power = peak_power.get_max();
    current_result.metrics.avg_power  = avg_power.get_mean();
    current_result.metrics.clipping =
        clipping_counters{.nof_clipped_samples = nof_clipped_samples, .nof_processed_samples = total_processed_samples};

    // Move context and result to the stack.
    resource_grid_context                 this_context = current_context;
    pdxch_processor_baseband::slot_result this_result  = std::move(current_result);

    // Transition modulator state to idle which becomes available for the next use.
    [[maybe_unused]] uint32_t expected_mask = current_state.exchange(state_idle);
    ocudu_assert(expected_mask == state_modulate_mask, "Unexpected state 0x{:08x}.", prev);

    // Notify completion of the OFDM modulation.
    notifier.on_modulation_completion(std::move(this_result), this_context);
  }

  /// State value for when the modulator is not processing any transmisson request.
  static constexpr uint32_t state_idle = 0x0;
  /// State mask for when the modulator is processing a transmission request and has not completed all the tasks yet.
  static constexpr uint32_t state_modulate_mask = 0x80000000;
  /// Expected state when the last task of the modulator fetch and decreases the count.
  static constexpr uint32_t state_completed = state_modulate_mask + 1;
  /// Maximum number of symbols per subframe. Used for the OFDM symbol size look-up table.
  static constexpr unsigned max_nof_symbols_per_subframe =
      MAX_NSYMB_PER_SLOT * pow2(to_numerology_value(subcarrier_spacing::kHz240));
  /// Maximum number of tasks. Used for keeping record of the metrics per symbol basis.
  static constexpr unsigned max_nof_tasks = MAX_NSYMB_PER_SLOT * MAX_PORTS;

  /// Physical layer logger. Used for logging when the executor cannot defer the modulation task.
  ocudulog::basic_logger& logger;
  /// Number of physical antenna ports.
  unsigned nof_ports;
  /// Number of beams - it must be equal to the number of the ports contained in the resource grids.
  unsigned nof_beams;
  /// Number of symbols per slot. Depends on the cyclic prefix.
  unsigned nof_symbols_per_slot;
  /// Look-up table with the OFDM symbol sizes (including the cyclic prefix) of each OFDM symbol within a subframe.
  std::array<unsigned, max_nof_symbols_per_subframe> symbol_sizes_sf;
  /// Current modulator state.
  std::atomic<unsigned> current_state = 0;
  /// Collection of metrics, each asynchronous task gets assigned a reference to this collection. These are combined
  /// once the processing is completed.
  static_vector<lower_phy_baseband_metrics, max_nof_tasks> metrics_collection;
  /// Current slot processing context. Used for reporting the completion of the modulation.
  resource_grid_context current_context;
  /// Current resource grid.
  shared_resource_grid current_grid;
  /// Keeps the reference of the current processing buffer and collects metrics.
  pdxch_processor_baseband::slot_result current_result;
  /// Executor for asynchronously modulating.
  task_executor& executor;
  /// OFDM modulator. Its implementation must be thread safe.
  ofdm_symbol_modulator& modulator;
  /// Amplitude controller. Its implementation must be thread safe.
  amplitude_controller& amplitude_control;
  /// \brief Reference to the beamforming codebook.
  ///
  /// Provides the beamforming weights for each of the resource grid ports.
  const beam_weights_codebook& beamforming_codebook;
  /// Notifier containing the completion notification callback.
  pdxch_processor_modulator_notifier& notifier;
  /// Buffer to hold complex floating-point based samples for processing.
  dynamic_tensor<2, cf_t> cf_buffer;
};

} // namespace ocudu
