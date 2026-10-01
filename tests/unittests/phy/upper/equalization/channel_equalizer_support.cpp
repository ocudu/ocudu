// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/phy/upper/equalization/equalization_factories.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace ocudu {

std::ostream& operator<<(std::ostream& os, channel_equalizer_algorithm_type type)
{
  switch (type) {
    case channel_equalizer_algorithm_type::zf:
      return os << "ZF";
    case channel_equalizer_algorithm_type::mmse:
      return os << "MMSE";
  }
  return os;
}

} // namespace ocudu

namespace {

using ChannelEqualizerParams = std::tuple<unsigned, unsigned, channel_equalizer_algorithm_type>;

class ChannelEqualizerFixture : public ::testing::TestWithParam<ChannelEqualizerParams>
{
protected:
  std::shared_ptr<channel_equalizer_factory> equalizer_factory;
  std::unique_ptr<channel_equalizer>         test_equalizer;

  ChannelEqualizerFixture() : TestWithParam<ParamType>() {}

  void SetUp() override
  {
    channel_equalizer_algorithm_type algorithm_type = std::get<2>(GetParam());

    equalizer_factory = create_channel_equalizer_generic_factory(algorithm_type);
    ASSERT_NE(equalizer_factory, nullptr) << "Cannot create equalizer factory";

    // Create channel equalizer.
    test_equalizer = equalizer_factory->create();
    ASSERT_NE(test_equalizer, nullptr) << "Cannot create channel equalizer";
  }
};

TEST_P(ChannelEqualizerFixture, ChannelEqualizerIsSupportedTest)
{
  const auto& param        = GetParam();
  unsigned    nof_rx_ports = std::get<0>(param);
  unsigned    nof_layers   = std::get<1>(param);

  // Support all combinations by default.
  bool expected_is_supported = true;

  // Exclude combinations that are not supported.
  expected_is_supported &=
      (nof_layers > 0) && (nof_layers <= 4) && (nof_rx_ports > 0) && (nof_rx_ports <= 4) && (nof_rx_ports != 3);

  // Exclude impossible combinations.
  expected_is_supported &= (nof_layers <= nof_rx_ports);

  // Request support to the equalizer.
  bool is_supported = test_equalizer->is_supported(nof_rx_ports, nof_layers);

  // Verify the expected support.
  ASSERT_EQ(expected_is_supported, is_supported);
}

} // namespace

INSTANTIATE_TEST_SUITE_P(ChannelEqualizerTest,
                         ChannelEqualizerFixture,
                         ::testing::Combine(::testing::Range(0U, 8U),
                                            ::testing::Range(0U, 8U),
                                            ::testing::Values(channel_equalizer_algorithm_type::zf,
                                                              channel_equalizer_algorithm_type::mmse)));
