// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/radio/radio_factory.h"
#include "ocudu/support/executors/task_worker.h"
#include "fmt/ostream.h"
#include "gtest/gtest.h"

using namespace ocudu;

namespace fmt {

template <>
struct formatter<ocudu::radio_configuration::clock_sources::source> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::radio_configuration::clock_sources::source& source, FormatContext& ctx) const
  {
    switch (source) {
      case ocudu::radio_configuration::clock_sources::source::DEFAULT:
        return fmt::format_to(ctx.out(), "default");
      case ocudu::radio_configuration::clock_sources::source::INTERNAL:
        return fmt::format_to(ctx.out(), "internal");
      case ocudu::radio_configuration::clock_sources::source::EXTERNAL:
        return fmt::format_to(ctx.out(), "external");
      case ocudu::radio_configuration::clock_sources::source::GPSDO:
      default:
        return fmt::format_to(ctx.out(), "gpsdo");
    }
  }
};

template <>
struct formatter<ocudu::radio_configuration::radio> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::radio_configuration::radio& config, FormatContext& ctx) const
  {
    return fmt::format_to(ctx.out(), "clock_src={} sync_src={}", config.clock.clock, config.clock.sync);
  }
};
} // namespace fmt

namespace {

const radio_configuration::clock_sources base_clock_sources = {radio_configuration::clock_sources::source::DEFAULT,
                                                               radio_configuration::clock_sources::source::DEFAULT};

const radio_configuration::lo_frequency base_lo_frequency = {3.5e9, 0.0};

const radio_configuration::channel base_tx_channel = {base_lo_frequency, 0.0, ""};

const radio_configuration::channel base_rx_channel = {base_lo_frequency, 0.0, ""};

const radio_configuration::stream base_tx_stream = {{base_tx_channel}, ""};

const radio_configuration::stream base_rx_stream = {{base_rx_channel}, ""};

const radio_configuration::radio radio_base_config = {base_clock_sources,
                                                      {base_tx_stream},
                                                      {base_rx_stream},
                                                      100e6,
                                                      radio_configuration::over_the_wire_format::DEFAULT,
                                                      radio_configuration::transmission_mode::continuous,
                                                      0.0F,
                                                      "",
                                                      ocudulog::basic_levels::none};

struct test_case_t {
  std::function<radio_configuration::radio()> get_config;
  std::string                                 message;
};

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os, "{}", test_case.get_config());
  return os;
}

const std::vector<test_case_t> realtime_loopback_radio_validator_test_data = {
    {[] {
       radio_configuration::radio config = radio_base_config;
       return config;
     },
     ""},
    {[] {
       radio_configuration::radio config = radio_base_config;
       config.clock.clock                = radio_configuration::clock_sources::source::INTERNAL;
       return config;
     },
     "Only 'default' clock source is available.\n"},
    {[] {
       radio_configuration::radio config = radio_base_config;
       config.clock.sync                 = radio_configuration::clock_sources::source::INTERNAL;
       return config;
     },
     "Only 'default' sync source is available.\n"},
    {[] {
       radio_configuration::radio config = radio_base_config;
       config.tx_streams.clear();
       return config;
     },
     "Transmit and receive number of streams must be equal.\n"},
    {[] {
       radio_configuration::radio config = radio_base_config;
       config.tx_streams.clear();
       config.rx_streams.clear();
       return config;
     },
     "At least one transmit and one receive stream must be configured.\n"},
    {[] {
       radio_configuration::radio config = radio_base_config;
       config.tx_streams.push_back(base_tx_stream);
       config.rx_streams.push_back(base_rx_stream);
       return config;
     },
     "Only a single transmit and receive stream is currently supported.\n"},
    {[] {
       radio_configuration::radio config = radio_base_config;
       config.tx_streams.front().channels.clear();
       return config;
     },
     "Streams must contain at least one channel.\n"},
    {[] {
       radio_configuration::radio config                                   = radio_base_config;
       config.tx_streams.front().channels.front().freq.center_frequency_Hz = 0.0;
       return config;
     },
     "The center frequency must be non-zero, NAN nor infinite.\n"},
    {[] {
       radio_configuration::radio config                               = radio_base_config;
       config.tx_streams.front().channels.front().freq.lo_frequency_Hz = 1.0;
       return config;
     },
     "The custom LO frequency is not currently supported.\n"},
    {[] {
       radio_configuration::radio config                  = radio_base_config;
       config.tx_streams.front().channels.front().gain_dB = NAN;
       return config;
     },
     "Channel gain must not be NAN nor infinite.\n"},
    {[] {
       radio_configuration::radio config                  = radio_base_config;
       config.tx_streams.front().channels.front().gain_dB = INFINITY;
       return config;
     },
     "Channel gain must not be NAN nor infinite.\n"},
    {[] {
       radio_configuration::radio config = radio_base_config;
       config.tx_streams.front().args    = "some args";
       return config;
     },
     "Stream arguments are not currently supported.\n"},
    {[] {
       radio_configuration::radio config = radio_base_config;
       config.rx_streams.front().channels.clear();
       return config;
     },
     "Streams must contain at least one channel.\n"},
    {[] {
       radio_configuration::radio config                                   = radio_base_config;
       config.rx_streams.front().channels.front().freq.center_frequency_Hz = 0.0;
       return config;
     },
     "The center frequency must be non-zero, NAN nor infinite.\n"},
    {[] {
       radio_configuration::radio config                               = radio_base_config;
       config.rx_streams.front().channels.front().freq.lo_frequency_Hz = 1.0;
       return config;
     },
     "The custom LO frequency is not currently supported.\n"},
    {[] {
       radio_configuration::radio config                  = radio_base_config;
       config.rx_streams.front().channels.front().gain_dB = NAN;
       return config;
     },
     "Channel gain must not be NAN nor infinite.\n"},
    {[] {
       radio_configuration::radio config                  = radio_base_config;
       config.rx_streams.front().channels.front().gain_dB = INFINITY;
       return config;
     },
     "Channel gain must not be NAN nor infinite.\n"},
    {[] {
       radio_configuration::radio config = radio_base_config;
       config.rx_streams.front().args    = "some args";
       return config;
     },
     "Stream arguments are not currently supported.\n"},
    {[] {
       radio_configuration::radio config = radio_base_config;
       config.sampling_rate_Hz           = 0.0;
       return config;
     },
     "The sampling rate must be non-zero, NAN nor infinite.\n"},
    {[] {
       radio_configuration::radio config = radio_base_config;
       config.sampling_rate_Hz           = -1.0;
       return config;
     },
     "The sampling rate must be greater than zero.\n"},
    {[] {
       radio_configuration::radio config = radio_base_config;
       config.otw_format                 = radio_configuration::over_the_wire_format::SC12;
       return config;
     },
     "Only default OTW format is currently supported.\n"},
    {[] {
       radio_configuration::radio config = radio_base_config;
       config.tx_mode                    = radio_configuration::transmission_mode::discontinuous;
       return config;
     },
     "Discontinuous transmission modes are not supported by the realtime loopback radio.\n"},
    {[] {
       radio_configuration::radio config = radio_base_config;
       config.power_ramping_us           = 1.0F;
       return config;
     },
     "Power ramping is not supported by the realtime loopback radio.\n"},
};

class RealtimeLoopbackRadioValidatorFixture : public ::testing::TestWithParam<test_case_t>
{
protected:
  static std::unique_ptr<radio_factory> factory;

  static void SetUpTestSuite()
  {
    if (factory) {
      return;
    }

    // Create pseudo-random sequence generator.
    factory = create_radio_factory("realtime_loopback");
    ASSERT_NE(factory, nullptr);
  }
};

class radio_notifier_spy : public radio_event_notifier
{
public:
  void on_radio_rt_event(const event_description& description) override {}
};

std::unique_ptr<radio_factory> RealtimeLoopbackRadioValidatorFixture::factory = nullptr;

TEST_P(RealtimeLoopbackRadioValidatorFixture, RealtimeLoopbackRadioValidatorTest)
{
  ASSERT_NE(factory, nullptr);

  const test_case_t& param = GetParam();

  // Create configuration.
  radio_configuration::radio config = param.get_config();

  // Redirect stdout to buffer.
  ::testing::internal::CaptureStdout();

  // Determine whether the configuration is valid.
  bool        is_valid = factory->get_configuration_validator().is_configuration_valid(config);
  std::string output   = ::testing::internal::GetCapturedStdout();

  // Asserts the validity of the configuration.
  ASSERT_EQ(param.message.empty(), is_valid);

  // Asserts the contents of the standard output.
  ASSERT_EQ(output, param.message);

  // Make sure the process of creation is valid.
  if (param.message.empty()) {
    // Asynchronous task executor.
    task_worker                    async_task_worker("async_thread", 2 * RADIO_MAX_NOF_PORTS);
    std::unique_ptr<task_executor> async_task_executor = make_task_executor_ptr(async_task_worker);

    // Notifier.
    radio_notifier_spy notifier;

    std::unique_ptr<radio_session> radio = factory->create(config, *async_task_executor, notifier);
    ASSERT_NE(radio, nullptr);
  }
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(RealtimeLoopbackRadioValidatorTest,
                         RealtimeLoopbackRadioValidatorFixture,
                         ::testing::ValuesIn(realtime_loopback_radio_validator_test_data));

} // namespace
