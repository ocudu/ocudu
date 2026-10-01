// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "typeii_csi_report_packer.h"
#include "ocudu/ran/csi_report/csi_report_size.h"
#include "ocudu/ran/precoding/precoding_codebook_type2_helpers.h"
#include "ocudu/support/ocudu_assert.h"
#include <algorithm>

using namespace ocudu;
using namespace test_helpers;

/// Returns the value that the rank indicator field carries for the given rank, as per TS38.212 Table 6.3.1.1.2-3.
static unsigned get_rank_index(const ri_restriction_type& ri_restriction, unsigned rank)
{
  const auto  allowed = ri_restriction.get_bit_positions();
  const auto* it      = std::find(allowed.begin(), allowed.end(), static_cast<size_t>(rank - 1));
  ocudu_assert(it != allowed.end(), "The rank (i.e., {}) is not allowed by the rank restriction.", rank);

  return static_cast<unsigned>(it - allowed.begin());
}

/// Returns the number of indicators of the number of non-zero wideband amplitudes that CSI Part 1 carries.
static unsigned get_nof_amplitude_indicators(const csi_report_configuration& config)
{
  return (config.ri_restriction.find_highest() >= 1) ? max_nof_typeII_layers : 1;
}

std::pair<csi_report_packed, csi_report_packed>
ocudu::test_helpers::pack_typeii_csi_report_pusch(const csi_report_configuration& config,
                                                  const typeii_csi_report_values& values)
{
  const auto* codebook = std::get_if<pmi_codebook_typeII>(&config.pmi_codebook);
  ocudu_assert(codebook != nullptr, "The report configuration does not select the Type II codebook.");
  ocudu_assert(!codebook->subband_amplitude, "The packer does not support the subband amplitude reporting.");
  ocudu_assert((config.quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
                   (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi),
               "The report quantities do not contain the PMI.");

  // Type II wideband amplitude index that corresponds to a unit amplitude.
  static constexpr unsigned max_wideband_amplitude_index = 7;

  const unsigned nof_coefficients = 2 * codebook->nof_beams.value();
  const unsigned nof_phase_bits   = log2_ceil(static_cast<unsigned>(codebook->phase_alphabet_size));

  for (unsigned i_layer = 0; i_layer != values.ri; ++i_layer) {
    ocudu_assert(
        values.i_1_3[i_layer] < nof_coefficients,
        "The strongest coefficient index of layer {} (i.e., {}) exceeds the number of coefficients (i.e., {}).",
        i_layer,
        values.i_1_3[i_layer],
        nof_coefficients);
  }

  const ri_li_cqi_cri_sizes sizes =
      get_ri_li_cqi_cri_sizes(config.pmi_codebook, config.ri_restriction, values.ri, config.nof_csi_rs_resources);

  // CSI Part 1, as per TS38.212 Table 6.3.2.1.2-3.
  csi_report_packed part1;

  // The report always selects the first CSI-RS resource.
  part1.push_back(0U, sizes.cri);
  part1.push_back(get_rank_index(config.ri_restriction, values.ri), sizes.ri);
  part1.push_back(values.wideband_cqi, sizes.wideband_cqi_first_tb);

  // Every coefficient carries a non-zero wideband amplitude. The field reports their number minus one.
  const unsigned nof_indicators = get_nof_amplitude_indicators(config);
  for (unsigned i_indicator = 0; i_indicator != nof_indicators; ++i_indicator) {
    part1.push_back(nof_coefficients - 1, sizes.nof_wideband_amplitudes);
  }

  // CSI Part 2, as per TS38.212 Table 6.3.2.1.2-4.
  csi_report_packed part2;

  if (sizes.wideband_cqi_second_tb != 0) {
    part2.push_back(values.wideband_cqi, sizes.wideband_cqi_second_tb);
  }
  if ((config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi) && (sizes.li != 0)) {
    part2.push_back(0U, sizes.li);
  }

  // The number of non-zero amplitudes of every layer selects the PMI field sizes.
  typeII_nof_amplitudes nof_amplitudes;
  for (unsigned i_layer = 0; i_layer != values.ri; ++i_layer) {
    nof_amplitudes.push_back(nof_coefficients);
  }
  const pmi_typeII_param_sizes pmi_sizes = get_pmi_sizes_typeII(*codebook, nof_amplitudes);

  part2.push_back(values.i_1_1, pmi_sizes.i_1_1);
  part2.push_back(values.i_1_2, pmi_sizes.i_1_2);

  // Strongest coefficient and wideband amplitudes of every layer. The amplitude of the strongest coefficient is not
  // reported.
  const std::array<unsigned, max_nof_typeII_layers> i_1_3_sizes = {pmi_sizes.i_1_3_1, pmi_sizes.i_1_3_2};
  for (unsigned i_layer = 0; i_layer != values.ri; ++i_layer) {
    part2.push_back(values.i_1_3[i_layer], i_1_3_sizes[i_layer]);

    for (unsigned i_coefficient = 0; i_coefficient != nof_coefficients; ++i_coefficient) {
      if (i_coefficient == values.i_1_3[i_layer]) {
        continue;
      }
      part2.push_back(max_wideband_amplitude_index, nof_typeII_wideband_amplitude_bits);
    }
  }

  // Phases of every layer. The phase of the strongest coefficient is not reported. Without the subband amplitude
  // reporting every reported coefficient carries a full resolution phase.
  for (unsigned i_layer = 0; i_layer != values.ri; ++i_layer) {
    for (unsigned i_coefficient = 0; i_coefficient != nof_coefficients; ++i_coefficient) {
      if (i_coefficient == values.i_1_3[i_layer]) {
        continue;
      }
      part2.push_back(values.phase[i_layer], nof_phase_bits);
    }
  }

  return {part1, part2};
}
