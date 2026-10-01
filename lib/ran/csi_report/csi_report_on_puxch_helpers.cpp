// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "csi_report_on_puxch_helpers.h"
#include "ocudu/adt/format.h"
#include "ocudu/adt/interval.h"
#include "ocudu/ran/precoding/precoding_codebook_type1_helpers.h"
#include "ocudu/ran/precoding/precoding_codebook_type2_helpers.h"
#include "ocudu/ran/precoding/precoding_constants.h"
#include "ocudu/support/error_handling.h"
#include <algorithm>
#include <array>

using namespace ocudu;

namespace {

/// Calculates the RI/LI/CQI/CRI field bit-widths for each codebook type.
struct ri_li_cqi_cri_size_calculator {
  /// Rank indicator restriction.
  const ri_restriction_type& ri_restriction;
  /// Input rank indicator.
  const csi_report_data::ri_type& ri;
  /// Number of configured CSI-RS resources.
  unsigned nof_csi_rs_resources;

  /// The default codebook reports an error.
  ri_li_cqi_cri_sizes operator()(std::monostate) const
  {
    report_error("Failed to get codebook RI/LI/CRI sizes: invalid codebook configuration.");
    return {};
  }

  /// Single-antenna port reports only contain wideband CQI, differential subband CQI, and CSI Resource indicator.
  ri_li_cqi_cri_sizes operator()(pmi_codebook_one_port) const
  {
    return {.ri                         = 0,
            .li                         = 0,
            .wideband_cqi_first_tb      = 4,
            .wideband_cqi_second_tb     = 0,
            .subband_diff_cqi_first_tb  = 2,
            .subband_diff_cqi_second_tb = 0,
            .cri                        = log2_ceil(nof_csi_rs_resources),
            .nof_wideband_amplitudes    = 0};
  }

  /// Calculates the field bit-widths for the two-antenna port configuration.
  ri_li_cqi_cri_sizes operator()(pmi_codebook_two_port) const
  {
    unsigned ri_uint              = ri.value();
    unsigned ri_restriction_count = static_cast<unsigned>(ri_restriction.count());

    return {.ri                         = std::min(1U, log2_ceil(ri_restriction_count)),
            .li                         = log2_ceil(ri_uint),
            .wideband_cqi_first_tb      = 4,
            .wideband_cqi_second_tb     = 0,
            .subband_diff_cqi_first_tb  = 2,
            .subband_diff_cqi_second_tb = 0,
            .cri                        = log2_ceil(nof_csi_rs_resources),
            .nof_wideband_amplitudes    = 0};
  }

  /// Calculates the field bit-widths for Type I Single-panel codebooks.
  ri_li_cqi_cri_sizes operator()(const pmi_codebook_typeI_single_panel& pmi_codebook) const
  {
    unsigned nof_csi_antenna_ports = get_precoding_codebook_antenna_ports(pmi_codebook);
    ocudu_assert(nof_csi_antenna_ports <= precoding_constants::MAX_NOF_PORTS,
                 "Maximum number of supported CSI-RS antenna ports is {}.",
                 precoding_constants::MAX_NOF_PORTS);

    unsigned ri_uint              = ri.value();
    unsigned ri_restriction_count = static_cast<unsigned>(ri_restriction.count());

    ocudu_assert(ri_restriction.find_lowest(true) >= 0,
                 "The RI restriction field (i.e., {}) must have at least one true value.",
                 ri_restriction);

    ri_li_cqi_cri_sizes result;
    result.ri =
        (nof_csi_antenna_ports == 4) ? std::min(2U, log2_ceil(ri_restriction_count)) : log2_ceil(ri_restriction_count);
    result.li                         = std::min(2U, log2_ceil(ri_uint));
    result.wideband_cqi_first_tb      = 4;
    result.wideband_cqi_second_tb     = (ri.value() > 4) ? 4 : 0;
    result.subband_diff_cqi_first_tb  = 2;
    result.subband_diff_cqi_second_tb = (ri.value() > 4) ? 2 : 0;
    result.cri                        = log2_ceil(nof_csi_rs_resources);
    result.nof_wideband_amplitudes    = 0;

    return result;
  }

  /// Calculates the field bit-widths for the Type II codebook as per TS38.212 Table 6.3.1.1.2-5.
  ri_li_cqi_cri_sizes operator()(const pmi_codebook_typeII& pmi_codebook) const
  {
    ocudu_assert(ri <= max_nof_typeII_layers,
                 "The rank indicator (i.e., {}) exceeds the maximum Type II number of layers (i.e., {}).",
                 ri,
                 max_nof_typeII_layers);
    ocudu_assert(ri_restriction.find_lowest(true) >= 0,
                 "The RI restriction field (i.e., {}) must have at least one true value.",
                 ri_restriction);

    unsigned nof_beams = pmi_codebook.nof_beams.value();
    ocudu_assert(
        (nof_beams >= 2) && (nof_beams <= max_nof_typeII_beams), "Invalid number of beams (i.e., {}).", nof_beams);

    unsigned ri_uint              = ri.value();
    unsigned ri_restriction_count = ri_restriction.count();

    return {.ri                         = std::min(1U, log2_ceil(ri_restriction_count)),
            .li                         = std::min(2U, log2_ceil(ri_uint)),
            .wideband_cqi_first_tb      = 4,
            .wideband_cqi_second_tb     = 0,
            .subband_diff_cqi_first_tb  = 2,
            .subband_diff_cqi_second_tb = 0,
            .cri                        = log2_ceil(nof_csi_rs_resources),
            .nof_wideband_amplitudes    = log2_ceil(2 * nof_beams - 1)};
  }
};

/// Calculates the total bit-width of the Precoding Matrix Indicator (PMI) fields.
struct pmi_size_calculator {
  /// Input Rank Indicator (RI).
  csi_report_data::ri_type ri;
  /// Number of non-zero wideband amplitude coefficients per layer, only used by the Type II codebook.
  const typeII_nof_amplitudes& nof_amplitudes;

  /// The PMI fields bit-width depends on the reported Rank Indicator (RI) and, in Type II codebook, the number of
  /// non-zero wideband amplitude coefficients.
  pmi_size_calculator(csi_report_data::ri_type ri_, const typeII_nof_amplitudes& nof_amplitudes_) :
    ri(ri_), nof_amplitudes(nof_amplitudes_)
  {
  }

  /// The default codebook reports an error.
  unsigned operator()(const std::monostate&) const
  {
    report_error("Failed to calculate PMI size: invalid codebook configuration.");
    return {};
  }

  /// Single-antenna port reports do not contain PMI.
  unsigned operator()(const pmi_codebook_one_port&) const { return 0; }

  /// Selects the PMI codebook size for the two-antenna port configuration.
  unsigned operator()(const pmi_codebook_two_port&) const
  {
    ocudu_assert(ri <= 2, "Invalid rank indicator (i.e., {}).", ri);
    if (ri == 2) {
      return 1;
    }

    return 2;
  }

  /// Calculates the PMI field size for Type I Single-panel codebooks.
  unsigned operator()(const pmi_codebook_typeI_single_panel& codebook) const
  {
    ocudu_assert(codebook.mode == pmi_codebook_typeI_mode::one, "Only mode 1 is currently supported.");

    unsigned count = 0;

    pmi_typeI_single_panel_param_sizes sizes =
        get_pmi_sizes_typeI_single_panel(get_single_panel_info(codebook.n1_n2), ri.value());

    count += sizes.i_1_1;
    count += sizes.i_1_2;
    count += sizes.i_1_3;
    count += sizes.i_2;

    return count;
  }

  /// Calculates the PMI field size for the Type II codebook.
  unsigned operator()(const pmi_codebook_typeII& codebook) const
  {
    ocudu_assert(nof_amplitudes.size() == ri.value(),
                 "The number of reported non-zero wideband amplitude counts (i.e., {}) does not match the rank "
                 "indicator (i.e., {}).",
                 nof_amplitudes.size(),
                 ri);

    pmi_typeII_param_sizes sizes = get_pmi_sizes_typeII(codebook, nof_amplitudes);

    // Non-reported fields are zero bit-width.
    return sizes.i_1_1 + sizes.i_1_2 + sizes.i_1_3_1 + sizes.i_1_4_1 + sizes.i_2_1_1 + sizes.i_2_2_1 + sizes.i_1_3_2 +
           sizes.i_1_4_2 + sizes.i_2_1_2 + sizes.i_2_2_2;
  }
};

/// Precoding Matrix Indicator (PMI) unpacking helper.
struct pmi_unpacker {
  /// Reference to the packed CSI Report.
  const csi_report_packed& packed;
  /// Unpacked Rank Indicator (RI).
  csi_report_data::ri_type ri;
  /// Number of non-zero wideband amplitude coefficients per layer, only used by the Type II codebook.
  const typeII_nof_amplitudes& nof_amplitudes;

  /// The unpacker requires the input packed bits and the Rank Indicator (RI).
  pmi_unpacker(const csi_report_packed&     packed_,
               csi_report_data::ri_type     ri_,
               const typeII_nof_amplitudes& nof_amplitudes_) :
    packed(packed_), ri(ri_), nof_amplitudes(nof_amplitudes_)
  {
  }

  /// The default codebook reports an error.
  precoding_matrix_indicator operator()(const std::monostate&) const
  {
    report_error("Failed to unpack PMI: invalid codebook configuration.");
    return {};
  }

  /// PMI is omitted present for single-antenna port.
  precoding_matrix_indicator operator()(const pmi_codebook_one_port&) const { return {}; }

  /// Unpacks the PMI for the two-antenna-port configuration.
  precoding_matrix_indicator operator()(const pmi_codebook_two_port&) const
  {
    pmi_two_antenna_port result;
    result.pmi = packed.extract(0, packed.size());
    return precoding_matrix_indicator{result};
  }

  /// Unpack the PMI for Type I Single-panel codebook configuration.
  precoding_matrix_indicator operator()(const pmi_codebook_typeI_single_panel& codebook) const
  {
    ocudu_assert(codebook.mode == pmi_codebook_typeI_mode::one, "Only mode 1 is currently supported.");

    unsigned                             count      = 0;
    const pmi_codebook_single_panel_info panel_info = get_single_panel_info(codebook.n1_n2);
    pmi_typeI_single_panel_param_sizes   sizes      = get_pmi_sizes_typeI_single_panel(panel_info, ri.value());

    uint8_t i_1_1 = packed.extract(count, sizes.i_1_1);
    count += sizes.i_1_1;

    std::optional<uint8_t> i_1_2;
    if (sizes.i_1_2 > 0) {
      i_1_2 = packed.extract(count, sizes.i_1_2);
      count += sizes.i_1_2;
    }

    std::optional<uint8_t> i_1_3;
    if (sizes.i_1_3 > 0) {
      i_1_3 = packed.extract(count, sizes.i_1_3);
      count += sizes.i_1_3;
    }

    uint8_t i_2 = packed.extract(count, sizes.i_2);
    count += sizes.i_2;

    ocudu_assert(packed.size() == count,
                 "Packed input size (i.e., {}) does not match with the fields size (i.e., {})",
                 packed.size(),
                 count);

    return pmi_typeI_single_panel{.panel_config = codebook, .i_1_1 = i_1_1, .i_1_2 = i_1_2, .i_1_3 = i_1_3, .i_2 = i_2};
  }

  /// Unpacks the PMI for the Type II codebook configuration.
  precoding_matrix_indicator operator()(const pmi_codebook_typeII& codebook) const
  {
    unsigned nof_layers = ri.value();
    ocudu_assert(nof_amplitudes.size() == nof_layers,
                 "The number of reported non-zero wideband amplitude counts (i.e., {}) does not match the rank "
                 "indicator (i.e., {}).",
                 nof_amplitudes.size(),
                 nof_layers);

    unsigned                     nof_beams        = codebook.nof_beams.value();
    unsigned                     nof_coefficients = 2 * nof_beams;
    unsigned                     nof_phase_bits   = log2_ceil(static_cast<unsigned>(codebook.phase_alphabet_size));
    const pmi_typeII_param_sizes sizes            = get_pmi_sizes_typeII(codebook, nof_amplitudes);

    pmi_typeII result;
    result.config = codebook;
    result.layers.resize(nof_layers);

    unsigned count = 0;

    // Beam rotation and beam group selection.
    result.i_1_1 = 0;
    if (sizes.i_1_1 > 0) {
      result.i_1_1 = packed.extract(count, sizes.i_1_1);
      count += sizes.i_1_1;
    }

    result.i_1_2 = 0;
    if (sizes.i_1_2 > 0) {
      result.i_1_2 = packed.extract(count, sizes.i_1_2);
      count += sizes.i_1_2;
    }

    // Type II wideband amplitude index \f$k^{(1)}_{l,i}\f$ that corresponds to a unit amplitude.
    static constexpr uint8_t max_typeII_wideband_amplitude_index = 7;

    // Strongest coefficient and wideband amplitudes of each layer.
    const std::array<unsigned, max_nof_typeII_layers> i_1_3_sizes = {sizes.i_1_3_1, sizes.i_1_3_2};
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      pmi_typeII::layer_coefficients& layer = result.layers[i_layer];

      layer.i_1_3 = packed.extract(count, i_1_3_sizes[i_layer]);
      count += i_1_3_sizes[i_layer];
      ocudu_assert(layer.i_1_3 < nof_coefficients,
                   "The strongest coefficient index (i.e., {}) exceeds the number of coefficients (i.e., {}).",
                   layer.i_1_3,
                   nof_coefficients);

      // The wideband amplitude of the strongest coefficient is not reported, it is set to the maximum value.
      layer.i_1_4.assign(nof_coefficients, max_typeII_wideband_amplitude_index);
      for (unsigned i_coefficient = 0; i_coefficient != nof_coefficients; ++i_coefficient) {
        if (i_coefficient == layer.i_1_3) {
          continue;
        }
        layer.i_1_4[i_coefficient] = packed.extract(count, nof_typeII_wideband_amplitude_bits);
        count += nof_typeII_wideband_amplitude_bits;
      }
    }

    // Flag the coefficients of each layer that carry a full resolution phase.
    std::array<std::array<bool, max_nof_typeII_coefficients>, max_nof_typeII_layers> is_full_res = {};
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      const pmi_typeII::layer_coefficients& layer = result.layers[i_layer];

      // Sort the layer coefficients in strength order. The vector contains the list of coefficient indices in
      // decreasing strength order, as per the wideband amplitude coefficient i_1_4, where the first element is the
      // strongest one, excluding i_1_3. Coefficients with identical wideband amplitudes keep the increasing coefficient
      // index order, as per TS38.214 Section 5.2.2.2.3.
      static_vector<uint8_t, max_nof_typeII_coefficients> order =
          get_typeII_coefficient_strength_order(layer.i_1_4, layer.i_1_3);
      ocudu_assert(order.size() + 1 == nof_amplitudes[i_layer],
                   "The number of unpacked non-zero wideband amplitudes (i.e., {}) does not match the number reported "
                   "in CSI Part 1 (i.e., {}).",
                   order.size() + 1,
                   nof_amplitudes[i_layer]);

      // Without subband amplitude reporting, all the reported coefficients carry a full resolution phase.
      unsigned nof_full_res = order.size();
      if (codebook.subband_amplitude) {
        nof_full_res = std::min<unsigned>(nof_amplitudes[i_layer], get_typeII_nof_full_res_coefficients(nof_beams)) - 1;
      }

      for (unsigned i_strongest = 0; i_strongest != nof_full_res; ++i_strongest) {
        is_full_res[i_layer][order[i_strongest]] = true;
      }
    }

    // Phases of each layer. The phase indices that are not reported are set to zero.
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      pmi_typeII::layer_coefficients& layer = result.layers[i_layer];
      layer.i_2_1.assign(nof_coefficients, 0);

      for (unsigned i_coefficient = 0; i_coefficient != nof_coefficients; ++i_coefficient) {
        // The phase of the strongest coefficient is fixed to zero and the phases of the coefficients with a zero
        // wideband amplitude are not reported.
        if ((i_coefficient == layer.i_1_3) || (layer.i_1_4[i_coefficient] == 0)) {
          continue;
        }

        unsigned nof_bits          = is_full_res[i_layer][i_coefficient] ? nof_phase_bits : nof_typeII_qpsk_phase_bits;
        layer.i_2_1[i_coefficient] = packed.extract(count, nof_bits);
        count += nof_bits;
      }
    }

    // Subband amplitudes of each layer, if enabled. The amplitudes that are not reported are set to the maximum value.
    if (codebook.subband_amplitude) {
      // Type II subband amplitude index \f$k^{(2)}_{l,i}\f$ that corresponds to a unit amplitude.
      static constexpr uint8_t max_typeII_subband_amplitude_index = 1;

      for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
        pmi_typeII::layer_coefficients& layer = result.layers[i_layer];
        layer.i_2_2.assign(nof_coefficients, max_typeII_subband_amplitude_index);

        for (unsigned i_coefficient = 0; i_coefficient != nof_coefficients; ++i_coefficient) {
          if (!is_full_res[i_layer][i_coefficient]) {
            continue;
          }
          layer.i_2_2[i_coefficient] = packed.extract(count, 1);
          ++count;
        }
      }
    }

    ocudu_assert(packed.size() == count,
                 "Packed input size (i.e., {}) does not match with the fields size (i.e., {})",
                 packed.size(),
                 count);

    return result;
  }
};

} // namespace

ri_li_cqi_cri_sizes ocudu::get_ri_li_cqi_cri_sizes(const pmi_codebook_config& pmi_codebook,
                                                   const ri_restriction_type& ri_restriction,
                                                   csi_report_data::ri_type   ri,
                                                   unsigned                   nof_csi_rs_resources)
{
  // Calculate CRI field size. The number of CSI resources in the corresponding resource set must be at least one and up
  // to 64 (see TS38.331 Section 6.3.2, Information Element \c NZP-CSI-RS-ResourceSet).
  constexpr interval<unsigned, true> nof_csi_res_range(1, 64);
  ocudu_assert(nof_csi_res_range.contains(nof_csi_rs_resources),
               "The number of CSI-RS resources in the resource set, i.e., {} exceeds the valid range {}.",
               nof_csi_rs_resources,
               nof_csi_res_range);

  return std::visit(ri_li_cqi_cri_size_calculator{ri_restriction, ri, nof_csi_rs_resources}, pmi_codebook);
}

cri_ssbri_rsrp_sizes ocudu::get_cri_ssbri_rsrp_sizes(unsigned nof_csi_rs_resources)
{
  return {.cri = log2_ceil(nof_csi_rs_resources), .rsrp = 7, .diff_rsrp = 4};
}

csi_report_size ocudu::get_csi_report_size_cri_ssbri_rsrp(unsigned                       nof_csi_rs_resources,
                                                          bounded_integer<uint8_t, 1, 4> nof_reported_rs_)
{
  using namespace units::literals;
  cri_ssbri_rsrp_sizes sizes           = get_cri_ssbri_rsrp_sizes(nof_csi_rs_resources);
  unsigned             nof_reported_rs = nof_reported_rs_.value();

  // The CSI report comprises a CRI/SSBRI field per Resource Set (RS) - the RSRP for the first entry and
  // differential RSRP for the rest of RS, as per TS38.214 Section 5.2.1.4.3.
  units::bits part1_size{nof_reported_rs * sizes.cri + sizes.rsrp + sizes.diff_rsrp * (nof_reported_rs - 1)};
  return {.part1_size = part1_size, .part2_correspondence = {}, .part2_min_size = 0_bits, .part2_max_size = 0_bits};
}

unsigned ocudu::csi_report_get_size_pmi(const pmi_codebook_config&   codebook,
                                        csi_report_data::ri_type     ri,
                                        const typeII_nof_amplitudes& nof_amplitudes)
{
  return std::visit(pmi_size_calculator{ri, nof_amplitudes}, codebook);
}

csi_report_data::wideband_cqi_type ocudu::csi_report_unpack_wideband_cqi(csi_report_packed packed)
{
  ocudu_assert(packed.size() == 4, "Packed size (i.e., {}) must be 4 bits.", packed.size());
  return packed.extract(0, 4);
}

precoding_matrix_indicator ocudu::csi_report_unpack_pmi(const csi_report_packed&     packed,
                                                        const pmi_codebook_config&   codebook,
                                                        csi_report_data::ri_type     ri,
                                                        const typeII_nof_amplitudes& nof_amplitudes)
{
  return std::visit(pmi_unpacker{packed, ri, nof_amplitudes}, codebook);
}

csi_report_data::ri_type ocudu::csi_report_unpack_ri(const csi_report_packed&   ri_packed,
                                                     const ri_restriction_type& ri_restriction)
{
  unsigned ri = 0;

  if (!ri_packed.empty()) {
    ri = ri_packed.extract(0, ri_packed.size());
  }

  if (!ri_restriction.empty()) {
    ocudu_assert(ri < ri_restriction.count(),
                 "Packed RI, i.e., {}, is out of bounds given the number of allowed rank values, i.e., {}.",
                 ri,
                 ri_restriction.count());

    return ri_restriction.get_bit_positions()[ri] + 1;
  }

  return 1;
}

csi_report_data ocudu::csi_report_unpack_cri_ssbri_rsrp(const csi_report_packed&       packed,
                                                        unsigned                       nof_csi_rs_resources,
                                                        bounded_integer<uint8_t, 1, 4> nof_reported_rs_)
{
  cri_ssbri_rsrp_sizes sizes           = get_cri_ssbri_rsrp_sizes(nof_csi_rs_resources);
  unsigned             nof_reported_rs = nof_reported_rs_.value();

  csi_report_data result;
  unsigned        offset = 0;

  // Unpack each CRI for each reported resource set.
  for (unsigned i = 0; i != nof_reported_rs; ++i) {
    result.cri.push_back(packed.extract(offset, sizes.cri));
    offset += sizes.cri;
  }

  // Unpack RSRP. The RSRP value is expressed in 1 dB steps from [-140, -44] dBm with 7-bit bitwidth, as per TS38.214
  // Section 5.2.1.4.3.
  int reference_rsrp = -140 + packed.extract(offset, sizes.rsrp);
  result.rsrp_dBm.push_back(reference_rsrp);
  offset += sizes.rsrp;

  // Unpack differential RSRP for each reported resource set. Conversion from the 4-bit field to dBm is described in
  // TS38.214 Section 5.2.1.4.3.
  for (unsigned i = 1; i != nof_reported_rs; ++i) {
    result.rsrp_dBm.push_back(reference_rsrp - 2 * packed.extract(offset, sizes.diff_rsrp));
    offset += sizes.diff_rsrp;
  }

  // Validate all packed bits have been used.
  ocudu_assert(packed.size() == offset,
               "Packet input size (i.e., {}) does not match with the fields size (i.e., {})",
               packed.size(),
               offset);
  result.valid = true;
  return result;
}
