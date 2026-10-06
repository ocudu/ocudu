// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../../../../support/resource_grid_test_doubles.h"
#include "../../../modulation/ofdm_modulator_test_doubles.h"
#include "pdxch_processor_notifier_test_doubles.h"
#include "support/compare_sequences.h"
#include "ocudu/adt/format.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/phy/antenna_ports.h"
#include "ocudu/phy/lower/processors/downlink/downlink_processor_baseband.h"
#include "ocudu/phy/lower/processors/downlink/downlink_processor_factories.h"
#include "ocudu/phy/lower/processors/downlink/pdxch/pdxch_processor_baseband.h"
#include "ocudu/phy/lower/processors/downlink/pdxch/pdxch_processor_request_handler.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include "ocudu/support/math/math_utils.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>
#include <random>

using namespace ocudu;

namespace ocudu {

std::ostream& operator<<(std::ostream& os, antenna_topology ant_topology)
{
  fmt::print(os, "{}", to_string(ant_topology));
  return os;
}

std::ostream& operator<<(std::ostream& os, subcarrier_spacing scs)
{
  fmt::print(os, "{}", to_string(scs));
  return os;
}

std::ostream& operator<<(std::ostream& os, sampling_rate srate)
{
  fmt::print(os, "{} MHz", srate.to_MHz());
  return os;
}

std::ostream& operator<<(std::ostream& os, cyclic_prefix cp)
{
  fmt::print(os, "{}", cp.to_string());
  return os;
}

std::ostream& operator<<(std::ostream& os, span<const cf_t> data)
{
  fmt::print(os, "{}", data);
  return os;
}

std::ostream& operator<<(std::ostream& os, const pdxch_processor_baseband::slot_context& context)
{
  fmt::print(os, "{} {}", context.slot, context.sector);
  return os;
}

std::ostream& operator<<(std::ostream& os, const ofdm_modulator_configuration& config)
{
  fmt::print(os,
             "Numerology={} BW={} DftSize={} CP={} Scale={} CenterFreq={}Hz",
             config.numerology,
             config.bw_rb,
             config.dft_size,
             config.cp.to_string(),
             config.scale,
             config.center_freq_Hz);
  return os;
}

bool operator==(const pdxch_processor_baseband::slot_context& left, const pdxch_processor_baseband::slot_context& right)
{
  return (left.slot == right.slot) && (left.sector == right.sector);
}

bool operator==(const ofdm_modulator_configuration& left, const ofdm_modulator_configuration& right)
{
  return (left.numerology == right.numerology) && (left.bw_rb == right.bw_rb) && (left.dft_size == right.dft_size) &&
         (left.cp == right.cp) && (left.scale == right.scale) && (left.center_freq_Hz == right.center_freq_Hz);
}

bool operator==(const baseband_gateway_buffer_reader& left, const baseband_gateway_buffer_reader& right)
{
  if (left.get_nof_channels() != right.get_nof_channels()) {
    return false;
  }
  unsigned nof_channels = left.get_nof_channels();

  for (unsigned i_channel = 0; i_channel != nof_channels; ++i_channel) {
    span<const ci16_t> left_channel  = left.get_channel_buffer(i_channel);
    span<const ci16_t> right_channel = right.get_channel_buffer(i_channel);
    if (!std::equal(left_channel.begin(), left_channel.end(), right_channel.begin(), right_channel.end())) {
      return false;
    }
  }

  return true;
}

} // namespace ocudu

using LowerPhyDownlinkProcessorParams = std::tuple<antenna_topology, sampling_rate, subcarrier_spacing, cyclic_prefix>;

namespace {

/// Baseband gain back-off in decibels.
constexpr float gain_backoff_dB = 12.0F;

class LowerPhyDownlinkProcessorFixture : public ::testing::TestWithParam<LowerPhyDownlinkProcessorParams>
{
protected:
  static void SetUpTestSuite()
  {
    ocudulog::init();

    if (pdxch_proc_factory == nullptr) {
      ofdm_mod_factory_spy = std::make_shared<ofdm_modulator_factory_spy>();
      ASSERT_NE(ofdm_mod_factory_spy, nullptr);

      pdxch_proc_factory = create_pdxch_processor_factory_sw(ofdm_mod_factory_spy, gain_backoff_dB);
      ASSERT_NE(pdxch_proc_factory, nullptr);
    }
  }

  LowerPhyDownlinkProcessorFixture() :
    ::testing::TestWithParam<LowerPhyDownlinkProcessorParams>(),
    rg_spy(rg_reader_spy, rg_writer_spy),
    shared_rg_spy(rg_spy),
    modulation_executor(MAX_NSYMB_PER_SLOT * MAX_PORTS)
  {
  }

  void SetUp() override
  {
    ASSERT_NE(pdxch_proc_factory, nullptr);

    // Select parameters.
    tx_ant_topology = std::get<0>(GetParam());
    srate           = std::get<1>(GetParam());
    scs             = std::get<2>(GetParam());
    cp              = std::get<3>(GetParam());

    nof_symbols_per_slot   = get_nsymb_per_slot(cp);
    nof_slots_per_subframe = get_nof_slots_per_subframe(scs);
    nof_slots_per_frame    = nof_slots_per_subframe * NOF_SUBFRAMES_PER_FRAME;

    bandwidth_rb   = dist_bandwidth_prb(rgen);
    center_freq_Hz = dist_center_freq_Hz(rgen);

    // Prepare configurations.
    pdxch_processor_configuration config = {.cp              = cp,
                                            .scs             = scs,
                                            .srate           = srate,
                                            .bandwidth_rb    = bandwidth_rb,
                                            .center_freq_Hz  = center_freq_Hz,
                                            .tx_ant_topology = tx_ant_topology};

    // Create processor.
    pdxch_proc = pdxch_proc_factory->create(config, modulation_executor);
    ASSERT_NE(pdxch_proc, nullptr);

    // Select OFDM modulator processor spy.
    ofdm_mod_spy = ofdm_mod_factory_spy->get_modulators().back();
  }

  shared_resource_grid get_shared_grid()
  {
    unsigned nof_tx_ports = get_total_nof_ports(tx_ant_topology);
    unsigned nof_tx_beams = get_total_nof_beams(tx_ant_topology);

    rg_reader_spy.reset(nof_tx_beams, MAX_NSYMB_PER_SLOT, MAX_NOF_PRBS);

    // Add a single resource grid entry per port. This makes the grid non-empty on all ports.
    for (unsigned i_port = 0; i_port != nof_tx_ports; ++i_port) {
      resource_grid_reader_spy::expected_entry_t entry;
      entry.port       = i_port;
      entry.symbol     = 0;
      entry.subcarrier = 0;
      entry.value      = cf_t(0.0F, 0.0F);
      rg_reader_spy.write(entry);
    }

    return shared_rg_spy.get_grid();
  }

  static constexpr unsigned                          nof_frames_test    = 3;
  static constexpr unsigned                          initial_slot_index = 0;
  static std::mt19937                                rgen;
  static std::uniform_int_distribution<unsigned>     dist_sector_id;
  static std::uniform_int_distribution<unsigned>     dist_bandwidth_prb;
  static std::uniform_real_distribution<double>      dist_center_freq_Hz;
  static std::uniform_real_distribution<float>       dist_sample;
  static std::shared_ptr<ofdm_modulator_factory_spy> ofdm_mod_factory_spy;
  static std::shared_ptr<pdxch_processor_factory>    pdxch_proc_factory;

  resource_grid_reader_spy rg_reader_spy;
  resource_grid_writer_spy rg_writer_spy;
  resource_grid_spy        rg_spy;
  shared_resource_grid_spy shared_rg_spy;

  cyclic_prefix      cp;
  subcarrier_spacing scs;
  sampling_rate      srate;
  unsigned           bandwidth_rb;
  double             center_freq_Hz;
  antenna_topology   tx_ant_topology;
  unsigned           nof_symbols_per_slot;
  unsigned           nof_slots_per_subframe;
  unsigned           nof_slots_per_frame;

  std::unique_ptr<pdxch_processor> pdxch_proc   = nullptr;
  ofdm_symbol_modulator_spy*       ofdm_mod_spy = nullptr;
  manual_task_worker               modulation_executor;
};

std::mt19937                                LowerPhyDownlinkProcessorFixture::rgen(0);
std::uniform_int_distribution<unsigned>     LowerPhyDownlinkProcessorFixture::dist_sector_id(0, 16);
std::uniform_int_distribution<unsigned>     LowerPhyDownlinkProcessorFixture::dist_bandwidth_prb(1, MAX_NOF_PRBS);
std::uniform_real_distribution<double>      LowerPhyDownlinkProcessorFixture::dist_center_freq_Hz(1e8, 6e9);
std::uniform_real_distribution<float>       LowerPhyDownlinkProcessorFixture::dist_sample(-1, 1);
std::shared_ptr<ofdm_modulator_factory_spy> LowerPhyDownlinkProcessorFixture::ofdm_mod_factory_spy = nullptr;
std::shared_ptr<pdxch_processor_factory>    LowerPhyDownlinkProcessorFixture::pdxch_proc_factory   = nullptr;

} // namespace

TEST_P(LowerPhyDownlinkProcessorFixture, ModulatorConfiguration)
{
  // The modulator scale normalizes the signal power according to the number of subcarriers and applies the back-off.
  float expected_scale = convert_dB_to_amplitude(
      -convert_power_to_dB(static_cast<float>(bandwidth_rb * NOF_SUBCARRIERS_PER_RB)) - gain_backoff_dB);

  ofdm_modulator_configuration expected_mod_config = {.numerology     = to_numerology_value(scs),
                                                      .bw_rb          = bandwidth_rb,
                                                      .dft_size       = srate.get_dft_size(scs),
                                                      .cp             = cp,
                                                      .scale          = expected_scale,
                                                      .center_freq_Hz = center_freq_Hz};

  ASSERT_EQ(ofdm_mod_spy->get_configuration(), expected_mod_config);
}

TEST_P(LowerPhyDownlinkProcessorFixture, FlowNoRequest)
{
  // Create notifiers and connect.
  pdxch_processor_notifier_spy pdxch_proc_notifier_spy;
  pdxch_proc->connect(pdxch_proc_notifier_spy);

  for (slot_point slot_begin{to_numerology_value(scs), initial_slot_index},
       slot(slot_begin),
       slot_end = slot_begin + NOF_SUBFRAMES_PER_FRAME * nof_slots_per_subframe * nof_frames_test;
       slot != slot_end;
       ++slot) {
    // Clear spies.
    pdxch_proc_notifier_spy.clear_notifications();
    ofdm_mod_spy->clear_modulate_entries();

    // Prepare expected PDxCH baseband entry context.
    pdxch_processor_baseband::slot_context context = {.slot = slot, .sector = dist_sector_id(rgen)};

    // Process baseband.
    baseband_gateway_buffer_ptr buffer = pdxch_proc->get_baseband().process_slot(context);

    // Verify buffer pointer.
    ASSERT_EQ(buffer, nullptr);

    // Assert OFDM modulator is not called.
    ASSERT_TRUE(ofdm_mod_spy->get_modulate_entries().empty());

    // Assert notification.
    ASSERT_EQ(pdxch_proc_notifier_spy.get_nof_notifications(), 0);
  }
}

TEST_P(LowerPhyDownlinkProcessorFixture, FlowFloodRequest)
{
  unsigned nof_tx_ports = get_total_nof_ports(tx_ant_topology);
  unsigned nof_tx_beams = get_total_nof_beams(tx_ant_topology);
  unsigned sector_id    = dist_sector_id(rgen);

  // Create notifiers and connect.
  pdxch_processor_notifier_spy pdxch_proc_notifier_spy;
  pdxch_proc->connect(pdxch_proc_notifier_spy);

  // Gets a grid.
  shared_resource_grid shared_rg = get_shared_grid();

  // Process a few slots.
  for (slot_point slot_begin{to_numerology_value(scs), initial_slot_index},
       slot(slot_begin),
       slot_end = slot_begin + NOF_SUBFRAMES_PER_FRAME * nof_slots_per_subframe * nof_frames_test;
       slot != slot_end;
       ++slot) {
    // Clear spies.
    pdxch_proc_notifier_spy.clear_notifications();
    ofdm_mod_spy->clear_modulate_entries();

    // Prepare expected PDxCH baseband entry context.
    resource_grid_context                  req_context  = {.slot = slot, .sector = sector_id};
    pdxch_processor_baseband::slot_context proc_context = {.slot = slot, .sector = sector_id};

    // Request resource grid modulation for the current slot.
    pdxch_proc->get_request_handler().handle_request(shared_rg.copy(), req_context);

    // Modulate.
    unsigned count = 0;
    for (; modulation_executor.try_run_next(); ++count) {
    }
    ASSERT_EQ(count, nof_symbols_per_slot * nof_tx_ports);

    // Assert OFDM modulator call.
    const auto& ofdm_mod_entries = ofdm_mod_spy->get_modulate_entries();
    ASSERT_EQ(ofdm_mod_entries.size(), nof_symbols_per_slot * nof_tx_ports);
    for (unsigned i_symbol = 0, i_symbol_subframe = nof_symbols_per_slot * slot.subframe_slot_index();
         i_symbol != nof_symbols_per_slot;
         ++i_symbol, ++i_symbol_subframe) {
      for (unsigned i_port = 0; i_port != nof_tx_ports; ++i_port) {
        const auto& ofdm_mod_entry = ofdm_mod_entries[i_symbol * nof_tx_ports + i_port];
        ASSERT_EQ(static_cast<const void*>(ofdm_mod_entry.grid), static_cast<const void*>(&rg_reader_spy));
        ASSERT_EQ(ofdm_mod_entry.port_weights.size(), nof_tx_beams);
        for (unsigned j = 0; j != nof_tx_ports; ++j) {
          if (j == i_port) {
            ASSERT_EQ(ofdm_mod_entry.port_weights[j].real(), 1.0f);
            ASSERT_EQ(ofdm_mod_entry.port_weights[j].imag(), 0.0f);
          } else {
            ASSERT_EQ(ofdm_mod_entry.port_weights[j].real(), 0.0f);
            ASSERT_EQ(ofdm_mod_entry.port_weights[j].imag(), 0.0f);
          }
        }
        ASSERT_EQ(ofdm_mod_entry.symbol_index, i_symbol_subframe);
      }
    }

    // Process baseband.
    auto result = pdxch_proc->get_baseband().process_slot(proc_context);

    // Assert the baseband buffer contains the OFDM modulator output without further processing.
    ASSERT_TRUE(result);
    for (unsigned i_port = 0; i_port != nof_tx_ports; ++i_port) {
      span<const ci16_t> channel = result->get_reader().get_channel_buffer(i_port);
      for (unsigned i_symbol = 0; i_symbol != nof_symbols_per_slot; ++i_symbol) {
        span<const ci16_t> expected = ofdm_mod_entries[i_symbol * nof_tx_ports + i_port].output;
        ASSERT_GE(channel.size(), expected.size());
        error_type<std::string> compare_result = compare_sequences(channel.first(expected.size()), expected);
        ASSERT_TRUE(compare_result.has_value()) << compare_result.error();
        channel = channel.last(channel.size() - expected.size());
      }
    }

    // Assert notification.
    ASSERT_EQ(pdxch_proc_notifier_spy.get_request_late().size(), 0);
  }
}

TEST_P(LowerPhyDownlinkProcessorFixture, LateRequest)
{
  unsigned nof_tx_ports     = get_total_nof_ports(tx_ant_topology);
  unsigned sector_id        = dist_sector_id(rgen);
  unsigned modulation_count = 0;

  // Create notifiers and connect.
  pdxch_processor_notifier_spy pdxch_proc_notifier_spy;
  pdxch_proc->connect(pdxch_proc_notifier_spy);

  // Gets a grid.
  shared_resource_grid shared_rg = get_shared_grid();

  // Initial request.
  resource_grid_context initial_rg_context;
  initial_rg_context.slot   = slot_point(to_numerology_value(scs), initial_slot_index);
  initial_rg_context.sector = sector_id;
  pdxch_proc->get_request_handler().handle_request(shared_rg.copy(), initial_rg_context);
  for (; modulation_executor.try_run_next(); ++modulation_count) {
  }

  // Late request.
  resource_grid_context late_rg_context;
  late_rg_context.slot   = initial_rg_context.slot - 1;
  late_rg_context.sector = sector_id;
  pdxch_proc->get_request_handler().handle_request(shared_rg.copy(), late_rg_context);
  for (; modulation_executor.try_run_next(); ++modulation_count) {
  }

  // Next request.
  resource_grid_context next_rg_context;
  next_rg_context.slot   = initial_rg_context.slot + 1;
  next_rg_context.sector = sector_id;
  pdxch_proc->get_request_handler().handle_request(shared_rg.copy(), next_rg_context);
  for (; modulation_executor.try_run_next(); ++modulation_count) {
  }

  // Verify the number of modulation tasks matches with the requests.
  ASSERT_EQ(modulation_count, 3 * nof_symbols_per_slot * nof_tx_ports);

  // Clear spies.
  pdxch_proc_notifier_spy.clear_notifications();
  ofdm_mod_spy->clear_modulate_entries();

  // Process a few slots.
  for (slot_point slot_begin{to_numerology_value(scs), initial_slot_index},
       slot(slot_begin),
       slot_end = slot_begin + NOF_SUBFRAMES_PER_FRAME * nof_slots_per_subframe * nof_frames_test;
       slot != slot_end;
       ++slot) {
    // Prepare expected PDxCH baseband entry context.
    pdxch_processor_baseband::slot_context proc_context = {.slot = slot, .sector = sector_id};

    // Process baseband.
    baseband_gateway_buffer_ptr buffer = pdxch_proc->get_baseband().process_slot(proc_context);

    // Assert results. Only two of the three request must have been processed.
    if ((slot == initial_rg_context.slot) || (slot == next_rg_context.slot)) {
      ASSERT_NE(buffer, nullptr);
    } else {
      ASSERT_EQ(buffer, nullptr);
    }
  }

  // Assert notifications. Only one late must have been detected.
  const auto& lates = pdxch_proc_notifier_spy.get_request_late();
  ASSERT_EQ(lates.size(), 1);
  ASSERT_EQ(lates.front().slot, late_rg_context.slot);
  ASSERT_EQ(lates.front().sector, late_rg_context.sector);
}

TEST_P(LowerPhyDownlinkProcessorFixture, OverflowWithRequest)
{
  // Maximum number of requests queued in the processor.
  static constexpr unsigned max_nof_concurrent_requests = 16;

  unsigned nof_tx_ports = get_total_nof_ports(tx_ant_topology);
  unsigned nof_tx_beams = get_total_nof_beams(tx_ant_topology);
  unsigned sector_id    = dist_sector_id(rgen);

  // Create notifiers and connect.
  pdxch_processor_notifier_spy pdxch_proc_notifier_spy;
  pdxch_proc->connect(pdxch_proc_notifier_spy);

  shared_resource_grid shared_rg = get_shared_grid();
  ofdm_mod_spy->clear_modulate_entries();

  // Generate requests.
  for (unsigned i_request = 0; i_request != max_nof_concurrent_requests + 1; ++i_request) {
    // Request grid.
    resource_grid_context rg_context = {.slot   = {to_numerology_value(scs), initial_slot_index + i_request},
                                        .sector = sector_id};
    if (i_request < max_nof_concurrent_requests) {
      pdxch_proc->get_request_handler().handle_request(shared_rg.copy(), rg_context);
    } else {
      pdxch_proc->get_request_handler().handle_request(shared_rg.copy(), rg_context);
    }

    // Verify the number of modulation tasks matches the number of symbols in the slot.
    unsigned count = 0;
    for (; modulation_executor.try_run_next(); ++count) {
    }
    ASSERT_EQ(count, nof_symbols_per_slot * nof_tx_ports) << "i_request=" << i_request;

    // Verify the current number of lates.
    unsigned nof_expected_lates =
        (i_request >= max_nof_concurrent_requests) ? (i_request + 1 - max_nof_concurrent_requests) : 0;
    ASSERT_EQ(pdxch_proc_notifier_spy.get_request_late().size(), nof_expected_lates);
  }

  // Assert OFDM modulator call only for the request enqueued.
  const auto& ofdm_mod_entries = ofdm_mod_spy->get_modulate_entries();
  ASSERT_EQ(ofdm_mod_entries.size(), (max_nof_concurrent_requests + 1) * nof_symbols_per_slot * nof_tx_ports);
  for (unsigned i_slot = 0, i_symbol_subframe = 0, entry_index = 0; i_slot != nof_slots_per_subframe; ++i_slot) {
    for (unsigned i_symbol = 0; i_symbol != nof_symbols_per_slot; ++i_symbol, ++i_symbol_subframe) {
      for (unsigned i_port = 0; i_port != nof_tx_ports; ++i_port, ++entry_index) {
        const auto& ofdm_mod_entry = ofdm_mod_entries[entry_index];
        ASSERT_EQ(static_cast<const void*>(ofdm_mod_entry.grid), static_cast<const void*>(&rg_reader_spy));
        // Verify only the expected port has a non-zero weight.
        ASSERT_EQ(ofdm_mod_entry.port_weights.size(), nof_tx_beams);
        for (unsigned j = 0; j != nof_tx_ports; ++j) {
          if (j == i_port) {
            ASSERT_EQ(ofdm_mod_entry.port_weights[j].real(), 1.0f);
            ASSERT_EQ(ofdm_mod_entry.port_weights[j].imag(), 0.0f);
          } else {
            ASSERT_EQ(ofdm_mod_entry.port_weights[j].real(), 0.0f);
            ASSERT_EQ(ofdm_mod_entry.port_weights[j].imag(), 0.0f);
          }
        }
        ASSERT_EQ(ofdm_mod_entry.symbol_index, i_symbol_subframe);
      }
    }
  }

  // Process a few slots.
  for (slot_point slot_begin{to_numerology_value(scs), initial_slot_index + 1},
       slot(slot_begin),
       slot_end = slot_begin + NOF_SUBFRAMES_PER_FRAME * nof_slots_per_subframe * nof_frames_test;
       slot != slot_end;
       ++slot) {
    // Clear spies.
    pdxch_proc_notifier_spy.clear_notifications();
    ofdm_mod_spy->clear_modulate_entries();

    // Prepare expected PDxCH baseband entry context.
    pdxch_processor_baseband::slot_context proc_context = {.slot = slot, .sector = sector_id};

    // Process baseband.
    baseband_gateway_buffer_ptr tx_buffer = pdxch_proc->get_baseband().process_slot(proc_context);

    // Process baseband.
    if (static_cast<unsigned>(slot - slot_begin) < max_nof_concurrent_requests) {
      ASSERT_NE(tx_buffer, nullptr);
    } else {
      ASSERT_EQ(tx_buffer, nullptr);
    }

    // Assert notifications.
    ASSERT_EQ(pdxch_proc_notifier_spy.get_request_late().size(), 0);
  }
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(LowerPhyDownlinkProcessor,
                         LowerPhyDownlinkProcessorFixture,
                         ::testing::Combine(::testing::Values(antenna_topology::one_port,
                                                              antenna_topology::two_port,
                                                              antenna_topology::four_ports,
                                                              antenna_topology::eight_ports),
                                            ::testing::Values(sampling_rate::from_MHz(3.84),
                                                              sampling_rate::from_MHz(7.68)),
                                            ::testing::Values(subcarrier_spacing::kHz15, subcarrier_spacing::kHz30),
                                            ::testing::Values(cyclic_prefix::NORMAL, cyclic_prefix::EXTENDED)));
