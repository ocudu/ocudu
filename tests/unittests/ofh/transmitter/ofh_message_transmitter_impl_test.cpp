// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "transmitter/ofh_message_transmitter_impl.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ofh;

namespace {

/// Ethernet transmitter spy that records the size and the contents of every sent burst of Ethernet frames.
class ethernet_transmitter_spy : public ether::transmitter
{
public:
  void send(span<span<const uint8_t>> eth_frames) override
  {
    burst_sizes.push_back(eth_frames.size());
    for (span<const uint8_t> eth_frame : eth_frames) {
      sent_eth_frames.emplace_back(eth_frame.begin(), eth_frame.end());
    }
  }

  ether::transmitter_metrics_collector* get_metrics_collector() override { return nullptr; }

  std::vector<unsigned>             burst_sizes;
  std::vector<std::vector<uint8_t>> sent_eth_frames;
};

} // namespace

static constexpr unsigned nof_symbols    = 14;
static constexpr unsigned eth_frame_size = 64;

static std::shared_ptr<ether::eth_frame_pool> create_pool(ocudulog::basic_logger& logger, message_type type)
{
  static constexpr units::bytes mtu{1500};
  static constexpr unsigned     nof_eth_frames_per_symbol = 6;
  return std::make_shared<ether::eth_frame_pool>(
      logger, mtu, nof_eth_frames_per_symbol, type, data_direction::downlink);
}

/// Creates transmission windows where the User-Plane one spans the given number of symbols.
static tx_window_timing_parameters create_timing_parameters(unsigned nof_up_symbols)
{
  return tx_window_timing_parameters{
      .sym_cp_dl_start = 0,
      .sym_cp_dl_end   = 0,
      .sym_cp_ul_start = 0,
      .sym_cp_ul_end   = 0,
      .sym_up_dl_start = nof_up_symbols - 1,
      .sym_up_dl_end   = 0,
  };
}

TEST(ofh_message_transmitter_impl, interval_exceeding_the_burst_size_is_sent_in_several_bursts)
{
  static constexpr unsigned nof_eth_frames_per_symbol = 200;
  static constexpr unsigned nof_symbols_in_window     = 2;
  static constexpr unsigned total_nof_eth_frames      = nof_eth_frames_per_symbol * nof_symbols_in_window;
  static_assert(total_nof_eth_frames > ether::MAX_TX_BURST_SIZE,
                "The Ethernet frames of the window must not fit in a single burst");

  ocudulog::basic_logger& logger = ocudulog::fetch_basic_logger("TEST");

  auto                      pool_dl_up = create_pool(logger, message_type::user_plane);
  auto                      eth_tx     = std::make_unique<ethernet_transmitter_spy>();
  ethernet_transmitter_spy& eth_spy    = *eth_tx;

  // The Control-Plane pools stay empty, so only User-Plane Ethernet frames are transmitted.
  message_transmitter_impl transmitter(logger,
                                       create_timing_parameters(nof_symbols_in_window),
                                       false,
                                       std::move(eth_tx),
                                       create_pool(logger, message_type::control_plane),
                                       create_pool(logger, message_type::control_plane),
                                       pool_dl_up);

  // Fill the window of two symbols.
  slot_symbol_point symbol_point(slot_point(0, 0), 0, nof_symbols);
  for (unsigned i_eth_frame = 0; i_eth_frame != total_nof_eth_frames; ++i_eth_frame) {
    auto buffer = pool_dl_up->reserve(symbol_point + i_eth_frame / nof_eth_frames_per_symbol);
    ASSERT_TRUE(buffer) << "Ethernet frame pool ran out of buffers";
    span<uint8_t> data = buffer->get_buffer().first(eth_frame_size);
    std::fill(data.begin(), data.end(), 0);
    // Tag each Ethernet frame with its index.
    data[0] = static_cast<uint8_t>(i_eth_frame & 0xff);
    data[1] = static_cast<uint8_t>(i_eth_frame >> 8);
    buffer->set_size(eth_frame_size);
  }

  transmitter.on_new_symbol({symbol_point, 0, {}});

  // Every Ethernet frame is transmitted once and in order, split in bursts that never exceed the maximum size.
  ASSERT_EQ(eth_spy.sent_eth_frames.size(), total_nof_eth_frames);
  ASSERT_EQ(eth_spy.burst_sizes.size(), 2);
  ASSERT_EQ(eth_spy.burst_sizes[0], ether::MAX_TX_BURST_SIZE);

  for (unsigned i_eth_frame = 0; i_eth_frame != total_nof_eth_frames; ++i_eth_frame) {
    const std::vector<uint8_t>& eth_frame = eth_spy.sent_eth_frames[i_eth_frame];
    ASSERT_EQ(eth_frame.size(), eth_frame_size);
    ASSERT_EQ(eth_frame[0] | (eth_frame[1] << 8), i_eth_frame);
  }

  // The transmitted buffers are returned to the pool, hence nothing is sent on the next symbol.
  transmitter.on_new_symbol({symbol_point + 1, 0, {}});
  ASSERT_EQ(eth_spy.sent_eth_frames.size(), total_nof_eth_frames);
}

TEST(ofh_message_transmitter_impl, exactly_one_full_burst_is_sent_once)
{
  ocudulog::basic_logger& logger = ocudulog::fetch_basic_logger("TEST");

  auto                      pool_dl_up = create_pool(logger, message_type::user_plane);
  auto                      eth_tx     = std::make_unique<ethernet_transmitter_spy>();
  ethernet_transmitter_spy& eth_spy    = *eth_tx;

  message_transmitter_impl transmitter(logger,
                                       create_timing_parameters(1),
                                       false,
                                       std::move(eth_tx),
                                       create_pool(logger, message_type::control_plane),
                                       create_pool(logger, message_type::control_plane),
                                       pool_dl_up);

  // The pending Ethernet frames fill the burst exactly.
  slot_symbol_point symbol_point(slot_point(0, 0), 0, nof_symbols);
  for (unsigned i_eth_frame = 0; i_eth_frame != ether::MAX_TX_BURST_SIZE; ++i_eth_frame) {
    auto buffer = pool_dl_up->reserve(symbol_point);
    ASSERT_TRUE(buffer) << "Ethernet frame pool ran out of buffers";
    buffer->set_size(eth_frame_size);
  }

  transmitter.on_new_symbol({symbol_point, 0, {}});

  // A single full burst is sent, without an extra empty one.
  ASSERT_EQ(eth_spy.burst_sizes.size(), 1);
  ASSERT_EQ(eth_spy.burst_sizes[0], ether::MAX_TX_BURST_SIZE);
}
