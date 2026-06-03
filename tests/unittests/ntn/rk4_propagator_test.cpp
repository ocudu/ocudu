// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/ntn/converters/coordinate_converter.h"
#include "lib/ntn/converters/ephemeris_info_converter.h"
#include "lib/ntn/coordinates_types.h"
#include "lib/ntn/orbit_ephemeris_info.h"
#include "lib/ntn/propagators/keplerian_propagator.h"
#include "lib/ntn/propagators/rk4_propagator.h"
#include "ocudu/support/test_utils.h"
#include "fmt/chrono.h"
#include <cmath>
#include <gtest/gtest.h>
#include <iomanip>
#include <string>
#include <vector>

using namespace ocudu;
using namespace ocudu_ntn;

using time_point                 = std::chrono::system_clock::time_point;
constexpr double LIGHT_SPEED_M_S = 299792458; /// Speed of light [km/s]
constexpr bool   debug_mode      = false;

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

TEST(test_converters, propagate_leo_orbit_with_initial_eci_rv_test)
{
  double dl_freq_hz = 2e9; // Hz, used to compute Doppler Shifts
  // Tolerances.
  double pos_tolerance           = 3;   // 3m -> delay error of 10ns
  double vel_tolerance           = 0.1; // 0.1m/s -> doppler error at 2GHz of 0.67Hz
  double link_delay_tolerance_ns = 10;  // ns
  double dl_doppler_tolerance_hz = 1;   // Hz

  double pos_error           = 0;
  double vel_error           = 0;
  double link_delay_error_ns = 0;
  double dl_doppler_error_hz = 0;

  std::string init_utc_time   = "2025-06-24T09:00:00"; // need to set some value
  time_point  init_epoch_time = string_to_timepoint(init_utc_time);

  // State Vector in ECI frame, LEO pass.
  // Note: All ECI vectors in the same reference frame.
  std::vector<std::tuple<unsigned, state_vector>> test_cases = {
      {0, {{-5785280.86959769, -3708553.63217423, 9543.556363940}, {2472.47999379, -3828.56716423, 6105.64101549}}},
      {1, {{-5782804.83125587, -3712379.91165124, 15649.18135765}, {2479.59454678, -3824.00317635, 6105.62548035}}},
      {2, {{-5780321.67987401, -3716201.62479657, 21754.78705034}, {2486.70607099, -3819.43447123, 6105.60241404}}},
      {3, {{-5777831.41848599, -3720018.76689462, 27860.36591089}, {2493.81455759, -3814.86105450, 6105.57181654}}},
      {4, {{-5775334.05013329, -3723831.33323741, 33965.91040798}, {2500.91999776, -3810.28293180, 6105.53368784}}},
      {5, {{-5772829.57786713, -3727639.31912109, 40071.41301063}, {2508.02238268, -3805.70010878, 6105.48802796}}},
      {10, {{-5760200.76522655, -3746610.3774430, 70598.03401548}, {2543.48817008, -3782.71568665, 6105.14676142}}},
      {15, {{-5747394.81743291, -3765466.22207272, 101122.47799603}, {2578.8762553, -3759.61460627, 6104.61722096}}},
      {20, {{-5734412.12574083, -3784206.27149417, 131643.80360972}, {2614.18554004, -3736.39757979, 6103.89941763}}},
      {25, {{-5721253.08688980, -3802829.94776207, 162161.06958434}, {2649.41492844, -3713.06532302, 6102.99336836}}},
      {30, {{-5707918.10309369, -3821336.67651772, 192673.33474678}, {2684.56332705, -3689.61855544, 6101.89909605}}},
      {35, {{-5694407.58202631, -3839725.88701048, 223179.65805342}, {2719.62964494, -3666.05800017, 6100.61662951}}},
      {40, {{-5680721.93681085, -3857997.01211283, 253679.09861932}, {2754.61279370, -3642.38438392, 6099.14600349}}},
      {45, {{-5666861.58600533, -3876149.48834117, 284170.71574817}, {2789.51168745, -3618.59843701, 6097.48725867}}},
      {50, {{-5652826.95359113, -3894182.75587162, 314653.56896170}, {2824.32524295, -3594.70089332, 6095.64044166}}},
      {55, {{-5638618.46895974, -3912096.25855800, 345126.71802942}, {2859.05237956, -3570.69249025, 6093.60560498}}},
      {60, {{-5624236.56689881, -3929889.44395053, 375589.22299825}, {2893.69201932, -3546.57396875, 6091.38280709}}},
      {100, {{-5502992.26752926, -4067838.44392353, 618793.39582637}, {3167.53080419, -3349.75158570, 6066.84279006}}},
      {120, {{-5438292.75159245, -4133826.94836620, 739967.40800393}, {3302.15436141, -3248.83686426, 6050.07761877}}},
      {150, {{-5336235.53883978, -4228985.30274277, 921025.06477330}, {3501.03159869, -3094.47538779, 6019.33313553}}},
      {200, {{-5153068.00944174, -4377126.57295113, 1220416.33253050}, {3823.79273305, -2829.66111157, 5953.25552948}}},
      {250, {{-4954051.84085137, -4511805.69852462, 1516043.62274279}, {4134.81290904, -2556.12764124, 5868.81253074}}},
      {300, {{-4739798.35193530, -4632607.76353195, 1806994.93515346}, {4433.12770422, -2274.72123225, 5766.26452027}}},
  };

  rk4_propagator       orbit_propagator;
  state_vector         init_eci_rv = std::get<1>(test_cases[0]);
  orbit_ephemeris_info orbit_ephemeris_info{orbit_propagator, init_epoch_time, init_eci_rv, true};

  for (const auto& [propagation_time, expected_eci_rv] : test_cases) {
    auto propagation_duration =
        init_epoch_time + std::chrono::seconds(propagation_time) - orbit_ephemeris_info.epoch_time();

    orbit_ephemeris_info.propagate(propagation_duration, false);

    state_vector rv_diff     = orbit_ephemeris_info.eci_rv() - expected_eci_rv;
    pos_error                = norm(rv_diff.position);
    vel_error                = norm(rv_diff.velocity);
    link_delay_error_ns      = norm(rv_diff.position) / LIGHT_SPEED_M_S * 1e9;
    dl_doppler_error_hz      = norm(rv_diff.velocity) / LIGHT_SPEED_M_S * dl_freq_hz;
    auto expected_epoch_time = init_epoch_time + std::chrono::seconds(propagation_time);
    auto expected_epoch_ts =
        std::chrono::duration_cast<std::chrono::milliseconds>(expected_epoch_time.time_since_epoch()).count();
    auto orbit_epoch_ts =
        std::chrono::duration_cast<std::chrono::milliseconds>(orbit_ephemeris_info.epoch_time().time_since_epoch())
            .count();
    ASSERT_EQ(orbit_epoch_ts, expected_epoch_ts);
    ASSERT_NEAR(pos_error, 0.0, pos_tolerance);
    ASSERT_NEAR(vel_error, 0.0, vel_tolerance);
    ASSERT_NEAR(link_delay_error_ns, 0.0, link_delay_tolerance_ns);
    ASSERT_NEAR(dl_doppler_error_hz, 0.0, dl_doppler_tolerance_hz);
    if (debug_mode) {
      fmt::print("Actual Position:    [{:.3f}, {:.3f}, {:.3f}]\n",
                 orbit_ephemeris_info.eci_rv().position.x,
                 orbit_ephemeris_info.eci_rv().position.y,
                 orbit_ephemeris_info.eci_rv().position.z);
      fmt::print("Expected Position:  [{:.3f}, {:.3f}, {:.3f}]\n",
                 expected_eci_rv.position.x,
                 expected_eci_rv.position.y,
                 expected_eci_rv.position.z);
      fmt::print("Actual Velocity:    [{:.3f}, {:.3f}, {:.3f}]\n",
                 orbit_ephemeris_info.eci_rv().velocity.x,
                 orbit_ephemeris_info.eci_rv().velocity.y,
                 orbit_ephemeris_info.eci_rv().velocity.z);
      fmt::print("Expected Velocity:  [{:.3f}, {:.3f}, {:.3f}]\n",
                 expected_eci_rv.velocity.x,
                 expected_eci_rv.velocity.y,
                 expected_eci_rv.velocity.z);
      fmt::print(
          "Propagation time: {}, pos_error {:.3f}, vel_error {:.3f}, link_delay_ns {:.3f}, dl_doppler_hz {:.3f}\n",
          propagation_time,
          pos_error,
          vel_error,
          link_delay_error_ns,
          dl_doppler_error_hz);
    }
  }
}

TEST(test_converters, propagate_leo_orbit_with_initial_ecef_rv_test)
{
  double dl_freq_hz = 2e9; // Hz, used to compute Doppler Shifts
  // Tolerances.
  double pos_tolerance           = 3;   // 3m -> delay error of 10ns
  double vel_tolerance           = 0.1; // 0.1m/s -> doppler error at 2GHz of 0.67Hz
  double link_delay_tolerance_ns = 10;  // ns
  double dl_doppler_tolerance_hz = 1;   // Hz

  double pos_error           = 0;
  double vel_error           = 0;
  double link_delay_error_ns = 0;
  double dl_doppler_error_hz = 0;

  // State Vector in ECEF frame, LEO pass.
  std::vector<std::tuple<unsigned, state_vector>> test_cases = {
      {0, {{-3673241.153680, -5807765.859375, 9541.602562}, {3436.468069, -2155.294072, 6105.641019}}},
      {1, {{-3669802.595816, -5809917.841417, 15647.227560}, {3440.650782, -2148.681393, 6105.625487}}},
      {2, {{-3666359.856850, -5812063.209770, 21752.833260}, {3444.830260, -2142.066671, 6105.602423}}},
      {3, {{-3662912.940019, -5814201.962397, 27858.412131}, {3449.006497, -2135.449914, 6105.571828}}},
      {4, {{-3659461.848567, -5816334.097264, 33963.956640}, {3453.179488, -2128.831129, 6105.533701}}},
      {5, {{-3656006.585742, -5818459.612349, 40069.459257}, {3457.349229, -2122.210324, 6105.488044}}},
      {10, {{-3638667.815034, -5828987.820733, 70596.080371}, {3478.149006, -2089.076268, 6105.146789}}},
      {15, {{-3621225.249991, -5839350.235389, 101120.524521}, {3498.866792, -2055.892866, 6104.617261}}},
      {20, {{-3603679.302088, -5849546.611981, 131641.850365}, {3519.501979, -2022.661078, 6103.899470}}},
      {25, {{-3586030.385827, -5859576.710990, 162159.116629}, {3540.053963, -1989.381869, 6102.993432}}},
      {30, {{-3568278.918731, -5869440.297714, 192671.382142}, {3560.522142, -1956.056203, 6101.899172}}},
      {35, {{-3550425.321332, -5879137.142276, 223177.705859}, {3580.905913, -1922.685045, 6100.616718}}},
      {40, {{-3532470.017164, -5888667.019628, 253677.146895}, {3601.204678, -1889.269362, 6099.146104}}},
      {45, {{-3514413.432751, -5898029.709557, 284168.764555}, {3621.417839, -1855.810121, 6097.487371}}},
      {50, {{-3496255.997599, -5907224.996690, 314651.618360}, {3641.544802, -1822.308291, 6095.640566}}},
      {55, {{-3477998.144184, -5916252.670500, 345124.768078}, {3661.584972, -1788.764840, 6093.605741}}},
      {60, {{-3459640.307946, -5925112.525307, 375587.273759}, {3681.537759, -1755.180740, 6091.382955}}},
      {100, {{-3309231.458261, -5989926.801973, 618791.454439}, {3837.943979, -1485.161278, 6066.843034}}},
      {150, {{-3112616.045880, -6055687.520758, 921023.138589}, {4025.083775, -1144.826790, 6019.333499}}},
      {200, {{-2906886.824425, -6104372.254326, 1220414.427491}, {4202.404202, -802.284788, 5953.256011}}},
      {250, {{-2692548.274116, -6135895.193327, 1516041.744725}, {4369.368925, -458.518365, 5868.813129}}},
      {300, {{-2470131.065247, -6150219.697846, 1806993.089951}, {4525.466989, -114.510674, 5766.265234}}},
  };
  std::string init_utc_time   = "2025-06-24T09:00:00"; // need to set some value
  time_point  init_epoch_time = string_to_timepoint(init_utc_time);

  rk4_propagator       orbit_propagator;
  state_vector         init_ecef_rv = std::get<1>(test_cases[0]);
  orbit_ephemeris_info orbit_ephemeris_info{orbit_propagator, init_epoch_time, init_ecef_rv, false};

  for (const auto& [propagation_time, expected_ecef_rv] : test_cases) {
    auto propagation_duration =
        init_epoch_time + std::chrono::seconds(propagation_time) - orbit_ephemeris_info.epoch_time();

    orbit_ephemeris_info.propagate(propagation_duration, true);
    state_vector rv_diff     = orbit_ephemeris_info.ecef_rv() - expected_ecef_rv;
    pos_error                = norm(rv_diff.position);
    vel_error                = norm(rv_diff.velocity);
    link_delay_error_ns      = norm(rv_diff.position) / LIGHT_SPEED_M_S * 1e9;
    dl_doppler_error_hz      = norm(rv_diff.velocity) / LIGHT_SPEED_M_S * dl_freq_hz;
    auto expected_epoch_time = init_epoch_time + std::chrono::seconds(propagation_time);
    auto expected_epoch_ts =
        std::chrono::duration_cast<std::chrono::milliseconds>(expected_epoch_time.time_since_epoch()).count();
    auto orbit_epoch_ts =
        std::chrono::duration_cast<std::chrono::milliseconds>(orbit_ephemeris_info.epoch_time().time_since_epoch())
            .count();
    ASSERT_EQ(orbit_epoch_ts, expected_epoch_ts);
    ASSERT_NEAR(pos_error, 0.0, pos_tolerance);
    ASSERT_NEAR(vel_error, 0.0, vel_tolerance);
    ASSERT_NEAR(link_delay_error_ns, 0.0, link_delay_tolerance_ns);
    ASSERT_NEAR(dl_doppler_error_hz, 0.0, dl_doppler_tolerance_hz);

    if (debug_mode) {
      fmt::print("Actual Position:    [{:.3f}, {:.3f}, {:.3f}]\n",
                 orbit_ephemeris_info.ecef_rv().position.x,
                 orbit_ephemeris_info.ecef_rv().position.y,
                 orbit_ephemeris_info.ecef_rv().position.z);
      fmt::print("Expected Position:  [{:.3f}, {:.3f}, {:.3f}]\n",
                 expected_ecef_rv.position.x,
                 expected_ecef_rv.position.y,
                 expected_ecef_rv.position.z);
      fmt::print("Actual Velocity:    [{:.3f}, {:.3f}, {:.3f}]\n",
                 orbit_ephemeris_info.ecef_rv().velocity.x,
                 orbit_ephemeris_info.ecef_rv().velocity.y,
                 orbit_ephemeris_info.ecef_rv().velocity.z);
      fmt::print("Expected Velocity:  [{:.3f}, {:.3f}, {:.3f}]\n",
                 expected_ecef_rv.velocity.x,
                 expected_ecef_rv.velocity.y,
                 expected_ecef_rv.velocity.z);
      fmt::print(
          "Propagation time: {}, pos_error {:.3f}, vel_error {:.3f}, link_delay_ns {:.3f}, dl_doppler_hz {:.3f}\n",
          propagation_time,
          pos_error,
          vel_error,
          link_delay_error_ns,
          dl_doppler_error_hz);
    }
  }
}

TEST(test_converters, propagate_geo_orbit_with_initial_ecef_rv_test)
{
  double dl_freq_hz = 2e9; // Hz, used to compute Doppler Shifts
  // Tolerances.
  double pos_tolerance           = 40;  // m -> delay error of 133ns
  double vel_tolerance           = 0.1; // m/s -> doppler error at 2GHz of 0.67Hz
  double link_delay_tolerance_ns = 133; // ns
  double dl_doppler_tolerance_hz = 1;   // Hz

  double pos_error           = 0;
  double vel_error           = 0;
  double link_delay_error_ns = 0;
  double dl_doppler_error_hz = 0;

  // State Vector in ECEF frame, LEO pass.
  std::vector<std::tuple<unsigned, state_vector>> test_cases = {
      {0, {{-12451277.794996, 40348000.090559, -1726486.314075}, {35.630744, -6.275751, -385.708419}}},
      {1, {{-12451242.141184, 40347993.821964, -1726872.016471}, {35.629378, -6.279487, -385.699303}}},
      {2, {{-12451206.488736, 40347987.549634, -1727257.709753}, {35.628012, -6.283223, -385.690184}}},
      {3, {{-12451170.837656, 40347981.273569, -1727643.393918}, {35.626645, -6.286958, -385.681063}}},
      {4, {{-12451135.187944, 40347974.993767, -1728029.068964}, {35.625278, -6.290693, -385.671940}}},
      {5, {{-12451099.539599, 40347968.710231, -1728414.734890}, {35.623910, -6.294428, -385.662816}}},
      {6, {{-12451063.892621, 40347962.422960, -1728800.391692}, {35.622541, -6.298163, -385.653689}}},
      {7, {{-12451028.247010, 40347956.131955, -1729186.039370}, {35.621173, -6.301897, -385.644560}}},
      {8, {{-12450992.602769, 40347949.837215, -1729571.677921}, {35.619803, -6.305632, -385.635429}}},
      {9, {{-12450956.959895, 40347943.538741, -1729957.307344}, {35.618433, -6.309366, -385.626296}}},
      {10, {{-12450921.318394, 40347937.236532, -1730342.927635}, {35.617062, -6.313100, -385.617161}}},
      {20, {{-12450564.978831, 40347874.009116, -1734198.627896}, {35.603327, -6.350430, -385.525699}}},
      {30, {{-12450208.776842, 40347810.408498, -1738053.412823}, {35.589538, -6.387740, -385.434033}}},
      {40, {{-12449852.712973, 40347746.434869, -1741907.280377}, {35.575694, -6.425032, -385.342164}}},
      {50, {{-12449496.787759, 40347682.088418, -1745760.228524}, {35.561797, -6.462305, -385.250091}}},
      {60, {{-12449141.001740, 40347617.369338, -1749612.255227}, {35.547846, -6.499558, -385.157815}}},
      {100, {{-12447719.260387, 40347354.770572, -1765011.106914}, {35.491503, -6.648378, -384.786675}}},
      {150, {{-12445945.280135, 40347018.161233, -1784238.694995}, {35.419866, -6.833963, -384.318177}}},
      {200, {{-12444174.914086, 40346672.285098, -1803442.738820}, {35.346889, -7.019049, -383.844604}}},
      {250, {{-12442408.229141, 40346317.167358, -1822622.984754}, {35.272576, -7.203625, -383.365960}}},
      {300, {{-12440645.292014, 40345952.833689, -1841779.179473}, {35.196931, -7.387683, -382.882253}}},
      {400, {{-12437130.927137, 40345196.623703, -1880018.403557}, {35.041658, -7.754205, -381.899675}}},
      {500, {{-12433632.349351, 40344403.870263, -1918158.390870}, {34.881101, -8.118537, -380.896921}}},
      {600, {{-12430150.085384, 40343574.796207, -1956197.126427}, {34.715294, -8.480600, -379.874044}}},
      {700, {{-12426684.658663, 40342709.632045, -1994132.600569}, {34.544269, -8.840321, -378.831097}}},
      {800, {{-12423236.589227, 40341808.615906, -2031962.809070}, {34.368061, -9.197621, -377.768135}}},
      {900, {{-12419806.393593, 40340871.993497, -2069685.753241}, {34.186707, -9.552426, -376.685214}}},
      {1000, {{-12416394.584662, 40339900.018044, -2107299.440037}, {34.000241, -9.904660, -375.582392}}},
      {1100, {{-12413001.671607, 40338892.950239, -2144801.882160}, {33.808703, -10.254250, -374.459725}}},
      {1200, {{-12409628.159756, 40337851.058195, -2182191.098170}, {33.612132, -10.601122, -373.317274}}},
  };

  std::string init_utc_time   = "2025-06-24T09:00:00"; // need to set some value
  time_point  init_epoch_time = string_to_timepoint(init_utc_time);

  rk4_propagator       orbit_propagator;
  state_vector         init_ecef_rv = std::get<1>(test_cases[0]);
  orbit_ephemeris_info orbit_ephemeris_info{orbit_propagator, init_epoch_time, init_ecef_rv, false};

  for (const auto& [propagation_time, expected_ecef_rv] : test_cases) {
    auto propagation_duration =
        init_epoch_time + std::chrono::seconds(propagation_time) - orbit_ephemeris_info.epoch_time();

    orbit_ephemeris_info.propagate(propagation_duration, true);
    state_vector rv_diff     = orbit_ephemeris_info.ecef_rv() - expected_ecef_rv;
    pos_error                = norm(rv_diff.position);
    vel_error                = norm(rv_diff.velocity);
    link_delay_error_ns      = norm(rv_diff.position) / LIGHT_SPEED_M_S * 1e9;
    dl_doppler_error_hz      = norm(rv_diff.velocity) / LIGHT_SPEED_M_S * dl_freq_hz;
    auto expected_epoch_time = init_epoch_time + std::chrono::seconds(propagation_time);
    auto expected_epoch_ts =
        std::chrono::duration_cast<std::chrono::milliseconds>(expected_epoch_time.time_since_epoch()).count();
    auto orbit_epoch_ts =
        std::chrono::duration_cast<std::chrono::milliseconds>(orbit_ephemeris_info.epoch_time().time_since_epoch())
            .count();
    ASSERT_EQ(orbit_epoch_ts, expected_epoch_ts);
    ASSERT_NEAR(pos_error, 0.0, pos_tolerance);
    ASSERT_NEAR(vel_error, 0.0, vel_tolerance);
    ASSERT_NEAR(link_delay_error_ns, 0.0, link_delay_tolerance_ns);
    ASSERT_NEAR(dl_doppler_error_hz, 0.0, dl_doppler_tolerance_hz);

    if (debug_mode) {
      fmt::print("Actual Position:    [{:.3f}, {:.3f}, {:.3f}]\n",
                 orbit_ephemeris_info.ecef_rv().position.x,
                 orbit_ephemeris_info.ecef_rv().position.y,
                 orbit_ephemeris_info.ecef_rv().position.z);
      fmt::print("Expected Position:  [{:.3f}, {:.3f}, {:.3f}]\n",
                 expected_ecef_rv.position.x,
                 expected_ecef_rv.position.y,
                 expected_ecef_rv.position.z);
      fmt::print("Actual Velocity:    [{:.3f}, {:.3f}, {:.3f}]\n",
                 orbit_ephemeris_info.ecef_rv().velocity.x,
                 orbit_ephemeris_info.ecef_rv().velocity.y,
                 orbit_ephemeris_info.ecef_rv().velocity.z);
      fmt::print("Expected Velocity:  [{:.3f}, {:.3f}, {:.3f}]\n",
                 expected_ecef_rv.velocity.x,
                 expected_ecef_rv.velocity.y,
                 expected_ecef_rv.velocity.z);
      fmt::print(
          "Propagation time: {}, pos_error {:.3f}, vel_error {:.3f}, link_delay_ns {:.3f}, dl_doppler_hz {:.3f}\n",
          propagation_time,
          pos_error,
          vel_error,
          link_delay_error_ns,
          dl_doppler_error_hz);
    }
  }
}

TEST(test_converters, propagate_leo_orbit_with_initial_oe_test)
{
  double dl_freq_hz = 2e9; // Hz, used to compute Doppler Shifts

  // Tolerances.
  double sma_tolerance           = 5;    // m
  double tolerance               = 1e-3; // rad
  double pos_tolerance           = 3;    // 3m -> delay error of 10ns
  double vel_tolerance           = 0.1;  // 0.1m/s -> doppler error at 2GHz of 0.67Hz
  double link_delay_tolerance_ns = 10;   // ns
  double dl_doppler_tolerance_hz = 1;    // Hz

  double pos_error           = 0;
  double vel_error           = 0;
  double link_delay_error_ns = 0;
  double dl_doppler_error_hz = 0;

  // Orbital elements in ECI frame, LEO pass.
  // Note: For each OE, ECI reference frame is aligned with ECEF ref frame at own epoch time.
  std::vector<std::tuple<unsigned, orbital_elements>> test_cases = {
      {0, {6877286.310, 0.0012074892907, 0.9295782970760, 2.0359402522293, 0.8669148350739, 5.4309100278415}},
      {1, {6877285.959, 0.0012086242483, 0.9295782771678, 2.0358673310466, 0.8677919832540, 5.4311429689726}},
      {2, {6877285.578, 0.0012097559878, 0.9295782556096, 2.0357944098085, 0.8686712308442, 5.4313738106369}},
      {3, {6877285.166, 0.0012108845043, 0.9295782324015, 2.0357214885101, 0.8695525689727, 5.4316025617017}},
      {4, {6877284.724, 0.0012120097933, 0.9295782075435, 2.0356485671472, 0.8704359888245, 5.4318292309773}},
      {5, {6877284.252, 0.0012131318500, 0.9295781810358, 2.0355756457149, 0.8713214816342, 5.4320538272247}},
      {10, {6877281.433, 0.0012186934891, 0.9295780237559, 2.0352110373551, 0.8757797369266, 5.4331460159985}},
      {15, {6877277.855, 0.0012241736422, 0.9295778252545, 2.0348464265788, 0.8802885311139, 5.4341876634238}},
      {20, {6877273.517, 0.0012295717623, 0.9295775855547, 2.0344818128157, 0.8848468256771, 5.4351798074435}},
      {25, {6877268.421, 0.0012348873211, 0.9295773046849, 2.0341171954953, 0.8894536123226, 5.4361234557754}},
      {30, {6877262.566, 0.0012401198084, 0.9295769826783, 2.0337525740479, 0.8941079119209, 5.4370195869736}},
      {35, {6877255.954, 0.0012452687320, 0.9295766195733, 2.0333879479040, 0.8988087734988, 5.4378691514366}},
      {40, {6877248.584, 0.0012503336174, 0.9295762154134, 2.0330233164950, 0.9035552732552, 5.4386730723910}},
      {45, {6877240.459, 0.0012553140074, 0.9295757702471, 2.0326586792525, 0.9083465136282, 5.4394322468252}},
      {50, {6877231.579, 0.0012602094619, 0.9295752841278, 2.0322940356091, 0.9131816223846, 5.4401475463994}},
      {55, {6877221.945, 0.0012650195573, 0.9295747571142, 2.0319293849980, 0.9180597517552, 5.4408198183105}},
      {60, {6877211.558, 0.0012697438868, 0.9295741892699, 2.0315647268533, 0.9229800775926, 5.4414498861348}},
      {100, {6877101.533, 0.0013044073720, 0.9295681859004, 2.0286471229033, 0.9637700427189, 5.4450627277479}},
      {120, {6877028.746, 0.0013196194574, 0.9295642203384, 2.0271880369327, 0.9850442696909, 5.4459896170885}},
      {150, {6876897.740, 0.0013397445714, 0.9295570884476, 2.0249989536293, 1.0179441836239, 5.4463909185072}},
      {200, {6876622.683, 0.0013660263228, 0.9295421270570, 2.0213489824678, 1.0751391853434, 5.4446964279514}},
      {250, {6876279.728, 0.0013831844694, 0.9295234846253, 2.0176966859862, 1.1349122783711, 5.4404214972293}},
      {300, {6875873.081, 0.0013912635678, 0.9295013894452, 2.0140415691012, 1.1969079560618, 5.4339211332626}}};

  std::string init_utc_time   = "2025-06-24T09:00:00"; // need to set some value
  time_point  init_epoch_time = string_to_timepoint(init_utc_time);

  rk4_propagator       orbit_propagator;
  orbital_elements     init_oe = std::get<1>(test_cases[0]);
  orbit_ephemeris_info orbit_ephemeris_info{orbit_propagator, init_epoch_time, init_oe};

  for (const auto& [propagation_time, expected_oe] : test_cases) {
    auto propagation_duration =
        init_epoch_time + std::chrono::seconds(propagation_time) - orbit_ephemeris_info.epoch_time();

    orbit_ephemeris_info.propagate(propagation_duration, true);
    state_vector propagated_eci_rv = orbit_ephemeris_info.eci_rv();
    state_vector expected_eci_rv   = ephemeris_info_converter::orbital_to_eci(expected_oe);

    state_vector rv_diff     = propagated_eci_rv - expected_eci_rv;
    pos_error                = norm(rv_diff.position);
    vel_error                = norm(rv_diff.velocity);
    link_delay_error_ns      = norm(rv_diff.position) / LIGHT_SPEED_M_S * 1e9;
    dl_doppler_error_hz      = norm(rv_diff.velocity) / LIGHT_SPEED_M_S * dl_freq_hz;
    auto expected_epoch_time = init_epoch_time + std::chrono::seconds(propagation_time);
    auto expected_epoch_ts =
        std::chrono::duration_cast<std::chrono::milliseconds>(expected_epoch_time.time_since_epoch()).count();
    auto orbit_epoch_ts =
        std::chrono::duration_cast<std::chrono::milliseconds>(orbit_ephemeris_info.epoch_time().time_since_epoch())
            .count();
    ASSERT_EQ(orbit_epoch_ts, expected_epoch_ts);
    ASSERT_NEAR(orbit_ephemeris_info.oe().semi_major_axis, expected_oe.semi_major_axis, sma_tolerance);
    ASSERT_NEAR(orbit_ephemeris_info.oe().eccentricity, expected_oe.eccentricity, tolerance);
    ASSERT_NEAR(orbit_ephemeris_info.oe().inclination, expected_oe.inclination, tolerance);
    ASSERT_NEAR(orbit_ephemeris_info.oe().longitude, expected_oe.longitude, tolerance);
    ASSERT_NEAR(orbit_ephemeris_info.oe().periapsis, expected_oe.periapsis, tolerance);
    ASSERT_NEAR(orbit_ephemeris_info.oe().mean_anomaly, expected_oe.mean_anomaly, tolerance);
    ASSERT_NEAR(pos_error, 0.0, pos_tolerance);
    ASSERT_NEAR(vel_error, 0.0, vel_tolerance);
    ASSERT_NEAR(link_delay_error_ns, 0.0, link_delay_tolerance_ns);
    ASSERT_NEAR(dl_doppler_error_hz, 0.0, dl_doppler_tolerance_hz);

    if (debug_mode) {
      fmt::print("Actual OE:  {}   [{:.3f}, {:.3f}, {:.3f}, {:.3f}, {:.3f}, {:.3f}]\n",
                 propagation_time,
                 orbit_ephemeris_info.oe().semi_major_axis,
                 orbit_ephemeris_info.oe().eccentricity,
                 orbit_ephemeris_info.oe().inclination,
                 orbit_ephemeris_info.oe().longitude,
                 orbit_ephemeris_info.oe().periapsis,
                 orbit_ephemeris_info.oe().mean_anomaly);
      fmt::print("Expected OE: {}  [{:.3f}, {:.3f}, {:.3f}, {:.3f}, {:.3f}, {:.3f}]\n",
                 propagation_time,
                 expected_oe.semi_major_axis,
                 expected_oe.eccentricity,
                 expected_oe.inclination,
                 expected_oe.longitude,
                 expected_oe.periapsis,
                 expected_oe.mean_anomaly);
      fmt::print("Actual Position:    [{:.3f}, {:.3f}, {:.3f}]\n",
                 propagated_eci_rv.position.x,
                 propagated_eci_rv.position.y,
                 propagated_eci_rv.position.z);
      fmt::print("Expected Position:  [{:.3f}, {:.3f}, {:.3f}]\n",
                 expected_eci_rv.position.x,
                 expected_eci_rv.position.y,
                 expected_eci_rv.position.z);
      fmt::print("Actual Velocity:    [{:.3f}, {:.3f}, {:.3f}]\n",
                 propagated_eci_rv.velocity.x,
                 propagated_eci_rv.velocity.y,
                 propagated_eci_rv.velocity.z);
      fmt::print("Expected Velocity:  [{:.3f}, {:.3f}, {:.3f}]\n",
                 expected_eci_rv.velocity.x,
                 expected_eci_rv.velocity.y,
                 expected_eci_rv.velocity.z);
      fmt::print(
          "Propagation time: {}, pos_error {:.3f}, vel_error {:.3f}, link_delay_ns {:.3f}, dl_doppler_hz {:.3f}\n",
          propagation_time,
          pos_error,
          vel_error,
          link_delay_error_ns,
          dl_doppler_error_hz);
    }
  }
}

TEST(test_converters, propagate_leo_orbit_backwards_with_initial_oe_test)
{
  double dl_freq_hz = 2e9; // Hz, used to compute Doppler Shifts

  // Tolerances.
  double sma_tolerance           = 5;    // m
  double tolerance               = 1e-3; // rad
  double pos_tolerance           = 3;    // 3m -> delay error of 10ns
  double vel_tolerance           = 0.1;  // 0.1m/s -> doppler error at 2GHz of 0.67Hz
  double link_delay_tolerance_ns = 10;   // ns
  double dl_doppler_tolerance_hz = 1;    // Hz

  double pos_error           = 0;
  double vel_error           = 0;
  double link_delay_error_ns = 0;
  double dl_doppler_error_hz = 0;

  // Orbital elements in ECI frame, LEO pass.
  // Note: For each OE, ECI reference frame is aligned with ECEF ref frame at own epoch time.
  std::vector<std::tuple<signed, orbital_elements>> test_cases = {
      {-60, {6877286.310, 0.0012074892907, 0.9295782970760, 2.0359402522293, 0.8669148350739, 5.4309100278415}},
      {-59, {6877285.959, 0.0012086242483, 0.9295782771678, 2.0358673310466, 0.8677919832540, 5.4311429689726}},
      {-58, {6877285.578, 0.0012097559878, 0.9295782556096, 2.0357944098085, 0.8686712308442, 5.4313738106369}},
      {-57, {6877285.166, 0.0012108845043, 0.9295782324015, 2.0357214885101, 0.8695525689727, 5.4316025617017}},
      {-56, {6877284.724, 0.0012120097933, 0.9295782075435, 2.0356485671472, 0.8704359888245, 5.4318292309773}},
      {-55, {6877284.252, 0.0012131318500, 0.9295781810358, 2.0355756457149, 0.8713214816342, 5.4320538272247}},
      {-50, {6877281.433, 0.0012186934891, 0.9295780237559, 2.0352110373551, 0.8757797369266, 5.4331460159985}},
      {-45, {6877277.855, 0.0012241736422, 0.9295778252545, 2.0348464265788, 0.8802885311139, 5.4341876634238}},
      {-40, {6877273.517, 0.0012295717623, 0.9295775855547, 2.0344818128157, 0.8848468256771, 5.4351798074435}},
      {-35, {6877268.421, 0.0012348873211, 0.9295773046849, 2.0341171954953, 0.8894536123226, 5.4361234557754}},
      {-30, {6877262.566, 0.0012401198084, 0.9295769826783, 2.0337525740479, 0.8941079119209, 5.4370195869736}},
      {-25, {6877255.954, 0.0012452687320, 0.9295766195733, 2.0333879479040, 0.8988087734988, 5.4378691514366}},
      {-20, {6877248.584, 0.0012503336174, 0.9295762154134, 2.0330233164950, 0.9035552732552, 5.4386730723910}},
      {-15, {6877240.459, 0.0012553140074, 0.9295757702471, 2.0326586792525, 0.9083465136282, 5.4394322468252}},
      {-10, {6877231.579, 0.0012602094619, 0.9295752841278, 2.0322940356091, 0.9131816223846, 5.4401475463994}},
      {-5, {6877221.945, 0.0012650195573, 0.9295747571142, 2.0319293849980, 0.9180597517552, 5.4408198183105}},
      {0, {6877211.558, 0.0012697438868, 0.9295741892699, 2.0315647268533, 0.9229800775926, 5.4414498861348}}};

  std::string init_utc_time   = "2025-06-24T09:05:00"; // need to set some value
  time_point  init_epoch_time = string_to_timepoint(init_utc_time);

  rk4_propagator       orbit_propagator;
  orbital_elements     init_oe = std::get<1>(test_cases.back());
  orbit_ephemeris_info orbit_ephemeris_info{orbit_propagator, init_epoch_time, init_oe};

  for (auto it = test_cases.rbegin(); it != test_cases.rend(); ++it) {
    const auto& [propagation_time, expected_oe] = *it;
    auto propagation_duration =
        init_epoch_time - orbit_ephemeris_info.epoch_time() + std::chrono::seconds(propagation_time);

    orbit_ephemeris_info.propagate(propagation_duration, true);
    state_vector propagated_eci_rv = orbit_ephemeris_info.eci_rv();
    state_vector expected_eci_rv   = ephemeris_info_converter::orbital_to_eci(expected_oe);

    state_vector rv_diff     = propagated_eci_rv - expected_eci_rv;
    pos_error                = norm(rv_diff.position);
    vel_error                = norm(rv_diff.velocity);
    link_delay_error_ns      = norm(rv_diff.position) / LIGHT_SPEED_M_S * 1e9;
    dl_doppler_error_hz      = norm(rv_diff.velocity) / LIGHT_SPEED_M_S * dl_freq_hz;
    auto expected_epoch_time = init_epoch_time + std::chrono::seconds(propagation_time);
    auto expected_epoch_ts =
        std::chrono::duration_cast<std::chrono::milliseconds>(expected_epoch_time.time_since_epoch()).count();
    auto orbit_epoch_ts =
        std::chrono::duration_cast<std::chrono::milliseconds>(orbit_ephemeris_info.epoch_time().time_since_epoch())
            .count();
    ASSERT_EQ(orbit_epoch_ts, expected_epoch_ts);
    ASSERT_NEAR(orbit_ephemeris_info.oe().semi_major_axis, expected_oe.semi_major_axis, sma_tolerance);
    ASSERT_NEAR(orbit_ephemeris_info.oe().eccentricity, expected_oe.eccentricity, tolerance);
    ASSERT_NEAR(orbit_ephemeris_info.oe().inclination, expected_oe.inclination, tolerance);
    ASSERT_NEAR(orbit_ephemeris_info.oe().longitude, expected_oe.longitude, tolerance);
    ASSERT_NEAR(orbit_ephemeris_info.oe().periapsis, expected_oe.periapsis, tolerance);
    ASSERT_NEAR(orbit_ephemeris_info.oe().mean_anomaly, expected_oe.mean_anomaly, tolerance);
    ASSERT_NEAR(pos_error, 0.0, pos_tolerance);
    ASSERT_NEAR(vel_error, 0.0, vel_tolerance);
    ASSERT_NEAR(link_delay_error_ns, 0.0, link_delay_tolerance_ns);
    ASSERT_NEAR(dl_doppler_error_hz, 0.0, dl_doppler_tolerance_hz);

    if (debug_mode) {
      fmt::print("Actual OE:  {}   [{:.3f}, {:.3f}, {:.3f}, {:.3f}, {:.3f}, {:.3f}]\n",
                 propagation_time,
                 orbit_ephemeris_info.oe().semi_major_axis,
                 orbit_ephemeris_info.oe().eccentricity,
                 orbit_ephemeris_info.oe().inclination,
                 orbit_ephemeris_info.oe().longitude,
                 orbit_ephemeris_info.oe().periapsis,
                 orbit_ephemeris_info.oe().mean_anomaly);
      fmt::print("Expected OE: {}  [{:.3f}, {:.3f}, {:.3f}, {:.3f}, {:.3f}, {:.3f}]\n",
                 propagation_time,
                 expected_oe.semi_major_axis,
                 expected_oe.eccentricity,
                 expected_oe.inclination,
                 expected_oe.longitude,
                 expected_oe.periapsis,
                 expected_oe.mean_anomaly);
      fmt::print("Actual Position:    [{:.3f}, {:.3f}, {:.3f}]\n",
                 propagated_eci_rv.position.x,
                 propagated_eci_rv.position.y,
                 propagated_eci_rv.position.z);
      fmt::print("Expected Position:  [{:.3f}, {:.3f}, {:.3f}]\n",
                 expected_eci_rv.position.x,
                 expected_eci_rv.position.y,
                 expected_eci_rv.position.z);
      fmt::print("Actual Velocity:    [{:.3f}, {:.3f}, {:.3f}]\n",
                 propagated_eci_rv.velocity.x,
                 propagated_eci_rv.velocity.y,
                 propagated_eci_rv.velocity.z);
      fmt::print("Expected Velocity:  [{:.3f}, {:.3f}, {:.3f}]\n",
                 expected_eci_rv.velocity.x,
                 expected_eci_rv.velocity.y,
                 expected_eci_rv.velocity.z);
      fmt::print(
          "Propagation time: {}, pos_error {:.3f}, vel_error {:.3f}, link_delay_ns {:.3f}, dl_doppler_hz {:.3f}\n",
          propagation_time,
          pos_error,
          vel_error,
          link_delay_error_ns,
          dl_doppler_error_hz);
    }
  }
}

TEST(test_converters, propagate_leo_orbit_for_long_time_with_initial_ecef_rv_test)
{
  double dl_freq_hz = 2e9; // Hz, used to compute Doppler Shifts
  // Tolerances.
  double pos_tolerance           = 60;  // 3m -> delay error of 200ns
  double vel_tolerance           = 1;   // 1m/s -> doppler error at 2GHz of 6.7Hz
  double link_delay_tolerance_ns = 200; // ns
  double dl_doppler_tolerance_hz = 5;   // Hz

  double pos_error           = 0;
  double vel_error           = 0;
  double link_delay_error_ns = 0;
  double dl_doppler_error_hz = 0;

  // State Vector in ECEF frame, LEO pass.
  std::vector<std::tuple<unsigned, state_vector>> test_cases = {
      {0, {{-3004146.945645, -6179976.010379, 70596.861829}, {3678.930520, -1710.915119, 6105.146778}}},
      {100, {{-2619699.817678, -6315669.760729, 679421.151919}, {4003.906381, -1000.957833, 6058.834218}}},
      {200, {{-2204650.459931, -6379901.716228, 1279871.137732}, {4290.359790, -283.053885, 5937.828960}}},
      {300, {{-1763028.953302, -6372290.410858, 1864543.706536}, {4534.798560, 434.532111, 5743.609691}}},
      {400, {{-1299194.582739, -6293279.112823, 2426229.869195}, {4734.138184, 1143.587409, 5478.575971}}},
      {500, {{-817793.007302, -6144125.546084, 2958005.301810}, {4885.742114, 1836.052609, 5146.015065}}},
      {600, {{-323709.684628, -5926881.634271, 3453316.941218}, {4987.456284, 2504.119366, 4750.056319}}},
      {700, {{177979.855212, -5644363.727941, 3906064.445744}, {5037.637540, 3140.322677, 4295.614299}}},
      {800, {{682062.268490, -5300113.930164, 4310675.466577}, {5035.175847, 3737.626349, 3788.322139}}},
      {900, {{1183240.569190, -4898353.257187, 4662173.826115}, {4979.510364, 4289.500617, 3234.456657}}},
      {1000, {{1676189.607899, -4443927.457053, 4956239.857014}, {4870.639588, 4789.991259, 2640.856797}}},
      {1100, {{2155612.280099, -3942246.359877, 5189262.309701}, {4709.125890, 5233.779832, 2014.836886}}},
      {1200, {{2616295.918741, -3399217.652247, 5358381.378076}, {4496.094774, 5616.235016, 1364.096019}}},
      {1300, {{3053168.358740, -2821175.961213, 5461522.517043}, {4233.229158, 5933.455208, 696.624721}}},
      {1400, {{3461353.188301, -2214808.108442, 5497420.828145}, {3922.758906, 6182.302677, 20.609775}}},
      {1500, {{3836223.719026, -1587075.361760, 5465635.871190}, {3567.445709, 6360.429606, -655.662041}}},
      {1600, {{4173455.211611, -945133.477588, 5366556.823497}, {3170.563303, 6466.296332, -1323.900742}}},
      {1700, {{4469074.885385, -296251.302400, 5201397.960293}, {2735.872876, 6499.181966, -1975.909956}}},
      {1800, {{4719509.220592, 352271.311537, 4972184.477301}, {2267.593417, 6459.187460, -2603.683267}}},
      {1900, {{4921628.035479, 993185.517779, 4681728.728038}, {1770.366750, 6347.230997, -3199.499532}}},
      {2000, {{5072784.791710, 1619374.638143, 4333597.011405}, {1249.216963, 6165.035484, -3756.016446}}},
      {2100, {{5170852.558034, 2223933.669576, 3932067.125793}, {709.504063, 5915.107802, -4266.361441}}},
      {2200, {{5214255.050330, 2800246.293563, 3482077.007684}, {156.871833, 5600.709477, -4724.218822}}},
      {2300, {{5201992.172547, 3342058.512681, 2989164.895829}, {-402.809959, 5225.818469, -5123.911794}}},
      {2400, {{5133659.512692, 3843548.015975, 2459401.603700}, {-963.508546, 4795.081940, -5460.477922}}},
      {2500, {{5009461.303994, 4299388.370609, 1899315.636998}, {-1519.095676, 4313.760064, -5729.736448}}},
      {2600, {{4830216.444277, 4704807.158841, 1315812.051106}, {-2063.418687, 3787.661251, -5928.345888}}},
      {2700, {{4597357.274617, 5055637.231479, 716086.095671}, {-2590.373969, 3223.069450, -6053.850479}}},
      {2800, {{4312920.947034, 5348360.333978, 107532.829331}, {-3093.981436, 2626.664544, -6104.714192}}},
      {2900, {{3979533.353992, 5580142.478305, -502346.002708}, {-3568.458547, 2005.437158, -6080.341395}}},
      {3000, {{3600385.741936, 5748860.578917, -1106036.460362}, {-4008.292369, 1366.599436, -5981.083585}}},
      {3100, {{3179204.278452, 5853120.038307, -1696106.866881}, {-4408.308245, 717.493564, -5808.232043}}},
      {3200, {{2720212.979552, 5892263.147956, -2265300.838996}, {-4763.733777, 65.499863, -5563.996723}}},
      {3300, {{2228090.522542, 5866368.354627, -2806627.545259}, {-5070.257025, -582.053719, -5251.472073}}},
      {3400, {{1707921.565204, 5776240.619419, -3313447.938440}, {-5324.078115, -1217.978989, -4874.590867}}},
      {3500, {{1165143.259824, 5623393.258531, -3779555.829856}, {-5521.953697, -1835.310276, -4438.067417}}},
      {3600, {{605487.689822, 5410021.791996, -4199252.818886}, {-5661.234021, -2427.381341, -3947.331690}}},
      {3700, {{34920.968587, 5138970.433984, -4567416.249613}, {-5739.892628, -2987.895523, -3408.455972}}},
      {3800, {{-540420.271783, 4813691.932366, -4879559.527918}, {-5756.548881, -3510.988547, -2828.075661}}},
      {3900, {{-1114294.304176, 4438201.505862, -5131884.286392}, {-5710.483723, -3991.283755, -2213.305664}}},
      {4000, {{-1680420.986312, 4017025.636920, -5321324.022904}, {-5601.649089, -4423.939793, -1571.653685}}},
      {4100, {{-2232549.085597, 3555146.462611, -5445578.955448}, {-5430.671442, -4804.691029, -910.931435}}},
      {4200, {{-2764523.679433, 3057942.471439, -5503141.928145}, {-5198.849794, -5129.881094, -239.164571}}},
      {4300, {{-3270353.093026, 2531126.169158, -5493315.271018}, {-4908.148467, -5396.490028, 435.498078}}},
      {4400, {{-3744274.853626, 1980679.330048, -5416218.562834}, {-4561.184672, -5602.155487, 1104.875627}}},
      {4500, {{-4180820.139691, 1412786.409225, -5272787.277659}, {-4161.210815, -5745.188350, 1760.846313}}},
      {4600, {{-4574876.185619, 833766.662803, -5064762.319732}, {-3712.091258, -5824.582957, 2395.438509}}},
      {4700, {{-4921746.069963, 250005.510273, -4794670.476379}, {-3218.273168, -5840.022008, 3000.921290}}},
      {4800, {{-5217205.272190, -332114.321196, -4465795.853690}, {-2684.750964, -5791.875985, 3569.894116}}},
      {4900, {{-5457554.336966, -906281.308517, -4082142.411661}, {-2117.023948, -5681.196805, 4095.375014}}},
      {5000, {{-5639666.943385, -1466322.702753, -3648387.789896}, {-1521.046722, -5509.705279, 4570.886402}}},
      {5100, {{-5761032.648027, -2006270.516090, -3169828.714018}, {-903.172225, -5279.771947, 4990.537434}}},
      {5200, {{-5819793.562841, -2520425.239614, -2652318.395998}, {-270.087420, -4994.390833, 5349.101532}}},
      {5300, {{-5814774.248152, -3003416.567726, -2102196.484749}, {371.258017, -4657.145802, 5642.087608}}},
      {5400, {{-5745504.151752, -3450260.381071, -1526212.279866}, {1013.729334, -4272.169378, 5865.803346}}},
      {5500, {{-5612232.008790, -3856411.236599, -931442.082430}, {1650.089670, -3844.094092, 6017.408961}}},
      {5600, {{-5415931.732764, -4217809.635798, -325201.712131}, {2273.087046, -3377.996750, 6094.959919}}},
      {5700, {{-5158299.471264, -4530923.393150, 285044.640988}, {2875.543819, -2879.336253, 6097.437361}}},
      {5800, {{-4841741.664686, -4792782.506777, 891777.948849}, {3450.446884, -2353.885941, 6024.765231}}},
      {5900, {{-4469354.123335, -5001007.040008, 1487518.901778}, {3991.036806, -1807.661638, 5877.813533}}},
      {6000, {{-4044892.318619, -5153827.651564, 2064922.090567}, {4490.894103, -1246.846788, 5658.387541}}},
      {6100, {{-3572733.257426, -5250098.556694, 2616868.420009}, {4944.021006, -677.716193, 5369.203233}}},
      {6200, {{-3057829.466032, -5289302.853908, 3136554.406594}, {5344.917241, -106.559869, 5013.849621}}},
      {6300, {{-2505655.743360, -5271550.303235, 3617577.098533}, {5688.648633, 460.391495, 4596.739024}}},
      {6400, {{-1922149.447591, -5197567.783727, 4054013.474061}, {5970.907659, 1017.038131, 4123.046594}}},
      {6500, {{-1313645.152306, -5068682.782579, 4440493.315031}, {6188.065403, 1557.478730, 3598.640594}}},
      {6600, {{-686804.548747, -4886800.370054, 4772264.707256}, {6337.214650, 2076.073014, 3030.004988}}},
      {6700, {{-48542.482562, -4654374.189635, 5045251.476353}, {6416.204140, 2567.498752, 2424.155904}}},
      {6800, {{594049.997995, -4374372.040571, 5256102.018453}, {6423.664184, 3026.802872, 1788.553382}}},
      {6900, {{1233784.732490, -4050236.651753, 5402229.121372}, {6359.023955, 3449.446560, 1131.009696}}},
      {7000, {{1863456.861972, -3685842.245591, 5481840.488410}, {6222.520805, 3831.344451, 459.595265}}},
      {7100, {{2475925.784242, -3285447.473883, 5493959.772037}, {6015.201904, 4168.898113, -217.456969}}},
      {7200, {{3064195.894887, -2853645.281505, 5438437.999516}, {5738.918387, 4459.024117, -891.848047}}},
      {7300, {{3621496.371129, -2395310.224958, 5315955.331344}, {5396.312070, 4699.176958, -1555.309853}}},
      {7400, {{4141359.252791, -1915543.747922, 5128013.142751}, {4990.794620, 4887.367036, -2199.701817}}},
      {7500, {{4617695.056866, -1419617.900303, 4876916.466586}, {4526.518941, 5022.173800, -2817.106978}}},
  };

  std::string init_utc_time   = "2025-06-24T09:00:00"; // need to set some value
  time_point  init_epoch_time = string_to_timepoint(init_utc_time);

  rk4_propagator       orbit_propagator;
  state_vector         init_ecef_rv = std::get<1>(test_cases[0]);
  orbit_ephemeris_info orbit_ephemeris_info{orbit_propagator, init_epoch_time, init_ecef_rv, false};

  for (const auto& [propagation_time, expected_ecef_rv] : test_cases) {
    auto propagation_duration =
        init_epoch_time + std::chrono::seconds(propagation_time) - orbit_ephemeris_info.epoch_time();

    orbit_ephemeris_info.propagate(propagation_duration, true);
    state_vector rv_diff     = orbit_ephemeris_info.ecef_rv() - expected_ecef_rv;
    pos_error                = norm(rv_diff.position);
    vel_error                = norm(rv_diff.velocity);
    link_delay_error_ns      = norm(rv_diff.position) / LIGHT_SPEED_M_S * 1e9;
    dl_doppler_error_hz      = norm(rv_diff.velocity) / LIGHT_SPEED_M_S * dl_freq_hz;
    auto expected_epoch_time = init_epoch_time + std::chrono::seconds(propagation_time);
    auto expected_epoch_ts =
        std::chrono::duration_cast<std::chrono::milliseconds>(expected_epoch_time.time_since_epoch()).count();
    auto orbit_epoch_ts =
        std::chrono::duration_cast<std::chrono::milliseconds>(orbit_ephemeris_info.epoch_time().time_since_epoch())
            .count();

    ASSERT_EQ(orbit_epoch_ts, expected_epoch_ts);
    ASSERT_NEAR(pos_error, 0.0, pos_tolerance);
    ASSERT_NEAR(vel_error, 0.0, vel_tolerance);
    ASSERT_NEAR(link_delay_error_ns, 0.0, link_delay_tolerance_ns);
    ASSERT_NEAR(dl_doppler_error_hz, 0.0, dl_doppler_tolerance_hz);
    if (debug_mode) {
      fmt::print("Actual Position:    [{:.3f}, {:.3f}, {:.3f}]\n",
                 orbit_ephemeris_info.ecef_rv().position.x,
                 orbit_ephemeris_info.ecef_rv().position.y,
                 orbit_ephemeris_info.ecef_rv().position.z);
      fmt::print("Expected Position:  [{:.3f}, {:.3f}, {:.3f}]\n",
                 expected_ecef_rv.position.x,
                 expected_ecef_rv.position.y,
                 expected_ecef_rv.position.z);
      fmt::print("Actual Velocity:    [{:.3f}, {:.3f}, {:.3f}]\n",
                 orbit_ephemeris_info.ecef_rv().velocity.x,
                 orbit_ephemeris_info.ecef_rv().velocity.y,
                 orbit_ephemeris_info.ecef_rv().velocity.z);
      fmt::print("Expected Velocity:  [{:.3f}, {:.3f}, {:.3f}]\n",
                 expected_ecef_rv.velocity.x,
                 expected_ecef_rv.velocity.y,
                 expected_ecef_rv.velocity.z);
      fmt::print("Propagation time: {}, pos_error {:.3f}, vel_error {:.3f}, link_delay_error_ns {:.3f}, "
                 "dl_doppler_error_hz {:.3f}\n",
                 propagation_time,
                 pos_error,
                 vel_error,
                 link_delay_error_ns,
                 dl_doppler_error_hz);
    }
  }
}

TEST(test_converters, propagate_leo_orbit_with_ref_from_matlab_sgp4_propagator)
{
  // Tolerances.
  double pos_tolerance_m         = 3.1;
  double vel_tolerance_m_s       = 0.1;
  double link_delay_tolerance_ns = 10;
  double range_tolerance_m       = 3;

  double pos_error_m         = 0;
  double vel_error_m_s       = 0;
  double range_m             = 0;
  double range_error_m       = 0;
  double max_range_error_m   = 0;
  double link_delay_us       = 0;
  double link_delay_error_ns = 0;
  double max_delay_error_ns  = 0;

  geodetic_coordinates ue_location = {7.734134202493742, 124.2115429594156, 1.0};
  state_vector         ue_ecef_rv  = coordinate_converter::geodetic_to_ecef(ue_location);

  // State Vector in ECEF frame, LEO pass.
  std::vector<std::tuple<unsigned, state_vector, double, double>> test_cases = {
      {0, {{-3342330.380, 6004311.941, 7495.104}, {-3537.680, -1984.780, 6105.646}}, 1167656.334, 3894.882},
      {1, {{-3345866.154, 6002323.740, 13600.735}, {-3533.874, -1991.617, 6105.633}}, 1161278.763, 3873.609},
      {2, {{-3349398.120, 6000328.702, 19706.349}, {-3530.065, -1998.452, 6105.612}}, 1154908.609, 3852.360},
      {3, {{-3352926.276, 5998326.831, 25811.939}, {-3526.253, -2005.285, 6105.584}}, 1148546.001, 3831.137},
      {4, {{-3356450.618, 5996318.128, 31917.497}, {-3522.437, -2012.116, 6105.549}}, 1142191.073, 3809.939},
      {5, {{-3359971.142, 5994302.595, 38023.015}, {-3518.619, -2018.945, 6105.506}}, 1135843.961, 3788.768},
      {6, {{-3363487.846, 5992280.234, 44128.487}, {-3514.796, -2025.772, 6105.455}}, 1129504.806, 3767.622},
      {7, {{-3367000.726, 5990251.046, 50233.904}, {-3510.971, -2032.598, 6105.397}}, 1123173.748, 3746.504},
      {8, {{-3370509.762, 5988215.044, 56339.260}, {-3507.142, -2039.421, 6105.331}}, 1116850.943, 3725.414},
      {9, {{-3374014.984, 5986172.209, 62444.546}, {-3503.310, -2046.243, 6105.258}}, 1110536.520, 3704.351},
      {10, {{-3377516.372, 5984122.554, 68549.754}, {-3499.474, -2053.062, 6105.177}}, 1104230.639, 3683.317},
      {11, {{-3381013.924, 5982066.080, 74654.879}, {-3495.635, -2059.880, 6105.089}}, 1097933.456, 3662.312},
      {12, {{-3384507.634, 5980002.790, 80759.911}, {-3491.793, -2066.696, 6104.993}}, 1091645.128, 3641.336},
      {13, {{-3387997.501, 5977932.684, 86864.843}, {-3487.948, -2073.510, 6104.889}}, 1085365.817, 3620.391},
      {14, {{-3391483.521, 5975855.766, 92969.669}, {-3484.099, -2080.322, 6104.779}}, 1079095.687, 3599.476},
      {15, {{-3394965.674, 5973772.047, 99074.380}, {-3480.247, -2087.131, 6104.660}}, 1072834.916, 3578.592},
      {16, {{-3398443.990, 5971681.509, 105178.968}, {-3476.392, -2093.939, 6104.534}}, 1066583.657, 3557.740},
      {17, {{-3401918.449, 5969584.164, 111283.427}, {-3472.533, -2100.745, 6104.401}}, 1060342.096, 3536.921},
      {18, {{-3405389.048, 5967480.014, 117387.749}, {-3468.671, -2107.549, 6104.260}}, 1054110.412, 3516.134},
      {19, {{-3408855.783, 5965369.061, 123491.925}, {-3464.806, -2114.351, 6104.111}}, 1047888.789, 3495.381},
      {20, {{-3412318.652, 5963251.308, 129595.950}, {-3460.938, -2121.151, 6103.955}}, 1041677.414, 3474.662},
      {21, {{-3415777.651, 5961126.755, 135699.814}, {-3457.066, -2127.949, 6103.791}}, 1035476.479, 3453.978},
      {22, {{-3419232.759, 5958995.416, 141803.511}, {-3453.191, -2134.745, 6103.620}}, 1029286.189, 3433.329},
      {23, {{-3422684.008, 5956857.271, 147907.034}, {-3449.313, -2141.539, 6103.442}}, 1023106.726, 3412.717},
      {24, {{-3426131.377, 5954712.334, 154010.374}, {-3445.432, -2148.331, 6103.255}}, 1016938.304, 3392.141},
      {25, {{-3429574.864, 5952560.606, 160113.523}, {-3441.547, -2155.120, 6103.062}}, 1010781.133, 3371.603},
      {26, {{-3433014.464, 5950402.089, 166216.476}, {-3437.659, -2161.908, 6102.860}}, 1004635.427, 3351.103},
      {27, {{-3436450.175, 5948236.785, 172319.223}, {-3433.768, -2168.694, 6102.651}}, 998501.404, 3330.642},
      {28, {{-3439881.993, 5946064.697, 178421.758}, {-3429.874, -2175.477, 6102.435}}, 992379.288, 3310.221},
      {29, {{-3443309.915, 5943885.826, 184524.072}, {-3425.976, -2182.259, 6102.211}}, 986269.309, 3289.840},
      {30, {{-3446733.920, 5941700.185, 190626.159}, {-3422.076, -2189.038, 6101.980}}, 980171.710, 3269.501},
      {40, {{-3480758.975, 5919471.217, 251632.854}, {-3382.892, -2256.717, 6099.252}}, 919931.531, 3068.561},
      {50, {{-3514390.607, 5896566.547, 312608.508}, {-3343.392, -2324.178, 6095.772}}, 861230.249, 2872.755},
      {60, {{-3547625.653, 5872988.395, 373545.597}, {-3303.579, -2391.414, 6091.539}}, 804412.441, 2683.231},
      {70, {{-3580461.045, 5848739.030, 434436.605}, {-3263.458, -2458.417, 6086.555}}, 749913.702, 2501.443},
      {80, {{-3612893.711, 5823820.829, 495274.016}, {-3223.034, -2525.179, 6080.820}}, 698284.348, 2329.226},
      {90, {{-3644920.624, 5798236.249, 556050.325}, {-3182.311, -2591.693, 6074.335}}, 650215.008, 2168.884},
      {100, {{-3676538.855, 5771987.794, 616758.032}, {-3141.295, -2657.951, 6067.100}}, 606558.924, 2023.263},
      {110, {{-3707745.470, 5745078.075, 677389.646}, {-3099.989, -2723.945, 6059.116}}, 568339.699, 1895.777},
      {120, {{-3738537.583, 5717509.783, 737937.683}, {-3058.398, -2789.667, 6050.385}}, 536724.988, 1790.322},
      {130, {{-3768912.405, 5689285.653, 798394.673}, {-3016.528, -2855.109, 6040.907}}, 512941.476, 1710.989},
      {140, {{-3798867.143, 5660408.530, 858753.153}, {-2974.382, -2920.264, 6030.683}}, 498115.189, 1661.533},
      {150, {{-3828399.052, 5630881.335, 919005.675}, {-2931.966, -2985.124, 6019.715}}, 493056.681, 1644.660},
      {160, {{-3857505.486, 5600707.032, 979144.802}, {-2889.284, -3049.682, 6008.005}}, 498064.327, 1661.364},
      {170, {{-3886183.796, 5569888.697, 1039163.113}, {-2846.342, -3113.929, 5995.552}}, 512842.867, 1710.660},
      {180, {{-3914431.382, 5538429.479, 1099053.198}, {-2803.143, -3177.859, 5982.360}}, 536584.044, 1789.852},
      {190, {{-3942245.741, 5506332.569, 1158807.667}, {-2759.694, -3241.464, 5968.429}}, 568162.978, 1895.188},
      {200, {{-3969624.354, 5473601.281, 1218419.143}, {-2715.998, -3304.736, 5953.762}}, 606353.062, 2022.576},
      {210, {{-3996564.814, 5440238.956, 1277880.270}, {-2672.061, -3367.668, 5938.359}}, 649986.101, 2168.120},
      {220, {{-4023064.718, 5406249.044, 1337183.707}, {-2627.887, -3430.252, 5922.224}}, 698037.663, 2328.403},
      {230, {{-4049121.709, 5371635.070, 1396322.136}, {-2583.482, -3492.481, 5905.358}}, 749653.612, 2500.575},
      {240, {{-4074733.529, 5336400.597, 1455288.256}, {-2538.850, -3554.348, 5887.763}}, 804142.556, 2682.331},
      {250, {{-4099897.922, 5300549.296, 1514074.790}, {-2493.997, -3615.846, 5869.441}}, 860953.522, 2871.832},
      {260, {{-4124612.682, 5264084.914, 1572674.481}, {-2448.928, -3676.966, 5850.395}}, 919650.362, 3067.623},
      {270, {{-4148875.698, 5227011.232, 1631080.096}, {-2403.646, -3737.702, 5830.626}}, 979888.109, 3268.555},
      {280, {{-4172684.864, 5189332.139, 1689284.427}, {-2358.158, -3798.047, 5810.138}}, 1041393.065, 3473.713},
      {290, {{-4196038.126, 5151051.598, 1747280.289}, {-2312.469, -3857.993, 5788.933}}, 1103946.949, 3682.371},
      {300, {{-4218933.523, 5112173.604, 1805060.524}, {-2266.583, -3917.533, 5767.013}}, 1167374.539, 3893.942},
  };

  std::string init_utc_time   = "2025-06-24T09:00:00"; // need to set some value
  time_point  init_epoch_time = string_to_timepoint(init_utc_time);

  rk4_propagator       orbit_propagator;
  state_vector         init_ecef_rv = std::get<1>(test_cases[0]);
  orbit_ephemeris_info orbit_ephemeris_info{orbit_propagator, init_epoch_time, init_ecef_rv, false};

  for (const auto& [propagation_time, expected_ecef_rv, exp_range_m, exp_delay_us] : test_cases) {
    auto propagation_duration =
        init_epoch_time + std::chrono::seconds(propagation_time) - orbit_ephemeris_info.epoch_time();

    orbit_ephemeris_info.propagate(propagation_duration, true);
    state_vector rv_diff     = orbit_ephemeris_info.ecef_rv() - expected_ecef_rv;
    pos_error_m              = norm(rv_diff.position);
    vel_error_m_s            = norm(rv_diff.velocity);
    range_m                  = norm((orbit_ephemeris_info.ecef_rv() - ue_ecef_rv).position);
    range_error_m            = range_m - exp_range_m;
    link_delay_us            = range_m / LIGHT_SPEED_M_S * 1e6;
    link_delay_error_ns      = (link_delay_us - exp_delay_us) * 1e3;
    max_range_error_m        = std::max(max_range_error_m, std::abs(range_error_m));
    max_delay_error_ns       = std::max(max_delay_error_ns, std::abs(link_delay_error_ns));
    auto expected_epoch_time = init_epoch_time + std::chrono::seconds(propagation_time);
    auto expected_epoch_ts =
        std::chrono::duration_cast<std::chrono::milliseconds>(expected_epoch_time.time_since_epoch()).count();
    auto orbit_epoch_ts =
        std::chrono::duration_cast<std::chrono::milliseconds>(orbit_ephemeris_info.epoch_time().time_since_epoch())
            .count();
    ASSERT_EQ(orbit_epoch_ts, expected_epoch_ts);
    ASSERT_NEAR(pos_error_m, 0.0, pos_tolerance_m);
    ASSERT_NEAR(vel_error_m_s, 0.0, vel_tolerance_m_s);
    ASSERT_NEAR(range_error_m, 0.0, range_tolerance_m);
    ASSERT_NEAR(link_delay_error_ns, 0.0, link_delay_tolerance_ns);

    if (debug_mode) {
      fmt::print("Actual Position:    [{:.3f}, {:.3f}, {:.3f}]\n",
                 orbit_ephemeris_info.ecef_rv().position.x,
                 orbit_ephemeris_info.ecef_rv().position.y,
                 orbit_ephemeris_info.ecef_rv().position.z);
      fmt::print("Expected Position:  [{:.3f}, {:.3f}, {:.3f}]\n",
                 expected_ecef_rv.position.x,
                 expected_ecef_rv.position.y,
                 expected_ecef_rv.position.z);
      fmt::print("Actual Velocity:    [{:.3f}, {:.3f}, {:.3f}]\n",
                 orbit_ephemeris_info.ecef_rv().velocity.x,
                 orbit_ephemeris_info.ecef_rv().velocity.y,
                 orbit_ephemeris_info.ecef_rv().velocity.z);
      fmt::print("Expected Velocity:  [{:.3f}, {:.3f}, {:.3f}]\n",
                 expected_ecef_rv.velocity.x,
                 expected_ecef_rv.velocity.y,
                 expected_ecef_rv.velocity.z);
      fmt::print("Propagation time [s]: {}, SAT pos_error: {:.3f} m, vel_error: {:.3f} m/s\n",
                 propagation_time,
                 pos_error_m,
                 vel_error_m_s);
      fmt::print("Propagation time [s]: {}, SAT-UE range: {:.3f} km, expected: {:.3f}, error: {:.3f} m\n",
                 propagation_time,
                 range_m / 1e3,
                 exp_range_m / 1e3,
                 range_error_m,
                 max_range_error_m);
      fmt::print("Propagation time [s]: {}, SAT-UE delay {:.3f} us, expected: {:.3f} us, error: {:.3f} ns\n",
                 propagation_time,
                 link_delay_us,
                 exp_delay_us,
                 link_delay_error_ns);
    }
  }
  if (debug_mode) {
    fmt::print("Comparison with SGP4 propagator: SAT-UE max range error: {:.3f} m, max delay error: {:.3f} ns\n",
               max_range_error_m,
               max_delay_error_ns);
  }
}

TEST(test_converters, propagate_leo_orbit_with_ref_from_matlab_numerical_propagator)
{
  // Tolerances.
  double pos_tolerance_m         = 17;
  double vel_tolerance_m_s       = 0.11;
  double range_tolerance_m       = 6;
  double link_delay_tolerance_ns = 16;

  double pos_error_m         = 0;
  double vel_error_m_s       = 0;
  double range_m             = 0;
  double range_error_m       = 0;
  double max_range_error_m   = 0;
  double link_delay_us       = 0;
  double link_delay_error_ns = 0;
  double max_delay_error_ns  = 0;

  geodetic_coordinates ue_location = {7.734134202493742, 124.2115429594156, 1.0};
  state_vector         ue_ecef_rv  = coordinate_converter::geodetic_to_ecef(ue_location);

  // State Vector in ECEF frame, LEO pass.
  std::vector<std::tuple<unsigned, state_vector, double, double>> test_cases = {
      {0, {{-3342330.380, 6004311.941, 7495.104}, {-3537.680, -1984.780, 6105.646}}, 1167656.334, 3894.882},
      {1, {{-3345866.160, 6002323.740, 13600.744}, {-3533.874, -1991.617, 6105.633}}, 1161278.756, 3873.609},
      {2, {{-3349398.132, 6000328.704, 19706.367}, {-3530.065, -1998.453, 6105.613}}, 1154908.594, 3852.360},
      {3, {{-3352926.294, 5998326.833, 25811.966}, {-3526.253, -2005.286, 6105.585}}, 1148545.978, 3831.137},
      {4, {{-3356450.642, 5996318.130, 31917.534}, {-3522.437, -2012.117, 6105.549}}, 1142191.043, 3809.939},
      {5, {{-3359971.172, 5994302.596, 38023.062}, {-3518.618, -2018.947, 6105.506}}, 1135843.923, 3788.768},
      {6, {{-3363487.882, 5992280.234, 44128.543}, {-3514.796, -2025.775, 6105.455}}, 1129504.760, 3767.622},
      {7, {{-3367000.768, 5990251.045, 50233.970}, {-3510.970, -2032.600, 6105.397}}, 1123173.694, 3746.504},
      {8, {{-3370509.809, 5988215.042, 56339.334}, {-3507.141, -2039.424, 6105.331}}, 1116850.880, 3725.414},
      {9, {{-3374015.037, 5986172.205, 62444.630}, {-3503.309, -2046.246, 6105.258}}, 1110536.448, 3704.351},
      {10, {{-3377516.431, 5984122.548, 68549.848}, {-3499.473, -2053.066, 6105.177}}, 1104230.559, 3683.317},
      {11, {{-3381013.987, 5982066.071, 74654.982}, {-3495.635, -2059.884, 6105.089}}, 1097933.367, 3662.311},
      {12, {{-3384507.704, 5980002.777, 80760.024}, {-3491.792, -2066.700, 6104.993}}, 1091645.029, 3641.336},
      {13, {{-3387997.576, 5977932.669, 86864.966}, {-3487.947, -2073.514, 6104.890}}, 1085365.708, 3620.390},
      {14, {{-3391483.601, 5975855.747, 92969.801}, {-3484.098, -2080.326, 6104.779}}, 1079095.569, 3599.475},
      {15, {{-3394965.759, 5973772.024, 99074.521}, {-3480.246, -2087.137, 6104.660}}, 1072834.788, 3578.592},
      {16, {{-3398444.080, 5971681.481, 105179.119}, {-3476.391, -2093.945, 6104.535}}, 1066583.519, 3557.740},
      {17, {{-3401918.544, 5969584.132, 111283.588}, {-3472.532, -2100.751, 6104.401}}, 1060341.948, 3536.920},
      {18, {{-3405389.148, 5967479.977, 117387.919}, {-3468.670, -2107.556, 6104.260}}, 1054110.253, 3516.133},
      {19, {{-3408855.888, 5965369.018, 123492.105}, {-3464.805, -2114.358, 6104.111}}, 1047888.619, 3495.380},
      {20, {{-3412318.762, 5963251.259, 129596.139}, {-3460.937, -2121.158, 6103.955}}, 1041677.233, 3474.661},
      {21, {{-3415777.765, 5961126.700, 135700.013}, {-3457.065, -2127.956, 6103.792}}, 1035476.287, 3453.977},
      {22, {{-3419232.878, 5958995.354, 141803.720}, {-3453.190, -2134.753, 6103.621}}, 1029285.986, 3433.328},
      {23, {{-3422684.131, 5956857.203, 147907.252}, {-3449.312, -2141.547, 6103.442}}, 1023106.511, 3412.716},
      {24, {{-3426131.505, 5954712.258, 154010.601}, {-3445.430, -2148.339, 6103.256}}, 1016938.078, 3392.140},
      {25, {{-3429574.996, 5952560.522, 160113.761}, {-3441.546, -2155.129, 6103.062}}, 1010780.895, 3371.602},
      {26, {{-3433014.601, 5950401.997, 166216.723}, {-3437.658, -2161.917, 6102.861}}, 1004635.176, 3351.102},
      {27, {{-3436450.316, 5948236.686, 172319.480}, {-3433.767, -2168.703, 6102.652}}, 998501.140, 3330.641},
      {28, {{-3439882.138, 5946064.589, 178422.024}, {-3429.872, -2175.487, 6102.436}}, 992379.012, 3310.220},
      {29, {{-3443310.064, 5943885.709, 184524.348}, {-3425.975, -2182.269, 6102.212}}, 986269.020, 3289.839},
      {30, {{-3446734.073, 5941700.058, 190626.445}, {-3422.074, -2189.049, 6101.980}}, 980171.408, 3269.500},
      {40, {{-3480759.164, 5919470.978, 251633.236}, {-3382.890, -2256.731, 6099.252}}, 919931.086, 3068.560},
      {50, {{-3514390.823, 5896566.160, 312608.985}, {-3343.388, -2324.195, 6095.772}}, 861229.639, 2872.753},
      {60, {{-3547625.888, 5872987.824, 373546.167}, {-3303.575, -2391.435, 6091.539}}, 804411.641, 2683.228},
      {70, {{-3580461.289, 5848738.240, 434437.264}, {-3263.453, -2458.442, 6086.555}}, 749912.688, 2501.439},
      {80, {{-3612893.955, 5823819.785, 495274.762}, {-3223.028, -2525.208, 6080.819}}, 698283.095, 2329.222},
      {90, {{-3644920.859, 5798234.916, 556051.152}, {-3182.305, -2591.725, 6074.333}}, 650213.492, 2168.879},
      {100, {{-3676539.069, 5771986.136, 616758.933}, {-3141.287, -2657.987, 6067.098}}, 606557.128, 2023.257},
      {110, {{-3707745.654, 5745076.057, 677390.613}, {-3099.980, -2723.984, 6059.113}}, 568337.610, 1895.770},
      {120, {{-3738537.724, 5717507.371, 737938.709}, {-3058.389, -2789.709, 6050.381}}, 536722.611, 1790.314},
      {130, {{-3768912.492, 5689282.812, 798395.746}, {-3016.517, -2855.155, 6040.902}}, 512938.837, 1710.980},
      {140, {{-3798867.163, 5660405.225, 858754.264}, {-2974.370, -2920.313, 6030.677}}, 498112.336, 1661.524},
      {150, {{-3828398.993, 5630877.534, 919006.811}, {-2931.953, -2985.177, 6019.708}}, 493053.686, 1644.650},
      {160, {{-3857505.334, 5600702.703, 979145.949}, {-2889.270, -3049.738, 6007.996}}, 498061.276, 1661.354},
      {170, {{-3886183.539, 5569883.808, 1039164.256}, {-2846.326, -3113.988, 5995.542}}, 512839.845, 1710.650},
      {180, {{-3914431.005, 5538424.000, 1099054.323}, {-2803.127, -3177.921, 5982.348}}, 536581.125, 1789.842},
      {190, {{-3942245.232, 5506326.472, 1158808.756}, {-2759.676, -3241.529, 5968.416}}, 568160.217, 1895.178},
      {200, {{-3969623.699, 5473594.538, 1218420.179}, {-2715.979, -3304.803, 5953.746}}, 606350.493, 2022.568},
      {210, {{-3996564.002, 5440231.540, 1277881.235}, {-2672.041, -3367.738, 5938.342}}, 649983.744, 2168.112},
      {220, {{-4023063.735, 5406240.931, 1337184.582}, {-2627.866, -3430.325, 5922.205}}, 698035.524, 2328.396},
      {230, {{-4049120.544, 5371626.236, 1396322.901}, {-2583.460, -3492.556, 5905.337}}, 749651.691, 2500.569},
      {240, {{-4074732.172, 5336391.020, 1455288.892}, {-2538.827, -3554.425, 5887.740}}, 804140.848, 2682.325},
      {250, {{-4099896.362, 5300538.954, 1514075.277}, {-2493.973, -3615.925, 5869.416}}, 860952.022, 2871.827},
      {260, {{-4124610.909, 5264073.786, 1572674.799}, {-2448.902, -3677.047, 5850.368}}, 919649.060, 3067.619},
      {270, {{-4148873.703, 5226999.297, 1631080.226}, {-2403.620, -3737.785, 5830.597}}, 979886.998, 3268.551},
      {280, {{-4172682.638, 5189319.378, 1689284.347}, {-2358.132, -3798.132, 5810.107}}, 1041392.138, 3473.710},
      {290, {{-4196035.660, 5151037.992, 1747279.980}, {-2312.441, -3858.080, 5788.900}}, 1103946.198, 3682.368},
      {300, {{-4218930.808, 5112159.136, 1805059.966}, {-2266.555, -3917.622, 5766.978}}, 1167373.958, 3893.940},
  };

  std::string init_utc_time   = "2025-06-24T09:00:00"; // need to set some value
  time_point  init_epoch_time = string_to_timepoint(init_utc_time);

  rk4_propagator       orbit_propagator(std::chrono::duration<double>(1.0));
  state_vector         init_ecef_rv = std::get<1>(test_cases[0]);
  orbit_ephemeris_info orbit_ephemeris_info{orbit_propagator, init_epoch_time, init_ecef_rv, false};

  for (const auto& [propagation_time, expected_ecef_rv, exp_range_m, exp_delay_us] : test_cases) {
    auto propagation_duration =
        init_epoch_time + std::chrono::seconds(propagation_time) - orbit_ephemeris_info.epoch_time();

    orbit_ephemeris_info.propagate(propagation_duration, true);
    state_vector rv_diff     = orbit_ephemeris_info.ecef_rv() - expected_ecef_rv;
    pos_error_m              = norm(rv_diff.position);
    vel_error_m_s            = norm(rv_diff.velocity);
    range_m                  = norm((orbit_ephemeris_info.ecef_rv() - ue_ecef_rv).position);
    range_error_m            = range_m - exp_range_m;
    link_delay_us            = range_m / LIGHT_SPEED_M_S * 1e6;
    link_delay_error_ns      = (link_delay_us - exp_delay_us) * 1e3;
    max_range_error_m        = std::max(max_range_error_m, std::abs(range_error_m));
    max_delay_error_ns       = std::max(max_delay_error_ns, std::abs(link_delay_error_ns));
    auto expected_epoch_time = init_epoch_time + std::chrono::seconds(propagation_time);
    auto expected_epoch_ts =
        std::chrono::duration_cast<std::chrono::milliseconds>(expected_epoch_time.time_since_epoch()).count();
    auto orbit_epoch_ts =
        std::chrono::duration_cast<std::chrono::milliseconds>(orbit_ephemeris_info.epoch_time().time_since_epoch())
            .count();
    ASSERT_EQ(orbit_epoch_ts, expected_epoch_ts);
    ASSERT_NEAR(pos_error_m, 0.0, pos_tolerance_m);
    ASSERT_NEAR(vel_error_m_s, 0.0, vel_tolerance_m_s);
    ASSERT_NEAR(range_error_m, 0.0, range_tolerance_m);
    ASSERT_NEAR(link_delay_error_ns, 0.0, link_delay_tolerance_ns);

    if (debug_mode) {
      fmt::print("Actual Position:    [{:.3f}, {:.3f}, {:.3f}]\n",
                 orbit_ephemeris_info.ecef_rv().position.x,
                 orbit_ephemeris_info.ecef_rv().position.y,
                 orbit_ephemeris_info.ecef_rv().position.z);
      fmt::print("Expected Position:  [{:.3f}, {:.3f}, {:.3f}]\n",
                 expected_ecef_rv.position.x,
                 expected_ecef_rv.position.y,
                 expected_ecef_rv.position.z);
      fmt::print("Actual Velocity:    [{:.3f}, {:.3f}, {:.3f}]\n",
                 orbit_ephemeris_info.ecef_rv().velocity.x,
                 orbit_ephemeris_info.ecef_rv().velocity.y,
                 orbit_ephemeris_info.ecef_rv().velocity.z);
      fmt::print("Expected Velocity:  [{:.3f}, {:.3f}, {:.3f}]\n",
                 expected_ecef_rv.velocity.x,
                 expected_ecef_rv.velocity.y,
                 expected_ecef_rv.velocity.z);
      fmt::print("Propagation time [s]: {}, SAT pos_error: {:.3f} m, vel_error: {:.3f} m/s\n",
                 propagation_time,
                 pos_error_m,
                 vel_error_m_s);
      fmt::print("Propagation time [s]: {}, SAT-UE range: {:.3f} km, expected: {:.3f}, error: {:.3f} m\n",
                 propagation_time,
                 range_m / 1e3,
                 exp_range_m / 1e3,
                 range_error_m,
                 max_range_error_m);
      fmt::print("Propagation time [s]: {}, SAT-UE delay {:.3f} us, expected: {:.3f} us, error: {:.3f} ns\n",
                 propagation_time,
                 link_delay_us,
                 exp_delay_us,
                 link_delay_error_ns);
    }
  }
  if (debug_mode) {
    fmt::print("Comparison with Numerical propagator: SAT-UE max range error: {:.3f} m, max delay error: {:.3f} ns\n",
               max_range_error_m,
               max_delay_error_ns);
  }
}

TEST(test_converters, propagate_leo_orbit_for_5s_step_160ms_with_ref_from_matlab_numerical_propagator)
{
  // Tolerances.
  double pos_tolerance_m         = 0.2;
  double vel_tolerance_m_s       = 0.02;
  double range_tolerance_m       = 0.2;
  double link_delay_tolerance_ns = 0.5;

  double pos_error_m         = 0;
  double vel_error_m_s       = 0;
  double range_m             = 0;
  double range_error_m       = 0;
  double max_range_error_m   = 0;
  double link_delay_us       = 0;
  double link_delay_error_ns = 0;
  double max_delay_error_ns  = 0;

  geodetic_coordinates ue_location = {7.734134202493742, 124.2115429594156, 1.0};
  state_vector         ue_ecef_rv  = coordinate_converter::geodetic_to_ecef(ue_location);

  // State Vector in ECEF frame, LEO pass.
  std::vector<std::tuple<unsigned, state_vector, double, double>> test_cases = {
      {0, {{-3342330.380, 6004311.941, 7495.104}, {-3537.680, -1984.780, 6105.646}}, 1167656.334, 3894.882},
      {1, {{-3342896.375, 6003994.280, 8472.007}, {-3537.071, -1985.874, 6105.644}}, 1166635.421, 3891.477},
      {2, {{-3343462.254, 6003676.455, 9448.910}, {-3536.462, -1986.968, 6105.643}}, 1165614.704, 3888.072},
      {3, {{-3344028.036, 6003358.454, 10425.813}, {-3535.854, -1988.062, 6105.641}}, 1164594.175, 3884.668},
      {4, {{-3344593.721, 6003040.278, 11402.715}, {-3535.245, -1989.156, 6105.639}}, 1163573.835, 3881.265},
      {5, {{-3345159.308, 6002721.928, 12379.617}, {-3534.636, -1990.250, 6105.636}}, 1162553.683, 3877.862},
      {6, {{-3345724.816, 6002403.392, 13356.519}, {-3534.027, -1991.344, 6105.634}}, 1161533.710, 3874.459},
      {7, {{-3346290.208, 6002084.691, 14333.420}, {-3533.417, -1992.438, 6105.631}}, 1160513.937, 3871.058},
      {8, {{-3346855.503, 6001765.815, 15310.320}, {-3532.808, -1993.531, 6105.628}}, 1159494.354, 3867.657},
      {9, {{-3347420.701, 6001446.765, 16287.221}, {-3532.199, -1994.625, 6105.625}}, 1158474.962, 3864.257},
      {10, {{-3347985.800, 6001127.539, 17264.120}, {-3531.589, -1995.719, 6105.622}}, 1157455.761, 3860.857},
      {11, {{-3348550.803, 6000808.138, 18241.020}, {-3530.980, -1996.812, 6105.618}}, 1156436.752, 3857.458},
      {12, {{-3349115.725, 6000488.552, 19217.918}, {-3530.370, -1997.906, 6105.614}}, 1155417.925, 3854.059},
      {13, {{-3349680.533, 6000168.802, 20194.816}, {-3529.760, -1998.999, 6105.611}}, 1154399.302, 3850.662},
      {14, {{-3350245.242, 5999848.876, 21171.714}, {-3529.151, -2000.093, 6105.607}}, 1153380.871, 3847.264},
      {15, {{-3350809.855, 5999528.776, 22148.610}, {-3528.541, -2001.186, 6105.602}}, 1152362.635, 3843.868},
      {16, {{-3351374.369, 5999208.500, 23125.506}, {-3527.931, -2002.279, 6105.598}}, 1151344.593, 3840.472},
      {17, {{-3351938.786, 5998888.050, 24102.402}, {-3527.321, -2003.373, 6105.593}}, 1150326.746, 3837.077},
      {18, {{-3352503.123, 5998567.415, 25079.296}, {-3526.711, -2004.466, 6105.588}}, 1149309.086, 3833.682},
      {19, {{-3353067.345, 5998246.614, 26056.190}, {-3526.100, -2005.559, 6105.583}}, 1148291.631, 3830.289},
      {20, {{-3353631.469, 5997925.639, 27033.083}, {-3525.490, -2006.652, 6105.578}}, 1147274.373, 3826.895},
      {21, {{-3354195.495, 5997604.489, 28009.975}, {-3524.880, -2007.745, 6105.573}}, 1146257.312, 3823.503},
      {22, {{-3354759.424, 5997283.164, 28986.866}, {-3524.269, -2008.838, 6105.567}}, 1145240.449, 3820.111},
      {23, {{-3355323.273, 5996961.654, 29963.756}, {-3523.659, -2009.931, 6105.561}}, 1144223.775, 3816.720},
      {24, {{-3355887.006, 5996639.980, 30940.645}, {-3523.048, -2011.024, 6105.555}}, 1143207.309, 3813.329},
      {25, {{-3356450.642, 5996318.130, 31917.534}, {-3522.437, -2012.117, 6105.549}}, 1142191.043, 3809.939},
      {26, {{-3357014.180, 5995996.106, 32894.421}, {-3521.826, -2013.210, 6105.543}}, 1141174.977, 3806.550},
      {27, {{-3357577.620, 5995673.906, 33871.307}, {-3521.216, -2014.303, 6105.536}}, 1140159.111, 3803.161},
      {28, {{-3358140.963, 5995351.532, 34848.193}, {-3520.605, -2015.396, 6105.529}}, 1139143.447, 3799.774},
      {29, {{-3358704.225, 5995028.973, 35825.077}, {-3519.993, -2016.488, 6105.522}}, 1138127.975, 3796.386},
      {30, {{-3359267.372, 5994706.250, 36801.960}, {-3519.382, -2017.581, 6105.515}}, 1137112.715, 3793.000},
      {31, {{-3359830.421, 5994383.351, 37778.842}, {-3518.771, -2018.674, 6105.508}}, 1136097.658, 3789.614},
      {32, {{-3360393.372, 5994060.277, 38755.722}, {-3518.160, -2019.766, 6105.500}}, 1135082.805, 3786.229},
  };

  std::string init_utc_time   = "2025-06-24T09:00:00"; // need to set some value
  time_point  init_epoch_time = string_to_timepoint(init_utc_time);

  rk4_propagator       orbit_propagator(std::chrono::duration<double>(1.0));
  state_vector         init_ecef_rv = std::get<1>(test_cases[0]);
  orbit_ephemeris_info orbit_ephemeris_info{orbit_propagator, init_epoch_time, init_ecef_rv, false};

  for (const auto& [propagation_time_idx, expected_ecef_rv, exp_range_m, exp_delay_us] : test_cases) {
    auto propagation_duration =
        init_epoch_time + std::chrono::milliseconds(propagation_time_idx * 160) - orbit_ephemeris_info.epoch_time();

    orbit_ephemeris_info.propagate(propagation_duration, true);
    state_vector rv_diff     = orbit_ephemeris_info.ecef_rv() - expected_ecef_rv;
    pos_error_m              = norm(rv_diff.position);
    vel_error_m_s            = norm(rv_diff.velocity);
    range_m                  = norm((orbit_ephemeris_info.ecef_rv() - ue_ecef_rv).position);
    range_error_m            = range_m - exp_range_m;
    link_delay_us            = range_m / LIGHT_SPEED_M_S * 1e6;
    link_delay_error_ns      = (link_delay_us - exp_delay_us) * 1e3;
    max_range_error_m        = std::max(max_range_error_m, std::abs(range_error_m));
    max_delay_error_ns       = std::max(max_delay_error_ns, std::abs(link_delay_error_ns));
    auto expected_epoch_time = init_epoch_time + std::chrono::milliseconds(propagation_time_idx * 160);
    auto expected_epoch_ts =
        std::chrono::duration_cast<std::chrono::milliseconds>(expected_epoch_time.time_since_epoch()).count();
    auto orbit_epoch_ts =
        std::chrono::duration_cast<std::chrono::milliseconds>(orbit_ephemeris_info.epoch_time().time_since_epoch())
            .count();

    ASSERT_EQ(orbit_epoch_ts, expected_epoch_ts);
    ASSERT_NEAR(pos_error_m, 0.0, pos_tolerance_m);
    ASSERT_NEAR(vel_error_m_s, 0.0, vel_tolerance_m_s);
    ASSERT_NEAR(range_error_m, 0.0, range_tolerance_m);
    ASSERT_NEAR(link_delay_error_ns, 0.0, link_delay_tolerance_ns);

    if (debug_mode) {
      fmt::print("Actual Position:    [{:.3f}, {:.3f}, {:.3f}]\n",
                 orbit_ephemeris_info.ecef_rv().position.x,
                 orbit_ephemeris_info.ecef_rv().position.y,
                 orbit_ephemeris_info.ecef_rv().position.z);
      fmt::print("Expected Position:  [{:.3f}, {:.3f}, {:.3f}]\n",
                 expected_ecef_rv.position.x,
                 expected_ecef_rv.position.y,
                 expected_ecef_rv.position.z);
      fmt::print("Actual Velocity:    [{:.3f}, {:.3f}, {:.3f}]\n",
                 orbit_ephemeris_info.ecef_rv().velocity.x,
                 orbit_ephemeris_info.ecef_rv().velocity.y,
                 orbit_ephemeris_info.ecef_rv().velocity.z);
      fmt::print("Expected Velocity:  [{:.3f}, {:.3f}, {:.3f}]\n",
                 expected_ecef_rv.velocity.x,
                 expected_ecef_rv.velocity.y,
                 expected_ecef_rv.velocity.z);
      fmt::print("Propagation time [s]: {}, SAT pos_error: {:.3f} m, vel_error: {:.3f} m/s\n",
                 propagation_time_idx * 0.16,
                 pos_error_m,
                 vel_error_m_s);
      fmt::print("Propagation time [s]: {}, SAT-UE range: {:.3f} km, expected: {:.3f}, error: {:.3f} m\n",
                 propagation_time_idx * 0.16,
                 range_m / 1e3,
                 exp_range_m / 1e3,
                 range_error_m,
                 max_range_error_m);
      fmt::print("Propagation time [s]: {}, SAT-UE delay {:.3f} us, expected: {:.3f} us, error: {:.3f} ns\n",
                 propagation_time_idx * 0.16,
                 link_delay_us,
                 exp_delay_us,
                 link_delay_error_ns);
    }
  }
  if (debug_mode) {
    fmt::print("Comparison with Numerical propagator: SAT-UE max range error: {:.3f} m, max delay error: {:.3f} ns\n",
               max_range_error_m,
               max_delay_error_ns);
  }
}

// ---------------------------------------------------------------------------
// Head-to-head comparison: RK4 vs Keplerian on the same LEO initial conditions
// ---------------------------------------------------------------------------
//
// Both propagators start from the same orbital elements and are advanced 1 s
// at a time for 5 minutes.  At every step the satellite ECEF position, the
// slant range to a reference UE, and the one-way propagation delay are
// compared.  With debug_mode=true a per-step table and a max-error summary
// are printed so the accuracy trade-off between the two propagators is
// immediately visible when the test is run.

TEST(test_converters, compare_rk4_vs_keplerian_leo_5min)
{
  std::string init_utc_time   = "2025-06-24T09:00:00";
  time_point  init_epoch_time = string_to_timepoint(init_utc_time);

  // LEO initial orbital elements - same as propagate_leo_orbit_with_initial_oe_test.
  orbital_elements init_oe{
      6877286.310, 0.0012074892907, 0.9295782970760, 2.0359402522293, 0.8669148350739, 5.4309100278415};

  // Reference UE location - same as the SGP4/numerical reference tests.
  geodetic_coordinates ue_location = {7.734134202493742, 124.2115429594156, 1.0};
  state_vector         ue_ecef     = coordinate_converter::geodetic_to_ecef(ue_location);

  rk4_propagator       rk4;
  keplerian_propagator kepler;

  orbit_ephemeris_info orbit_rk4{rk4, init_epoch_time, init_oe};
  orbit_ephemeris_info orbit_kep{kepler, init_epoch_time, init_oe};

  double max_pos_error_m    = 0;
  double max_range_error_m  = 0;
  double max_delay_error_ns = 0;

  if (debug_mode) {
    fmt::print("\n{:>6s} | {:>12s} | {:>14s} | {:>14s}\n", "t [s]", "pos_err [m]", "range_err [m]", "delay_err [ns]");
    fmt::print("{:-<54}\n", "");
  }

  for (int t = 1; t <= 300; ++t) {
    orbit_rk4.propagate(init_epoch_time + std::chrono::seconds(t) - orbit_rk4.epoch_time(), true);
    orbit_kep.propagate(init_epoch_time + std::chrono::seconds(t) - orbit_kep.epoch_time(), true);

    const state_vector& ecef_rk4 = orbit_rk4.ecef_rv();
    const state_vector& ecef_kep = orbit_kep.ecef_rv();

    double pos_error_m    = norm((ecef_rk4 - ecef_kep).position);
    double range_rk4_m    = norm((ecef_rk4 - ue_ecef).position);
    double range_kep_m    = norm((ecef_kep - ue_ecef).position);
    double range_error_m  = std::abs(range_rk4_m - range_kep_m);
    double delay_error_ns = range_error_m / LIGHT_SPEED_M_S * 1e9;

    max_pos_error_m    = std::max(max_pos_error_m, pos_error_m);
    max_range_error_m  = std::max(max_range_error_m, range_error_m);
    max_delay_error_ns = std::max(max_delay_error_ns, delay_error_ns);

    if (debug_mode) {
      fmt::print("{:6d} | {:12.3f} | {:14.3f} | {:14.3f}\n", t, pos_error_m, range_error_m, delay_error_ns);
    }
  }

  if (debug_mode) {
    fmt::print("{:-<54}\n", "");
    fmt::print(
        "{:>6s} | {:12.3f} | {:14.3f} | {:14.3f}\n", "MAX", max_pos_error_m, max_range_error_m, max_delay_error_ns);
  }

  // Check that errors do not exceed thresholds.
  EXPECT_LT(max_pos_error_m, 600.0);
  EXPECT_LT(max_range_error_m, 300.0);
  EXPECT_LT(max_delay_error_ns, 1000.0);
}
