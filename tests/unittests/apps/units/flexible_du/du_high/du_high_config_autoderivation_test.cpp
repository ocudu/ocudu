// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config.h"
#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config_cli11_schema.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

/// Auto-derives the parameters of a single cell with the given number of DL antennas, SSB candidates and beams.
du_high_unit_base_cell_config derive_cell(unsigned                                  nof_antennas_dl,
                                          std::vector<du_high_unit_ssb_beam_config> ssb_beams,
                                          std::vector<du_high_unit_ref_beam_config> beams = {})
{
  du_high_unit_config cfg;
  cfg.cells_cfg.front().cell.nof_antennas_dl = nof_antennas_dl;
  cfg.cells_cfg.front().cell.ssb_cfg.beams   = std::move(ssb_beams);
  cfg.cells_cfg.front().cell.ref_beams       = std::move(beams);

  autoderive_du_high_parameters_after_parsing(cfg);

  return cfg.cells_cfg.front().cell;
}

/// Asserts that the beam of the cell holds the given identifier and coordinates.
void assert_beam(const du_high_unit_ref_beam_config& beam,
                 unsigned                            ref_beam_id,
                 unsigned                            i_pol,
                 unsigned                            i_beam_dim1,
                 unsigned                            i_beam_dim2)
{
  ASSERT_EQ(beam.ref_beam_id, ref_beam_id);
  ASSERT_EQ(beam.i_pol, i_pol);
  ASSERT_EQ(beam.i_beam_dim1, i_beam_dim1);
  ASSERT_EQ(beam.i_beam_dim2, i_beam_dim2);
}

/// Asserts that the SSB candidate selects the given beam of the cell.
void assert_ssb_beam(const du_high_unit_ssb_beam_config& ssb_beam, unsigned ssb_index, unsigned ref_beam_id)
{
  ASSERT_EQ(ssb_beam.ssb_index, ssb_index);
  ASSERT_TRUE(ssb_beam.ref_beam_id.has_value());
  ASSERT_EQ(ssb_beam.ref_beam_id.value(), ref_beam_id);
}

} // namespace

TEST(du_high_ssb_beam_autoderivation_test, single_ssb_candidate_uses_the_first_beam)
{
  const du_high_unit_base_cell_config cell = derive_cell(1, {{.ssb_index = 0}});

  ASSERT_EQ(cell.ref_beams.size(), 1);
  assert_beam(cell.ref_beams[0], 0, 0, 0, 0);
  ASSERT_EQ(cell.ssb_cfg.beams.size(), 1);
  assert_ssb_beam(cell.ssb_cfg.beams[0], 0, 0);
}

// TODO: updated antenna topology, fix test.
#if 0
TEST(du_high_ssb_beam_autoderivation_test, derived_beams_sweep_the_polarization_before_the_first_dimension)
{
  // A four antenna cell uses the 2x1 single-panel topology: two polarizations and eight beams in the first dimension.
  const du_high_unit_base_cell_config cell =
      derive_cell(4, {{.ssb_index = 0}, {.ssb_index = 2}, {.ssb_index = 4}, {.ssb_index = 5}});

  ASSERT_EQ(cell.ref_beams.size(), 4);
  assert_beam(cell.ref_beams[0], 0, 0, 0, 0);
  assert_beam(cell.ref_beams[1], 1, 1, 0, 0);
  assert_beam(cell.ref_beams[2], 2, 0, 1, 0);
  assert_beam(cell.ref_beams[3], 3, 1, 1, 0);

  ASSERT_EQ(cell.ssb_cfg.beams.size(), 4);
  assert_ssb_beam(cell.ssb_cfg.beams[0], 0, 0);
  assert_ssb_beam(cell.ssb_cfg.beams[1], 2, 1);
  assert_ssb_beam(cell.ssb_cfg.beams[2], 4, 2);
  assert_ssb_beam(cell.ssb_cfg.beams[3], 5, 3);
}

TEST(du_high_ssb_beam_autoderivation_test, the_sweep_follows_the_ssb_candidate_order_not_the_configuration_order)
{
  const du_high_unit_base_cell_config cell = derive_cell(4, {{.ssb_index = 5}, {.ssb_index = 0}});

  ASSERT_EQ(cell.ref_beams.size(), 2);
  assert_beam(cell.ref_beams[0], 0, 0, 0, 0);
  assert_beam(cell.ref_beams[1], 1, 1, 0, 0);

  ASSERT_EQ(cell.ssb_cfg.beams.size(), 2);
  assert_ssb_beam(cell.ssb_cfg.beams[0], 5, 1);
  assert_ssb_beam(cell.ssb_cfg.beams[1], 0, 0);
}

TEST(du_high_ssb_beam_autoderivation_test, configured_ssb_beams_are_not_derived)
{
  const du_high_unit_base_cell_config cell = derive_cell(
      4, {{.ssb_index = 0, .ref_beam_id = 7}, {.ssb_index = 1}}, {{.ref_beam_id = 7, .i_pol = 1, .i_beam_dim1 = 7}});

  // The derived beam is appended after the configured one, which keeps its identifier.
  ASSERT_EQ(cell.ref_beams.size(), 2);
  assert_beam(cell.ref_beams[0], 7, 1, 7, 0);
  assert_beam(cell.ref_beams[1], 8, 1, 0, 0);

  assert_ssb_beam(cell.ssb_cfg.beams[0], 0, 7);
  // The configured candidate still takes a position in the sweep.
  assert_ssb_beam(cell.ssb_cfg.beams[1], 1, 8);
}

TEST(du_high_ssb_beam_autoderivation_test, a_derived_beam_reuses_a_configured_beam_with_the_same_coordinates)
{
  const du_high_unit_base_cell_config cell = derive_cell(4, {{.ssb_index = 0}}, {{.ref_beam_id = 5}});

  ASSERT_EQ(cell.ref_beams.size(), 1);
  assert_beam(cell.ref_beams[0], 5, 0, 0, 0);
  assert_ssb_beam(cell.ssb_cfg.beams[0], 0, 5);
}

TEST(du_high_ssb_beam_autoderivation_test, the_second_dimension_overflows_when_the_grid_runs_out_of_beams)
{
  // A single antenna cell defines a single beam, so the second candidate falls outside the grid and the configuration
  // validator rejects it.
  const du_high_unit_base_cell_config cell = derive_cell(1, {{.ssb_index = 0}, {.ssb_index = 1}});

  ASSERT_EQ(cell.ref_beams.size(), 2);
  assert_beam(cell.ref_beams[0], 0, 0, 0, 0);
  assert_beam(cell.ref_beams[1], 1, 0, 0, 1);
}
#endif

TEST(du_high_ssb_beam_autoderivation_test, beams_are_not_derived_when_the_nof_dl_antennas_has_no_topology)
{
  const du_high_unit_base_cell_config cell = derive_cell(3, {{.ssb_index = 0}});

  ASSERT_TRUE(cell.ref_beams.empty());
  ASSERT_FALSE(cell.ssb_cfg.beams[0].ref_beam_id.has_value());
}
