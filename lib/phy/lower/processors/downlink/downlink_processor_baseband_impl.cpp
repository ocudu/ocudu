// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "downlink_processor_baseband_impl.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_writer_view.h"
#include "ocudu/instrumentation/traces/ru_traces.h"
#include "ocudu/ocuduvec/conversion.h"
#include "ocudu/ocuduvec/dot_prod.h"
#include "ocudu/ocuduvec/zero.h"
#include "ocudu/phy/lower/lower_phy_baseband_metrics.h"
#include "ocudu/phy/lower/lower_phy_timing_context.h"

using namespace ocudu;

downlink_processor_baseband_impl::downlink_processor_baseband_impl(
    pdxch_processor_baseband&                        pdxch_proc_baseband_,
    const downlink_processor_baseband_configuration& config) :
  pdxch_proc_baseband(pdxch_proc_baseband_),
  nof_slot_tti_in_advance(config.nof_slot_tti_in_advance),
  nof_slot_tti_in_advance_ns(config.nof_slot_tti_in_advance * 1000000 /
                             slot_point(config.scs, 0).nof_slots_per_subframe()),
  sector_id(config.sector_id),
  rate(config.rate),
  scs(config.scs),
  nof_samples_per_subframe(config.rate.to_kHz()),
  nof_slots_per_subframe(get_nof_slots_per_subframe(config.scs)),
  nof_symbols_per_slot(get_nsymb_per_slot(config.cp)),
  temp_buffer(config.nof_tx_ports, 2 * config.rate.get_dft_size(config.scs)),
  cfo_processor(config.rate),
  buffer_pool(nof_slots_per_subframe * NOF_SUBFRAMES_PER_FRAME, config.nof_tx_ports, nof_samples_per_subframe)
{
  unsigned symbol_size_no_cp        = config.rate.get_dft_size(config.scs);
  unsigned nof_symbols_per_subframe = nof_symbols_per_slot * nof_slots_per_subframe;

  // Setup symbol sizes.
  symbol_sizes_sf.reserve(nof_symbols_per_subframe);
  for (unsigned i_symbol = 0; i_symbol != nof_symbols_per_subframe; ++i_symbol) {
    unsigned cp_size = config.cp.get_length(i_symbol, config.scs).to_samples(config.rate.to_Hz());
    symbol_sizes_sf.emplace_back(cp_size + symbol_size_no_cp);
  }
}

/// Fills the unprocessed regions of a baseband buffer with zeros, according to the downlink processor baseband
/// metadata.
static void fill_zeros(baseband_gateway_buffer_writer& buffer, const baseband_gateway_transmitter_metadata& md)
{
  // If discontinuous mode is disabled, fill the non-processed regions with zeros and report full buffer metadata.
  if (md.is_empty) {
    for (unsigned i_channel = 0, i_channel_end = buffer.get_nof_channels(); i_channel != i_channel_end; ++i_channel) {
      ocuduvec::zero(buffer.get_channel_buffer(i_channel));
    }
    return;
  }

  if (md.tx_start.has_value()) {
    for (unsigned i_channel = 0, i_channel_end = buffer.get_nof_channels(); i_channel != i_channel_end; ++i_channel) {
      ocuduvec::zero(buffer.get_channel_buffer(i_channel).first(*md.tx_start));
    }
  }
  if (md.tx_end.has_value()) {
    for (unsigned i_channel = 0, i_channel_end = buffer.get_nof_channels(); i_channel != i_channel_end; ++i_channel) {
      ocuduvec::zero(buffer.get_channel_buffer(i_channel).last(buffer.get_nof_samples() - *md.tx_end));
    }
  }
}

/// Fills the unprocessed destination buffer with the last samples in the source.
static void fill_buffer_from_tail(baseband_gateway_buffer_writer&       destination,
                                  const baseband_gateway_buffer_reader& source)
{
  ocudu_assert((destination.get_nof_samples() <= source.get_nof_samples()) &&
                   (destination.get_nof_channels() == source.get_nof_channels()),
               "Unmatch buffer dimensions.");
  unsigned nof_samples = destination.get_nof_samples();
  for (unsigned i_channel = 0, i_channel_end = destination.get_nof_channels(); i_channel != i_channel_end;
       ++i_channel) {
    ocuduvec::copy(destination[i_channel], source[i_channel].last(nof_samples));
  }
}

downlink_processor_baseband::processing_result
downlink_processor_baseband_impl::process(baseband_gateway_timestamp timestamp)
{
  // Calculate an adjusted timestamp for the samples to be generated. The transmit time offset is subtracted from
  // the requested buffer timestamp to shift the signal in time. The generated signal will then be stored in the
  // destination buffer according to the timestamp at which it should be transmitted.
  baseband_gateway_timestamp proc_timestamp_offset  = timestamp;
  int                        current_tx_time_offset = tx_time_offset.load(std::memory_order::memory_order_relaxed);
  if ((current_tx_time_offset < 0) ||
      (static_cast<baseband_gateway_timestamp>(current_tx_time_offset) < proc_timestamp_offset)) {
    // Make sure the subtraction does not overflow.
    proc_timestamp_offset -= current_tx_time_offset;
  }

  // Calculate the subframe index within a hyper-system frame.
  auto i_sf = static_cast<unsigned>((proc_timestamp_offset / nof_samples_per_subframe) %
                                    (NOF_HYPER_SFNS * NOF_SFNS * NOF_SUBFRAMES_PER_FRAME));
  // Calculate the sample index within the subframe.
  unsigned i_sample_sf = proc_timestamp_offset % nof_samples_per_subframe;

  // Calculate symbol index within the subframe and the sample index within the OFDM symbol.
  unsigned i_sample_symbol = i_sample_sf;
  unsigned i_symbol_sf     = 0;
  while (i_sample_symbol >= symbol_sizes_sf[i_symbol_sf]) {
    i_sample_symbol -= symbol_sizes_sf[i_symbol_sf];
    ++i_symbol_sf;
  }

  // Calculate system slot index and the symbol index within the slot.
  unsigned i_slot_sf = i_symbol_sf / nof_symbols_per_slot;
  unsigned i_slot    = i_sf * nof_slots_per_subframe + i_slot_sf;

  // Calculate the number of samples in the slot.
  span<const unsigned> symbol_sizes_slot =
      span<const unsigned>(symbol_sizes_sf).subspan(i_slot_sf * nof_symbols_per_slot, nof_symbols_per_slot);
  unsigned nof_samples_slot = std::accumulate(symbol_sizes_slot.begin(), symbol_sizes_slot.end(), 0U);

  // Calculate the sample index from the beginning of the slot.
  span<const unsigned> symbol_sizes_before_slot =
      span<const unsigned>(symbol_sizes_sf).first(i_slot_sf * nof_symbols_per_slot);
  unsigned i_sample_slot =
      i_sample_sf - std::accumulate(symbol_sizes_before_slot.begin(), symbol_sizes_before_slot.end(), 0U);

  // Note that the slot could be equal to the previous slot if tx_time_offset was modified. So, the processor notifies
  // the slot boundary only if no previous slot has been processed before or the new slot is different from the
  // previous.
  pdxch_processor_baseband::slot_result pdxch_baseband_result;
  if (slot_point_extended slot(scs, i_slot); !previous_slot.has_value() || (*previous_slot != slot)) {
    ocudu_assert(notifier != nullptr, "Timing notifier is not connected.");
    trace_point tp = ru_tracer.now();
    notifier->on_tti_boundary(
        lower_phy_timing_context{.slot       = slot + nof_slot_tti_in_advance,
                                 .time_point = std::chrono::system_clock::now() + nof_slot_tti_in_advance_ns});
    previous_slot = slot;
    ru_tracer << trace_event("on_tti_boundary", tp);

    // Obtain the downlink baseband processing for the slot independently of the sample alignment. This avoids leaving
    // resource grids in the PDxCH processor.
    pdxch_baseband_result = pdxch_proc_baseband.process_slot({.slot = slot.without_hyper_sfn(), .sector = sector_id});
  }

  // Handle CFO and metrics if the PDxCH baseband result contains a buffer.
  if (pdxch_baseband_result.buffer) {
    // Apply carrier frequency offset for the entire transmit slot buffer.
    cfo_processor.next_cfo_command();
    cfo_processor.process(pdxch_baseband_result.buffer->get_writer());

    // Notify metrics.
    ocudu_assert(notifier != nullptr, "Timing notifier is not connected.");
    notifier->on_new_metrics(pdxch_baseband_result.metrics);
  }

  // Align with the next slot using a different baseband buffer if the next sample is not aligned with the beginning of
  // a slot or no PDxCH baseband is available to transmit.
  if ((i_sample_slot != 0) || !pdxch_baseband_result.buffer) {
    // Prepare result metadata and obtain baseband buffer from the pool.
    processing_result result = {.metadata = {.ts = timestamp, .is_empty = true}, .buffer = buffer_pool.get()};
    report_fatal_error_if_not(result.buffer, "Failed to retrieve a baseband buffer.");

    // Calculate the number of samples to the next slot.
    unsigned nof_samples_to_next_slot = nof_samples_slot - i_sample_slot;

    // Resize buffer and zero.
    result.buffer->resize(nof_samples_to_next_slot);

    // Fill the baseband buffer with the generated PDxCH if possible. Otherwise, fill it with zeros.
    if (pdxch_baseband_result.buffer) {
      fill_buffer_from_tail(result.buffer->get_writer(), pdxch_baseband_result.buffer->get_reader());
      result.metadata.is_empty = false;
    } else {
      fill_zeros(result.buffer->get_writer(), result.metadata);
      result.metadata.is_empty = true;
    }

    return result;
  }

  // Prepare result metadata.
  return processing_result{.metadata = {.ts = timestamp, .is_empty = false},
                           .buffer   = std::move(pdxch_baseband_result.buffer)};
}

void downlink_processor_baseband_impl::set_tx_time_offset(phy_time_unit tx_time_offset_)
{
  tx_time_offset.store(tx_time_offset_.to_nearest_samples(rate.to_Hz()), std::memory_order_relaxed);
}
