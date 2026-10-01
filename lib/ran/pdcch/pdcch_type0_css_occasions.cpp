// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/pdcch/pdcch_type0_css_occasions.h"
#include "ocudu/adt/format.h"

using namespace ocudu;

static constexpr unsigned N_SYMB_CORESET = UINT32_MAX;

/// TS38.213 Table 13-11 for FR1.
static const std::array<pdcch_type0_css_occasion_pattern1_description, 16> TABLE_FR1 = {{
    {0, 1.0, {0}},
    {0, 0.5, {0, N_SYMB_CORESET}},
    {2, 1.0, {0}},
    {2, 0.5, {0, N_SYMB_CORESET}},
    {5, 1.0, {0}},
    {5, 0.5, {0, N_SYMB_CORESET}},
    {7, 1.0, {0}},
    {7, 0.5, {0, N_SYMB_CORESET}},
    {0, 2.0, {0}},
    {5, 2.0, {0}},
    {0, 1.0, {1}},
    {0, 1.0, {2}},
    {2, 1.0, {1}},
    {2, 1.0, {2}},
    {5, 1.0, {1}},
    {5, 1.0, {2}},
}};

/// TS38.213 Table 13-12 for FR2.
static const std::array<pdcch_type0_css_occasion_pattern1_description, 14> TABLE_FR2 = {{
    {0, 1.0, {0}},
    {0, 0.5, {0, 7}},
    {2.5, 1.0, {0}},
    {2.5, 0.5, {0, 7}},
    {5.0, 1.0, {0}},
    {5.0, 0.5, {0, 7}},
    {0.0, 0.5, {0, N_SYMB_CORESET}},
    {2.5, 0.5, {0, N_SYMB_CORESET}},
    {5.0, 0.5, {0, N_SYMB_CORESET}},
    {7.5, 1.0, {0}},
    {7.5, 0.5, {0, 7}},
    {7.5, 0.5, {0, N_SYMB_CORESET}},
    {0.0, 2.0, {0}},
    {5.0, 2.0, {0}},
}};

pdcch_type0_css_occasion_pattern1_description
ocudu::pdcch_type0_css_occasions_get_pattern1(const pdcch_type0_css_occasion_pattern1_configuration& config)
{
  interval<unsigned, true> ss_zero_index_range(0, config.is_fr2 ? 13 : 15);
  ocudu_assert(ss_zero_index_range.contains(config.ss0_index.value()),
               "SearchSpaceZero ({}) must be {} for {}.",
               config.ss0_index,
               ss_zero_index_range,
               config.is_fr2 ? "FR2" : "FR1");

  // Select occasion from the tables.
  pdcch_type0_css_occasion_pattern1_description occasion = TABLE_FR1[config.ss0_index.value()];
  if (config.is_fr2) {
    occasion = TABLE_FR2[config.ss0_index.value()];
  }

  // Substitute the start symbol with the CORESET number of symbols.
  if (occasion.start_symbol.size() == occasion.MAX_NOF_OCCASIONS_PER_SLOT and
      occasion.start_symbol.back() == N_SYMB_CORESET) {
    occasion.start_symbol.back() = config.nof_symb_coreset;
  }

  return occasion;
}

unsigned ocudu::type0_pdcch_css_n0_is_even_frame(double   table_13_11_and_13_12_O,
                                                 double   table_13_11_and_13_12_M,
                                                 uint8_t  numerology_mu,
                                                 unsigned ssb_index)
{
  // This is only used to retrieve the nof_slots_per_frame.
  slot_point sl_point{numerology_mu, 0};

  // Compute floor( ( O * 2^mu + floor(i*M) ) / nof_slots_per_frame  ) mod 2, as per TS 38.213, Section 13.
  unsigned is_even =
      static_cast<unsigned>(std::floor(static_cast<double>(table_13_11_and_13_12_O * (1U << numerology_mu)) +
                                       std::floor(static_cast<double>(ssb_index) * table_13_11_and_13_12_M)) /
                            sl_point.nof_slots_per_frame()) %
      2;
  return is_even;
}

slot_point ocudu::get_type0_pdcch_css_n0(double             table_13_11_and_13_12_O,
                                         double             table_13_11_and_13_12_M,
                                         subcarrier_spacing scs_common,
                                         unsigned           ssb_index)
{
  // Initialize n0 to a slot_point = 0.
  const uint8_t numerology_mu = to_numerology_value(scs_common);
  slot_point    type0_pdcch_css_n0{numerology_mu, 0};

  // Compute n0 = ( O * 2^mu + floor(i*M)  )  % nof_slots_per_frame, as per TS 38.213, Section 13.
  type0_pdcch_css_n0 += static_cast<unsigned>(static_cast<double>(table_13_11_and_13_12_O * (1U << numerology_mu)) +
                                              std::floor(static_cast<double>(ssb_index) * table_13_11_and_13_12_M)) %
                        type0_pdcch_css_n0.nof_slots_per_frame();

  // We want to express n0 as a value from 0 to max_nof_slots. Since the mod operation above cap n0 to
  // (nof_slots_per_frame - 1), we need to add nof_slots_per_frame to n0 if this falls into an odd frame.
  type0_pdcch_css_n0 +=
      type0_pdcch_css_n0_is_even_frame(table_13_11_and_13_12_O, table_13_11_and_13_12_M, numerology_mu, ssb_index) *
      type0_pdcch_css_n0.nof_slots_per_frame();

  return type0_pdcch_css_n0;
}
