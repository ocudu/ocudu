// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "uplink_processor_impl.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_reader_view.h"
#include "ocudu/ocuduvec/copy.h"
#include "ocudu/phy/lower/lower_phy_rx_symbol_context.h"
#include "ocudu/phy/lower/lower_phy_timing_context.h"
#include "ocudu/phy/lower/processors/uplink/prach/prach_processor_baseband.h"
#include "ocudu/phy/lower/processors/uplink/puxch/puxch_processor_baseband.h"
#include "ocudu/phy/lower/processors/uplink/uplink_processor_notifier.h"
#include "ocudu/support/math/stats.h"

using namespace ocudu;

lower_phy_uplink_processor_impl::lower_phy_uplink_processor_impl(std::unique_ptr<prach_processor> prach_proc_,
                                                                 std::unique_ptr<puxch_processor> puxch_proc_,
                                                                 const configuration&             config) :
  sector_id(config.sector_id),
  scs(config.scs),
  nof_rx_ports(config.nof_rx_ports),
  nof_slots_per_subframe(get_nof_slots_per_subframe(config.scs)),
  nof_symbols_per_slot(get_nsymb_per_slot(config.cp)),
  nof_samples_per_subframe(config.rate.to_kHz()),
  nof_symbols_per_subframe(nof_symbols_per_slot * get_nof_slots_per_subframe(config.scs)),
  temp_buffer_write_index(0),
  current_symbol_index(0),
  temp_buffer(config.nof_rx_ports, 2 * config.rate.get_dft_size(config.scs)),
  prach_proc(std::move(prach_proc_)),
  puxch_proc(std::move(puxch_proc_)),
  cfo_processor(config.rate)
{
  ocudu_assert(prach_proc, "Invalid PRACH processor.");
  ocudu_assert(puxch_proc, "Invalid PUxCH processor.");

  unsigned symbol_size_no_cp = config.rate.get_dft_size(config.scs);

  // Setup symbol sizes.
  symbol_sizes.reserve(nof_symbols_per_subframe);
  unsigned sf_sample_count = 0;
  for (unsigned i_symbol = 0; i_symbol != nof_symbols_per_subframe; ++i_symbol) {
    unsigned cp_size     = config.cp.get_length(i_symbol, config.scs).to_samples(config.rate.to_Hz());
    unsigned symbol_size = cp_size + symbol_size_no_cp;
    symbol_sizes.emplace_back(symbol_size);
    sf_sample_count += symbol_size;
  }

  // Make sure the number of samples per subframe match the total number.
  report_fatal_error_if_not(sf_sample_count == nof_samples_per_subframe,
                            "The number of samples per subframe does not match the sampling rate.");
}

void lower_phy_uplink_processor_impl::connect(uplink_processor_notifier& notifier_,
                                              prach_processor_notifier&  prach_notifier,
                                              puxch_processor_notifier&  puxch_notifier)
{
  notifier = &notifier_;
  prach_proc->connect(prach_notifier);
  puxch_proc->connect(puxch_notifier);
}

prach_processor_request_handler& lower_phy_uplink_processor_impl::get_prach_request_handler()
{
  return prach_proc->get_request_handler();
}

puxch_processor_request_handler& lower_phy_uplink_processor_impl::get_puxch_request_handler()
{
  return puxch_proc->get_request_handler();
}

uplink_processor_baseband& lower_phy_uplink_processor_impl::get_baseband()
{
  return *this;
}

void lower_phy_uplink_processor_impl::process(const baseband_gateway_buffer_reader& samples,
                                              baseband_gateway_timestamp            timestamp)
{
  switch (state) {
    case fsm_states::alignment:
      process_alignment(samples, timestamp);
      break;
    case fsm_states::collecting:
      process_collecting(samples, timestamp);
      break;
  }
}

void lower_phy_uplink_processor_impl::process_alignment(const baseband_gateway_buffer_reader& samples,
                                                        baseband_gateway_timestamp            timestamp)
{
  // Calculate the sample index within a subframe.
  unsigned i_sample_sf = timestamp % nof_samples_per_subframe;
  unsigned nof_samples = samples.get_nof_samples();

  // Calculate the number of samples from the beginning of the buffer to the next subframe.
  unsigned nof_samples_next_sf = 0;
  if (i_sample_sf != 0) {
    nof_samples_next_sf = nof_samples_per_subframe - i_sample_sf;
  }

  // If the next subframe boundary is within the buffer, then process.
  if (nof_samples_next_sf < nof_samples) {
    baseband_gateway_buffer_reader_view samples2(samples, nof_samples_next_sf, nof_samples - nof_samples_next_sf);
    process_symbol_boundary(samples2, timestamp + nof_samples_next_sf);
    return;
  }

  // Otherwise, keep in state alignment.
  state = fsm_states::alignment;
}

void lower_phy_uplink_processor_impl::process_symbol_boundary(const baseband_gateway_buffer_reader& samples,
                                                              baseband_gateway_timestamp            timestamp)
{
  // Calculate the subframe index.
  unsigned i_sf = static_cast<uint64_t>((timestamp / nof_samples_per_subframe) % (NOF_SFNS * NOF_SUBFRAMES_PER_FRAME));

  // Calculate the sample index within the subframe.
  unsigned i_sample_sf = timestamp % nof_samples_per_subframe;

  // Calculate symbol index within the subframe and the sample index within the OFDM symbol.
  unsigned i_sample_symbol = i_sample_sf;
  unsigned i_symbol_sf     = 0;
  while (i_sample_symbol >= symbol_sizes[i_symbol_sf]) {
    i_sample_symbol -= symbol_sizes[i_symbol_sf];
    ++i_symbol_sf;
  }

  // If the sample is not aligned with the beginning of the OFDM symbol, align to next subframe.
  if (i_sample_symbol != 0) {
    process_alignment(samples, timestamp);
    return;
  }

  // Calculate system slot index and the symbol index within the slot.
  unsigned i_slot   = i_sf * nof_slots_per_subframe + i_symbol_sf / nof_symbols_per_slot;
  unsigned i_symbol = i_symbol_sf % nof_symbols_per_slot;

  // Create slot point.
  slot_point slot(to_numerology_value(scs), i_slot % (NOF_SFNS * NOF_SUBFRAMES_PER_FRAME * nof_slots_per_subframe));

  // Prepare current symbol context before collect samples.
  current_slot             = slot;
  current_symbol_index     = i_symbol;
  current_symbol_size      = symbol_sizes[i_symbol_sf];
  temp_buffer_write_index  = 0;
  current_symbol_timestamp = timestamp;
  temp_buffer.resize(current_symbol_size);

  if (i_symbol == 0) {
    cfo_processor.next_cfo_command();
  }

  // Process baseband.
  process_collecting(samples, timestamp);
}

void lower_phy_uplink_processor_impl::process_collecting(const baseband_gateway_buffer_reader& samples,
                                                         baseband_gateway_timestamp            timestamp)
{
  ocudu_assert(notifier != nullptr, "Notifier has not been connected.");
  ocudu_assert(nof_rx_ports == samples.get_nof_channels(), "Invalid number of channels.");

  // Check that the timestamp matches with the current sample timestamp.
  if ((current_symbol_timestamp + temp_buffer_write_index) != timestamp) {
    // If the timestamp does not match, the alignment has been lost.
    process_alignment(samples, timestamp);
    return;
  }

  // Get the number of input samples.
  unsigned nof_input_samples = samples.get_nof_samples();

  // Select the minimum among the remainder of samples to process and the number of samples to complete the buffer.
  unsigned nof_samples = std::min(nof_input_samples, current_symbol_size - temp_buffer_write_index);

  // Copy samples to a temporary buffer if the buffer requires alignment or if CFO compensation is required.
  baseband_gateway_buffer_reader_view samples_view(samples, 0, nof_samples);
  if ((nof_samples != current_symbol_size) || cfo_processor.has_cfo()) {
    // For each port, concatenate samples.
    for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
      // Select view of the temporary buffer.
      span<ci16_t> temp_buffer_dst = temp_buffer[i_port].subspan(temp_buffer_write_index, nof_samples);

      // Select view of the input samples.
      span<const ci16_t> temp_buffer_src = samples.get_channel_buffer(i_port).first(nof_samples);

      // Append input samples into the temporary buffer.
      ocuduvec::copy(temp_buffer_dst, temp_buffer_src);
    }

    // Increment the count of samples stored in the temporal buffer.
    temp_buffer_write_index += nof_samples;

    // If the temporal buffer is not full, keep state in-sync and return.
    if (temp_buffer_write_index < current_symbol_size) {
      state = fsm_states::collecting;
      return;
    }

    // Perform carrier frequency offset compensation.
    cfo_processor.process(temp_buffer.get_writer());

    // Advance CFO processor number of samples.
    cfo_processor.advance(temp_buffer.get_nof_samples());

    // Update the samples view with the temporary buffer.
    samples_view = baseband_gateway_buffer_reader_view(temp_buffer.get_reader(), 0, temp_buffer.get_nof_samples());
  }

  // Process symbol by PRACH processor.
  prach_processor_baseband::symbol_context prach_context = {
      .slot = current_slot, .symbol = current_symbol_index, .sector = sector_id};
  prach_proc->get_baseband().process_symbol(samples_view, prach_context);

  // Process symbol by PUxCH processor.
  lower_phy_rx_symbol_context puxch_context = {
      .slot = current_slot, .sector = sector_id, .nof_symbols = current_symbol_index};
  puxch_proc->get_baseband().process_symbol(samples_view, puxch_context);

  // Detect half-slot boundary.
  if (current_symbol_index == (nof_symbols_per_slot / 2) - 1) {
    // Notify half slot boundary.
    notifier->on_half_slot(lower_phy_timing_context{.slot = slot_point_extended(current_slot), .time_point = {}});
  }

  // Detect full slot boundary.
  if (current_symbol_index == nof_symbols_per_slot - 1) {
    // Notify full slot boundary.
    notifier->on_full_slot(lower_phy_timing_context{.slot = slot_point_extended(current_slot), .time_point = {}});
  }

  // Process next symbol with the remainder samples.
  baseband_gateway_buffer_reader_view samples2(samples, nof_samples, nof_input_samples - nof_samples);
  process_symbol_boundary(samples2, timestamp + nof_samples);
}

baseband_cfo_processor& lower_phy_uplink_processor_impl::get_cfo_control()
{
  return cfo_processor;
}

lower_phy_center_freq_controller& lower_phy_uplink_processor_impl::get_carrier_center_frequency_control()
{
  return puxch_proc->get_center_freq_control();
}
