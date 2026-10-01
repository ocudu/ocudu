// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_reader.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/phy/lower/modulation/ofdm_prach_demodulator.h"
#include "ocudu/phy/lower/processors/uplink/prach/prach_processor_baseband.h"
#include "ocudu/phy/lower/processors/uplink/prach/prach_processor_notifier.h"
#include "ocudu/phy/lower/sampling_rate.h"
#include "ocudu/phy/support/prach_buffer.h"
#include "ocudu/phy/support/prach_buffer_context.h"
#include "ocudu/phy/support/shared_prach_buffer.h"
#include "ocudu/ran/phy_time_unit.h"
#include "ocudu/ran/prach/prach_constants.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/synchronization/stop_event.h"

namespace ocudu {

/// \brief Lower PHY PRACH processor worker subcomponent.
///
/// The worker contains a finite-state machine with four possible states: idle, wait, collecting and processing. See
/// \ref states for more information.
///
/// The implementation assumes that:
/// - handle_request() and is_available() are called from a first thread, and
/// - process_symbol() is called from a second, different thread.
class prach_processor_worker
{
  /// Worker internal states.
  enum class states {
    /// The worker has not been configured yet with any request or the previously configured request has already been
    /// processed. It transitions to \c wait when a request is set.
    idle,
    /// A PRACH request is configured and the worker is waiting for the start of the PRACH occasion window. The worker
    /// transitions to \c collecting if it detects the beginning of the PRACH occasion window for the configured PRACH
    /// context. When in this state, the worker also detects late requests, in which case it notifies the event and
    /// transitions back to \c idle.
    wait,
    /// The PRACH occasion window started and baseband samples are collected. Once the internal buffer has been filled
    /// with the PRACH occasion window samples, the worker transitions to \c processing for the demodulation phase.
    collecting,
    /// The PRACH occasion has been captured and it is processed by the asynchronous task executor. The worker
    /// transitions to \c idle as soon as the PRACH occasion is processed and notified.
    processing
  };

  /// PHY logger.
  ocudulog::basic_logger& logger;
  /// OFDM PRACH demodulator.
  std::unique_ptr<ofdm_prach_demodulator> demodulator;
  /// Asynchronous task executor.
  task_executor& async_task_executor;
  /// Sampling rate in Hz.
  unsigned sampling_rate_Hz = 0;
  /// Temporary baseband buffer of size \ref TEMP_BASEBAND_BUFFER_SIZE.
  baseband_gateway_buffer_dynamic temp_baseband;
  /// PRACH processor notifier.
  prach_processor_notifier* notifier = nullptr;
  /// Current worker state.
  std::atomic<states> state = {states::idle};
  /// Current context.
  prach_buffer_context prach_context;
  /// Current PRACH buffer.
  shared_prach_buffer buffer;
  /// Current PRACH occasion window length.
  unsigned window_length = 0;
  /// Current number of collected samples.
  unsigned nof_samples = 0;
  /// Manager to handle the stop process.
  rt_stop_event_source stop_manager;

  /// Runs state \c wait.
  void run_state_wait(const baseband_gateway_buffer_reader&           samples,
                      const prach_processor_baseband::symbol_context& context,
                      rt_stop_event_token                             token);

  /// Runs state \c collecting.
  void run_state_collecting(const baseband_gateway_buffer_reader&           samples,
                            const prach_processor_baseband::symbol_context& context,
                            rt_stop_event_token                             token);

  /// Accumulates \c samples in the internal buffer.
  void accumulate_samples(const baseband_gateway_buffer_reader& samples, rt_stop_event_token token);

public:
  /// Creates a PRACH processor worker.
  prach_processor_worker(std::unique_ptr<ofdm_prach_demodulator> demodulator_,
                         task_executor&                          async_task_executor_,
                         sampling_rate                           srate,
                         unsigned                                max_nof_ports) :
    logger(ocudulog::fetch_basic_logger("PHY")),
    demodulator(std::move(demodulator_)),
    async_task_executor(async_task_executor_),
    sampling_rate_Hz(srate.to_Hz()),
    temp_baseband(max_nof_ports, prach_constants::MAX_WINDOW_LENGTH.to_samples(sampling_rate_Hz))
  {
    ocudu_assert(sampling_rate_Hz && prach_constants::MAX_WINDOW_LENGTH.is_sample_accurate(sampling_rate_Hz),
                 "Invalid sampling rate of {} Hz.",
                 sampling_rate_Hz);
  }

  /// Connects the worker with the given PRACH processor notifier.
  void connect(prach_processor_notifier& notifier_) { notifier = &notifier_; }

  /// \brief Handles a PRACH occasion request.
  /// \param[in] buffer  PRACH buffer.
  /// \param[in] context PRACH occasion context.
  void handle_request(shared_prach_buffer buffer, const prach_buffer_context& context);

  /// \brief Processes an OFDM symbol.
  /// \param[in] samples Baseband samples.
  /// \param[in] context OFDM symbol context.
  void process_symbol(const baseband_gateway_buffer_reader&           samples,
                      const prach_processor_baseband::symbol_context& context);

  /// \brief Determines whether the PRACH processor worker is available.
  ///
  /// A PRACH processor is available when it is \c idle. See \ref states for more information regarding the PRACH worker
  /// internal states.
  bool is_available() const { return state == states::idle; }

  /// Stops operation of the PRACH processor worker.
  void stop();
};

} // namespace ocudu
