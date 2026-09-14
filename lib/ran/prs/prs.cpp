// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/prs/prs.h"
#include "ocudu/support/ocudu_assert.h"
#include <array>

using namespace ocudu;

bool ocudu::prs_valid_num_symbols_and_comb_size(prs_num_symbols nsymb, prs_comb_size comb_sz)
{
  uint8_t nsymb_u8   = static_cast<uint8_t>(nsymb);
  uint8_t comb_sz_u8 = static_cast<uint8_t>(comb_sz);
  return (nsymb_u8 >= comb_sz_u8) && (nsymb_u8 % comb_sz_u8 == 0);
}

bool ocudu::prs_valid_periodicity(unsigned int periodicity_slots, unsigned int numerology)
{
  const unsigned scaling = 1U << numerology;
  if (periodicity_slots % scaling != 0) {
    return false;
  }

  const auto& valid = prs_constants::VALID_PERIODICITIES_NUMEROLOGY0;
  return std::find(valid.begin(), valid.end(), periodicity_slots / scaling) != valid.end();
}

unsigned ocudu::get_prs_freq_offset(prs_comb_size comb_sz, unsigned l_minus_lstart)
{
  switch (comb_sz) {
    case prs_comb_size::two: {
      static constexpr std::array<uint8_t, 2> offsets = {0, 1};
      return offsets[l_minus_lstart % offsets.size()];
    }
    case prs_comb_size::four: {
      static constexpr std::array<uint8_t, 4> offsets = {0, 2, 1, 3};
      return offsets[l_minus_lstart % offsets.size()];
    }
    case prs_comb_size::six: {
      static constexpr std::array<uint8_t, 6> offsets = {0, 3, 1, 4, 2, 5};
      return offsets[l_minus_lstart % offsets.size()];
    }
    case prs_comb_size::twelve: {
      static constexpr std::array<uint8_t, 12> offsets = {0, 6, 3, 9, 1, 7, 4, 10, 2, 8, 5, 11};
      return offsets[l_minus_lstart % offsets.size()];
    }
  }
  ocudu_assert(false, "Invalid PRS comb size");
  return 0;
}
