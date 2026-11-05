// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_factory_realtime_loopback_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/gateways/baseband/baseband_gateway_receiver.h"
#include "ocudu/gateways/baseband/baseband_gateway_transmitter.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "ocudu/ocuduvec/zero.h"
#include "ocudu/radio/capabilities/radio_rx_frequency_control.h"
#include "ocudu/radio/capabilities/radio_rx_gain_control.h"
#include "ocudu/radio/capabilities/radio_tx_frequency_control.h"
#include "ocudu/radio/capabilities/radio_tx_gain_control.h"
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

/// Creates a valid radio configuration with the given number of streams and channels per stream.
static radio_configuration::radio make_radio_config(unsigned nof_streams, unsigned nof_channels)
{
  radio_configuration::channel channel_config;
  channel_config.freq.center_frequency_Hz = 3.5e9;
  channel_config.gain_dB                  = 60.0;

  radio_configuration::stream stream_config;
  stream_config.channels.assign(nof_channels, channel_config);

  radio_configuration::radio radio_config;
  radio_config.tx_streams.assign(nof_streams, stream_config);
  radio_config.rx_streams.assign(nof_streams, stream_config);
  radio_config.log_level        = log_level;
  radio_config.sampling_rate_Hz = 192e3;
  radio_config.tx_mode          = radio_configuration::transmission_mode::continuous;
  radio_config.clock.clock      = radio_configuration::clock_sources::source::DEFAULT;
  radio_config.clock.sync       = radio_configuration::clock_sources::source::DEFAULT;
  radio_config.power_ramping_us = 0.0f;
  radio_config.otw_format       = radio_configuration::over_the_wire_format::DEFAULT;
  return radio_config;
}

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

  // Creates a standard radio configuration with the given number of streams.
  radio_configuration::radio create_radio_config(unsigned nof_streams = 1) const
  {
    return make_radio_config(nof_streams, nof_channels);
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

  void zero_baseband_buffer(baseband_gateway_buffer_writer& buffer) const
  {
    for (unsigned channel_id = 0; channel_id != nof_channels; ++channel_id) {
      span<ci16_t> channel_view = buffer.get_channel_buffer(channel_id);
      ocuduvec::zero(channel_view);
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

// The session exposes no radio capabilities.
TEST_P(RealtimeLoopbackRadioFixture, NoCapabilities)
{
  // Asynchronous task executor.
  std::unique_ptr<task_executor> async_task_executor = make_task_executor_ptr(*async_task_worker);

  // Notifier.
  radio_notifier_spy radio_notifier;

  // Create radio session.
  std::unique_ptr<radio_session> session = factory->create(create_radio_config(), *async_task_executor, radio_notifier);
  ASSERT_NE(session, nullptr);

  // The loopback radio does not support any capability.
  radio_capabilities_registry& capabilities = session->get_capabilities_registry();
  EXPECT_EQ(capabilities.get<radio_tx_gain_control>(), nullptr);
  EXPECT_EQ(capabilities.get<radio_rx_gain_control>(), nullptr);
  EXPECT_EQ(capabilities.get<radio_tx_frequency_control>(), nullptr);
  EXPECT_EQ(capabilities.get<radio_rx_frequency_control>(), nullptr);

  session->stop();
}

// Samples transmitted ahead of the realtime RF clock are looped back.
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

// Samples transmitted ahead of a manual RF clock are looped back.
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

// Triggers a TX underflow followed by an in-time transmission, and checks that the received samples are still valid.
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

// This test checks that when the transmitted samples run out, the receive calls still return without errors
// indefinitely, and that a subsequent valid transmission is processed correctly.
TEST_P(RealtimeLoopbackRadioFixture, ReceiveAfterTransmitSamplesRunOut)
{
  // Asynchronous task executor.
  std::unique_ptr<task_executor> async_task_executor = make_task_executor_ptr(*async_task_worker);

  // Notifier.
  radio_notifier_spy radio_notifier;

  // Radio configuration.
  radio_configuration::radio radio_config = create_radio_config();

  // Create a custom current time function for the realtime loopback radio. This function advances the RF
  // timestamp in increments of the RX block size.
  baseband_gateway_timestamp                    current_rf_timestamp = 1000000;
  unique_function<baseband_gateway_timestamp()> get_current_rf_timestamp_manual_clock =
      [&current_rf_timestamp, rx_block_sz = rx_block_size]() { return current_rf_timestamp += rx_block_sz; };

  // Create radio session.
  std::unique_ptr<radio_session> session = factory->create_with_custom_time(
      radio_config, *async_task_executor, radio_notifier, get_current_rf_timestamp_manual_clock);
  ASSERT_NE(session, nullptr);

  // Set starting time.
  baseband_gateway_timestamp start_time = current_rf_timestamp - rx_block_size;

  // Start processing.
  session->start(start_time);

  // Get the transmitter and receiver.
  baseband_gateway_receiver&    receiver    = session->get_baseband_gateway(0).get_receiver();
  baseband_gateway_transmitter& transmitter = session->get_baseband_gateway(0).get_transmitter();

  // Prepare buffers.
  baseband_gateway_buffer_dynamic rx_buffer(nof_channels, rx_block_size);
  baseband_gateway_buffer_dynamic tx_buffer(nof_channels, tx_block_size);
  tx_buffer.resize(tx_block_size);

  // Write valid buffer samples. The samples are set to zero to avoid confusion with later writes.
  zero_baseband_buffer(tx_buffer.get_writer());
  baseband_gateway_transmitter_metadata tx_md;
  tx_md.ts = current_rf_timestamp + tx_advance_samples;
  transmitter.transmit(tx_buffer.get_reader(), tx_md);
  tx_md.ts += tx_block_size;

  // Attempt to read for 100 ms. When the transmitted samples run out, the receive calls should still return without
  // errors, even if the buffer contents are not valid.
  baseband_gateway_receiver::metadata rx_md = {};
  while (rx_md.ts < start_time + radio_config.sampling_rate_Hz / 10) {
    rx_md = receiver.receive(rx_buffer.get_writer());
  }

  // Generate transmit random data for each channel.
  generate_random_samples(tx_buffer.get_writer());

  // After 100 ms, transmit a valid buffer with random samples. The samples should be processed correctly. Place the
  // transmission timestamp tx_advance_samples ahead of the last received timestamp.
  tx_md.ts = rx_md.ts + rx_block_size + tx_advance_samples;
  transmitter.transmit(tx_buffer.get_reader(), tx_md);

  // Compute the remaining samples to receive.
  unsigned remaining_rx_samples = (tx_md.ts + tx_block_size) - (rx_md.ts + rx_block_size);
  unsigned rx_sample_count      = 0;
  while (remaining_rx_samples > 0) {
    // Compute the receive block size and receive.
    unsigned receive_block_size = std::min(rx_block_size, remaining_rx_samples);
    rx_buffer.resize(receive_block_size);
    rx_md = receiver.receive(rx_buffer.get_writer());

    // Validate data for each channel. Since this test does not run in real time, the received samples should match the
    // transmitted ones in all cases.
    check_received_samples(rx_buffer.get_reader(), rx_sample_count);

    rx_sample_count += receive_block_size;
    remaining_rx_samples -= receive_block_size;
  }

  // Stop session.
  session->stop();
}

// A transmission in the past of the previous one is dropped, and later transmissions are looped back.
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

// A clock jump beyond the RX buffering depth zeroes the lost samples and keeps the RX timestamps contiguous.
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

// An RX overflow that loses part of a request zeroes only the lost samples.
TEST_P(RealtimeLoopbackRadioFixture, PartialRxOverflow)
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
    return current_rf_timestamp;
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

  // Valid transmissions.
  baseband_gateway_transmitter_metadata tx_md;
  tx_md.ts = start_time + tx_advance_samples;

  // Fill the radio loopback buffer, so there is enough data for multiple receive calls.
  for (unsigned i_transmission = 0; i_transmission != 4; ++i_transmission) {
    transmitter.transmit(tx_buffer.get_reader(), tx_md);
    current_rf_timestamp += tx_buffer.get_nof_samples();
    tx_md.ts += tx_buffer.get_nof_samples();
  }

  // Attempt to receive.
  rx_buffer.resize(rx_block_size);
  baseband_gateway_receiver::metadata md = receiver.receive(rx_buffer.get_writer());

  // Advance the RF timestamp to cause a partial RX overflow.
  current_rf_timestamp += 2000;
  md = receiver.receive(rx_buffer.get_writer());

  // Check that the timestamp matches the expected value.
  ASSERT_EQ(md.ts, start_time + rx_buffer.get_nof_samples());

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

// Each stream loops back its own samples, independently of the other streams.
TEST_P(RealtimeLoopbackRadioFixture, MultipleStreamsAreIsolated)
{
  static constexpr unsigned nof_streams = 3;

  std::unique_ptr<task_executor> async_task_executor = make_task_executor_ptr(*async_task_worker);
  radio_notifier_spy             radio_notifier;

  // The RF time is driven by the test, since all the streams consult the same clock.
  baseband_gateway_timestamp                    current_rf_timestamp                  = 0;
  unique_function<baseband_gateway_timestamp()> get_current_rf_timestamp_manual_clock = [&current_rf_timestamp]() {
    return current_rf_timestamp;
  };

  std::unique_ptr<radio_session> session = factory->create_with_custom_time(
      create_radio_config(nof_streams), *async_task_executor, radio_notifier, get_current_rf_timestamp_manual_clock);
  ASSERT_NE(session, nullptr);
  session->start(0);

  // Each stream carries a constant that identifies the stream, the channel and the sample.
  auto stream_sample = [](unsigned i_stream, unsigned i_channel, baseband_gateway_timestamp ts) {
    return ci16_t(static_cast<int16_t>(100 * (i_stream + 1) + i_channel), static_cast<int16_t>(ts % 30000));
  };

  baseband_gateway_buffer_dynamic tx_buffer(nof_channels, tx_block_size);
  baseband_gateway_buffer_dynamic rx_buffer(nof_channels, tx_block_size);
  for (unsigned tx_count = 0; tx_count < nof_samples; tx_count += tx_block_size) {
    baseband_gateway_timestamp tx_ts = tx_count + tx_advance_samples;
    current_rf_timestamp             = tx_count;

    // Transmit the block on every stream before receiving, so that the stream buffers are filled concurrently.
    for (unsigned i_stream = 0; i_stream != nof_streams; ++i_stream) {
      for (unsigned i_channel = 0; i_channel != nof_channels; ++i_channel) {
        span<ci16_t> ch = tx_buffer.get_writer().get_channel_buffer(i_channel);
        for (unsigned i = 0; i != ch.size(); ++i) {
          ch[i] = stream_sample(i_stream, i_channel, tx_ts + i);
        }
      }
      session->get_baseband_gateway(i_stream).get_transmitter().transmit(tx_buffer.get_reader(), {.ts = tx_ts});
    }

    // Receive the same number of samples on every stream in reverse order.
    current_rf_timestamp = tx_count + tx_block_size;
    for (unsigned i_stream = nof_streams; i_stream-- != 0;) {
      baseband_gateway_receiver::metadata md =
          session->get_baseband_gateway(i_stream).get_receiver().receive(rx_buffer.get_writer());
      ASSERT_EQ(md.ts, tx_count);
      for (unsigned i_channel = 0; i_channel != nof_channels; ++i_channel) {
        span<const ci16_t> ch = rx_buffer.get_reader().get_channel_buffer(i_channel);
        for (unsigned i = 0; i != ch.size(); ++i) {
          baseband_gateway_timestamp ts = md.ts + i;
          ci16_t ref = (ts < tx_advance_samples) ? ci16_t(0, 0) : stream_sample(i_stream, i_channel, ts);
          ASSERT_EQ(ch[i], ref) << fmt::format("stream={} channel={} ts={}", i_stream, i_channel, ts);
        }
      }
    }
  }

  session->stop();
}

// All the streams share the same RF clock.
TEST_P(RealtimeLoopbackRadioFixture, MultipleStreamsShareClock)
{
  std::unique_ptr<task_executor> async_task_executor = make_task_executor_ptr(*async_task_worker);
  radio_notifier_spy             radio_notifier;

  unsigned                                      nof_clock_calls = 0;
  unique_function<baseband_gateway_timestamp()> manual_clock    = [&nof_clock_calls]() {
    ++nof_clock_calls;
    return 1000000;
  };

  std::unique_ptr<radio_session> session =
      factory->create_with_custom_time(create_radio_config(4), *async_task_executor, radio_notifier, manual_clock);
  ASSERT_NE(session, nullptr);
  ASSERT_EQ(session->read_current_time(), 1000000);
  ASSERT_EQ(nof_clock_calls, 1);

  // Transmitting on any stream consults the same clock.
  session->start(0);
  baseband_gateway_buffer_dynamic tx_buffer(nof_channels, tx_block_size);
  zero_baseband_buffer(tx_buffer.get_writer());
  session->get_baseband_gateway(3).get_transmitter().transmit(tx_buffer.get_reader(),
                                                              {.ts = 1000000 + tx_advance_samples});
  ASSERT_GT(nof_clock_calls, 1);

  session->stop();
}

// A stop request releases a receiver that is waiting for the radio to start.
TEST_P(RealtimeLoopbackRadioFixture, StopBeforeStartReleasesReceiver)
{
  std::unique_ptr<task_executor> async_task_executor = make_task_executor_ptr(*async_task_worker);
  radio_notifier_spy             radio_notifier;

  std::unique_ptr<radio_session> session =
      factory->create(create_radio_config(2), *async_task_executor, radio_notifier);
  ASSERT_NE(session, nullptr);

  baseband_gateway_buffer_dynamic rx_buffer(nof_channels, rx_block_size);
  std::atomic<bool>               returned = false;
  std::thread                     rx_thread([&]() {
    session->get_baseband_gateway(1).get_receiver().receive(rx_buffer.get_writer());
    returned = true;
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  ASSERT_FALSE(returned);
  session->stop();
  rx_thread.join();
  ASSERT_TRUE(returned);

  // The released receiver does not deliver samples.
  for (unsigned i_channel = 0; i_channel != nof_channels; ++i_channel) {
    for (ci16_t sample : rx_buffer.get_reader().get_channel_buffer(i_channel)) {
      ASSERT_EQ(sample, ci16_t(0, 0));
    }
  }
}

// A stop request releases a receiver that is waiting for samples that never reach the antenna.
TEST_P(RealtimeLoopbackRadioFixture, StopReleasesReceiverWaitingForSamples)
{
  std::unique_ptr<task_executor> async_task_executor = make_task_executor_ptr(*async_task_worker);
  radio_notifier_spy             radio_notifier;

  // The RF time never advances.
  unique_function<baseband_gateway_timestamp()> frozen_clock = []() { return 0; };

  std::unique_ptr<radio_session> session =
      factory->create_with_custom_time(create_radio_config(), *async_task_executor, radio_notifier, frozen_clock);
  ASSERT_NE(session, nullptr);
  session->start(0);

  baseband_gateway_buffer_dynamic rx_buffer(nof_channels, rx_block_size);
  std::atomic<bool>               returned = false;
  std::thread                     rx_thread([&]() {
    session->get_baseband_gateway(0).get_receiver().receive(rx_buffer.get_writer());
    returned = true;
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  ASSERT_FALSE(returned);
  session->stop();
  rx_thread.join();
  ASSERT_TRUE(returned);
}

// Transmitting an empty buffer has no effect.
TEST_P(RealtimeLoopbackRadioFixture, EmptyTransmission)
{
  std::unique_ptr<task_executor> async_task_executor = make_task_executor_ptr(*async_task_worker);
  radio_notifier_spy             radio_notifier;

  baseband_gateway_timestamp                    current_rf_timestamp = 0;
  unique_function<baseband_gateway_timestamp()> manual_clock         = [&current_rf_timestamp]() {
    return current_rf_timestamp += 10;
  };

  std::unique_ptr<radio_session> session =
      factory->create_with_custom_time(create_radio_config(), *async_task_executor, radio_notifier, manual_clock);
  ASSERT_NE(session, nullptr);
  session->start(0);

  baseband_gateway_transmitter&   transmitter = session->get_baseband_gateway(0).get_transmitter();
  baseband_gateway_buffer_dynamic empty_buffer(nof_channels, 0);
  transmitter.transmit(empty_buffer.get_reader(), {.ts = 0});

  // A regular transmission afterwards is received intact.
  baseband_gateway_buffer_dynamic tx_buffer(nof_channels, tx_block_size);
  generate_random_samples(tx_buffer.get_writer());
  transmitter.transmit(tx_buffer.get_reader(), {.ts = tx_advance_samples});

  baseband_gateway_receiver&      receiver = session->get_baseband_gateway(0).get_receiver();
  baseband_gateway_buffer_dynamic rx_buffer(nof_channels, rx_block_size);
  for (unsigned rx_count = 0; rx_count < tx_advance_samples + tx_block_size;) {
    unsigned block_size = std::min(rx_block_size, tx_advance_samples + tx_block_size - rx_count);
    rx_buffer.resize(block_size);
    baseband_gateway_receiver::metadata md = receiver.receive(rx_buffer.get_writer());
    ASSERT_EQ(md.ts, rx_count);
    check_received_samples(rx_buffer.get_reader(), rx_count);
    rx_count += block_size;
  }

  session->stop();
}

// A receiver that stops reading for longer than the loopback buffer depth does not abort the transmitter, and the
// stream recovers once the receiver catches up.
TEST_P(RealtimeLoopbackRadioFixture, ReceiverStallDoesNotAbortTransmitter)
{
  std::unique_ptr<task_executor> async_task_executor = make_task_executor_ptr(*async_task_worker);
  radio_notifier_spy             radio_notifier;

  baseband_gateway_timestamp                    current_rf_timestamp = 0;
  unique_function<baseband_gateway_timestamp()> manual_clock         = [&current_rf_timestamp]() {
    return current_rf_timestamp;
  };

  radio_configuration::radio     radio_config = create_radio_config();
  std::unique_ptr<radio_session> session =
      factory->create_with_custom_time(radio_config, *async_task_executor, radio_notifier, manual_clock);
  ASSERT_NE(session, nullptr);
  session->start(0);

  baseband_gateway_transmitter& transmitter = session->get_baseband_gateway(0).get_transmitter();
  baseband_gateway_receiver&    receiver    = session->get_baseband_gateway(0).get_receiver();

  // The transmitter keeps running for one second of RF time while the receiver is stalled at timestamp zero.
  baseband_gateway_buffer_dynamic tx_buffer(nof_channels, tx_block_size);
  zero_baseband_buffer(tx_buffer.get_writer());
  auto                       one_second = static_cast<baseband_gateway_timestamp>(radio_config.sampling_rate_Hz);
  baseband_gateway_timestamp tx_end     = tx_advance_samples;
  for (; tx_end < one_second; tx_end += tx_block_size) {
    current_rf_timestamp = tx_end - tx_advance_samples;
    transmitter.transmit(tx_buffer.get_reader(), {.ts = tx_end});
  }

  // The stalled receiver reports an overflow without samples.
  baseband_gateway_buffer_dynamic     rx_buffer(nof_channels, rx_block_size);
  baseband_gateway_receiver::metadata md = receiver.receive(rx_buffer.get_writer());
  ASSERT_EQ(md.ts, 0);

  // Catch up with the RF time.
  while (md.ts + 2 * rx_block_size < current_rf_timestamp) {
    md = receiver.receive(rx_buffer.get_writer());
  }

  // Transmit random samples ahead of the receiver, after the samples dropped during the stall.
  baseband_gateway_timestamp next_rx_ts = md.ts + rx_block_size;
  baseband_gateway_timestamp tx_ts = std::max(tx_end, std::max(next_rx_ts, current_rf_timestamp) + tx_advance_samples);
  current_rf_timestamp             = tx_ts - tx_advance_samples;
  generate_random_samples(tx_buffer.get_writer());
  transmitter.transmit(tx_buffer.get_reader(), {.ts = tx_ts});

  // Receives a block of the given size, advancing the RF time just enough for the samples to reach the antenna.
  auto receive_block = [&](unsigned block_size) {
    rx_buffer.resize(block_size);
    current_rf_timestamp = std::max(current_rf_timestamp, next_rx_ts + block_size);
    md                   = receiver.receive(rx_buffer.get_writer());
    EXPECT_EQ(md.ts, next_rx_ts);
    next_rx_ts += block_size;
  };

  // Skip the samples before the transmission.
  while (next_rx_ts < tx_ts) {
    receive_block(std::min<baseband_gateway_timestamp>(rx_block_size, tx_ts - next_rx_ts));
  }

  // The transmitted block is received intact.
  for (unsigned rx_count = 0; rx_count < tx_block_size;) {
    unsigned block_size = std::min(rx_block_size, tx_block_size - rx_count);
    receive_block(block_size);
    // Offset the expected sample index so the expected samples start with the first transmitted sample.
    check_received_samples(rx_buffer.get_reader(), tx_advance_samples + rx_count);
    rx_count += block_size;
  }

  session->stop();
}

// The RF time scale device argument slows down the RF clock relative to the steady clock. Keys that only end with
// rf_time_scale are ignored.
TEST_P(RealtimeLoopbackRadioFixture, RfTimeScale)
{
  std::unique_ptr<task_executor> async_task_executor = make_task_executor_ptr(*async_task_worker);
  radio_notifier_spy             radio_notifier;

  radio_configuration::radio radio_config = create_radio_config(2);
  radio_config.args                       = "xrf_time_scale=-1,rf_time_scale=0.25,bar=2";

  std::unique_ptr<radio_session> session = factory->create(radio_config, *async_task_executor, radio_notifier);
  ASSERT_NE(session, nullptr);

  auto                       t0  = std::chrono::steady_clock::now();
  baseband_gateway_timestamp ts0 = session->read_current_time();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  baseband_gateway_timestamp ts1 = session->read_current_time();
  auto                       t1  = std::chrono::steady_clock::now();

  double elapsed_s = std::chrono::duration<double>(t1 - t0).count();
  double expected  = elapsed_s * radio_config.sampling_rate_Hz * 0.25;
  ASSERT_NEAR(static_cast<double>(ts1 - ts0), expected, 0.1 * expected);

  session->stop();
}

// The stream reports its optimal buffer sizes.
TEST_P(RealtimeLoopbackRadioFixture, OptimalBufferSizes)
{
  std::unique_ptr<task_executor> async_task_executor = make_task_executor_ptr(*async_task_worker);
  radio_notifier_spy             radio_notifier;

  radio_configuration::radio     radio_config = create_radio_config();
  std::unique_ptr<radio_session> session      = factory->create(radio_config, *async_task_executor, radio_notifier);
  ASSERT_NE(session, nullptr);

  // The receiver prefers blocks of 100 microseconds, the transmitter has no preference.
  baseband_gateway& gateway = session->get_baseband_gateway(0);
  EXPECT_EQ(gateway.get_receiver_optimal_buffer_size(), static_cast<unsigned>(radio_config.sampling_rate_Hz / 10000));
  EXPECT_EQ(gateway.get_transmitter_optimal_buffer_size(), 0);

  session->stop();
}

// Transmissions after a stop request are ignored.
TEST_P(RealtimeLoopbackRadioFixture, TransmitAfterStopIsIgnored)
{
  std::unique_ptr<task_executor> async_task_executor = make_task_executor_ptr(*async_task_worker);
  radio_notifier_spy             radio_notifier;

  baseband_gateway_timestamp                    current_rf_timestamp = 0;
  unique_function<baseband_gateway_timestamp()> manual_clock         = [&current_rf_timestamp]() {
    return current_rf_timestamp;
  };

  std::unique_ptr<radio_session> session =
      factory->create_with_custom_time(create_radio_config(), *async_task_executor, radio_notifier, manual_clock);
  ASSERT_NE(session, nullptr);
  session->start(0);
  session->stop();

  baseband_gateway_buffer_dynamic tx_buffer(nof_channels, tx_block_size);
  generate_random_samples(tx_buffer.get_writer());
  session->get_baseband_gateway(0).get_transmitter().transmit(tx_buffer.get_reader(), {.ts = tx_advance_samples});

  // The samples are at the antenna, but nothing was transmitted.
  current_rf_timestamp = tx_advance_samples + tx_block_size;
  baseband_gateway_buffer_dynamic     rx_buffer(nof_channels, tx_advance_samples + tx_block_size);
  baseband_gateway_receiver::metadata md =
      session->get_baseband_gateway(0).get_receiver().receive(rx_buffer.get_writer());
  ASSERT_EQ(md.ts, 0);
  for (unsigned i_channel = 0; i_channel != nof_channels; ++i_channel) {
    for (ci16_t sample : rx_buffer.get_reader().get_channel_buffer(i_channel)) {
      ASSERT_EQ(sample, ci16_t(0, 0));
    }
  }
}

// A stop request releases a transmitter that is waiting for room in the emulated transmit buffer.
TEST_P(RealtimeLoopbackRadioFixture, StopReleasesBlockedTransmitter)
{
  std::unique_ptr<task_executor> async_task_executor = make_task_executor_ptr(*async_task_worker);
  radio_notifier_spy             radio_notifier;

  // The RF time never advances.
  unique_function<baseband_gateway_timestamp()> frozen_clock = []() { return 0; };

  radio_configuration::radio     radio_config = create_radio_config();
  std::unique_ptr<radio_session> session =
      factory->create_with_custom_time(radio_config, *async_task_executor, radio_notifier, frozen_clock);
  ASSERT_NE(session, nullptr);
  session->start(0);

  // The block is one second ahead of the RF time, beyond the emulated transmit buffer.
  auto                            one_second = static_cast<baseband_gateway_timestamp>(radio_config.sampling_rate_Hz);
  baseband_gateway_buffer_dynamic tx_buffer(nof_channels, tx_block_size);
  zero_baseband_buffer(tx_buffer.get_writer());
  std::atomic<bool> returned = false;
  std::thread       tx_thread([&]() {
    session->get_baseband_gateway(0).get_transmitter().transmit(tx_buffer.get_reader(), {.ts = one_second});
    returned = true;
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  ASSERT_FALSE(returned);
  session->stop();
  tx_thread.join();
  ASSERT_TRUE(returned);
}

// Samples transmitted while a stalled receiver catches up are looped back.
TEST_P(RealtimeLoopbackRadioFixture, TransmitWhileReceiverCatchesUp)
{
  std::unique_ptr<task_executor> async_task_executor = make_task_executor_ptr(*async_task_worker);
  radio_notifier_spy             radio_notifier;

  baseband_gateway_timestamp                    current_rf_timestamp = 0;
  unique_function<baseband_gateway_timestamp()> manual_clock         = [&current_rf_timestamp]() {
    return current_rf_timestamp;
  };

  radio_configuration::radio     radio_config = create_radio_config();
  std::unique_ptr<radio_session> session =
      factory->create_with_custom_time(radio_config, *async_task_executor, radio_notifier, manual_clock);
  ASSERT_NE(session, nullptr);
  session->start(0);

  baseband_gateway_transmitter& transmitter = session->get_baseband_gateway(0).get_transmitter();
  baseband_gateway_receiver&    receiver    = session->get_baseband_gateway(0).get_receiver();

  // The receiver stalls at timestamp zero for one second of RF time.
  current_rf_timestamp = static_cast<baseband_gateway_timestamp>(radio_config.sampling_rate_Hz);

  // The receiver catches up while each request is entirely older than the receive buffering depth of 10 ms.
  auto max_nof_buffered_rx_samples = static_cast<baseband_gateway_timestamp>(radio_config.sampling_rate_Hz / 100);
  baseband_gateway_buffer_dynamic rx_buffer(nof_channels, rx_block_size);
  baseband_gateway_timestamp      next_rx_ts = 0;
  while (next_rx_ts + 2 * rx_block_size <= current_rf_timestamp - max_nof_buffered_rx_samples) {
    ASSERT_EQ(receiver.receive(rx_buffer.get_writer()).ts, next_rx_ts);
    next_rx_ts += rx_block_size;
  }

  // Transmit ahead of the RF time.
  baseband_gateway_timestamp      tx_ts = current_rf_timestamp + tx_advance_samples;
  baseband_gateway_buffer_dynamic tx_buffer(nof_channels, tx_block_size);
  generate_random_samples(tx_buffer.get_writer());
  transmitter.transmit(tx_buffer.get_reader(), {.ts = tx_ts});

  // Receives a block, advancing the RF time just enough for the samples to reach the antenna.
  auto receive_block = [&](unsigned block_size) {
    rx_buffer.resize(block_size);
    current_rf_timestamp = std::max(current_rf_timestamp, next_rx_ts + block_size);
    EXPECT_EQ(receiver.receive(rx_buffer.get_writer()).ts, next_rx_ts);
    next_rx_ts += block_size;
  };
  while (next_rx_ts < tx_ts) {
    receive_block(std::min<baseband_gateway_timestamp>(rx_block_size, tx_ts - next_rx_ts));
  }

  // The transmitted block is received intact.
  for (unsigned rx_count = 0; rx_count < tx_block_size;) {
    unsigned block_size = std::min(rx_block_size, tx_block_size - rx_count);
    receive_block(block_size);
    check_received_samples(rx_buffer.get_reader(), tx_advance_samples + rx_count);
    rx_count += block_size;
  }

  session->stop();
}

// Misusing the radio session, or a configuration that bypasses the validator, is an error.
TEST(RealtimeLoopbackRadioDeathTest, Misuse)
{
  // Forking is unsafe while the logger backend and the task worker threads run.
  GTEST_FLAG_SET(death_test_style, "threadsafe");

  task_worker                          async_task_worker("async_thread", 2 * RADIO_MAX_NOF_PORTS);
  std::unique_ptr<task_executor>       async_task_executor = make_task_executor_ptr(async_task_worker);
  radio_factory_realtime_loopback_impl factory;
  radio_notifier_spy                   notifier;

  auto create = [&](const radio_configuration::radio& config) {
    return factory.create(config, *async_task_executor, notifier);
  };

  ASSERT_DEATH(
      {
        auto session = create(make_radio_config(1, 1));
        session->start(0);
        session->start(0);
      },
      "already running");
  ASSERT_DEATH(
      {
        auto session = create(make_radio_config(2, 1));
        session->get_baseband_gateway(2);
      },
      "exceeds the number of streams");
  ASSERT_DEATH(
      {
        // The receiver buffers up to 10 ms of samples.
        auto                            session = create(make_radio_config(1, 1));
        baseband_gateway_buffer_dynamic rx_buffer(1, 192e3 / 100 + 1);
        session->start(0);
        session->get_baseband_gateway(0).get_receiver().receive(rx_buffer.get_writer());
      },
      "Cannot provide the requested number of samples");
  ASSERT_DEATH(
      {
        radio_configuration::radio config = make_radio_config(2, 1);
        config.rx_streams.pop_back();
        create(config);
      },
      "number of transmit .* and receive .* streams must be equal");
  ASSERT_DEATH(
      {
        radio_configuration::radio config = make_radio_config(2, 1);
        config.rx_streams.back().channels.emplace_back();
        create(config);
      },
      "Stream 1 has different number of transmit");

  // Invalid RF time scales are reported as user errors.
  for (const char* args : {"rf_time_scale=0",
                           "rf_time_scale=-1",
                           "rf_time_scale=abc",
                           "rf_time_scale=",
                           "rf_time_scale=0.5x",
                           "rf_time_scale=0.5x,foo=1"}) {
    radio_configuration::radio config = make_radio_config(1, 1);
    config.args                       = args;
    ASSERT_EXIT(create(config), ::testing::ExitedWithCode(1), "Invalid rf_time_scale") << args;
  }

  async_task_worker.stop();
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(RealtimeLoopbackRadioTest,
                         RealtimeLoopbackRadioFixture,
                         ::testing::Combine(::testing::Values(1, 2),
                                            ::testing::Values(39, 123),
                                            ::testing::Values(39, 123)));
