// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/precoding/precoding_and_beamforming_info.h"
#include <gtest/gtest.h>

using namespace ocudu;

TEST(precoding_and_beamforming_info_test, default_built_info_selects_no_precoding)
{
  const precoding_and_beamforming_info info;

  ASSERT_TRUE(std::holds_alternative<precoding_matrix_indicator>(info));
  ASSERT_TRUE(std::holds_alternative<std::monostate>(std::get<precoding_matrix_indicator>(info)));
}

TEST(precoding_and_beamforming_info_test, single_beam_precoding_maps_the_whole_allocation_onto_one_beam)
{
  const beam_identifier                beam_id = to_beam_id(5);
  const precoding_and_beamforming_info info    = make_single_beam_precoding(beam_id);

  ASSERT_TRUE(std::holds_alternative<beam_identifier>(info));
  ASSERT_EQ(std::get<beam_identifier>(info), beam_id);
}

TEST(precoding_and_beamforming_info_test, default_precoding_selects_the_default_beams)
{
  const precoding_and_beamforming_info info = make_default_precoding();

  ASSERT_TRUE(std::holds_alternative<precoding_matrix_indicator>(info));
  ASSERT_TRUE(std::holds_alternative<std::monostate>(std::get<precoding_matrix_indicator>(info)));
}
