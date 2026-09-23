// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/ntn/converters/ephemeris_info_converter.h"
#include "lib/ntn/converters/reference_frame_converter.h"
#include "lib/ntn/coordinates_types.h"
#include "tests/ocudu_test_requirements.h"
#include "ocudu/support/test_utils.h"
#include "fmt/chrono.h"
#include <cmath>
#include <gtest/gtest.h>
#include <iomanip>
#include <string>
#include <utility>
#include <vector>

using namespace ocudu;
using namespace ocudu_ntn;

static std::chrono::system_clock::time_point string_to_timepoint(const std::string& input)
{
  std::tm            tm = {};
  std::istringstream ss(input);
  ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
  if (ss.fail()) {
    throw std::runtime_error("Failed to parse time string");
  }
  std::time_t time = timegm(&tm);
  return std::chrono::system_clock::from_time_t(time);
}

std::vector<std::tuple<std::string, state_vector, state_vector, orbital_elements>> test_cases = {
    {"2025-06-24T09:56:06",
     {{426298.38780814, -6858652.52514232, 9543.75174417}, {4047.63303185, 266.96864565, 6105.64101511}},
     {{426298.38780814, -6858652.52514232, 9543.75174417}, {4547.7738715, 298.05481495, 6105.64101511}},
     {6877288.146125763,
      0.0011959644075247937,
      0.9295784053891714,
      4.77342729482894,
      0.8582612758585331,
      5.428463295733399}}, // LEO
    {"2025-06-24T10:05:05",
     {{156461.00865337, -6870106.60784671, 9541.60256166}, {4054.99738691, 107.71034985, 6105.64101926}},
     {{156461.00865337, -6870106.60784671, 9541.60256166}, {4555.97347145, 119.11966676, 6105.64101926}},
     {6877288.14613685,
      0.0011959639962290003,
      0.9295784053900793,
      4.734122790332842,
      0.8582609750543692,
      5.428463205786013}}, // LEO
    {"2025-06-24T10:08:20",
     {{-37503343.02913827, -19400141.56092045, -1761936.81616018}, {2.23911112e-01, 3.61092234e+01, -3.84824389e+02}},
     {{-37503343.02913827, -19400141.56092045, -1761936.81616018}, {1414.90457236, -2698.67773412, -384.82438876}},
     {42265116.092437394,
      0.00025944641683577183,
      0.13244745668928243,
      0.15881196577965917,
      4.623911838194668,
      5.122540519142772}}, // GEO
    {"2025-06-24T10:12:48",
     {{-37875288.29121766, -18663567.84665889, -1761937.13881396}, {0.92949129, 36.09800434, -384.82479912}},
     {{-37875288.29121766, -18663567.84665889, -1761937.13881396}, {1361.89834915, -2725.81163, -384.82479912}},
     {42265116.09214217,
      0.00025944659083169117,
      0.13244758696156164,
      0.1392695527848184,
      4.623909661517107,
      5.122542431510426}}, // GEO
};

TEST(test_converters, ecef_rv_2_oe_test)
{
  OCUDU_TEST_REQUIREMENTS("DU-NTN-SI-1");

  double sma_tolerance = 1e-2; // 1cm
  double tolerance     = 1e-6;
  double pos_tolerance = 1e-3; // m -> 0.1cm
  double vel_tolerance = 1e-6; // m/s
  double pos_error     = 0;
  double vel_error     = 0;

  for (const auto& [utc_time, ecef_rv, expected_eci_rv, expected_oe] : test_cases) {
    auto                      tp = string_to_timepoint(utc_time);
    reference_frame_converter ref_frame_converter(tp);
    state_vector              eci_rv = ref_frame_converter.ecef_to_eci(ecef_rv);

    state_vector rv_diff = eci_rv - expected_eci_rv;
    pos_error            = norm(rv_diff.position);
    vel_error            = norm(rv_diff.velocity);
    ASSERT_NEAR(pos_error, 0.0, pos_tolerance);
    ASSERT_NEAR(vel_error, 0.0, vel_tolerance);

    orbital_elements oe = ephemeris_info_converter::eci_to_orbital(eci_rv);
    ASSERT_NEAR(oe.semi_major_axis, expected_oe.semi_major_axis, sma_tolerance);
    ASSERT_NEAR(oe.eccentricity, expected_oe.eccentricity, tolerance);
    ASSERT_NEAR(oe.inclination, expected_oe.inclination, tolerance);
    ASSERT_NEAR(oe.longitude, expected_oe.longitude, tolerance);
    ASSERT_NEAR(oe.periapsis, expected_oe.periapsis, tolerance);
    ASSERT_NEAR(oe.mean_anomaly, expected_oe.mean_anomaly, tolerance);

    state_vector eci_rv_again = ephemeris_info_converter::orbital_to_eci(oe);
    rv_diff                   = eci_rv_again - expected_eci_rv;
    pos_error                 = norm(rv_diff.position);
    vel_error                 = norm(rv_diff.velocity);
    ASSERT_NEAR(pos_error, 0.0, pos_tolerance);
    ASSERT_NEAR(vel_error, 0.0, vel_tolerance);
  }
}

TEST(test_converters, oe_2_ecef_rvs_test)
{
  OCUDU_TEST_REQUIREMENTS("DU-NTN-SI-1");

  double sma_tolerance = 1e-2; // 1cm
  double tolerance     = 1e-6;
  double pos_tolerance = 1e-3; // m -> 0.1cm
  double vel_tolerance = 1e-6; // m/s
  double pos_error     = 0;
  double vel_error     = 0;

  for (const auto& [utc_time, expected_ecef_rv, expected_eci_rv, oe_gold] : test_cases) {
    state_vector eci_rv  = ephemeris_info_converter::orbital_to_eci(oe_gold);
    state_vector rv_diff = eci_rv - expected_eci_rv;
    pos_error            = norm(rv_diff.position);
    vel_error            = norm(rv_diff.velocity);
    ASSERT_NEAR(pos_error, 0.0, pos_tolerance);
    ASSERT_NEAR(vel_error, 0.0, vel_tolerance);

    auto                      tp = string_to_timepoint(utc_time);
    reference_frame_converter ref_frame_converter(tp);
    state_vector              ecef_rv = ref_frame_converter.eci_to_ecef(eci_rv);
    rv_diff                           = ecef_rv - expected_ecef_rv;
    pos_error                         = norm(rv_diff.position);
    vel_error                         = norm(rv_diff.velocity);
    ASSERT_NEAR(pos_error, 0.0, pos_tolerance);
    ASSERT_NEAR(vel_error, 0.0, vel_tolerance);

    state_vector eci_rv_again = ref_frame_converter.ecef_to_eci(ecef_rv);
    rv_diff                   = eci_rv_again - expected_eci_rv;
    pos_error                 = norm(rv_diff.position);
    vel_error                 = norm(rv_diff.velocity);
    ASSERT_NEAR(pos_error, 0.0, pos_tolerance);
    ASSERT_NEAR(vel_error, 0.0, vel_tolerance);

    orbital_elements oe = ephemeris_info_converter::eci_to_orbital(eci_rv_again);
    ASSERT_NEAR(oe.semi_major_axis, oe_gold.semi_major_axis, sma_tolerance);
    ASSERT_NEAR(oe.eccentricity, oe_gold.eccentricity, tolerance);
    ASSERT_NEAR(oe.inclination, oe_gold.inclination, tolerance);
    ASSERT_NEAR(oe.longitude, oe_gold.longitude, tolerance);
    ASSERT_NEAR(oe.periapsis, oe_gold.periapsis, tolerance);
    ASSERT_NEAR(oe.mean_anomaly, oe_gold.mean_anomaly, tolerance);
  }
}

namespace {

bool all_elements_finite(const orbital_elements& oe)
{
  return std::isfinite(oe.semi_major_axis) && std::isfinite(oe.eccentricity) && std::isfinite(oe.inclination) &&
         std::isfinite(oe.longitude) && std::isfinite(oe.periapsis) && std::isfinite(oe.mean_anomaly);
}

// Round-trips a set of orbital elements through the ECI state and back to elements, then reconstructs the state
// once more. The classical elements are convention-dependent at degenerate geometries, so the state -- which is
// unambiguous -- is what must be preserved. Returns the {position, velocity} error between the two states.
std::pair<double, double> element_state_round_trip_error(const orbital_elements& oe)
{
  const state_vector     s0  = ephemeris_info_converter::orbital_to_eci(oe);
  const orbital_elements oe1 = ephemeris_info_converter::eci_to_orbital(s0);
  const state_vector     s1  = ephemeris_info_converter::orbital_to_eci(oe1);
  const state_vector     d   = s1 - s0;
  return {norm(d.position), norm(d.velocity)};
}

// Mean anomalies at, and just either side of, the two points where a cosine-ratio formulation of the in-plane
// angles is ill-conditioned: the satellite crosses that angle's own reference direction at M = 0 (periapsis, or
// the ascending node when circular) and the opposite one at M = pi, so the cosine is +/-1 at both. The
// off-boundary neighbours catch a formulation that is exact at the boundary itself but not around it.
const std::vector<double> boundary_phases = {0.0, 1e-9, 1e-6, 1e-3, M_PI - 1e-6, M_PI, M_PI + 1e-6, 2.0 * M_PI - 1e-6};

// Reconstruction accuracy required of every round trip below. Set around a thousand times the error actually
// observed, which leaves a different libm room to round an atan2 or a sine its own way while still catching any
// real loss of conditioning: every defect these tests were written for overshot these bounds by 600x or more.
const double pos_tolerance = 1e-5; // m
const double vel_tolerance = 1e-8; // m/s

// The three degenerate geometries, shared so that the single-phase tests and the phase sweep cover the same
// orbits. The mean anomaly carried here is the single-phase sample; the sweep overrides it.
const orbital_elements circular_inclined_orbit{7000e3, 0.0, 30.0 * M_PI / 180.0, 0.8, 0.0, 1.5};
const orbital_elements equatorial_elliptical_orbit{7000e3, 0.1, 0.0, 0.0, 0.6, 1.2};
const orbital_elements circular_equatorial_orbit{7000e3, 0.0, 0.0, 0.0, 0.0, 1.0};

} // namespace

// eci_to_orbital robustness where the classical angle formulas are undefined or ill-conditioned: the circular
// (eccentricity vector -> 0) and equatorial (node vector -> 0) singularities, and the phases where a cosine ratio
// lands on +/-1. Both failure modes are quiet -- elements come back finite but wrong, or carrying only half the
// available digits -- and either way the error propagates into the anomaly chain and any derived SIB19 ephemeris.
// So the tests below check that every recovered element stays finite and that the state itself round-trips.

TEST(test_converters, eci_to_orbital_at_apoapsis_stays_finite)
{
  OCUDU_TEST_REQUIREMENTS("DU-NTN-SI-1");

  // Near-circular inclined orbit sampled at apoapsis (mean anomaly = pi), where the true-anomaly cosine is -1:
  // the case that historically returned NaN and propagated it into mean_anomaly.
  const orbital_elements oe{7000e3, 1e-3, 30.0 * M_PI / 180.0, 0.8, 1.1, M_PI};
  const orbital_elements got = ephemeris_info_converter::eci_to_orbital(ephemeris_info_converter::orbital_to_eci(oe));
  ASSERT_TRUE(all_elements_finite(got)) << "apoapsis true anomaly poisons the whole element set with a NaN";
  // Beyond finiteness: the recovered mean anomaly must be pi (apoapsis), not merely some finite value.
  EXPECT_NEAR(got.mean_anomaly, M_PI, 1e-11);
}

TEST(test_converters, eci_to_orbital_boundary_geometries_stay_finite)
{
  OCUDU_TEST_REQUIREMENTS("DU-NTN-SI-1");

  // Sweep periapsis and apoapsis over eccentricity and orientation, staying clear of the e=0 / i=0 singularities
  // (covered separately below). A large fraction of these historically produced NaN.
  for (double e : {1e-3, 1e-2, 0.1, 0.4}) {
    for (int i_deg : {1, 20, 60, 89, 91, 120, 179}) {
      for (int raan_deg : {0, 90, 200, 300}) {
        for (int argp_deg : {0, 90, 200, 300}) {
          for (double M : {0.0, M_PI}) {
            const orbital_elements oe{
                7000e3, e, i_deg * M_PI / 180.0, raan_deg * M_PI / 180.0, argp_deg * M_PI / 180.0, M};
            const orbital_elements got =
                ephemeris_info_converter::eci_to_orbital(ephemeris_info_converter::orbital_to_eci(oe));
            ASSERT_TRUE(all_elements_finite(got)) << "NaN element at e=" << e << " i=" << i_deg << " raan=" << raan_deg
                                                  << " argp=" << argp_deg << " M=" << M;
          }
        }
      }
    }
  }
}

TEST(test_converters, eci_to_orbital_circular_inclined_orbit_is_finite_and_round_trips)
{
  OCUDU_TEST_REQUIREMENTS("DU-NTN-SI-1");

  // Exactly circular (e = 0): the eccentricity vector vanishes, so argument of periapsis and true anomaly are
  // 0/0. Convention: periapsis = 0, and the in-plane phase is carried by the argument of latitude in mean_anomaly.
  const orbital_elements& oe  = circular_inclined_orbit;
  const orbital_elements  got = ephemeris_info_converter::eci_to_orbital(ephemeris_info_converter::orbital_to_eci(oe));

  ASSERT_TRUE(all_elements_finite(got)) << "circular orbit (ev_mag -> 0) must not yield NaN";
  EXPECT_NEAR(got.eccentricity, 0.0, 1e-12);
  EXPECT_NEAR(got.inclination, 30.0 * M_PI / 180.0, 1e-12);
  EXPECT_NEAR(got.longitude, 0.8, 1e-12);
  EXPECT_NEAR(got.periapsis, 0.0, 1e-12) << "argument of periapsis is 0 by convention when circular";
  EXPECT_NEAR(got.mean_anomaly, 1.5, 1e-12) << "in-plane phase (argument of latitude) carried by mean_anomaly";

  const auto [pos_err, vel_err] = element_state_round_trip_error(oe);
  EXPECT_LT(pos_err, pos_tolerance);
  EXPECT_LT(vel_err, vel_tolerance);
}

TEST(test_converters, eci_to_orbital_equatorial_elliptical_orbit_is_finite_and_round_trips)
{
  OCUDU_TEST_REQUIREMENTS("DU-NTN-SI-1");

  // Exactly equatorial (i = 0): the node vector vanishes, so the ascending node (longitude) and the periapsis
  // measured from it are undefined. Convention: longitude = 0, periapsis = longitude of periapsis from the x-axis.
  const orbital_elements& oe  = equatorial_elliptical_orbit;
  const orbital_elements  got = ephemeris_info_converter::eci_to_orbital(ephemeris_info_converter::orbital_to_eci(oe));

  ASSERT_TRUE(all_elements_finite(got)) << "equatorial orbit (n_mag -> 0) must not yield NaN";
  EXPECT_NEAR(got.eccentricity, 0.1, 1e-12);
  EXPECT_NEAR(got.inclination, 0.0, 1e-12);
  EXPECT_NEAR(got.longitude, 0.0, 1e-12) << "ascending node is 0 by convention when equatorial";
  EXPECT_NEAR(got.periapsis, 0.6, 1e-12) << "longitude of periapsis measured from the x-axis";
  EXPECT_NEAR(got.mean_anomaly, 1.2, 1e-12);

  const auto [pos_err, vel_err] = element_state_round_trip_error(oe);
  EXPECT_LT(pos_err, pos_tolerance);
  EXPECT_LT(vel_err, vel_tolerance);
}

TEST(test_converters, eci_to_orbital_circular_equatorial_orbit_is_finite_and_round_trips)
{
  OCUDU_TEST_REQUIREMENTS("DU-NTN-SI-1");

  // Both singularities at once (e = 0 and i = 0): longitude and periapsis undefined. Convention: both 0, and the
  // phase is the true longitude carried by mean_anomaly.
  const orbital_elements& oe  = circular_equatorial_orbit;
  const orbital_elements  got = ephemeris_info_converter::eci_to_orbital(ephemeris_info_converter::orbital_to_eci(oe));

  ASSERT_TRUE(all_elements_finite(got)) << "circular equatorial orbit must not yield NaN";
  EXPECT_NEAR(got.eccentricity, 0.0, 1e-12);
  EXPECT_NEAR(got.inclination, 0.0, 1e-12);
  EXPECT_NEAR(got.longitude, 0.0, 1e-12);
  EXPECT_NEAR(got.periapsis, 0.0, 1e-12);
  EXPECT_NEAR(got.mean_anomaly, 1.0, 1e-12) << "true longitude carried by mean_anomaly";

  const auto [pos_err, vel_err] = element_state_round_trip_error(oe);
  EXPECT_LT(pos_err, pos_tolerance);
  EXPECT_LT(vel_err, vel_tolerance);
}

// The three tests above each sample a single, benign phase. Phase is an independent axis of ill-conditioning: at
// the boundary phases a cosine-ratio formulation silently degrades to sqrt(machine epsilon), roughly 1e-8 rad,
// which at LEO radii is a ~0.1 m error in the reconstructed state -- two orders past the tolerance below, and of
// the same order as the SIB19 mean-anomaly quantisation step of 2.341e-8 rad.
//
// The recovered elements are deliberately not compared against the inputs here: at exactly these phases the
// angles are free to wrap (0 versus 2*pi) and, at a degenerate geometry, to redistribute between the
// convention-dependent pair. The state vector has neither freedom, so it is the invariant worth asserting.

TEST(test_converters, eci_to_orbital_degenerate_orbits_round_trip_at_boundary_phases)
{
  OCUDU_TEST_REQUIREMENTS("DU-NTN-SI-1");

  // The same three degenerate geometries as the tests above, each swept over the boundary phases. For the
  // circular ones the swept element is the argument of latitude rather than a true anomaly, which is the
  // quantity that vanishes at the ascending node.
  const std::vector<std::pair<std::string, orbital_elements>> geometries = {
      {"circular inclined", circular_inclined_orbit},
      {"equatorial elliptical", equatorial_elliptical_orbit},
      {"circular equatorial", circular_equatorial_orbit}};

  for (const auto& [name, base_oe] : geometries) {
    for (double phase : boundary_phases) {
      orbital_elements oe = base_oe;
      oe.mean_anomaly     = phase;

      const orbital_elements got =
          ephemeris_info_converter::eci_to_orbital(ephemeris_info_converter::orbital_to_eci(oe));
      ASSERT_TRUE(all_elements_finite(got)) << "NaN element for " << name << " at phase " << phase;

      const auto [pos_err, vel_err] = element_state_round_trip_error(oe);
      EXPECT_LT(pos_err, pos_tolerance) << name << " at phase " << phase;
      EXPECT_LT(vel_err, vel_tolerance) << name << " at phase " << phase;
    }
  }
}

TEST(test_converters, eci_to_orbital_round_trips_at_the_apsides_and_with_periapsis_on_the_line_of_nodes)
{
  OCUDU_TEST_REQUIREMENTS("DU-NTN-SI-1");

  // Ordinary, fully non-degenerate orbits: no eccentricity or node vector is anywhere near vanishing, so this
  // exercises the shared elliptical path rather than either singularity branch. Two angles are ill-conditioned
  // here for reasons that have nothing to do with degeneracy: the true anomaly at the apsides (swept via the
  // boundary phases) and the argument of periapsis when periapsis sits on the line of nodes (argp = 0 or pi,
  // both routine configured values, hence swept explicitly).
  for (double e : {1e-3, 0.1, 0.4}) {
    for (int argp_deg : {0, 90, 180, 270}) {
      for (double phase : boundary_phases) {
        const orbital_elements oe{7000e3, e, 60.0 * M_PI / 180.0, 0.8, argp_deg * M_PI / 180.0, phase};

        const orbital_elements got =
            ephemeris_info_converter::eci_to_orbital(ephemeris_info_converter::orbital_to_eci(oe));
        ASSERT_TRUE(all_elements_finite(got)) << "NaN element at e=" << e << " argp=" << argp_deg << " phase=" << phase;

        const auto [pos_err, vel_err] = element_state_round_trip_error(oe);
        EXPECT_LT(pos_err, pos_tolerance) << "e=" << e << " argp=" << argp_deg << " phase=" << phase;
        EXPECT_LT(vel_err, vel_tolerance) << "e=" << e << " argp=" << argp_deg << " phase=" << phase;
      }
    }
  }
}

TEST(test_converters, eci_to_orbital_recovers_inclinations_just_above_the_equatorial_threshold)
{
  OCUDU_TEST_REQUIREMENTS("DU-NTN-SI-1");

  // The band between the equatorial threshold and the smallest inclination the other tests cover. Nothing here is
  // degenerate -- the node vector is small but perfectly well determined -- yet cos(inclination) is 1.0 to within
  // a rounding error across this whole range, so recovering the inclination from that cosine would floor it to 0
  // and drop the orbit onto the equator. Assert the inclination itself, not just the round trip: at these angles
  // an error in it is partly absorbed by the other elements, so the state alone understates the damage.
  for (double inclination : {1e-10, 1e-9, 1e-8, 1e-7, 1e-6, 1e-5}) {
    for (double e : {0.0, 1e-3, 0.1}) {
      const orbital_elements oe{7000e3, e, inclination, 0.0, 0.6, 1.2};
      const orbital_elements got =
          ephemeris_info_converter::eci_to_orbital(ephemeris_info_converter::orbital_to_eci(oe));

      ASSERT_TRUE(all_elements_finite(got)) << "NaN element at i=" << inclination << " e=" << e;
      EXPECT_NEAR(got.inclination, inclination, 1e-9 * inclination)
          << "inclination floored towards the equator at i=" << inclination << " e=" << e;

      const auto [pos_err, vel_err] = element_state_round_trip_error(oe);
      EXPECT_LT(pos_err, pos_tolerance) << "i=" << inclination << " e=" << e;
      EXPECT_LT(vel_err, vel_tolerance) << "i=" << inclination << " e=" << e;
    }
  }
}
