// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/phy/support/support_factories.h"
#include "ocudu/phy/upper/channel_processors/prach/factories.h"
#include "ocudu/phy/upper/channel_processors/prach/prach_detector.h"
#include "ocudu/phy/upper/channel_processors/prach/prach_generator.h"
#include "ocudu/ran/prach/prach_preamble_information.h"
#include <algorithm>
#include <cmath>
#include <complex>
#include <gtest/gtest.h>
#include <random>

using namespace ocudu;

namespace {

struct test_case {
  prach_format_type        format;
  prach_subcarrier_spacing ra_scs;
  unsigned                 zero_correlation_zone;
  unsigned                 nof_rx_ports;
  unsigned                 target_preamble;
  unsigned                 delay_samples = 0;
};

/// Offload limits that send every occasion to the device, so that the tests compare the accelerated
/// detection rather than the fallback.
constexpr prach_detector_cuda_configuration always_offload = {.min_short_preamble_work = 0,
                                                              .min_long_preamble_work  = 0};

std::unique_ptr<prach_buffer> create_test_buffer(const test_case& test)
{
  if (is_long_preamble(test.format)) {
    return create_prach_buffer_long(test.nof_rx_ports, 1);
  }
  return create_prach_buffer_short(test.nof_rx_ports, 1, 1);
}

/// Writes a preamble the detector is expected to find, optionally delayed by a number of samples.
void fill_detectable_preamble(prach_buffer& buffer, const test_case& test)
{
  std::unique_ptr<prach_generator> generator = create_prach_generator_factory_sw()->create();
  prach_generator::configuration   generator_config;
  generator_config.format                = test.format;
  generator_config.root_sequence_index   = 0;
  generator_config.preamble_index        = test.target_preamble;
  generator_config.restricted_set        = restricted_set_config::UNRESTRICTED;
  generator_config.zero_correlation_zone = test.zero_correlation_zone;

  span<const cf_t> sequence    = generator->generate(generator_config);
  unsigned         nof_symbols = is_long_preamble(test.format)
                                     ? get_prach_preamble_long_info(test.format).nof_symbols
                                     : get_prach_preamble_short_info(test.format, test.ra_scs, false).nof_symbols;
  unsigned         dft_size    = is_long_preamble(test.format) ? 1024 : 256;
  unsigned         half        = sequence.size() / 2;

  for (unsigned i_port = 0; i_port != test.nof_rx_ports; ++i_port) {
    for (unsigned i_symbol = 0; i_symbol != nof_symbols; ++i_symbol) {
      span<cbf16_t> symbol = buffer.get_symbol(i_port, 0, 0, i_symbol);
      for (unsigned i_re = 0; i_re != sequence.size(); ++i_re) {
        // A delay is a linear phase ramp across the sequence.
        int   fft_bin = (i_re < half) ? static_cast<int>(dft_size - half + i_re) : static_cast<int>(i_re - half);
        float phase   = -2.0F * static_cast<float>(M_PI) * static_cast<float>(fft_bin * test.delay_samples) /
                      static_cast<float>(dft_size);
        symbol[i_re] = to_cbf16(sequence[i_re] * std::polar(1.0F, phase));
      }
    }
  }
}

prach_detector::configuration make_detector_config(const test_case& test)
{
  return {.root_sequence_index   = 0,
          .format                = test.format,
          .restricted_set        = restricted_set_config::UNRESTRICTED,
          .zero_correlation_zone = test.zero_correlation_zone,
          .start_preamble_index  = 0,
          .nof_preamble_indices  = 64,
          .ra_scs                = test.ra_scs,
          .nof_rx_ports          = test.nof_rx_ports,
          .slot                  = slot_point(0, 0, 0)};
}

const prach_detection_result::preamble_indication* find_preamble(const prach_detection_result& result,
                                                                 unsigned                      preamble_index)
{
  for (const auto& preamble : result.preambles) {
    if (preamble.preamble_index == preamble_index) {
      return &preamble;
    }
  }
  return nullptr;
}

/// Detector pair used by every test: the generic detector and the accelerated one falling back to an
/// instance of it.
struct detector_pair {
  std::unique_ptr<prach_detector> cpu;
  std::unique_ptr<prach_detector> gpu;
};

detector_pair create_detectors(const prach_detector_cuda_configuration& config)
{
  std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw_fast();
  if (dft_factory == nullptr) {
    return {};
  }
  std::shared_ptr<prach_generator_factory> generator_factory = create_prach_generator_factory_sw();
  std::shared_ptr<prach_detector_factory>  cpu_factory =
      create_prach_detector_factory_sw(dft_factory, generator_factory);

  std::shared_ptr<prach_detector_factory> gpu_factory =
      create_prach_detector_factory_cuda(cpu_factory, generator_factory, config);
  if (gpu_factory == nullptr) {
    return {};
  }

  return {cpu_factory->create(), gpu_factory->create()};
}

class prach_detector_cuda_fixture : public ::testing::TestWithParam<test_case>
{};

} // namespace

TEST_P(prach_detector_cuda_fixture, matches_cpu_detection_for_known_preamble)
{
  detector_pair detectors = create_detectors(always_offload);
  if (detectors.gpu == nullptr) {
    GTEST_SKIP() << "No CUDA device is available for the PRACH detector";
  }

  test_case                     test   = GetParam();
  std::unique_ptr<prach_buffer> buffer = create_test_buffer(test);
  fill_detectable_preamble(*buffer, test);

  prach_detector::configuration config = make_detector_config(test);
  prach_detection_result        cpu    = detectors.cpu->detect(*buffer, config);
  prach_detection_result        gpu    = detectors.gpu->detect(*buffer, config);

  const auto* cpu_preamble = find_preamble(cpu, test.target_preamble);
  const auto* gpu_preamble = find_preamble(gpu, test.target_preamble);
  ASSERT_NE(cpu_preamble, nullptr);
  ASSERT_NE(gpu_preamble, nullptr);
  EXPECT_EQ(cpu_preamble->preamble_index, gpu_preamble->preamble_index);
  EXPECT_EQ(cpu.preambles.size(), gpu.preambles.size());
  EXPECT_NEAR(cpu_preamble->time_advance.to_seconds(), gpu_preamble->time_advance.to_seconds(), 1e-9);
  if (test.delay_samples != 0) {
    EXPECT_GT(cpu_preamble->time_advance.to_seconds(), 0.0);
  }
  // The device accumulates the correlation in a different order than the host, so the comparison
  // scales with the magnitude of the metric, which spans several orders of magnitude across the
  // cases below. The absolute margin is the floor for the small ones.
  EXPECT_NEAR(cpu_preamble->detection_metric,
              gpu_preamble->detection_metric,
              std::max(0.25F, 1.0e-3F * std::abs(cpu_preamble->detection_metric)));
  EXPECT_GT(gpu_preamble->detection_metric, 1.0F);
  EXPECT_NEAR(cpu_preamble->preamble_power_dB, gpu_preamble->preamble_power_dB, 0.3);
  EXPECT_NEAR(cpu.rssi_dB, gpu.rssi_dB, 0.2);
}

TEST(prach_detector_cuda, zero_input_does_not_create_false_preamble)
{
  detector_pair detectors = create_detectors(always_offload);
  if (detectors.gpu == nullptr) {
    GTEST_SKIP() << "No CUDA device is available for the PRACH detector";
  }

  test_case                     test{prach_format_type::B4, prach_subcarrier_spacing::kHz30, 14, 2, 20};
  std::unique_ptr<prach_buffer> buffer = create_test_buffer(test);
  ASSERT_NE(buffer, nullptr);

  prach_detector::configuration config = make_detector_config(test);
  prach_detection_result        cpu    = detectors.cpu->detect(*buffer, config);
  prach_detection_result        gpu    = detectors.gpu->detect(*buffer, config);

  EXPECT_TRUE(cpu.preambles.empty());
  EXPECT_TRUE(gpu.preambles.empty());
}

/// The accelerated detector must not raise more false alarms than the generic one on the same noise,
/// which the detection threshold alone does not guarantee: the two accumulate the correlation
/// differently.
TEST(prach_detector_cuda, awgn_false_alarm_rate_matches_cpu)
{
  detector_pair detectors = create_detectors(always_offload);
  if (detectors.gpu == nullptr) {
    GTEST_SKIP() << "No CUDA device is available for the PRACH detector";
  }

  test_case                     test{prach_format_type::zero, prach_subcarrier_spacing::kHz1_25, 0, 1, 0};
  prach_detector::configuration config = make_detector_config(test);

  constexpr unsigned              nof_trials     = 64;
  unsigned                        cpu_detections = 0;
  unsigned                        gpu_detections = 0;
  std::mt19937                    rng(0xc0ffeeu);
  std::normal_distribution<float> noise(0.0F, 0.05F);

  for (unsigned trial = 0; trial != nof_trials; ++trial) {
    std::unique_ptr<prach_buffer> buffer = create_test_buffer(test);
    ASSERT_NE(buffer, nullptr);
    unsigned nof_symbols = get_prach_preamble_long_info(test.format).nof_symbols;
    for (unsigned i_port = 0; i_port != test.nof_rx_ports; ++i_port) {
      for (unsigned i_symbol = 0; i_symbol != nof_symbols; ++i_symbol) {
        span<cbf16_t> symbol = buffer->get_symbol(i_port, 0, 0, i_symbol);
        for (unsigned i_re = 0; i_re != prach_constants::LONG_SEQUENCE_LENGTH; ++i_re) {
          symbol[i_re] = to_cbf16(cf_t{noise(rng), noise(rng)});
        }
      }
    }

    cpu_detections += static_cast<unsigned>(detectors.cpu->detect(*buffer, config).preambles.size());
    gpu_detections += static_cast<unsigned>(detectors.gpu->detect(*buffer, config).preambles.size());
  }

  EXPECT_LE(gpu_detections, cpu_detections + 2)
      << "cpu_fa=" << cpu_detections << " gpu_fa=" << gpu_detections << " trials=" << nof_trials;
}

/// The root sequences are generated once per configuration and reused, but only those carrying the
/// requested preambles are generated. A detector that keyed the reuse on the configuration alone
/// would search the second occasion below against the sequences of the first one.
TEST(prach_detector_cuda, narrowed_preamble_range_regenerates_the_root_sequences)
{
  detector_pair detectors = create_detectors(always_offload);
  if (detectors.gpu == nullptr) {
    GTEST_SKIP() << "No CUDA device is available for the PRACH detector";
  }

  test_case                     test{prach_format_type::zero, prach_subcarrier_spacing::kHz1_25, 0, 1, 40};
  std::unique_ptr<prach_buffer> buffer = create_test_buffer(test);
  fill_detectable_preamble(*buffer, test);

  // Search every preamble first, so that the whole set of root sequences is the one held.
  prach_detector::configuration full_range = make_detector_config(test);
  ASSERT_NE(find_preamble(detectors.gpu->detect(*buffer, full_range), test.target_preamble), nullptr);

  // Then search a range that starts at a later sequence, which needs a different set.
  prach_detector::configuration narrow_range = full_range;
  narrow_range.start_preamble_index          = 32;
  narrow_range.nof_preamble_indices          = 32;
  prach_detection_result cpu                 = detectors.cpu->detect(*buffer, narrow_range);
  prach_detection_result gpu                 = detectors.gpu->detect(*buffer, narrow_range);

  const auto* cpu_preamble = find_preamble(cpu, test.target_preamble);
  const auto* gpu_preamble = find_preamble(gpu, test.target_preamble);
  ASSERT_NE(cpu_preamble, nullptr);
  ASSERT_NE(gpu_preamble, nullptr);
  EXPECT_EQ(cpu.preambles.size(), gpu.preambles.size());
  EXPECT_NEAR(cpu_preamble->time_advance.to_seconds(), gpu_preamble->time_advance.to_seconds(), 1e-9);
}

/// Occasions below the offload limits are served by the fallback detector, which must produce
/// exactly what the generic detector produces.
TEST(prach_detector_cuda, occasion_below_the_offload_limit_matches_the_generic_detector)
{
  // Limits no occasion in this test can reach.
  detector_pair detectors = create_detectors({.min_short_preamble_work = 4096, .min_long_preamble_work = 4096});
  if (detectors.gpu == nullptr) {
    GTEST_SKIP() << "No CUDA device is available for the PRACH detector";
  }

  test_case                     test{prach_format_type::zero, prach_subcarrier_spacing::kHz1_25, 0, 1, 12, 8};
  std::unique_ptr<prach_buffer> buffer = create_test_buffer(test);
  fill_detectable_preamble(*buffer, test);

  prach_detector::configuration config = make_detector_config(test);
  prach_detection_result        cpu    = detectors.cpu->detect(*buffer, config);
  prach_detection_result        gpu    = detectors.gpu->detect(*buffer, config);

  ASSERT_EQ(cpu.preambles.size(), gpu.preambles.size());
  const auto* cpu_preamble = find_preamble(cpu, test.target_preamble);
  const auto* gpu_preamble = find_preamble(gpu, test.target_preamble);
  ASSERT_NE(cpu_preamble, nullptr);
  ASSERT_NE(gpu_preamble, nullptr);
  // The fallback is the generic detector, so the results are identical rather than merely close.
  EXPECT_EQ(cpu_preamble->detection_metric, gpu_preamble->detection_metric);
  EXPECT_EQ(cpu_preamble->time_advance.to_seconds(), gpu_preamble->time_advance.to_seconds());
  EXPECT_EQ(cpu.rssi_dB, gpu.rssi_dB);
}

INSTANTIATE_TEST_SUITE_P(cpu_gpu_parity,
                         prach_detector_cuda_fixture,
                         ::testing::Values(
                             // Format 0 / ZCZ 0, the common over-the-air long-preamble profile.
                             test_case{prach_format_type::zero, prach_subcarrier_spacing::kHz1_25, 0, 1, 0},
                             test_case{prach_format_type::zero, prach_subcarrier_spacing::kHz1_25, 0, 1, 46},
                             test_case{prach_format_type::zero, prach_subcarrier_spacing::kHz1_25, 0, 1, 12, 8},
                             test_case{prach_format_type::zero, prach_subcarrier_spacing::kHz1_25, 0, 1, 27, 16},
                             // Non-zero zero-correlation zones, which give the sequences several shifts.
                             test_case{prach_format_type::zero, prach_subcarrier_spacing::kHz1_25, 1, 2, 12},
                             test_case{prach_format_type::zero, prach_subcarrier_spacing::kHz1_25, 5, 1, 3, 4},
                             // Short preambles.
                             test_case{prach_format_type::B4, prach_subcarrier_spacing::kHz30, 14, 4, 20},
                             test_case{prach_format_type::B4, prach_subcarrier_spacing::kHz30, 0, 1, 1, 6}));
