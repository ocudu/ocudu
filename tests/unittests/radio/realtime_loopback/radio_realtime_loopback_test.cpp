// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_factory_realtime_loopback_impl.h"
#include "ocudu/gateways/baseband/baseband_gateway_receiver.h"
#include "ocudu/gateways/baseband/baseband_gateway_transmitter.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "ocudu/support/executors/task_worker.h"
#include "ocudu/support/math/complex_normal_random.h"
#include <gtest/gtest.h>
#include <random>

using namespace ocudu;

// Parameters:
// - Number of channels;
// - Transmit block size;
// - Receive block size;
using radio_realtime_loopback_test_parameters = std::tuple<unsigned, unsigned, unsigned>;

/// Indicates the test logging level.
static const ocudulog::basic_levels log_level = ocudulog::basic_levels::warning;

class RealtimeLoopbackRadioFixture : public ::testing::TestWithParam<radio_realtime_loopback_test_parameters>
{
protected:
  static constexpr float ASSERT_MAX_ERROR_COMPLEX = 1.414213562f;

  class radio_notifier_spy : public radio_event_notifier
  {
  public:
    void on_radio_rt_event(const event_description& description) override {}
  };

  /// Indicates the number of channels per stream.
  unsigned nof_channels;
  /// Provides the number of samples per transmission.
  unsigned tx_block_size;
  /// Provides the number of samples per reception.
  unsigned rx_block_size;
  /// Number of samples to transmit ahead of the receive timestamp.
  unsigned tx_advance_samples = 1000U;
  /// Total number of samples to transmit and receive.
  unsigned nof_samples = 10000U;

  static std::unique_ptr<radio_factory_realtime_loopback_impl> factory;
  static std::unique_ptr<task_worker>                          async_task_worker;
  std::vector<std::mt19937>                                    tx_rgen;
  std::vector<std::mt19937>                                    rx_rgen;
  complex_normal_distribution<cf_t>                            tx_dist;
  complex_normal_distribution<cf_t>                            rx_dist;

  static void SetUpTestSuite()
  {
    if (!async_task_worker) {
      async_task_worker = std::make_unique<task_worker>("async_thread", 2 * RADIO_MAX_NOF_PORTS);
    }

    if (factory) {
      return;
    }

    // Create realtime loopback radio factory.
    factory = std::make_unique<radio_factory_realtime_loopback_impl>();
    ASSERT_NE(factory, nullptr);

    ocudulog::init();
  }

  static void TearDownTestSuite() { async_task_worker->stop(); }

  static ci16_t generate_random_ci16(complex_normal_distribution<cf_t> dist, std::mt19937 gen)
  {
    static constexpr float scaling_factor = std::numeric_limits<int16_t>::max() / 4;
    return to_ci16(dist(gen) * static_cast<float>(scaling_factor));
  }

  void SetUp() override
  {
    // Actual parameters.
    nof_channels  = std::get<0>(GetParam());
    tx_block_size = std::get<1>(GetParam());
    rx_block_size = std::get<2>(GetParam());

    // Setup random generators.
    for (unsigned i_port = 0; i_port != nof_channels; ++i_port) {
      unsigned seed = 0x1234 + i_port;
      tx_rgen.emplace_back(std::mt19937(seed));
      rx_rgen.emplace_back(std::mt19937(seed));
    }
  }

  // Creates a standard radio configuration.
  radio_configuration::radio create_radio_config() const
  {
    // Prepare radio configuration.
    radio_configuration::radio  radio_config;
    radio_configuration::stream tx_stream_config;
    for (unsigned channel_id = 0; channel_id != nof_channels; ++channel_id) {
      radio_configuration::channel channel_config;
      channel_config.freq.center_frequency_Hz = 3.5e9;
      channel_config.gain_dB                  = 60.0;
      channel_config.args                     = "";
      tx_stream_config.channels.push_back(channel_config);
    }
    radio_config.tx_streams.push_back(tx_stream_config);

    radio_configuration::stream rx_stream_config;
    for (unsigned channel_id = 0; channel_id != nof_channels; ++channel_id) {
      radio_configuration::channel channel_config;
      channel_config.freq.center_frequency_Hz = 3.5e9;
      channel_config.gain_dB                  = 60.0;
      channel_config.args                     = "";
      rx_stream_config.channels.push_back(channel_config);
    }
    radio_config.rx_streams.push_back(rx_stream_config);

    radio_config.log_level        = log_level;
    radio_config.sampling_rate_Hz = 192e3;
    radio_config.tx_mode          = radio_configuration::transmission_mode::continuous;
    radio_config.clock.clock      = radio_configuration::clock_sources::source::DEFAULT;
    radio_config.clock.sync       = radio_configuration::clock_sources::source::DEFAULT;
    radio_config.power_ramping_us = 0.0f;
    radio_config.otw_format       = radio_configuration::over_the_wire_format::DEFAULT;

    return radio_config;
  }

  // Function that generates and transmits samples until the number of samples to process reached.
  std::function<void(baseband_gateway_transmitter&, baseband_gateway_timestamp)> defer_transmission =
      [this](baseband_gateway_transmitter& transmitter, baseband_gateway_timestamp start_time) {
        // Prepare transmit buffer
        baseband_gateway_buffer_dynamic tx_buffer(nof_channels, tx_block_size);

        unsigned tx_sample_count = 0U;
        while (tx_sample_count != nof_samples) {
          unsigned block_size = std::min(tx_block_size, nof_samples - tx_sample_count);
          tx_buffer.resize(block_size);

          // Generate transmit random data for each channel.
          generate_random_samples(tx_buffer.get_writer());

          // Transmit stream buffer.
          baseband_gateway_transmitter_metadata tx_md;
          tx_md.ts = start_time + tx_sample_count + tx_advance_samples;
          transmitter.transmit(tx_buffer.get_reader(), tx_md);

          tx_sample_count += block_size;
        }
      };

  void generate_random_samples(baseband_gateway_buffer_writer& buffer)
  {
    for (unsigned channel_id = 0; channel_id != nof_channels; ++channel_id) {
      span<ci16_t> channel_view = buffer.get_channel_buffer(channel_id);
      for (ci16_t& sample : channel_view) {
        sample = generate_random_ci16(tx_dist, tx_rgen[channel_id]);
      }
    }
  }

  void check_received_samples(const baseband_gateway_buffer_reader& rx_buffer,
                              baseband_gateway_timestamp            first_sample_index)
  {
    for (unsigned channel_id = 0; channel_id != nof_channels; ++channel_id) {
      span<const ci16_t> buffer = rx_buffer[channel_id];
      for (unsigned i_sample = 0; i_sample != rx_buffer.get_nof_samples(); ++i_sample) {
        // Compute the expected samples. The first received samples should be 0. After tx_advance_samples have been
        // received, the transmitted signal should appear in the RX buffer.
        ci16_t expected_sample = 0;
        if (first_sample_index + i_sample >= tx_advance_samples) {
          expected_sample = generate_random_ci16(rx_dist, rx_rgen[channel_id]);
        }

        ASSERT_LE(std::abs(expected_sample - buffer[i_sample]), ASSERT_MAX_ERROR_COMPLEX) << fmt::format(
            "expected={} sample={} sample_index={}", expected_sample, buffer[i_sample], first_sample_index + i_sample);
      }
    }
  }
};

class radio_notifier_spy : public radio_event_notifier
{
public:
  void on_radio_rt_event(const event_description& description) override {}
};

std::unique_ptr<radio_factory_realtime_loopback_impl> RealtimeLoopbackRadioFixture::factory           = nullptr;
std::unique_ptr<task_worker>                          RealtimeLoopbackRadioFixture::async_task_worker = nullptr;

TEST_P(RealtimeLoopbackRadioFixture, RealtimeFlow)
{
  // Asynchronous and TX task executors.
  std::unique_ptr<task_executor> async_task_executor = make_task_executor_ptr(*async_task_worker);

  // Notifier.
  radio_notifier_spy radio_notifier;

  // Radio configuration.
  radio_configuration::radio radio_config = create_radio_config();

  // Create radio session.
  std::unique_ptr<radio_session> session = factory->create(radio_config, *async_task_executor, radio_notifier);
  ASSERT_NE(session, nullptr);

  // Calculate starting time.
  double                     delay_s      = 0.1;
  baseband_gateway_timestamp current_time = session->read_current_time();
  baseband_gateway_timestamp start_time = current_time + static_cast<uint64_t>(delay_s * radio_config.sampling_rate_Hz);

  // Start processing.
  session->start(start_time);

  // Defer transmission to a separate thread.
  std::thread tx_thread(defer_transmission, std::ref(session->get_baseband_gateway(0).get_transmitter()), start_time);

  // Get the receiver.
  baseband_gateway_receiver& receiver = session->get_baseband_gateway(0).get_receiver();

  // Prepare receive buffer.
  baseband_gateway_buffer_dynamic rx_buffer(nof_channels, rx_block_size);

  unsigned rx_sample_count = 0U;
  while (rx_sample_count != nof_samples) {
    // Calculate block size.
    unsigned block_size = std::min(rx_block_size, nof_samples - rx_sample_count);
    rx_buffer.resize(block_size);

    // Receive.
    baseband_gateway_receiver::metadata md = receiver.receive(rx_buffer.get_writer());

    // Check that the timestamp of the received samples matches the expected value, which is the sample index offset by
    // the starting timestamp. The actual sample values are not checked, since in realtime operation, the realtime
    // loopback radio can drop samples in the event of lates, underflows or overflows.
    ASSERT_EQ(md.ts, start_time + rx_sample_count);

    rx_sample_count += block_size;
  }

  tx_thread.join();

  // Stop session.
  session->stop();
}

TEST_P(RealtimeLoopbackRadioFixture, NonRealtimeFlow)
{
  // Asynchronous task executor.
  std::unique_ptr<task_executor> async_task_executor = make_task_executor_ptr(*async_task_worker);

  // Notifier.
  radio_notifier_spy radio_notifier;

  // Radio configuration.
  radio_configuration::radio radio_config = create_radio_config();

  // Create a custom current time function for the realtime loopback radio. This function increments the RF timestamp
  // each time it is called.
  baseband_gateway_timestamp                    current_rf_timestamp                  = 0;
  unique_function<baseband_gateway_timestamp()> get_current_rf_timestamp_manual_clock = [&current_rf_timestamp]() {
    return current_rf_timestamp += 10;
  };

  // Create radio session.
  std::unique_ptr<radio_session> session = factory->create_with_custom_time(
      radio_config, *async_task_executor, radio_notifier, get_current_rf_timestamp_manual_clock);
  ASSERT_NE(session, nullptr);

  // Set starting time.
  baseband_gateway_timestamp start_time = 0;

  // Start processing.
  session->start(start_time);

  // Get the transmitter and receiver.
  baseband_gateway_receiver&    receiver    = session->get_baseband_gateway(0).get_receiver();
  baseband_gateway_transmitter& transmitter = session->get_baseband_gateway(0).get_transmitter();

  // Prepare buffers.
  baseband_gateway_buffer_dynamic rx_buffer(nof_channels, rx_block_size);
  baseband_gateway_buffer_dynamic tx_buffer(nof_channels, tx_block_size);

  unsigned tx_sample_count = 0U;
  while (tx_sample_count != nof_samples) {
    // Calculate transmit block size.
    unsigned transmit_block_size = std::min(tx_block_size, nof_samples - tx_sample_count);
    tx_buffer.resize(transmit_block_size);

    // Fill the buffer with samples.
    generate_random_samples(tx_buffer.get_writer());

    // Transmit stream buffer.
    baseband_gateway_transmitter_metadata tx_md;
    tx_md.ts = start_time + tx_sample_count + tx_advance_samples;
    transmitter.transmit(tx_buffer.get_reader(), tx_md);

    // Receive as many samples as the transmitted ones, in multiple calls if necessary.
    unsigned remaining_rx_samples = transmit_block_size;
    while (remaining_rx_samples > 0) {
      // Number of samples received in this iteration.
      unsigned nof_rx_samples = transmit_block_size - remaining_rx_samples;

      // Number of samples to receive in this iteration.
      unsigned receive_block_size = std::min(remaining_rx_samples, rx_block_size);
      rx_buffer.resize(receive_block_size);

      // Receive.
      baseband_gateway_receiver::metadata md = receiver.receive(rx_buffer.get_writer());

      // Check that the received timestamp matches the expected value, which is the number of received samples offset by
      // the start timestamp.
      ASSERT_EQ(md.ts, start_time + tx_sample_count + nof_rx_samples);

      // Validate data for each channel. Since this test does not run in real time, the received samples should match
      // the transmitted ones in all cases.
      check_received_samples(rx_buffer.get_reader(), tx_sample_count + nof_rx_samples);

      // Subtract the received samples from the remaining ones.
      remaining_rx_samples -= receive_block_size;
    }
    tx_sample_count += transmit_block_size;
  }

  // Stop session.
  session->stop();
}

// Triggers a TX underflow followed by an in-time transmisswhich are in the past compared to the last TX timestampion,
// and checks that the received samples are still valid.
TEST_P(RealtimeLoopbackRadioFixture, TxUnderflow)
{
  // Asynchronous task executor.
  std::unique_ptr<task_executor> async_task_executor = make_task_executor_ptr(*async_task_worker);

  // Notifier.
  radio_notifier_spy radio_notifier;

  // Radio configuration.
  radio_configuration::radio radio_config = create_radio_config();

  // Create a custom current time function for the realtime loopback radio. This function increments the RF timestamp
  // each time it is called.
  baseband_gateway_timestamp                    current_rf_timestamp                  = 0;
  unique_function<baseband_gateway_timestamp()> get_current_rf_timestamp_manual_clock = [&current_rf_timestamp]() {
    return current_rf_timestamp++;
  };

  // Create radio session.
  std::unique_ptr<radio_session> session = factory->create_with_custom_time(
      radio_config, *async_task_executor, radio_notifier, get_current_rf_timestamp_manual_clock);
  ASSERT_NE(session, nullptr);

  // Set starting time.
  baseband_gateway_timestamp start_time = 0;

  // Start processing.
  session->start(start_time);

  // Get the transmitter and receiver.
  baseband_gateway_receiver&    receiver    = session->get_baseband_gateway(0).get_receiver();
  baseband_gateway_transmitter& transmitter = session->get_baseband_gateway(0).get_transmitter();

  // Prepare buffers.
  baseband_gateway_buffer_dynamic rx_buffer(nof_channels, rx_block_size);
  baseband_gateway_buffer_dynamic tx_buffer(nof_channels, tx_block_size);
  tx_buffer.resize(tx_block_size);

  // Generate transmit random data for each channel.
  generate_random_samples(tx_buffer.get_writer());

  // Transmit stream buffer. The transmission timestamp is too early, since the realtime loopback radio emulates the
  // constraint of a minimum required radio processing time, leading to an underflow. The samples should be discarded.
  baseband_gateway_transmitter_metadata tx_md;
  tx_md.ts = start_time;
  transmitter.transmit(tx_buffer.get_reader(), tx_md);

  // Actual valid transmission.
  tx_md.ts = start_time + tx_advance_samples;
  transmitter.transmit(tx_buffer.get_reader(), tx_md);

  // Compute the remaining samples to receive.
  unsigned remaining_rx_samples = tx_advance_samples + tx_block_size;
  unsigned rx_sample_count      = 0;
  while (remaining_rx_samples > 0) {
    // Compute the receive block size and receive.
    unsigned receive_block_size = std::min(rx_block_size, remaining_rx_samples);
    rx_buffer.resize(receive_block_size);
    baseband_gateway_receiver::metadata md = receiver.receive(rx_buffer.get_writer());

    // Check that the timestamp matched the expected value.
    ASSERT_EQ(md.ts, start_time + rx_sample_count);

    // Validate data for each channel. Since this test does not run in real time, the received samples should match the
    // transmitted ones in all cases.
    check_received_samples(rx_buffer.get_reader(), rx_sample_count);

    rx_sample_count += receive_block_size;
    remaining_rx_samples -= receive_block_size;
  }

  // Stop session.
  session->stop();
}

TEST_P(RealtimeLoopbackRadioFixture, TxLate)
{
  // Asynchronous task executor.
  std::unique_ptr<task_executor> async_task_executor = make_task_executor_ptr(*async_task_worker);

  // Notifier.
  radio_notifier_spy radio_notifier;

  // Radio configuration.
  radio_configuration::radio radio_config = create_radio_config();

  // Create a custom current time function for the realtime loopback radio. This function increments the RF timestamp
  // each time it is called.
  baseband_gateway_timestamp                    current_rf_timestamp                  = 0;
  unique_function<baseband_gateway_timestamp()> get_current_rf_timestamp_manual_clock = [&current_rf_timestamp]() {
    return current_rf_timestamp++;
  };

  // Create radio session.
  std::unique_ptr<radio_session> session = factory->create_with_custom_time(
      radio_config, *async_task_executor, radio_notifier, get_current_rf_timestamp_manual_clock);
  ASSERT_NE(session, nullptr);

  // Set starting time.
  baseband_gateway_timestamp start_time = 0;

  // Start processing.
  session->start(start_time);

  // Get the transmitter and receiver.
  baseband_gateway_receiver&    receiver    = session->get_baseband_gateway(0).get_receiver();
  baseband_gateway_transmitter& transmitter = session->get_baseband_gateway(0).get_transmitter();

  // Prepare buffers.
  baseband_gateway_buffer_dynamic rx_buffer(nof_channels, rx_block_size);
  baseband_gateway_buffer_dynamic tx_buffer(nof_channels, tx_block_size);
  tx_buffer.resize(tx_block_size);

  // Generate transmit random data for each channel.
  generate_random_samples(tx_buffer.get_writer());

  // Valid transmission.
  baseband_gateway_transmitter_metadata tx_md;
  tx_md.ts = start_time + tx_advance_samples;
  transmitter.transmit(tx_buffer.get_reader(), tx_md);

  // Transmit samples in the past, triggering a late. The previously transmitted samples should not be affected.
  tx_md.ts = start_time + tx_advance_samples - 1;
  transmitter.transmit(tx_buffer.get_reader(), tx_md);

  // Compute the remaining samples to receive.
  unsigned remaining_rx_samples = tx_advance_samples + tx_block_size;
  unsigned rx_sample_count      = 0;
  while (remaining_rx_samples > 0) {
    // Compute the receive block size and receive.
    unsigned receive_block_size = std::min(rx_block_size, remaining_rx_samples);
    rx_buffer.resize(receive_block_size);
    baseband_gateway_receiver::metadata md = receiver.receive(rx_buffer.get_writer());

    // Check that the timestamp matched the expected value.
    ASSERT_EQ(md.ts, start_time + rx_sample_count);

    // Validate data for each channel. Since this test does not run in real time, the received samples should match the
    // transmitted ones in all cases.
    check_received_samples(rx_buffer.get_reader(), rx_sample_count);

    rx_sample_count += receive_block_size;
    remaining_rx_samples -= receive_block_size;
  }
}

TEST_P(RealtimeLoopbackRadioFixture, RxOverflow)
{
  // Asynchronous task executor.
  std::unique_ptr<task_executor> async_task_executor = make_task_executor_ptr(*async_task_worker);

  // Notifier.
  radio_notifier_spy radio_notifier;

  // Radio configuration.
  radio_configuration::radio radio_config = create_radio_config();

  // Create a custom current time function for the realtime loopback radio. This function adds a large jump to the RF
  // timestamp each time it is called.
  baseband_gateway_timestamp                    current_rf_timestamp                  = 0;
  unique_function<baseband_gateway_timestamp()> get_current_rf_timestamp_manual_clock = [&current_rf_timestamp]() {
    baseband_gateway_timestamp ret = current_rf_timestamp;
    current_rf_timestamp += 100000;
    return ret;
  };

  // Create radio session.
  std::unique_ptr<radio_session> session = factory->create_with_custom_time(
      radio_config, *async_task_executor, radio_notifier, get_current_rf_timestamp_manual_clock);
  ASSERT_NE(session, nullptr);

  // Set starting time.
  baseband_gateway_timestamp start_time = 0;

  // Start processing.
  session->start(start_time);

  // Get the transmitter and receiver.
  baseband_gateway_receiver&    receiver    = session->get_baseband_gateway(0).get_receiver();
  baseband_gateway_transmitter& transmitter = session->get_baseband_gateway(0).get_transmitter();

  // Prepare buffers.
  baseband_gateway_buffer_dynamic rx_buffer(nof_channels, rx_block_size);
  baseband_gateway_buffer_dynamic tx_buffer(nof_channels, tx_block_size);
  tx_buffer.resize(tx_block_size);

  // Generate transmit random data for each channel.
  generate_random_samples(tx_buffer.get_writer());

  // Valid transmission. It implicitly causes a large jump on the RF timestamp, due to the injected time function.
  baseband_gateway_transmitter_metadata tx_md;
  tx_md.ts = start_time + tx_advance_samples;
  transmitter.transmit(tx_buffer.get_reader(), tx_md);

  // Attempt to receive. It should lead to an RX overflow.
  rx_buffer.resize(rx_block_size);
  baseband_gateway_receiver::metadata md = receiver.receive(rx_buffer.get_writer());

  // Check that the timestamp matches the expected value. It should be unaffected by the RX overflow.
  ASSERT_EQ(md.ts, start_time);

  // Check that the received samples are zero.
  for (unsigned channel_id = 0; channel_id != nof_channels; ++channel_id) {
    span<const ci16_t> buffer = rx_buffer[channel_id];
    for (unsigned i_sample = 0; i_sample != rx_buffer.get_nof_samples(); ++i_sample) {
      ASSERT_EQ(buffer[i_sample], ci16_t(0, 0))
          << fmt::format("expected={} sample={} sample_index={}", ci16_t(0, 0), buffer[i_sample], i_sample);
    }
  }

  // Stop session.
  session->stop();
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(RealtimeLoopbackRadioTest,
                         RealtimeLoopbackRadioFixture,
                         ::testing::Combine(::testing::Values(1, 2),
                                            ::testing::Values(39, 123),
                                            ::testing::Values(39, 123)));
