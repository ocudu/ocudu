// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config_cli11_schema.h"
#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config_validator.h"
#include "ocudu/ran/ntn.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace std::chrono_literals;

namespace {

/// Single cell configuration, with the defaults of the CLI11 schema and its auto-derived parameters.
class du_high_config_bench
{
public:
  du_high_config_bench()
  {
    configure_cli11_with_du_high_config_schema(app, parsed_cfg);
    app.parse(std::vector<std::string>{});
    parsed_cfg.config.cells_cfg.resize(1);
  }

  du_high_unit_base_cell_config& cell() { return parsed_cfg.config.cells_cfg[0].cell; }
  mac_sr_unit_config&            sr_cfg() { return cell().mcg_cfg.sr_cfg; }

  /// \brief Turns the cell into an NTN cell with the given round trip.
  ///
  /// Only \c cell_specific_koffset matters to the SR checks. The rest makes the cell pass the other NTN checks.
  void make_ntn_cell(std::chrono::milliseconds koffset)
  {
    auto& c          = cell();
    c.dl_f_ref_arfcn = 437000;
    c.band           = nr_band::n256;
    // An NTN cell must broadcast SIB19, which in turn needs an SI window position and a SIB below SIB15 before it.
    c.sib_cfg.si_window_len_slots = 5;
    c.sib_cfg.si_sched_info       = {{.sib_mapping_info = {2}, .si_period_rf = 16},
                                     {.sib_mapping_info = {19}, .si_period_rf = 16, .si_window_position = 2}};
    c.sib_cfg.sib2_cfg.emplace();

    auto& serving                   = c.ntn_cfg.emplace().serving.emplace();
    serving.cell_specific_koffset   = koffset;
    serving.sat_ref.epoch_timestamp = std::chrono::system_clock::time_point{};
    serving.sat_ref.ephemeris_info  = ecef_coordinates_t{.position_x  = 20922195,
                                                         .position_y  = 1967783,
                                                         .position_z  = 19770302,
                                                         .velocity_vx = 0,
                                                         .velocity_vy = 0,
                                                         .velocity_vz = 0};
  }

  /// Configuration with its auto-derived parameters filled in.
  const du_high_unit_config& derived_config()
  {
    unit_cfg = parsed_cfg.config;
    autoderive_du_high_parameters_after_parsing(unit_cfg);
    return unit_cfg;
  }

private:
  CLI::App              app{"du_high_sr_config_test"};
  du_high_parsed_config parsed_cfg;
  du_high_unit_config   unit_cfg;
};

/// SR configuration and round trip of an NTN cell.
struct ntn_sr_params {
  float                     sr_period_msec = 80;
  std::optional<unsigned>   sr_prohibit_timer;
  unsigned                  sr_trans_max = 64;
  std::chrono::milliseconds koffset;
};

/// Whether an NTN cell with the given SR configuration and round trip is valid.
bool validate_ntn_sr_config(const ntn_sr_params& params)
{
  du_high_config_bench bench;
  bench.make_ntn_cell(params.koffset);
  bench.cell().pucch_cfg.sr_period_msec = params.sr_period_msec;
  bench.sr_cfg().sr_prohibit_timer      = params.sr_prohibit_timer;
  bench.sr_cfg().sr_trans_max           = params.sr_trans_max;
  return validate_du_high_config(bench.derived_config());
}

} // namespace

TEST(du_high_sr_config_test, legacy_sr_prohibit_timer_is_accepted_in_a_tn_cell)
{
  du_high_config_bench bench;
  bench.sr_cfg().sr_prohibit_timer = 128;

  EXPECT_TRUE(validate_du_high_config(bench.derived_config()));
}

TEST(du_high_sr_config_test, extended_sr_prohibit_timer_is_rejected_in_a_tn_cell)
{
  for (unsigned timer : {192, 256, 320, 384, 448, 512, 576, 640, 1082}) {
    du_high_config_bench bench;
    bench.sr_cfg().sr_prohibit_timer = timer;

    EXPECT_FALSE(validate_du_high_config(bench.derived_config())) << timer << "ms was accepted";
  }
}

TEST(du_high_sr_config_test, extended_sr_prohibit_timer_is_accepted_in_an_ntn_cell)
{
  // One SR per round trip.
  EXPECT_TRUE(validate_ntn_sr_config({.sr_prohibit_timer = 320, .koffset = 150ms}));
}

TEST(du_high_sr_config_test, sr_period_alone_spaces_sr_retransmissions_without_a_prohibit_timer)
{
  // One SR every 80ms, 4 within the round trip, out of 8.
  EXPECT_TRUE(validate_ntn_sr_config({.sr_period_msec = 80, .sr_trans_max = 8, .koffset = 300ms}));
  // One SR every 40ms, 8 within the round trip, out of 8.
  EXPECT_FALSE(validate_ntn_sr_config({.sr_period_msec = 40, .sr_trans_max = 8, .koffset = 300ms}));
}

TEST(du_high_sr_config_test, sr_retransmissions_wait_for_the_next_sr_opportunity_after_the_prohibit_timer)
{
  // One SR every 240ms, not 192ms, so 5 within the round trip, half of sr_trans_max.
  EXPECT_TRUE(validate_ntn_sr_config({.sr_prohibit_timer = 192, .sr_trans_max = 10, .koffset = 1000ms}));
  // A prohibit timer shorter than the SR period has no effect: one SR every 80ms, 13 within the round trip.
  EXPECT_FALSE(validate_ntn_sr_config({.sr_prohibit_timer = 32, .sr_trans_max = 25, .koffset = 1000ms}));
}

TEST(du_high_sr_config_test, sr_retransmissions_using_more_than_half_of_sr_trans_max_are_rejected)
{
  // 5 SRs within the round trip, out of 8.
  EXPECT_FALSE(validate_ntn_sr_config({.sr_prohibit_timer = 192, .sr_trans_max = 8, .koffset = 1000ms}));
}

TEST(du_high_sr_config_test, sr_retransmissions_running_out_before_the_round_trip_are_only_warned_about)
{
  // 4 SRs, one every 240ms, last 960ms.
  EXPECT_TRUE(validate_ntn_sr_config({.sr_prohibit_timer = 192, .sr_trans_max = 4, .koffset = 1000ms}));
  // 64 SRs, one every 2ms, last 128ms.
  EXPECT_TRUE(validate_ntn_sr_config({.sr_period_msec = 2, .koffset = 239ms}));
}
