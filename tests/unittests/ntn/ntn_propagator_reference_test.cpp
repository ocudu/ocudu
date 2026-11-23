// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/ntn/converters/coordinate_converter.h"
#include "lib/ntn/orbit_ephemeris_info.h"
#include "lib/ntn/propagators/keplerian_propagator.h"
#include "lib/ntn/propagators/rk4_propagator.h"
#include "tests/ocudu_test_requirements.h"
#include "ocudu/adt/span.h"
#include <array>
#include <cstdint>
#include <gtest/gtest.h>
#include <memory>

using namespace ocudu;
using namespace ocudu_ntn;

static constexpr double SPEED_OF_LIGHT_M_S = 299792458.0;

static std::chrono::system_clock::time_point make_epoch()
{
  return std::chrono::system_clock::from_time_t(0);
}

// LEO-600 satellite state at epoch, as per TS 38.101-5, Table G.4.1-1, decoded with the TS 38.331 EphemerisInfo
// resolutions (1.3 m, 0.06 m/s).
static const state_vector annex_g_sat_ecef = {{-2654549 * 1.3, 4386991 * 1.3, 1594205 * 1.3},
                                              {14581 * 0.06, -34487 * 0.06, 120182 * 0.06}};

// Orbital elements of the same satellite, as per TS 38.101-5, Section G.4.1.
static const orbital_elements annex_g_oe =
    {7041480.926238, 0.00895555575, 1.46884879182, 2.08314240832, 0.03544505759, 0.26290478768};

// UE location, as per TS 38.101-5, Section G.4.2.
static const coord_3d annex_g_ue_ecef = {-3025296.935779, 4925102.789830, 2687544.210048};

TEST(ntn_propagator_reference_test, annex_g_state_vector_to_orbital_elements)
{
  OCUDU_TEST_REQUIREMENTS("DU-NTN-SI-1");

  keplerian_propagator   propagator;
  orbit_ephemeris_info   orbit{propagator, make_epoch(), annex_g_sat_ecef, false};
  const orbital_elements oe = orbit.oe();

  // The reference elements are quantized to the EphemerisInfo resolutions, 4.249e-3 m and 2.341e-8 rad.
  EXPECT_NEAR(oe.semi_major_axis, annex_g_oe.semi_major_axis, 0.01);
  EXPECT_NEAR(oe.eccentricity, annex_g_oe.eccentricity, 1e-7);
  EXPECT_NEAR(oe.inclination, annex_g_oe.inclination, 1e-7);
  EXPECT_NEAR(oe.longitude, annex_g_oe.longitude, 1e-7);
  EXPECT_NEAR(oe.periapsis, annex_g_oe.periapsis, 1e-7);
  EXPECT_NEAR(oe.mean_anomaly, annex_g_oe.mean_anomaly, 1e-7);
}

TEST(ntn_propagator_reference_test, annex_g_orbital_elements_to_state_vector)
{
  OCUDU_TEST_REQUIREMENTS("DU-NTN-SI-1");

  keplerian_propagator propagator;
  orbit_ephemeris_info orbit{propagator, make_epoch(), annex_g_oe};

  // Both ephemeris formats of Section G.4.1 describe the same satellite, up to their quantization.
  const state_vector diff = orbit.ecef_rv() - annex_g_sat_ecef;
  EXPECT_NEAR(norm(diff.position), 0.0, 0.1);
  EXPECT_NEAR(norm(diff.velocity), 0.0, 1e-3);
}

TEST(ntn_propagator_reference_test, annex_g_ue_location)
{
  OCUDU_TEST_REQUIREMENTS("DU-NTN-SI-1");

  const state_vector ue = coordinate_converter::geodetic_to_ecef(25.08439333, 121.56076999, 0.0);
  EXPECT_NEAR(ue.position.x, annex_g_ue_ecef.x, 1e-3);
  EXPECT_NEAR(ue.position.y, annex_g_ue_ecef.y, 1e-3);
  EXPECT_NEAR(ue.position.z, annex_g_ue_ecef.z, 1e-3);
}

TEST(ntn_propagator_reference_test, annex_g_epoch_delay_and_doppler)
{
  OCUDU_TEST_REQUIREMENTS("DU-NTN-SI-1");

  keplerian_propagator propagator;
  orbit_ephemeris_info orbit{propagator, make_epoch(), annex_g_oe};
  const state_vector&  sat = orbit.ecef_rv();

  // Delay and Doppler as per TS 38.101-5, Section G.3.2.
  const coord_3d rho = {
      sat.position.x - annex_g_ue_ecef.x, sat.position.y - annex_g_ue_ecef.y, sat.position.z - annex_g_ue_ecef.z};
  const double range      = norm(rho);
  const double range_rate = dot(rho, sat.velocity) / range;

  // Expected values from the note of Table G.4.1-2: one-way delay 3.60 ms, Doppler 19.83 ppm.
  EXPECT_NEAR(range / SPEED_OF_LIGHT_M_S * 1e3, 3.60, 0.005);
  EXPECT_NEAR(-range_rate / SPEED_OF_LIGHT_M_S * 1e6, 19.83, 0.005);
}

namespace {

enum class propagator_kind { keplerian, rk4 };

class ntn_propagator_geo_reference_test : public ::testing::TestWithParam<propagator_kind>
{
protected:
  std::unique_ptr<orbital_propagation_model> make_propagator() const
  {
    if (GetParam() == propagator_kind::keplerian) {
      return std::make_unique<keplerian_propagator>();
    }
    return std::make_unique<rk4_propagator>();
  }
};

} // namespace

// GEO ephemeris in ECEF at 5 minute intervals, as per TR 38.821, Annex A.1. Positions are given to 1 m and velocities
// to 1 cm/s.
static const std::array<state_vector, 4> tr38821_geo_ecef = {{
    {{19151529.0, -37578251.0, 17682.0}, {-1.51, -1.02, -1.06}},
    {{19151073.0, -37578556.0, 17359.0}, {-1.52, -1.01, -1.09}},
    {{19150614.0, -37578855.0, 17029.0}, {-1.54, -0.99, -1.12}},
    {{19150150.0, -37579151.0, 16690.0}, {-1.55, -0.98, -1.14}},
}};

TEST_P(ntn_propagator_geo_reference_test, tr38821_annex_a_trajectory)
{
  OCUDU_TEST_REQUIREMENTS("DU-NTN-SI-4");

  for (unsigned i = 1, e = tr38821_geo_ecef.size(); i != e; ++i) {
    auto                 propagator = make_propagator();
    orbit_ephemeris_info orbit{*propagator, make_epoch(), tr38821_geo_ecef[0], false};
    orbit.propagate(std::chrono::duration<double>(300.0 * i));

    // The tolerances cover the table rounding: 1 cm/s of velocity error drifts 9 m over 15 minutes at most.
    const state_vector diff = orbit.ecef_rv() - tr38821_geo_ecef[i];
    EXPECT_NEAR(norm(diff.position), 0.0, 5.0) << "row=" << i;
    EXPECT_NEAR(norm(diff.velocity), 0.0, 0.02) << "row=" << i;
  }
}

INSTANTIATE_TEST_SUITE_P(propagators,
                         ntn_propagator_geo_reference_test,
                         ::testing::Values(propagator_kind::keplerian, propagator_kind::rk4));

namespace {

/// Ephemeris of one RRM test cell, given in both EphemerisInfo formats, with the delay and Doppler of its note.
struct rrm_cell_ephemeris {
  const char*            name;
  std::array<int64_t, 6> position_velocity;
  std::array<int64_t, 6> orbital;
  double                 ue_latitude_deg;
  double                 one_way_delay_ms;
  double                 doppler_ppm;
};

} // namespace

// Decodes positionVelocity-r17 fields with the resolutions of TS 38.331 EphemerisInfo.
static state_vector decode_position_velocity(const std::array<int64_t, 6>& f)
{
  return {{f[0] * 1.3, f[1] * 1.3, f[2] * 1.3}, {f[3] * 0.06, f[4] * 0.06, f[5] * 0.06}};
}

// Decodes orbital-r17 fields, in ASN.1 order (a, e, periapsis, longitude, inclination, mean anomaly).
static orbital_elements decode_orbital(const std::array<int64_t, 6>& f)
{
  constexpr double angle_step = 2.341e-8;
  return {6500000.0 + f[0] * 4.249e-3,
          f[1] * 1.431e-8,
          f[4] * angle_step,
          f[3] * angle_step,
          f[2] * angle_step,
          f[5] * angle_step};
}

// RRM test cells as per TS 38.508-1, Tables 7.6.2.1-1a to 7.6.2.1-3d. The GSO notes assume the UE at latitude 55
// degrees, as per TS 38.101-5, Section G.4.2.
static const std::array<rrm_cell_ephemeris, 6> ts38508_rrm_cells = {{
    {"gso_serving",
     {-17129898, 27505094, 1413939, 116, 378, -5944},
     {8393368589, 1257, 260802122, 240339872, 5306937, 126437178},
     55.0,
     128.7702617,
     -0.15533},
    {"gso_neighbour_1",
     {-25235934, 20178903, 2814664, 298, -241, 4398},
     {8393276102, 1898, 163688732, 71760192, 5226759, -129902267},
     55.0,
     128.775521,
     0.11173},
    {"gso_neighbour_2",
     {-6457459, 31531426, 4004156, -387, -66, -104},
     {8393723250, 1261, 65621150, 7921238, 5287833, 2180338},
     55.0,
     128.1430182,
     0.0},
    {"leo1200_serving",
     {-2935552, 4895460, 1188458, 8506, -23314, 118253},
     {273617934, 768959, 822470, 89184517, 62370124, 7827202},
     25.08439333,
     6.6726842,
     17.54711},
    {"leo1200_neighbour_1",
     {-2470752, 3858366, 3624634, 31542, -67516, 94597},
     {272202365, 715387, 335728, 87666392, 62368169, 27926093},
     25.08439333,
     6.7883454,
     -17.65191},
    {"leo1200_neighbour_2",
     {-2771542, 4509243, 2455572, 20837, -46176, 109528},
     {273071393, 745494, 1155622, 88436606, 62369223, 17161259},
     25.08439333,
     4.0391205,
     0.0},
}};

TEST(ntn_propagator_reference_test, ts38508_rrm_cells_formats_and_geometry)
{
  OCUDU_TEST_REQUIREMENTS("DU-NTN-SI-1");

  for (const rrm_cell_ephemeris& cell : ts38508_rrm_cells) {
    keplerian_propagator propagator;
    orbit_ephemeris_info orbit{propagator, make_epoch(), decode_orbital(cell.orbital)};
    const state_vector   sat_sv = decode_position_velocity(cell.position_velocity);

    // Both formats describe the same satellite, up to their quantization.
    const state_vector diff = orbit.ecef_rv() - sat_sv;
    EXPECT_NEAR(norm(diff.position), 0.0, 1.0) << cell.name;
    EXPECT_NEAR(norm(diff.velocity), 0.0, 1e-3) << cell.name;

    const coord_3d ue    = coordinate_converter::geodetic_to_ecef(cell.ue_latitude_deg, 121.56076999, 0.0).position;
    const coord_3d rho   = {sat_sv.position.x - ue.x, sat_sv.position.y - ue.y, sat_sv.position.z - ue.z};
    const double   range = norm(rho);
    // The notes round a near-zero Doppler of about 0.004 ppm to 0.
    EXPECT_NEAR(range / SPEED_OF_LIGHT_M_S * 1e3, cell.one_way_delay_ms, 1e-4) << cell.name;
    EXPECT_NEAR(-dot(rho, sat_sv.velocity) / range / SPEED_OF_LIGHT_M_S * 1e6, cell.doppler_ppm, 5e-3) << cell.name;
  }
}

namespace {

/// One row of a GMAT satellite pass report.
struct gmat_pass_sample {
  /// Time relative to the initial state, in seconds.
  double                 time_s;
  std::array<int64_t, 6> position_velocity;
  double                 one_way_delay_ms;
  double                 doppler_ppm;
};

} // namespace

// LEO-600 pass, every 40th row of the 0.64 s GMAT report attached to TR 38.905, Section 5.4. The initial state is the
// TS 38.101-5, Table G.4.1-1 state.
static const std::array<gmat_pass_sample, 31> tr38905_leo600_pass = {{
    {-256.00, {-2729074, 4620421, 131320, -2237, -4870, 125850}, 9.3021652, 23.0257828},
    {-230.40, {-2730684, 4612901, 279947, -490, -7859, 125715}, 8.7128914, 23.0077070},
    {-204.80, {-2730236, 4601849, 428357, 1246, -10849, 125484}, 8.1243714, 22.9660419},
    {-179.20, {-2727744, 4587266, 576437, 2970, -13836, 125156}, 7.5372826, 22.8946958},
    {-153.60, {-2723223, 4569155, 724072, 4680, -16819, 124731}, 6.9524893, 22.7849815},
    {-128.00, {-2716691, 4547523, 871149, 6375, -19795, 124210}, 6.3711255, 22.6242079},
    {-102.40, {-2708165, 4522380, 1017553, 8054, -22762, 123594}, 5.7947251, 22.3933397},
    {-76.80, {-2697665, 4493739, 1163173, 9715, -25717, 122882}, 5.2254307, 22.0629910},
    {-51.20, {-2685214, 4461614, 1307895, 11358, -28658, 122075}, 4.6663398, 21.5863905},
    {-25.60, {-2670834, 4426025, 1451609, 12980, -31582, 121175}, 4.1220960, 20.8867863},
    {25.60, {-2636386, 4344536, 1735571, 16160, -37371, 119096}, 3.1113891, 18.2109656},
    {51.20, {-2616371, 4298688, 1875601, 17715, -40232, 117919}, 2.6754287, 15.6539779},
    {76.80, {-2594533, 4249476, 2014187, 19245, -43066, 116651}, 2.3221882, 11.6592523},
    {102.40, {-2570903, 4196931, 2151222, 20750, -45872, 115294}, 2.0942016, 5.8517901},
    {128.00, {-2545511, 4141088, 2286602, 22227, -48648, 113849}, 2.0341341, -1.2622378},
    {153.60, {-2518390, 4081986, 2420223, 23676, -51390, 112318}, 2.1560231, -8.0613342},
    {179.20, {-2489574, 4019663, 2551983, 25097, -54098, 110700}, 2.4325078, -13.2258675},
    {204.80, {-2459096, 3954163, 2681782, 26487, -56768, 108998}, 2.8182067, -16.6526757},
    {230.40, {-2426994, 3885531, 2809521, 27847, -59399, 107213}, 3.2744519, -18.8259778},
    {256.00, {-2393304, 3813814, 2935104, 29175, -61989, 105347}, 3.7754144, -20.2104142},
    {281.60, {-2358065, 3739064, 3058434, 30470, -64534, 103402}, 4.3051453, -21.1126402},
    {307.20, {-2321315, 3661333, 3179419, 31732, -67034, 101378}, 4.8538486, -21.7156011},
    {332.80, {-2283094, 3580675, 3297967, 32959, -69487, 99277}, 5.4153604, -22.1269027},
    {358.40, {-2243443, 3497150, 3413988, 34152, -71889, 97102}, 5.9856670, -22.4109565},
    {384.00, {-2202405, 3410816, 3527396, 35308, -74240, 94853}, 6.5620580, -22.6074298},
    {409.60, {-2160021, 3321735, 3638106, 36429, -76538, 92534}, 7.1426374, -22.7415194},
    {435.20, {-2116335, 3229973, 3746033, 37512, -78780, 90145}, 7.7260337, -22.8297201},
    {460.80, {-2071392, 3135595, 3851099, 38558, -80965, 87689}, 8.3112233, -22.8831377},
    {486.40, {-2025235, 3038670, 3953223, 39566, -83091, 85168}, 8.8974197, -22.9094442},
    {512.00, {-1977911, 2939269, 4052331, 40534, -85156, 82583}, 9.4840022, -22.9140651},
    {520.32, {-1962286, 2906442, 4083880, 40841, -85814, 81730}, 9.6746381, -22.9115841},
}};

// LEO-1200 pass, every 60th row of the 0.64 s GMAT report attached to TR 38.905, Section 5.4. The initial state is the
// TS 38.508-1, Table 7.6.2.1-3a serving cell.
static const std::array<gmat_pass_sample, 32> tr38905_leo1200_pass = {{
    {-345.60, {-2915203, 4995795, -728073, -11323, 10684, 119851}, 13.4479573, 20.3594054},
    {-307.20, {-2933260, 5011422, -515215, -9056, 6946, 120326}, 12.6665518, 20.3334098},
    {-268.80, {-2947306, 5020403, -301658, -6797, 3186, 120639}, 11.8868073, 20.2717322},
    {-230.40, {-2957360, 5022705, -87693, -4550, -591, 120788}, 11.1102423, 20.1662438},
    {-192.00, {-2963444, 5018300, 126391, -2318, -4380, 120773}, 10.3387409, 20.0058416},
    {-153.60, {-2965588, 5007174, 340303, -105, -8176, 120594}, 9.5746910, 19.7751230},
    {-115.20, {-2963828, 4989319, 553755, 2087, -11973, 120253}, 8.8211853, 19.4523925},
    {-76.80, {-2958204, 4964738, 766456, 4255, -15765, 119748}, 8.0823159, 19.0066614},
    {-38.40, {-2948761, 4933444, 978119, 6396, -19547, 119081}, 7.3636123, 18.3931879},
    {38.40, {-2918631, 4850816, 1397187, 10583, -27060, 117266}, 6.0201466, 16.3753757},
    {76.80, {-2898061, 4799556, 1604026, 12624, -30781, 116120}, 5.4208552, 14.7493132},
    {115.20, {-2873907, 4741730, 1808694, 14626, -34469, 114817}, 4.8952780, 12.5075576},
    {153.60, {-2846240, 4677398, 2010917, 16588, -38121, 113360}, 4.4702210, 9.4919013},
    {192.00, {-2815135, 4606630, 2210421, 18505, -41731, 111750}, 4.1770209, 5.6473742},
    {230.40, {-2780673, 4529507, 2406940, 20377, -45293, 109991}, 4.0447544, 1.1692714},
    {268.80, {-2742936, 4446115, 2600209, 22200, -48803, 108084}, 4.0891136, -3.4535531},
    {307.20, {-2702012, 4356552, 2789969, 23973, -52256, 106032}, 4.3044491, -7.6522148},
    {345.60, {-2657993, 4260925, 2975968, 25692, -55646, 103839}, 4.6667603, -11.0770905},
    {384.00, {-2610974, 4159349, 3157957, 27358, -58969, 101508}, 5.1445378, -13.6771416},
    {422.40, {-2561054, 4051947, 3335695, 28966, -62219, 99042}, 5.7081916, -15.5772741},
    {460.80, {-2508334, 3938851, 3508945, 30517, -65393, 96445}, 6.3340775, -16.9456450},
    {499.20, {-2452919, 3820201, 3677480, 32007, -68486, 93720}, 7.0047069, -17.9291153},
    {537.60, {-2394916, 3696146, 3841076, 33437, -71493, 90873}, 7.7075323, -18.6379566},
    {576.00, {-2334436, 3566839, 3999518, 34803, -74410, 87906}, 8.4335939, -19.1500638},
    {614.40, {-2271591, 3432446, 4152600, 36105, -77233, 84824}, 9.1764356, -19.5193128},
    {652.80, {-2206495, 3293136, 4300122, 37343, -79958, 81632}, 9.9313295, -19.7829070},
    {691.20, {-2139265, 3149085, 4441891, 38513, -82581, 78334}, 10.6947452, -19.9667455},
    {729.60, {-2070019, 3000479, 4577725, 39617, -85099, 74935}, 11.4639906, -20.0890905},
    {768.00, {-1998877, 2847507, 4707449, 40653, -87507, 71439}, 12.2369690, -20.1630122},
    {806.40, {-1925960, 2690366, 4830896, 41620, -89803, 67852}, 13.0120128, -20.1980118},
    {844.80, {-1851390, 2529258, 4947908, 42518, -91983, 64179}, 13.7877687, -20.2011044},
    {852.48, {-1836288, 2496579, 4970526, 42690, -92405, 63435}, 13.9429033, -20.1983815},
}};

// GMAT models gravity up to J2, as per TR 38.905, Section 5.2.4, so only the RK4 propagator is checked over the pass.
// keplerian_propagator ignores J2 and drifts by over 1 us of one-way delay within 300 s.
// [Implementation-defined] tolerances: delay and Doppler reuse TS 38.101-5, Section G.5 values as per-sample maxima.
static void check_gmat_pass(const state_vector& initial, span<const gmat_pass_sample> pass)
{
  const coord_3d ue = coordinate_converter::geodetic_to_ecef(25.08439333, 121.56076999, 0.0).position;

  for (const gmat_pass_sample& sample : pass) {
    rk4_propagator       propagator;
    orbit_ephemeris_info orbit{propagator, make_epoch(), initial, false};
    orbit.propagate(std::chrono::duration<double>(sample.time_s));
    const state_vector& sat = orbit.ecef_rv();

    const state_vector diff = sat - decode_position_velocity(sample.position_velocity);
    EXPECT_NEAR(norm(diff.position), 0.0, 40.0) << "t=" << sample.time_s;
    EXPECT_NEAR(norm(diff.velocity), 0.0, 0.15) << "t=" << sample.time_s;

    const coord_3d rho   = {sat.position.x - ue.x, sat.position.y - ue.y, sat.position.z - ue.z};
    const double   range = norm(rho);
    EXPECT_NEAR(range / SPEED_OF_LIGHT_M_S * 1e3, sample.one_way_delay_ms, 50e-6) << "t=" << sample.time_s;
    EXPECT_NEAR(-dot(rho, sat.velocity) / range / SPEED_OF_LIGHT_M_S * 1e6, sample.doppler_ppm, 5e-4)
        << "t=" << sample.time_s;
  }
}

TEST(ntn_propagator_reference_test, tr38905_leo600_pass_rk4)
{
  OCUDU_TEST_REQUIREMENTS("DU-NTN-SI-4");

  check_gmat_pass(annex_g_sat_ecef, tr38905_leo600_pass);
}

TEST(ntn_propagator_reference_test, tr38905_leo1200_pass_rk4)
{
  OCUDU_TEST_REQUIREMENTS("DU-NTN-SI-4");

  check_gmat_pass(decode_position_velocity(ts38508_rrm_cells[3].position_velocity), tr38905_leo1200_pass);
}
