// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/adt/to_array.h"
#include "ocudu/ran/precoding/precoding_codebook_type2_helpers.h"
#include "ocudu/support/math/math_utils.h"
#include "ocudu/support/ocudu_assert.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <utility>

using namespace ocudu;

/// Wideband amplitude coefficients \f$p^{(1)}\f$ indexed by \f$k^{(1)}\f$, as per TS38.214 Table 5.2.2.2.3-2.
static const auto typeII_wideband_amplitudes = to_array<float>({0.0F,
                                                                std::sqrt(1.0F / 64),
                                                                std::sqrt(1.0F / 32),
                                                                std::sqrt(1.0F / 16),
                                                                std::sqrt(1.0F / 8),
                                                                std::sqrt(1.0F / 4),
                                                                std::sqrt(1.0F / 2),
                                                                1.0F});

/// Subband amplitude coefficients \f$p^{(2)}\f$ indexed by \f$k^{(2)}\f$, as per TS38.214 Table 5.2.2.2.3-3.
static const auto typeII_subband_amplitudes = to_array<float>({std::sqrt(1.0F / 2), 1.0F});

/// Combinatorial coefficients \f$C(x, y)\f$ as per TS38.214 Table 5.2.2.2.3-1, indexed as [x][y-1].
static constexpr std::array<std::array<unsigned, max_nof_typeII_beams>, 16> combinatorial_coefficient_table = {
    {{0, 0, 0, 0},
     {1, 0, 0, 0},
     {2, 1, 0, 0},
     {3, 3, 1, 0},
     {4, 6, 4, 1},
     {5, 10, 10, 5},
     {6, 15, 20, 15},
     {7, 21, 35, 35},
     {8, 28, 56, 70},
     {9, 36, 84, 126},
     {10, 45, 120, 210},
     {11, 55, 165, 330},
     {12, 66, 220, 495},
     {13, 78, 286, 715},
     {14, 91, 364, 1001},
     {15, 105, 455, 1365}}};

/// Returns the combinatorial coefficient \f$C(x, y)\f$ from TS38.214 Table 5.2.2.2.3-1.
static unsigned combinatorial_coefficient(unsigned x, unsigned y)
{
  ocudu_assert((x < combinatorial_coefficient_table.size()) && (y >= 1) && (y <= max_nof_typeII_beams),
               "The combinatorial coefficient C({}, {}) is out of the table range.",
               x,
               y);
  return combinatorial_coefficient_table[x][y - 1];
}

pmi_typeII_beam_selection ocudu::get_typeII_beam_selection(unsigned i_1_1, unsigned o1, unsigned o2)
{
  ocudu_assert((o1 != 0) && (o2 != 0), "The oversampling factors must be non-zero.");

  // O2 is always a power of two, so extracting those subfields is equivalent to a plain division/modulo.
  unsigned q2 = i_1_1 % o2;
  unsigned q1 = i_1_1 / o2;

  ocudu_assert(q1 < o1, "The decoded beam selection q1={} is out of range (O1={}).", q1, o1);
  ocudu_assert(q2 < o2, "The decoded beam selection q2={} is out of range (O2={}).", q2, o2);

  return {q1, q2};
}

static_vector<pmi_typeII_beam_group, max_nof_typeII_beams>
ocudu::get_typeII_beam_groups(unsigned i_1_2, unsigned n1, unsigned n2, unsigned nof_beams)
{
  unsigned n1n2 = n1 * n2;

  ocudu_assert((nof_beams != 0) && (nof_beams <= max_nof_typeII_beams) && (nof_beams <= n1n2),
               "Invalid number of beams L={} for N1 * N2={}.",
               nof_beams,
               n1n2);

  // Greedy decode combinatorial index i_1_2. The search starts from the largest allowed value downwards. Each linear
  // position n^(i) is then split into its (n1, n2) indices.
  static_vector<pmi_typeII_beam_group, max_nof_typeII_beams> beam_groups;
  unsigned                                                   remaining = i_1_2;
  unsigned                                                   upper     = n1n2 - 1;
  for (unsigned i = 0; i != nof_beams; ++i) {
    unsigned x = upper;
    unsigned y = nof_beams - i;
    unsigned comb_coef;
    while ((comb_coef = combinatorial_coefficient(x, y)) > remaining) {
      --x;
    }
    remaining -= comb_coef;

    unsigned linear = n1n2 - 1 - x;
    beam_groups.push_back({linear % n1, linear / n1});
    upper = (x == 0) ? 0 : (x - 1);
  }

  return beam_groups;
}

float ocudu::get_typeII_wideband_amplitude(unsigned k1)
{
  ocudu_assert(k1 < typeII_wideband_amplitudes.size(),
               "The wideband amplitude index (i.e., {}) is out of range (i.e., {}).",
               k1,
               typeII_wideband_amplitudes.size());
  return typeII_wideband_amplitudes[k1];
}

float ocudu::get_typeII_subband_amplitude(unsigned k2)
{
  ocudu_assert(k2 < typeII_subband_amplitudes.size(),
               "The subband amplitude index (i.e., {}) is out of range (i.e., {}).",
               k2,
               typeII_subband_amplitudes.size());
  return typeII_subband_amplitudes[k2];
}

unsigned ocudu::get_typeII_nof_total_beam_groups(const pmi_codebook_typeII& codebook)
{
  // Extract antenna panel information.
  const pmi_codebook_single_panel_info& panel_info = get_single_panel_info(codebook.n1_n2);

  // Number of beam groups in the antenna panel.
  unsigned nof_beam_groups = panel_info.n1 * panel_info.n2;

  // Number of selected beams in the codebook.
  unsigned nof_beams = codebook.nof_beams.value();

  // Number of possible beam group combinations based on the number of beams. Uses Pascal's rule combinatorial identity
  // to find C(N1N2, L), which is not indexed in TS38.214 Table 5.2.2.2.3-1.
  unsigned nof_beam_groups_combination = combinatorial_coefficient(nof_beam_groups - 1, nof_beams) +
                                         combinatorial_coefficient(nof_beam_groups - 1, nof_beams - 1);

  return nof_beam_groups_combination;
}

pmi_typeII_param_ranges ocudu::get_pmi_ranges_typeII(const pmi_codebook_typeII& panel, uint8_t ri)
{
  // The Type II codebook supports rank 1 or 2 only.
  ocudu_assert((ri == 1) || (ri == 2), "The Type II codebook supports one or two layers, requested ri={}.", ri);

  // Extract antenna panel information.
  const pmi_codebook_single_panel_info& panel_info = get_single_panel_info(panel.n1_n2);

  // The i_1_1 range is the total number of beams per group.
  unsigned i_1_1_range = panel_info.o1 * panel_info.o2;

  // The i_1_2 range is the total number of possible beam group combinations.
  unsigned i_1_2_range = get_typeII_nof_total_beam_groups(panel);

  // The remaining parameters are reported per beam and polarization, so their range is the number of beams, i.e., 2*L.
  unsigned nof_beams = 2 * panel.nof_beams.value();

  pmi_typeII_param_ranges ranges = {
      .i_1_1   = i_1_1_range,
      .i_1_2   = i_1_2_range,
      .i_1_3_1 = nof_beams,
      .i_1_3_2 = (ri == 2) ? nof_beams : 0,
      .i_1_4_1 = nof_beams,
      .i_1_4_2 = (ri == 2) ? nof_beams : 0,
      .i_2_1_1 = nof_beams,
      .i_2_1_2 = (ri == 2) ? nof_beams : 0,
      .i_2_2_1 = panel.subband_amplitude ? nof_beams : 0,
      .i_2_2_2 = ((ri == 2) && panel.subband_amplitude) ? nof_beams : 0,
  };

  return ranges;
}

static_vector<uint8_t, max_nof_typeII_coefficients>
ocudu::get_typeII_coefficient_strength_order(span<const uint8_t> wideband_amplitudes, unsigned strongest_coefficient)
{
  ocudu_assert(wideband_amplitudes.size() <= max_nof_typeII_coefficients,
               "The number of wideband amplitudes (i.e., {}) exceeds the maximum (i.e., {}).",
               wideband_amplitudes.size(),
               max_nof_typeII_coefficients);
  ocudu_assert(strongest_coefficient < wideband_amplitudes.size(),
               "The strongest coefficient index (i.e., {}) exceeds the number of coefficients (i.e., {}).",
               strongest_coefficient,
               wideband_amplitudes.size());

  // Gather the indices of the non-zero coefficients, excluding the strongest one.
  static_vector<uint8_t, max_nof_typeII_coefficients> result;
  for (unsigned i = 0, i_end = wideband_amplitudes.size(); i != i_end; ++i) {
    if ((i != strongest_coefficient) && (wideband_amplitudes[i] != 0)) {
      result.push_back(i);
    }
  }

  // Sort by decreasing wideband amplitude, preserving the order of equivalent elements.
  std::stable_sort(result.begin(), result.end(), [&wideband_amplitudes](uint8_t left, uint8_t right) {
    return wideband_amplitudes[left] > wideband_amplitudes[right];
  });

  return result;
}

pmi_typeII_param_sizes ocudu::get_pmi_sizes_typeII(const pmi_codebook_typeII&   codebook,
                                                   const typeII_nof_amplitudes& nof_amplitudes)
{
  const pmi_codebook_single_panel_info& panel_info = get_single_panel_info(codebook.n1_n2);

  unsigned nof_beams        = codebook.nof_beams.value();
  unsigned nof_coefficients = 2 * nof_beams;
  unsigned nof_beam_groups  = panel_info.n1 * panel_info.n2;
  unsigned nof_phase_bits   = log2_ceil(static_cast<unsigned>(codebook.phase_alphabet_size));
  unsigned nof_layers       = nof_amplitudes.size();

  ocudu_assert(nof_beams <= nof_beam_groups,
               "The number of combined beams (i.e., {}) exceeds the number of available beam groups N1*N2 (i.e., {}).",
               nof_beams,
               nof_beam_groups);
  ocudu_assert(nof_layers <= max_nof_typeII_layers,
               "The number of layers (i.e., {}) exceeds the maximum for Type II codebook (i.e., {}).",
               nof_layers,
               max_nof_typeII_layers);

  // Ensure that the number of non-zero wideband amplitudes for the first layer is within the valid range.
  ocudu_assert((nof_amplitudes[0] >= 1) && (nof_amplitudes[0] <= nof_coefficients),
               "The number of non-zero wideband amplitudes of the first layer (i.e., {}) is out of range [1, {}].",
               nof_amplitudes[0],
               nof_coefficients);

  // In case of two layers, ensure that the number of non-zero wideband amplitudes for the second layer is within the
  // valid range.
  ocudu_assert((nof_layers == 1) || ((nof_amplitudes[1] >= 1) && (nof_amplitudes[1] <= nof_coefficients)),
               "The number of non-zero wideband amplitudes of the second layer (i.e., {}) is out of range [1, {}].",
               nof_amplitudes[1],
               nof_coefficients);

  // Helper lambda to compute the bit-width of the phase coefficient and subband amplitude coefficient for one layer. It
  // is the same in case of both layers.
  auto get_coefficient_sizes =
      [&codebook, nof_beams, nof_phase_bits](unsigned nof_amplitudes_layer) -> std::pair<unsigned, unsigned> {
    if (!codebook.subband_amplitude) {
      // All the reported coefficients other than the strongest one carry a full resolution phase.
      return {(nof_amplitudes_layer - 1) * nof_phase_bits, 0};
    }

    // The strongest coefficients carry a full resolution phase and an amplitude, while the weakest non-zero
    // coefficients carry a QPSK phase.
    unsigned nof_full_res = std::min<unsigned>(nof_amplitudes_layer, get_typeII_nof_full_res_coefficients(nof_beams));

    return {(nof_full_res - 1) * nof_phase_bits + nof_typeII_qpsk_phase_bits * (nof_amplitudes_layer - nof_full_res),
            nof_full_res - 1};
  };

  pmi_typeII_param_sizes result = {};

  // Wideband information fields, common to all the reported layers.
  result.i_1_1 = log2_ceil(panel_info.o1 * panel_info.o2);
  result.i_1_2 = log2_ceil(get_typeII_nof_total_beam_groups(codebook));

  // Fields of the first layer.
  result.i_1_3_1                           = log2_ceil(nof_coefficients);
  result.i_1_4_1                           = nof_typeII_wideband_amplitude_bits * (nof_coefficients - 1);
  std::tie(result.i_2_1_1, result.i_2_2_1) = get_coefficient_sizes(nof_amplitudes[0]);

  // Fields of the second layer.
  if (nof_layers == 2) {
    result.i_1_3_2                           = log2_ceil(nof_coefficients);
    result.i_1_4_2                           = nof_typeII_wideband_amplitude_bits * (nof_coefficients - 1);
    std::tie(result.i_2_1_2, result.i_2_2_2) = get_coefficient_sizes(nof_amplitudes[1]);
  }

  return result;
}
