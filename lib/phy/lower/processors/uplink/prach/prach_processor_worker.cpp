// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "prach_processor_worker.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_reader_view.h"
#include "ocudu/instrumentation/traces/ru_traces.h"
#include "ocudu/ocuduvec/copy.h"
#include "ocudu/ran/prach/prach_preamble_information.h"

using namespace ocudu;

void prach_processor_worker::run_state_wait(const baseband_gateway_buffer_reader&           samples,
                                            const prach_processor_baseband::symbol_context& context,
                                            rt_stop_event_token                             token)
{
  // Check if the context slot is in the past even if the sector/port does not match.
  if ((context.slot > prach_context.slot) || (context.slot == prach_context.slot && context.symbol > 0)) {
    // Notify a late PRACH request.
    notifier->on_prach_request_late(prach_context);

    // Release the ownership of the buffer.
    buffer.reset();

    // Transition to idle.
    state = states::idle;

    // Early return.
    return;
  }

  // Ignore symbol if the sector and port do not match with the prach_context.
  if (context.sector != prach_context.sector) {
    return;
  }

  // The slot has not reached the beginning of the PRACH window.
  if (context.slot != prach_context.slot) {
    return;
  }

  // Reset the number of collected samples.
  nof_samples = 0;

  // Otherwise, transition to process CP.
  state = states::collecting;

  // Accumulate the samples for the first segment.
  accumulate_samples(samples, std::move(token));
}

void prach_processor_worker::run_state_collecting(const baseband_gateway_buffer_reader&           samples,
                                                  const prach_processor_baseband::symbol_context& context,
                                                  rt_stop_event_token                             token)
{
  // Ignore symbol if the sector and port do not match with the prach_context.
  if (context.sector != prach_context.sector) {
    return;
  }

  // Accumulate samples of the n-th segment.
  accumulate_samples(samples, std::move(token));
}

void prach_processor_worker::accumulate_samples(const baseband_gateway_buffer_reader& samples,
                                                rt_stop_event_token                   token)
{
  // Select number of samples to append.
  unsigned count = std::min(window_length - nof_samples, samples.get_nof_samples());

  unsigned nof_ports = prach_context.ports.size();
  for (uint8_t i_channel = 0; i_channel != nof_ports; ++i_channel) {
    // Get PRACH port identifier.
    unsigned i_port = prach_context.ports[i_channel];
    ocudu_assert(i_port < samples.get_nof_channels(),
                 "The port identifier (i.e., {}) exceeds the number of input ports (i.e., {}).",
                 i_port,
                 samples.get_nof_channels());

    // PRACH buffer destination buffer view.
    span<ci16_t> dst_prach_buffer =
        temp_baseband.get_writer().get_channel_buffer(i_channel).subspan(nof_samples, count);

    // Append samples in temporary buffer.
    ocuduvec::copy(dst_prach_buffer, samples.get_channel_buffer(i_port).first(count));
  }

  // Increment number of samples.
  nof_samples += count;

  // Keep collecting if the number of samples has not reached the window length.
  if (nof_samples < window_length) {
    return;
  }

  // Otherwise, transition to processing.
  state = states::processing;

  if (!async_task_executor.defer([this, nof_ports, moved_token = std::move(token)]() {
        trace_point tp = ru_tracer.now();

        for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
          // Prepare PRACH demodulator configuration.
          ofdm_prach_demodulator::configuration config = {.slot             = prach_context.slot,
                                                          .format           = prach_context.format,
                                                          .nof_td_occasions = prach_context.nof_td_occasions,
                                                          .nof_fd_occasions = prach_context.nof_fd_occasions,
                                                          .start_symbol     = prach_context.start_symbol,
                                                          .rb_offset        = prach_context.rb_offset,
                                                          .nof_prb_ul_grid  = prach_context.nof_prb_ul_grid,
                                                          .port             = i_port};

          // Make a view of the first samples in the buffer.
          baseband_gateway_buffer_reader_view buffered_samples(temp_baseband.get_reader(), 0, nof_samples);

          // Demodulate all candidates.
          demodulator->demodulate(*buffer, buffered_samples.get_channel_buffer(i_port), config);
        }

        ru_tracer << trace_event("prach_demodulate", tp);

        // Notify PRACH window reception.
        notifier->on_rx_prach_window(std::move(buffer), prach_context);

        // Transition to idle.
        state = states::idle;
      })) {
    logger.warning(
        prach_context.slot.sfn(), prach_context.slot.slot_index(), "Unable to dispatch PRACH demodulation task");
  }
}

void prach_processor_worker::handle_request(shared_prach_buffer request_buffer, const prach_buffer_context& context)
{
  ocudu_assert(state == states::idle, "Invalid state.");

  auto token = stop_manager.get_token();

  if (OCUDU_UNLIKELY(token.is_stop_requested())) {
    return;
  }

  prach_context = context;
  buffer        = std::move(request_buffer);

  // Calculate the PRACH window size starting at the beginning of the slot.
  window_length =
      get_prach_window_duration(context.format, context.pusch_scs, context.start_symbol, context.nof_td_occasions)
          .to_samples(sampling_rate_Hz);

  // Reset number of collected samples.
  nof_samples = 0;

  // Transition to wait for the beginning of the PRACH window.
  state = states::wait;
}

void prach_processor_worker::process_symbol(const baseband_gateway_buffer_reader&           samples,
                                            const prach_processor_baseband::symbol_context& context)
{
  auto token = stop_manager.get_token();

  if (OCUDU_UNLIKELY(token.is_stop_requested())) {
    return;
  }

  // Run FSM.
  switch (state) {
    case states::idle:
      // Do nothing.
      break;
    case states::wait:
      run_state_wait(samples, context, std::move(token));
      break;
    case states::collecting:
      run_state_collecting(samples, context, std::move(token));
      break;
    case states::processing:
      // Do nothing.
      break;
  }
}

void prach_processor_worker::stop()
{
  stop_manager.stop();
  buffer.reset();
}
