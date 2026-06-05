// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/ntn/converters/ephemeris_info_converter.h"
#include "lib/ntn/orbit_ephemeris_info.h"
#include "lib/ntn/propagators/keplerian_propagator.h"
#include <cmath>
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocudu_ntn;

static constexpr double MU_EARTH = 3.986004418e14;

static std::chrono::system_clock::time_point make_epoch()
{
  return std::chrono::system_clock::from_time_t(0);
}

// Zero duration must leave the ECI state and epoch unchanged.
TEST(keplerian_propagator_test, zero_duration_is_no_op)
{
  keplerian_propagator propagator;
  auto                 epoch = make_epoch();
  orbital_elements oe{6877286.310, 0.0012074892907, 0.9295782970760, 2.0359402522293, 0.8669148350739, 5.4309100278415};
  orbit_ephemeris_info orbit{propagator, epoch, oe};

  const state_vector init_eci = orbit.eci_rv();
  orbit.propagate(std::chrono::duration<double>(0.0), false);
  const state_vector final_eci = orbit.eci_rv();

  state_vector diff = final_eci - init_eci;
  EXPECT_NEAR(norm(diff.position), 0.0, 1e-9);
  EXPECT_NEAR(norm(diff.velocity), 0.0, 1e-12);
  EXPECT_EQ(orbit.epoch_time(), epoch);
}

// ECI state after dt must match orbital_to_eci(oe with M advanced by n*dt).
TEST(keplerian_propagator_test, eci_state_matches_advanced_mean_anomaly)
{
  const orbital_elements oe{
      6877286.310, 0.0012074892907, 0.9295782970760, 2.0359402522293, 0.8669148350739, 5.4309100278415};
  auto   epoch = make_epoch();
  double n     = std::sqrt(MU_EARTH / (oe.semi_major_axis * oe.semi_major_axis * oe.semi_major_axis));

  for (double dt : {1.0, 5.0, 10.0, 60.0, 300.0}) {
    keplerian_propagator propagator;
    orbit_ephemeris_info orbit{propagator, epoch, oe};
    orbit.propagate(std::chrono::duration<double>(dt), false);

    // Expected: advance M by n*dt, then convert to ECI.
    orbital_elements expected_oe = oe;
    expected_oe.mean_anomaly     = std::fmod(oe.mean_anomaly + n * dt, 2.0 * M_PI);
    state_vector expected_eci    = ephemeris_info_converter::orbital_to_eci(expected_oe);

    state_vector diff = orbit.eci_rv() - expected_eci;
    EXPECT_NEAR(norm(diff.position), 0.0, 1e-6) << "dt=" << dt; // 1 um position
    EXPECT_NEAR(norm(diff.velocity), 0.0, 1e-9) << "dt=" << dt; // 1 nm/s velocity

    auto expected_epoch =
        epoch + std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::duration<double>(dt));
    auto expected_ms = std::chrono::duration_cast<std::chrono::milliseconds>(expected_epoch.time_since_epoch()).count();
    auto actual_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(orbit.epoch_time().time_since_epoch()).count();
    EXPECT_EQ(actual_ms, expected_ms) << "dt=" << dt;
  }
}

// Negative dt propagates backward correctly.
TEST(keplerian_propagator_test, backward_propagation)
{
  const orbital_elements oe{
      6877286.310, 0.0012074892907, 0.9295782970760, 2.0359402522293, 0.8669148350739, 5.4309100278415};
  auto   epoch = make_epoch();
  double n     = std::sqrt(MU_EARTH / (oe.semi_major_axis * oe.semi_major_axis * oe.semi_major_axis));

  for (double dt : {-1.0, -60.0, -300.0}) {
    keplerian_propagator propagator;
    orbit_ephemeris_info orbit{propagator, epoch, oe};
    orbit.propagate(std::chrono::duration<double>(dt), false);

    orbital_elements expected_oe = oe;
    expected_oe.mean_anomaly     = std::fmod(oe.mean_anomaly + n * dt, 2.0 * M_PI);
    if (expected_oe.mean_anomaly < 0.0) {
      expected_oe.mean_anomaly += 2.0 * M_PI;
    }
    state_vector expected_eci = ephemeris_info_converter::orbital_to_eci(expected_oe);

    state_vector diff = orbit.eci_rv() - expected_eci;
    EXPECT_NEAR(norm(diff.position), 0.0, 1e-6) << "dt=" << dt;
    EXPECT_NEAR(norm(diff.velocity), 0.0, 1e-9) << "dt=" << dt;
  }
}

// After one full orbital period T = 2pi/n, ECI state is recovered.
TEST(keplerian_propagator_test, full_period_recovers_initial_state)
{
  // Circular orbit (e=0) for exact ECI recovery.
  const orbital_elements oe{7000000.0, 0.0, 0.5, 1.0, 0.0, 0.3};
  auto                   epoch = make_epoch();

  double n = std::sqrt(MU_EARTH / (oe.semi_major_axis * oe.semi_major_axis * oe.semi_major_axis));
  double T = 2.0 * M_PI / n;

  keplerian_propagator propagator;
  orbit_ephemeris_info orbit{propagator, epoch, oe};
  state_vector         init_eci = orbit.eci_rv();

  orbit.propagate(std::chrono::duration<double>(T), false);

  state_vector final_eci = orbit.eci_rv();
  state_vector diff      = final_eci - init_eci;
  EXPECT_NEAR(norm(diff.position), 0.0, 1e-3); // 1 mm
  EXPECT_NEAR(norm(diff.velocity), 0.0, 1e-6); // 1 um/s
}

// For a circular orbit (e=0) the orbital radius |r| must equal a at every time step.
TEST(keplerian_propagator_test, circular_orbit_constant_radius)
{
  const orbital_elements oe{7000000.0, 0.0, 0.5, 1.0, 0.0, 0.0};
  auto                   epoch = make_epoch();

  for (double dt : {100.0, 500.0, 1000.0, 3600.0}) {
    keplerian_propagator propagator;
    orbit_ephemeris_info orbit{propagator, epoch, oe};
    orbit.propagate(std::chrono::duration<double>(dt), false);

    double radius = norm(orbit.eci_rv().position);
    EXPECT_NEAR(radius, oe.semi_major_axis, 1.0) << "dt=" << dt; // 1 m tolerance
  }
}
