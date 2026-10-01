// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_sidekiq_tx_stream.h"
#include "sidekiq_helper_functions.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_reader_view.h"
#include "ocudu/ocuduvec/conversion.h"
#include <thread>

using namespace ocudu;

/// \brief Computes the number of samples that fit in a Sidekiq RF block for each transmit port.
///
/// The number of samples per port is derived from the total number of blocks in the RF packet. In dual channel mode, a
/// single RF block carries samples for both ports.
///
/// \param[in] nof_words    Total number of words in the RF block, including the header.
/// \param[in] channel_mode Sidekiq channel mode.
/// \param[in] packed_mode  Packed mode flag.
/// \return the number of radio samples that fit in the RF block per transmit port.
static constexpr unsigned get_nof_samples_block(unsigned nof_words, skiq_chan_mode_t channel_mode, bool packed_mode)
{
  ocudu_assert(nof_words > 0, "The number of must not be zero. ");
  ocudu_assert((nof_words % SKIQ_TX_PACKET_SIZE_INCREMENT_IN_WORDS == 0),
               "The number of words (i.e., {}) is not multiple of the TX packet size increment (i.e., {}).",
               nof_words,
               SKIQ_TX_PACKET_SIZE_INCREMENT_IN_WORDS);
  // Derive the number of data words in the block.
  unsigned nof_data_words = nof_words - SKIQ_TX_HEADER_SIZE_IN_WORDS;

  // Compute the number of data words per port.
  unsigned nof_ports = (channel_mode == skiq_chan_mode_dual) ? 2 : 1;

  ocudu_assert(
      (nof_data_words % nof_ports == 0),
      "The total number of configured data words in an RF block (i.e., {}) cannot be evenly split for {} ports",
      nof_data_words,
      nof_ports);

  unsigned nof_data_words_port = nof_data_words / nof_ports;

  // Compute the number of samples per port.
  ocudu_assert(!packed_mode || (nof_data_words_port % 3 == 0),
               "In 12-bit packed mode, the number of configured data words per port (i.e., {}) must be a multiple of 3",
               nof_data_words_port);

  unsigned nof_samples_port = packed_mode ? (nof_data_words_port / 3) * 4 : nof_data_words;

  return nof_samples_port;
}

/// \brief Computes the number of data words in a Sidekiq RF block (header not included).
///
/// \param[in] nof_samples_port    Number of RF samples per port of an RF block.
/// \param[in] channel_mode        Sidekiq channel mode.
/// \param[in] packed_mode         Packed mode flag.
/// \return the number of data words of the RF block.
static constexpr unsigned
get_nof_data_words_block(unsigned nof_samples_port, skiq_chan_mode_t channel_mode, bool packed_mode)
{
  ocudu_assert(!packed_mode || (nof_samples_port % 4 == 0),
               "In 12-bit packed mode, the number of configured samples per port (i.e., {}) must be a multiple of 4.",
               nof_samples_port);

  unsigned nof_ports = (channel_mode == skiq_chan_mode_dual) ? 2 : 1;

  // Derive the number of 32-bit data words per port.
  unsigned nof_data_words_port = packed_mode ? (nof_samples_port / 4) * 3 : nof_samples_port;

  return nof_ports * nof_data_words_port;
}

radio_sidekiq_tx_stream::radio_sidekiq_tx_stream(const stream_description& description,
                                                 radio_event_notifier&     notifier_) :
  notifier(notifier_),
  card_id(description.card_id),
  tx_port_handles(description.tx_port_handles),
  channel_mode(description.channel_mode),
  alignment_buffer(tx_port_handles.size(), 0),
  last_late_check_ts(0),
  total_nof_lates({0}),
  packed_mode(description.packed_mode)
{
  // Defines the block size in multiples of 256 words.
  uint32_t nof_blocks_per_packet = 16;
  switch (description.stream_mode) {
    case skiq_rx_stream_mode_high_tput:
      nof_blocks_per_packet = 60;
      break;
    case skiq_rx_stream_mode_low_latency:
      nof_blocks_per_packet = 2;
      break;
    case skiq_rx_stream_mode_balanced:
    default:
      break;
  }

  // If using 12-bit packed mode, use a number of 256 word blocks that fulfills all the size requirements.
  if (description.packed_mode) {
    nof_blocks_per_packet = 40;
  }

  ocudu_assert((description.channel_mode == skiq_chan_mode_single) || (description.tx_port_handles.size() == 2),
               "Dual channel mode is only valid for 2 TX ports.");

  // Compute the number of samples per port in a single transmit packet.
  nof_port_samples_block = get_nof_samples_block(
      nof_blocks_per_packet * SKIQ_TX_PACKET_SIZE_INCREMENT_IN_WORDS, channel_mode, description.packed_mode);

  // Get the number of 32-bit data words per RF block given the configured number of samples.
  uint32_t rf_block_nof_data_words = get_nof_data_words_block(nof_port_samples_block, channel_mode, packed_mode);

  // Derive the number of data words in an RF block for each transmit port.
  nof_port_data_words_block =
      channel_mode == skiq_chan_mode_dual ? (rf_block_nof_data_words / 2) : rf_block_nof_data_words;

  unsigned nof_ports = tx_port_handles.size();
  for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
    skiq_tx_hdl_t port_handle = tx_port_handles[i_port];

    // Configure the data flow mode to use timestamps.
    if (skiq_write_tx_data_flow_mode(card_id, port_handle, skiq_tx_with_timestamps_data_flow_mode) != 0) {
      fmt::print("Error: Failed to set the data flow mode.\n");
      return;
    }

    // Configure the transfer mode to synchronous.
    if (skiq_write_tx_transfer_mode(card_id, port_handle, skiq_tx_transfer_mode_sync) != 0) {
      fmt::print("Error: Failed to set the Tx transfer mode.\n");
      return;
    }

    // Configure Tx block size.
    if (skiq_write_tx_block_size(card_id, port_handle, nof_port_data_words_block) != 0) {
      fmt::print("Error: Failed to set the Tx block size.\n");
      return;
    }
  }

  tx_block = skiq_tx_block_allocate(rf_block_nof_data_words);
  if (tx_block == nullptr) {
    fmt::print("Error: Failed to allocate a Sidekiq Tx block.\n");
    return;
  }

  alignment_buffer.resize(nof_port_samples_block);

  state = tx_states::SUCCESSFUL_INIT;
}

void radio_sidekiq_tx_stream::transmit_block(const baseband_gateway_buffer_reader& data,
                                             baseband_gateway_timestamp            timestamp)
{
  ocudu_assert(
      data.get_nof_samples() == nof_port_samples_block,
      "The provided number of samples (i.e. {}) does not match the number of samples in a radio block (i.e. {}).",
      data.get_nof_samples(),
      nof_port_samples_block);
  ocudu_assert(data.get_nof_channels() == tx_port_handles.size(),
               "The data number of ports (i.e. {}) does not match the number of configured card ports (i.e. {})",
               data.get_nof_channels(),
               tx_port_handles.size());

  skiq_tx_set_block_timestamp(tx_block, timestamp);

  // Transmit on all ports.
  if (channel_mode == skiq_chan_mode_dual) {
    // Use the secondary handle to transmit on both ports.
    auto& secondary_handle = tx_port_handles.back();
    // Views over the buffer of samples to transmit.
    span<const ci16_t> port0_data = data.get_channel_buffer(0);
    span<const ci16_t> port1_data = data.get_channel_buffer(1);

    if (packed_mode) {
      // View over the RF block data words in 32-bit format.
      span<uint32_t> block_samples(reinterpret_cast<uint32_t*>(tx_block->data), 2 * nof_port_data_words_block);

      // Convert from ci16_t to 12 bit integers and pack into the data words.
      convert_ci16_to_i12(block_samples.first(nof_port_data_words_block), port0_data);
      convert_ci16_to_i12(block_samples.last(nof_port_data_words_block), port1_data);
    } else {
      // Number of 16-bit I and Q samples for a single port.
      unsigned nof_iq_samples_port = 2 * nof_port_samples_block;

      // View over the entire RF block.
      span<ci16_t> block_samples(reinterpret_cast<ci16_t*>(tx_block->data), nof_iq_samples_port);

      // Convert and write samples for port 0.
      ocuduvec::copy(block_samples.first(nof_iq_samples_port), port0_data);
      // Convert and write samples for port 1.
      ocuduvec::copy(block_samples.last(nof_iq_samples_port), port1_data);
    }

    // Transmit the RF block.
    if (skiq_transmit(card_id, secondary_handle, tx_block, nullptr)) {
      fmt::print("Error: failed to transmit sample block for card {} in dual channel mode\n", card_id);
    }

  } else {
    // Transmit on a port basis.
    for (unsigned i_port = 0, nof_ports = data.get_nof_channels(); i_port != nof_ports; ++i_port) {
      // View over the input samples.
      span<const ci16_t> port_data = data.get_channel_buffer(i_port);

      if (packed_mode) {
        // View over the RF block data words in 32-bit format.
        span<uint32_t> block_samples(reinterpret_cast<uint32_t*>(tx_block->data), nof_port_data_words_block);

        // Convert from ci16_t to 12 bit integers and pack into the data words.
        convert_ci16_to_i12(block_samples, port_data);
      } else {
        // Convert to fixed point.
        span<ci16_t> block_samples(reinterpret_cast<ci16_t*>(tx_block->data), port_data.size());
        ocuduvec::copy(block_samples, port_data);
      }

      // Transmit the RF block.
      if (skiq_transmit(card_id, tx_port_handles[i_port], tx_block, nullptr)) {
        fmt::print("Error: failed to transmit sample block for card {} port {}\n", card_id, i_port);
      }
    }
  }
}

void radio_sidekiq_tx_stream::transmit(const baseband_gateway_buffer_reader&        data,
                                       const baseband_gateway_transmitter_metadata& tx_md)
{
  // If waiting for a stream stop, do not transmit any more samples and transition into stopped state.
  if (state.load() == tx_states::WAIT_STOP) {
    // Stop the TX stream.
    if (channel_mode == skiq_chan_mode_dual) {
      skiq_tx_hdl_t secondary_handle = tx_port_handles.back();
      if (skiq_stop_tx_streaming(card_id, secondary_handle)) {
        fmt::print("Error stopping Tx stream {} in dual channel mode\n", card_id);
        return;
      }
    } else {
      for (auto& handle : tx_port_handles) {
        if (skiq_stop_tx_streaming(card_id, handle)) {
          fmt::print("Error stopping Tx stream {}:{}\n", card_id, static_cast<unsigned>(handle));
          return;
        }
      }
    }
    // Handle transition into the STOPPED state.
    on_stream_stop();
    return;
  }

  if (state.load() != tx_states::IN_BURST) {
    // Attempt to transition to IN_BURST. Will fail if the current state is not SUCCESSFUL_INIT.
    radio_sidekiq_tx_stream::tx_states expected_state = tx_states::SUCCESSFUL_INIT;
    bool                               to_in_burst = state.compare_exchange_weak(expected_state, tx_states::IN_BURST);
    if (!to_in_burst) {
      return;
    }

    // If transitioning to IN_BURST was successful, start the TX stream for all ports.
    if (channel_mode == skiq_chan_mode_dual) {
      // Use the secondary handle to start the stream on both ports.
      auto& secondary_handle = tx_port_handles.back();
      if (skiq_start_tx_streaming(card_id, secondary_handle)) {
        fmt::print("Error starting Tx stream {} in dual channel mode\n", card_id);
        return;
      }
    } else {
      for (auto& handle : tx_port_handles) {
        // Start streaming for every port.
        if (skiq_start_tx_streaming(card_id, handle)) {
          fmt::print("Error starting Tx stream {}:{}\n", card_id, static_cast<unsigned>(handle));
          return;
        }
      }
    }
  }

  // Offset for reading from the input data.
  unsigned nof_data_samples = data.get_nof_samples();
  unsigned reading_offset   = 0;

  while (reading_offset != nof_data_samples) {
    unsigned nof_remaining_samples = nof_data_samples - reading_offset;
    // If the alignment buffer is empty, it is possible to transmit input samples directly.
    if (alignment_buffer.is_empty()) {
      // Transmit as many full radio blocks as possible.
      unsigned nof_full_blocks_input = nof_remaining_samples / nof_port_samples_block;
      for (unsigned i_block = 0; i_block != nof_full_blocks_input;
           ++i_block, reading_offset += nof_port_samples_block) {
        baseband_gateway_buffer_reader_view block_data(data, reading_offset, nof_port_samples_block);
        transmit_block(block_data, tx_md.ts + static_cast<baseband_gateway_timestamp>(reading_offset));
      }
    }

    // Add as many samples as possible to the alignment buffer.
    nof_remaining_samples = nof_data_samples - reading_offset;
    if (nof_remaining_samples != 0) {
      // View over the remaining input data samples.
      baseband_gateway_buffer_reader_view block_data(data, reading_offset, nof_remaining_samples);
      // Append samples into the alignment buffer.
      reading_offset +=
          alignment_buffer.append(block_data, tx_md.ts + static_cast<baseband_gateway_timestamp>(reading_offset));

      // Transmit the contents of the alignment buffer if it is full.
      if (alignment_buffer.is_full()) {
        baseband_gateway_timestamp            tx_timestamp;
        const baseband_gateway_buffer_reader& alignment_buffer_reader = alignment_buffer.read(tx_timestamp);
        transmit_block(alignment_buffer_reader, tx_timestamp);

        // Invalidate the current buffer contents.
        alignment_buffer.clear();
      }
    }
  }

  if (tx_md.ts > last_late_check_ts + (nof_port_samples_block * tx_late_check_period)) {
    // Detect and notify late events.
    detect_handle_lates();
    // Update last check TS.
    last_late_check_ts = tx_md.ts;
  }
}

unsigned radio_sidekiq_tx_stream::get_buffer_size() const
{
  return nof_port_samples_block;
}

void radio_sidekiq_tx_stream::stop()
{
  // Attempt to transition from IN_BURST to WAIT_STOP.
  radio_sidekiq_tx_stream::tx_states expected_state = tx_states::IN_BURST;
  bool to_wait_stop = state.compare_exchange_weak(expected_state, radio_sidekiq_tx_stream::tx_states::WAIT_STOP);

  // It the stream was not in IN_BURST state, transition directly to STOPPED.
  if (!to_wait_stop) {
    on_stream_stop();
  }
}

void radio_sidekiq_tx_stream::wait_stop()
{
  while (state.load() != radio_sidekiq_tx_stream::tx_states::STOPPED) {
    std::this_thread::sleep_for(std::chrono::microseconds(10));
  }
}

void radio_sidekiq_tx_stream::on_stream_stop()
{
  radio_sidekiq_tx_stream::tx_states stop_state = state.exchange(radio_sidekiq_tx_stream::tx_states::STOPPED);
  ocudu_assert(stop_state != radio_sidekiq_tx_stream::tx_states::IN_BURST, "Invalid state to stop the stream.");

  // Deallocate the Tx buffer.
  if (tx_block) {
    skiq_tx_block_free(tx_block);
  }
}

bool radio_sidekiq_tx_stream::init_successful()
{
  return state.load() == tx_states::SUCCESSFUL_INIT;
}

void radio_sidekiq_tx_stream::detect_handle_lates()
{
  // Handle late timestamps events.
  for (auto port : tx_port_handles) {
    uint32_t current_port_lates;
    if (skiq_read_tx_num_late_timestamps(card_id, port, &current_port_lates)) {
      fmt::print("Error reading lates from port {}:{}\n", card_id, static_cast<unsigned>(port));
    }

    // Detect new lates.
    uint32_t new_port_lates = current_port_lates - total_nof_lates[port];

    if (new_port_lates > 0) {
      // Update late counter for the current port.
      total_nof_lates[port] = current_port_lates;

      // Create radio event and send to notifier.
      radio_event_notifier::event_description event_description;
      event_description.stream_id  = card_id;
      event_description.channel_id = port;
      event_description.source     = radio_event_source::TRANSMIT;
      event_description.type       = radio_event_type::LATE;
      notifier.on_radio_rt_event(event_description);
    }
  }
}
