// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_sidekiq_rx_stream.h"
#include "sidekiq_helper_functions.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_writer_view.h"
#include "fmt/format.h"
#include <thread>

using namespace ocudu;

// Receive data word size.
static constexpr units::bytes rx_data_word_size(4);

radio_sidekiq_rx_stream::radio_sidekiq_rx_stream(const stream_description& description,
                                                 radio_event_notifier&     notifier_) :
  notifier(notifier_),
  card_id(description.card_id),
  rx_port_handles(description.rx_port_handles),
  alignment_buffer(rx_port_handles.size(), 0),
  packed_mode(description.packed_mode)
{
  for (unsigned i_port = 0, nof_ports = rx_port_handles.size(); i_port != nof_ports; ++i_port) {
    skiq_rx_hdl_t port_handle = rx_port_handles[i_port];

    // Enabling DC offset correction can cause an IQ impairment. This is not supported in some devices, in this case,
    // it returns an error. Ignore the error.
    skiq_write_rx_dc_offset_corr(card_id, port_handle, false);

    // Set manual Rx gain mode.
    if (skiq_write_rx_gain_mode(card_id, port_handle, skiq_rx_gain_manual)) {
      fmt::print("Error: failed to set Rx gain mode.\n");
      return;
    }
  }
  // Rx block size in bytes including the header.
  int32_t rx_block_size = skiq_read_rx_block_size(card_id, description.stream_mode);

  // Size of the Rx data words in a block in bytes.
  units::bytes block_size(rx_block_size - SKIQ_RX_HEADER_SIZE_IN_BYTES);

  ocudu_assert((block_size.value() % rx_data_word_size.value() == 0),
               "Rx block size (i.e. {}) must contain an integer number of data words of size {}",
               block_size,
               rx_data_word_size);

  // Number of data words in the RF block.
  nof_data_words_block = block_size.value() / rx_data_word_size.value();

  ocudu_assert(!packed_mode || (nof_data_words_block % 3 == 0),
               "In 12-bit packed mode, the number of configured data words per port (i.e., {}) must be a multiple of 3",
               nof_data_words_block);

  // Number of samples per RF block.
  nof_samples_block = packed_mode ? (nof_data_words_block / 3) * 4 : nof_data_words_block;

  alignment_buffer.resize(nof_samples_block);

  state = rx_states::SUCCESSFUL_INIT;
}

unsigned radio_sidekiq_rx_stream::get_buffer_size() const
{
  return nof_samples_block;
}

bool radio_sidekiq_rx_stream::receive_block(skiq_rx_block_t** rx_block, skiq_rx_hdl_t* rx_port_handle)
{
  bool     ret = false;
  uint32_t block_size;

  // Attempt to receive an RF block.
  skiq_rx_status_t rx_status = skiq_receive(static_cast<uint8_t>(card_id), rx_port_handle, rx_block, &block_size);

  switch (rx_status) {
    case skiq_rx_status_success:
      if ((*rx_port_handle < rx_port_handles.size()) && (rx_block != nullptr) &&
          (block_size > SKIQ_RX_HEADER_SIZE_IN_BYTES)) {
        // Compute the number of data words given the received block size.
        uint32_t nof_data_words = (block_size / rx_data_word_size.value()) - SKIQ_RX_HEADER_SIZE_IN_WORDS;
        ocudu_assert(nof_data_words == nof_data_words_block,
                     "the number of received data words (i.e. {}) does not match the expected (i.e.) {}",
                     nof_data_words,
                     nof_data_words_block);
        ret = true;
      } else {
        fmt::print("Card {} received data with corrupted pointers\n", card_id);
      }
      break;
    case skiq_rx_status_no_data:
      // Do nothing.
      break;
    case skiq_rx_status_error_generic:
      fmt::print("Error: Generic error occurred during skiq_receive.\n");
      break;
    case skiq_rx_status_error_overrun:
      // Handle the overflow event.
      handle_rx_overflow();
      break;
    case skiq_rx_status_error_packet_malformed:
      fmt::print("Error: packet malformed error occurred during skiq_receive.\n");
      break;
    case skiq_rx_status_error_card_not_active:
      fmt::print("Error: inactive card error occurred during skiq_receive.\n");
      break;
    case skiq_rx_status_error_not_streaming:
    default:
      fmt::print("Error: not streaming card error occurred during skiq_receive.\n");
      break;
  }

  return ret;
}

baseband_gateway_receiver::metadata radio_sidekiq_rx_stream::receive(baseband_gateway_buffer_writer& data)
{
  baseband_gateway_receiver::metadata ret_md = {};

  // If waiting for a stream stop, do not receive any more samples and transition into stopped state.
  if (state.load() == rx_states::WAIT_STOP) {
    on_stream_stop();
    return ret_md;
  }

  // Do nothing if the stream has not started.
  if (state.load() != rx_states::STREAMING) {
    return ret_md;
  }

  unsigned nof_ports = rx_port_handles.size();
  ocudu_assert(data.get_nof_channels() == nof_ports,
               "The input data number of ports does not match the number of active card ports.");

  unsigned writing_offset     = 0;
  unsigned nof_output_samples = data.get_nof_samples();

  ocudu_assert(nof_output_samples != 0, "The baseband buffer number of samples cannot be zero.");

  while (writing_offset != nof_output_samples) {
    // If there is no data in the alignment buffer, generate new data by receiving samples from the radio.
    if (alignment_buffer.is_empty()) {
      // Remaining samples to write to the output.
      unsigned remaining_samples = nof_output_samples - writing_offset;
      // If the number of samples in an RF block exceeds the remaining output buffer space, use the alignment buffer.
      bool use_alignment_buffer = remaining_samples < nof_samples_block;

      unsigned                                 nof_received_ports = 0;
      std::array<bool, RADIO_MAX_NOF_CHANNELS> rx_port_status     = {false};

      while (nof_received_ports != nof_ports) {
        skiq_rx_block_t* p_rx_block = nullptr;
        skiq_rx_hdl_t    curr_rx_hdl;

        // Attempt to receive a single Rx block.
        bool good_rx = receive_block(&p_rx_block, &curr_rx_hdl);

        // Ignore the RF block if the Rx operation was unsuccessful.
        if (!good_rx) {
          continue;
        }

        if (rx_port_status[curr_rx_hdl]) {
          fmt::print("Card {} received data for port {} twice\n", card_id, fmt::underlying(curr_rx_hdl));
        } else {
          ++nof_received_ports;
        }

        auto current_timestamp = static_cast<baseband_gateway_timestamp>(p_rx_block->rf_timestamp);

        // Write the current received port status.
        rx_port_status[curr_rx_hdl] = true;

        // Write the port samples into the buffer.
        span<ci16_t> port_data;
        // Write into the output directly if there is space for an entire Rx block.
        if (use_alignment_buffer) {
          // View of the alignment buffer for the current port.
          port_data = alignment_buffer.write_port(curr_rx_hdl, current_timestamp);
        } else {
          // Get a view of the output buffer.
          port_data = data.get_channel_buffer(curr_rx_hdl).subspan(writing_offset, nof_samples_block);

          // Set the output timestamp if writing the first sample to the output buffer.
          if (writing_offset == 0) {
            ret_md.ts = current_timestamp;
          }
        }

        // Convert samples and write into the selected buffer.
        if (packed_mode) {
          // View over the data words.
          span<const uint32_t> block_samples(
              reinterpret_cast<const uint32_t*>(const_cast<const int16_t*>(p_rx_block->data)), nof_data_words_block);

          // Unpack and convert from 12-bit integer to ci16_t.
          convert_i12_to_ci16(port_data, block_samples);
        } else {
          span<const ci16_t> block_samples(
              reinterpret_cast<const ci16_t*>(const_cast<const int16_t*>(p_rx_block->data)), nof_samples_block);
          ocuduvec::copy(port_data, block_samples);
        }
      }

      // Increment the writing offset if writing directly to the output.
      if (!use_alignment_buffer) {
        writing_offset += nof_samples_block;
      }
    }

    // Write the contents of the alignment buffer into the output.
    if (!alignment_buffer.is_empty()) {
      baseband_gateway_timestamp current_out_ts;

      unsigned remaining_nof_samples = nof_output_samples - writing_offset;

      ocudu_assert(remaining_nof_samples > 0, "No remaining samples to write.");

      // Get a view of the output buffer for the remaining number of samples.
      baseband_gateway_buffer_writer_view data_writer(data, writing_offset, remaining_nof_samples);

      // The output buffer view will not be filled entirely if there are not enough samples in the buffer.
      unsigned read_count = alignment_buffer.read(data_writer, current_out_ts);

      // Set the output timestamp if writing the first output sample.
      if (writing_offset == 0) {
        ret_md.ts = current_out_ts;
      }

      // If the timestamp of the samples in the alignment buffer does not match the output writing timestamp,
      // handle it as an overflow.
      if (current_out_ts != ret_md.ts + static_cast<baseband_gateway_timestamp>(writing_offset)) {
        fmt::print(
            "The timestamp of the samples in the RX alignment buffer (i.e. {}) does not match the expected value "
            "(i.e. {})\n",
            current_out_ts,
            ret_md.ts + static_cast<baseband_gateway_timestamp>(writing_offset));
        handle_rx_overflow();
      }

      // Increment writing offset.
      writing_offset += read_count;
    }
  }

  return ret_md;
}

void radio_sidekiq_rx_stream::start(baseband_gateway_timestamp init_time)
{
  if (state != rx_states::SUCCESSFUL_INIT) {
    return;
  }

  rx_states expected_state = rx_states::SUCCESSFUL_INIT;
  bool      to_streaming   = state.compare_exchange_weak(expected_state, rx_states::STREAMING);

  ocudu_assert(to_streaming, "Failed to transition into the streaming state.");

  // Start all Rx ports.
  if (to_streaming) {
    if (skiq_start_rx_streaming_multi_on_trigger(
            card_id, rx_port_handles.data(), rx_port_handles.size(), skiq_trigger_src_synced, init_time)) {
      fmt::print("Failed to start card {} Rx streaming\n", card_id);
    }
  }
}

void radio_sidekiq_rx_stream::stop()
{
  // Attempt to transition from STREAMING to WAIT_STOP.
  rx_states expected_state = rx_states::STREAMING;
  bool      to_wait_stop   = state.compare_exchange_weak(expected_state, rx_states::WAIT_STOP);

  // It the stream was not in STREAMING state, transition directly to STOPPED.
  if (!to_wait_stop) {
    on_stream_stop();
  }
}

void radio_sidekiq_rx_stream::wait_stop()
{
  while (state.load() != rx_states::STOPPED) {
    std::this_thread::sleep_for(std::chrono::microseconds(10));
  }
}

void radio_sidekiq_rx_stream::on_stream_stop()
{
  rx_states stop_state = state.exchange(rx_states::STOPPED);
  ocudu_assert(stop_state != rx_states::STREAMING, "Invalid state to reset the stream.");

  // Stop all Rx ports.
  if (skiq_stop_rx_streaming_multi_immediate(card_id, rx_port_handles.data(), rx_port_handles.size())) {
    fmt::print("Failed to stop card {} Rx streaming\n", card_id);
    return;
  }
}

bool radio_sidekiq_rx_stream::init_successful()
{
  return state == rx_states::SUCCESSFUL_INIT;
}

void radio_sidekiq_rx_stream::handle_rx_overflow() const
{
  radio_event_notifier::event_description event;
  event.stream_id = card_id;
  event.source    = radio_event_source::RECEIVE;
  event.type      = radio_event_type::OVERFLOW;

  // Sidekiq RF block buffer is common for all receive ports.
  event.channel_id = 0;

  notifier.on_radio_rt_event(event);
}
