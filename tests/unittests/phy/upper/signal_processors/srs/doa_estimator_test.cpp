// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/adt/format.h"
#include "ocudu/phy/upper/signal_processors/srs/doa_estimator_configuration.h"
#include "ocudu/phy/upper/signal_processors/srs/doa_estimator_factory.h"
#include "ocudu/phy/upper/signal_processors/srs/formatters.h"
#include <gtest/gtest.h>
#include <random>

using namespace ocudu;

/// \brief Checks the contract of \c doa_estimator::estimate on every returned result.
///
/// The contract has two requirements:
/// - \c doa_components is sorted by decreasing \c spectrum_strength, so the first component is the strongest one.
/// - The number of components does not exceed the number of antennas.
///
/// Consumers of the result, such as the FAPI adaptor that reports the Angle of Arrival, take the first component as
/// the strongest one without searching the list. A failure of this test means the estimator breaks that assumption.
///
/// The test checks the ordering only, not the accuracy of the estimated angles.
TEST(doa_estimator_test, result_meets_the_interface_contract)
{
  static constexpr unsigned nof_antennas = 8;
  static constexpr unsigned nof_samples  = 272;
  static constexpr unsigned nof_trials   = 100;
  static constexpr unsigned nof_sources  = 3;
  static constexpr float    noise_stddev = 0.01F;

  std::mt19937                    rgen(1234);
  std::normal_distribution<float> dist;

  for (bool cross_polarized : {false, true}) {
    std::unique_ptr<doa_estimator> estimator = create_doa_estimator_factory({.nof_antennas = nof_antennas,
                                                                             .antenna_distance_over_wavelength = 0.5F,
                                                                             .cross_polarized = cross_polarized})
                                                   ->create();

    // Number of results with more than one component. The ordering requirement is only exercised on these.
    unsigned nof_multi_component_results = 0;

    for (unsigned i_trial = 0; i_trial != nof_trials; ++i_trial) {
      // Each antenna receives a random mix of a few random sequences plus weak noise. White noise alone has no signal
      // subspace, so the estimator never reports more than one component and the ordering goes unchecked.
      std::array<cf_t, nof_sources * nof_antennas> mix;
      for (cf_t& coefficient : mix) {
        coefficient = {dist(rgen), dist(rgen)};
      }
      dynamic_tensor<2, cf_t> data({nof_samples, nof_antennas});
      for (unsigned i_sample = 0; i_sample != nof_samples; ++i_sample) {
        std::array<cf_t, nof_sources> sources;
        for (cf_t& source : sources) {
          source = {dist(rgen), dist(rgen)};
        }
        for (unsigned i_antenna = 0; i_antenna != nof_antennas; ++i_antenna) {
          cf_t& sample = data.get_view({i_antenna})[i_sample];
          sample       = noise_stddev * cf_t(dist(rgen), dist(rgen));
          for (unsigned i_source = 0; i_source != nof_sources; ++i_source) {
            sample += sources[i_source] * mix[i_source * nof_antennas + i_antenna];
          }
        }
      }

      std::optional<doa_estimator_result> result = estimator->estimate(data);
      // The contract applies to returned results only. An empty optional means the estimation failed.
      if (!result.has_value()) {
        continue;
      }

      const auto& components = result->doa_components;

      // Requirement: the number of components does not exceed the number of antennas.
      ASSERT_LE(components.size(), nof_antennas)
          << fmt::format("cross_polarized={}: more components than antennas: {}", cross_polarized, *result);

      // Requirement: the components are sorted by decreasing spectrum strength.
      ASSERT_TRUE(std::is_sorted(
          components.begin(),
          components.end(),
          [](const auto& lhs, const auto& rhs) { return lhs.spectrum_strength > rhs.spectrum_strength; }))
          << fmt::format("cross_polarized={}: components are not sorted by decreasing spectrum strength: {}",
                         cross_polarized,
                         *result);
      nof_multi_component_results += (components.size() > 1) ? 1 : 0;
    }

    // Without results carrying several components, the ordering check above passes trivially and proves nothing.
    ASSERT_GT(nof_multi_component_results, 0) << fmt::format("cross_polarized={}", cross_polarized);
  }
}
