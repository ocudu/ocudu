// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/adt/format.h"
#include "ocudu/ran/csi_report/csi_report_configuration.h"
#include "ocudu/ran/csi_report/csi_report_formatters.h"
#include "ocudu/ran/csi_report/csi_report_on_pucch_helpers.h"
#include "ocudu/ran/csi_report/csi_report_on_pusch_helpers.h"
#include "ocudu/ran/precoding/precoding_codebook_type1_helpers.h"
#include "ocudu/ran/precoding/precoding_codebook_type2_helpers.h"
#include "ocudu/ran/uci/uci_part2_size_calculator.h"
#include "ocudu/support/math/math_utils.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>
#include <random>

using namespace ocudu;

namespace ocudu {

auto to_tuple(const csi_report_data& data)
{
  return std::tie(data.cri, data.ri, data.li, data.pmi, data.first_tb_wideband_cqi, data.second_tb_wideband_cqi);
}

bool operator==(const pmi_typeII::layer_coefficients& left, const pmi_typeII::layer_coefficients& right)
{
  return (left.i_1_3 == right.i_1_3) &&
         std::equal(left.i_1_4.begin(), left.i_1_4.end(), right.i_1_4.begin(), right.i_1_4.end()) &&
         std::equal(left.i_2_1.begin(), left.i_2_1.end(), right.i_2_1.begin(), right.i_2_1.end()) &&
         std::equal(left.i_2_2.begin(), left.i_2_2.end(), right.i_2_2.begin(), right.i_2_2.end());
}

bool operator==(const precoding_matrix_indicator& left, const precoding_matrix_indicator& right)
{
  if (std::holds_alternative<pmi_two_antenna_port>(left) && std::holds_alternative<pmi_two_antenna_port>(right)) {
    pmi_two_antenna_port left2  = std::get<pmi_two_antenna_port>(left);
    pmi_two_antenna_port right2 = std::get<pmi_two_antenna_port>(right);
    return left2.pmi == right2.pmi;
  }
  if (std::holds_alternative<pmi_typeI_single_panel>(left) && std::holds_alternative<pmi_typeI_single_panel>(right)) {
    pmi_typeI_single_panel left2  = std::get<pmi_typeI_single_panel>(left);
    pmi_typeI_single_panel right2 = std::get<pmi_typeI_single_panel>(right);
    return (left2.i_1_1 == right2.i_1_1) && (left2.i_1_2 == right2.i_1_2) && (left2.i_1_3 == right2.i_1_3) &&
           (left2.i_2 == right2.i_2);
  }
  if (std::holds_alternative<pmi_typeII>(left) && std::holds_alternative<pmi_typeII>(right)) {
    const pmi_typeII& left2  = std::get<pmi_typeII>(left);
    const pmi_typeII& right2 = std::get<pmi_typeII>(right);
    return (left2.i_1_1 == right2.i_1_1) && (left2.i_1_2 == right2.i_1_2) &&
           std::equal(left2.layers.begin(), left2.layers.end(), right2.layers.begin(), right2.layers.end());
  }

  return false;
}

bool operator==(const csi_report_data& left, const csi_report_data& right)
{
  return to_tuple(left) == to_tuple(right);
}

bool operator==(const uci_part2_size_description::parameter left, const uci_part2_size_description::parameter right)
{
  return (left.offset == right.offset) && (left.width == right.width);
}

bool operator==(const uci_part2_size_description::entry left, const uci_part2_size_description::entry right)
{
  if (!std::equal(left.parameters.begin(), left.parameters.end(), right.parameters.begin(), right.parameters.end())) {
    return false;
  }

  return std::equal(left.map.begin(), left.map.end(), right.map.begin(), right.map.end());
}

bool operator==(const uci_part2_size_description& left, const uci_part2_size_description& right)
{
  return std::equal(left.entries.begin(), left.entries.end(), right.entries.begin(), right.entries.end());
}

bool operator!=(const uci_part2_size_description& left, const uci_part2_size_description& right)
{
  return !(left == right);
}

bool operator==(const csi_report_size& left, const csi_report_size& right)
{
  if (left.part1_size != right.part1_size) {
    return false;
  }

  if (left.part2_correspondence != right.part2_correspondence) {
    return false;
  }

  if (left.part2_min_size != right.part2_min_size) {
    return false;
  }

  if (left.part2_max_size != right.part2_max_size) {
    return false;
  }

  return true;
}

std::ostream& operator<<(std::ostream& os, const csi_report_size& data)
{
  fmt::print(os, "{}", data);
  return os;
}

std::ostream& operator<<(std::ostream& os, csi_report_data data)
{
  fmt::print(os, "{}", data);
  return os;
}

std::ostream& operator<<(std::ostream& os, units::bits data)
{
  fmt::print(os, "{}", data);
  return os;
}

std::ostream& operator<<(std::ostream& os, const pmi_codebook_config& codebook)
{
  fmt::print(os, "{}", to_string(codebook));
  return os;
}

std::ostream& operator<<(std::ostream& os, const pmi_codebook_typeII& codebook)
{
  fmt::print(os, "{}", to_string(codebook));
  return os;
}

std::ostream& operator<<(std::ostream& os, csi_report_quantities quantities)
{
  fmt::print(os, "{}", to_string(quantities));
  return os;
}

} // namespace ocudu

std::ostream& operator<<(std::ostream& os, const pmi_codebook_config& codebook)
{
  fmt::print(os, "{}", to_string(codebook));
  return os;
}

namespace {

using Repetitions = unsigned;

using csi_report_size_params = std::tuple<pmi_codebook_config, csi_report_quantities, Repetitions>;

class CsiReportPuschFixture : public ::testing::TestWithParam<csi_report_size_params>
{
protected:
  csi_report_configuration configuration            = {};
  csi_report_size          expected_csi_report_size = {};
  csi_report_data          expected_unpacked_data;
  csi_report_packed        csi1_packed;
  csi_report_packed        csi2_packed;

  void SetUp() override
  {
    const pmi_codebook_config&   pmi_codebook = std::get<0>(GetParam());
    const csi_report_quantities& quantities   = std::get<1>(GetParam());

    unsigned nof_csi_rs_antenna_ports = get_precoding_codebook_antenna_ports(pmi_codebook);

    // The maximum rank is limited by the number of CSI-RS ports and by the codebook type, since the Type II codebook
    // never reports a rank above two, as per TS38.214 Section 5.2.2.2.3.
    unsigned max_rank = get_precoding_codebook_max_rank(pmi_codebook);

    // Prepare CSI report configuration.
    configuration.nof_csi_rs_resources = nof_csi_rs_resources_dist(rgen);
    configuration.nof_reported_rs      = 1;
    configuration.pmi_codebook         = pmi_codebook;
    configuration.ri_restriction       = ~ri_restriction_type(max_rank);
    configuration.quantities           = quantities;

    if (configuration.ri_restriction.count() > 2) {
      // Set a random RI restriction element to false.
      std::uniform_int_distribution<unsigned> ri_restriction_dist(1, max_rank - 1);
      configuration.ri_restriction.set(ri_restriction_dist(rgen), false);
    } else if (max_rank == max_nof_typeII_layers) {
      // Forbid the two layer reporting for some of the repetitions.
      configuration.ri_restriction.set(1, rank_restriction_dist(rgen) != 0);
    }

    // Fill CRI and calculate CRI size if enabled.
    unsigned cri_size = 0;
    if (configuration.quantities < csi_report_quantities::other) {
      cri_size = fill_cri(csi1_packed, expected_unpacked_data, configuration);
    }

    // Fill RI and calculate RI size if enabled.
    unsigned ri_size = 0;
    if (configuration.quantities < csi_report_quantities::other) {
      ri_size = fill_ri(csi1_packed, expected_unpacked_data, configuration);
    }

    // Fill wideband CQI for the first TB and calculated its size if enabled.
    unsigned wideband_cqi_1st_tb_size = 0;
    if ((quantities == csi_report_quantities::cri_ri_pmi_cqi) || (quantities == csi_report_quantities::cri_ri_cqi) ||
        (quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
      wideband_cqi_1st_tb_size = fill_wideband_cqi_1st_tb(csi1_packed, expected_unpacked_data, configuration);
    }

    // Fill Subband differential CQI for the first TB if enabled.
    // ... Not supported.

    // Fill the indicators of the number of non-zero wideband amplitude coefficients, exclusive to the Type II
    // codebook, and calculate their total size.
    typeII_nof_amplitudes nof_amplitudes;
    unsigned              nof_amplitudes_size =
        fill_nof_amplitudes(csi1_packed, nof_amplitudes, expected_unpacked_data, configuration);

    // Calculate CSI Part 1 size as described in TS38.212 Table 6.3.2.1.2-3.
    expected_csi_report_size.part1_size =
        units::bits{cri_size + ri_size + wideband_cqi_1st_tb_size + nof_amplitudes_size};

    // Skip CSI Part 2 if it is not present. The cri-RI-CQI quantity reports the wideband CQI for the second TB in CSI
    // Part 2 when more than four CSI-RS ports are configured.
    const bool has_part2_content = (quantities == csi_report_quantities::cri_ri_li_pmi_cqi) ||
                                   (quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
                                   ((quantities == csi_report_quantities::cri_ri_cqi) && (max_rank > 4));
    if ((nof_csi_rs_antenna_ports == 1) || !has_part2_content) {
      return;
    }

    // Fill wideband CQI for the second TB if enabled and calculate its possible sizes.
    std::vector<unsigned> wideband_cqi_2nd_tb_size = {};
    if ((max_rank > 4) &&
        ((quantities == csi_report_quantities::cri_ri_pmi_cqi) || (quantities == csi_report_quantities::cri_ri_cqi) ||
         (quantities == csi_report_quantities::cri_ri_li_pmi_cqi))) {
      fill_wideband_cqi_2nd_tb(csi2_packed, expected_unpacked_data, configuration);

      for (unsigned i_nof_layers = 1; i_nof_layers <= max_rank; ++i_nof_layers) {
        wideband_cqi_2nd_tb_size.emplace_back(get_second_tb_wideband_cqi_size(i_nof_layers));
      }
    }

    // Fill LI and calculate its possible sizes if enabled.
    std::vector<unsigned> li_size = {};
    if (quantities == csi_report_quantities::cri_ri_li_pmi_cqi) {
      fill_li(csi2_packed, expected_unpacked_data, configuration);

      for (unsigned i_nof_layers = 1; i_nof_layers <= max_rank; ++i_nof_layers) {
        li_size.emplace_back(get_li_size(configuration, i_nof_layers));
      }
    }

    // Fill PMI if enabled.
    const bool has_pmi = (quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
                         (quantities == csi_report_quantities::cri_ri_li_pmi_cqi);
    if (has_pmi) {
      fill_pmi(csi2_packed, expected_unpacked_data, configuration, nof_amplitudes);
    }

    // Select CSI report entry.
    uci_part2_size_description::entry& entry = expected_csi_report_size.part2_correspondence.entries.emplace_back();

    // Select CSI Part 1 parameter and setup for matching RI.
    uci_part2_size_description::parameter& parameter = entry.parameters.emplace_back();
    parameter.offset                                 = cri_size;
    parameter.width                                  = ri_size;

    // Setup one CSI Part 1 parameter per indicator of the number of non-zero wideband amplitude coefficients - used by
    // Type II codebook, transparent for Type I.
    unsigned indicator_width = get_nof_amplitudes_indicator_size(configuration);
    for (unsigned i_layer = 0, nof_indicators = get_nof_amplitudes_indicators(configuration); i_layer != nof_indicators;
         ++i_layer) {
      uci_part2_size_description::parameter& indicator_parameter = entry.parameters.emplace_back();
      indicator_parameter.offset = cri_size + ri_size + wideband_cqi_1st_tb_size + i_layer * indicator_width;
      indicator_parameter.width  = indicator_width;
    }

    // The codebooks that report no indicator of the number of non-zero wideband amplitude coefficients leave the table
    // indexed by the RI alone.
    unsigned nof_m0_values = 1U << indicator_width;
    unsigned nof_m1_values = (get_nof_amplitudes_indicators(configuration) > 1) ? nof_m0_values : 1;
    for (unsigned rank_idx = 0; rank_idx != max_rank; ++rank_idx) {
      // Skip the CSI Part 2 sizes that correspond with forbidden RI values.
      if (!configuration.ri_restriction.test(rank_idx)) {
        continue;
      }

      for (unsigned m0_value = 0; m0_value != nof_m0_values; ++m0_value) {
        for (unsigned m1_value = 0; m1_value != nof_m1_values; ++m1_value) {
          unsigned csi_part2_size = 0;

          // Add second TB wideband CQI if available.
          if (!wideband_cqi_2nd_tb_size.empty()) {
            csi_part2_size += wideband_cqi_2nd_tb_size[rank_idx];
          }

          // Add LI if available.
          if (!li_size.empty()) {
            csi_part2_size += li_size[rank_idx];
          }

          // Add PMI if available.
          if (has_pmi) {
            csi_part2_size += get_pmi_size(
                configuration, rank_idx + 1, table_nof_amplitudes(configuration, rank_idx + 1, m0_value, m1_value));
          }

          entry.map.emplace_back(csi_part2_size);
        }
      }
    }

    expected_csi_report_size.part2_min_size = units::bits(*std::min_element(entry.map.begin(), entry.map.end()));
    expected_csi_report_size.part2_max_size = units::bits(*std::max_element(entry.map.begin(), entry.map.end()));
  }

private:
  static unsigned get_cri_size(const csi_report_configuration& config)
  {
    return log2_ceil(config.nof_csi_rs_resources);
  }

  static unsigned get_li_size(const csi_report_configuration& config, unsigned nof_layers)
  {
    if (std::holds_alternative<pmi_codebook_two_port>(config.pmi_codebook)) {
      return log2_ceil(nof_layers);
    }

    if (std::holds_alternative<pmi_codebook_typeI_single_panel>(config.pmi_codebook) ||
        std::holds_alternative<pmi_codebook_typeII>(config.pmi_codebook)) {
      return std::min(2U, log2_ceil(nof_layers));
    }

    return 0;
  }

  /// Bit-width of one indicator of the number of non-zero wideband amplitude coefficients, as per TS38.212
  /// Table 6.3.1.1.2-5. It is exclusive to the Type II codebook.
  static unsigned get_nof_amplitudes_indicator_size(const csi_report_configuration& config)
  {
    const auto* codebook = std::get_if<pmi_codebook_typeII>(&config.pmi_codebook);
    if (codebook == nullptr) {
      return 0;
    }

    return log2_ceil(2 * codebook->nof_beams.value() - 1);
  }

  /// \brief Number of indicators of the number of non-zero wideband amplitude coefficients present in CSI Part 1.
  ///
  /// One indicator per layer for which the PMI can be reported, as per TS38.212 Table 6.3.2.1.2-3.
  static unsigned get_nof_amplitudes_indicators(const csi_report_configuration& config)
  {
    if (!std::holds_alternative<pmi_codebook_typeII>(config.pmi_codebook) ||
        ((config.quantities != csi_report_quantities::cri_ri_pmi_cqi) &&
         (config.quantities != csi_report_quantities::cri_ri_li_pmi_cqi))) {
      return 0;
    }

    return (config.ri_restriction.find_highest() >= 1) ? max_nof_typeII_layers : 1;
  }

  /// Number of non-zero wideband amplitude coefficients of each reported layer for a pair of indicator values.
  static typeII_nof_amplitudes
  table_nof_amplitudes(const csi_report_configuration& config, unsigned ri, unsigned m0_value, unsigned m1_value)
  {
    typeII_nof_amplitudes result;

    const auto* codebook = std::get_if<pmi_codebook_typeII>(&config.pmi_codebook);
    if (codebook == nullptr) {
      return result;
    }

    // The indicators report the number of non-zero wideband amplitude coefficients minus one.
    unsigned nof_coefficients = 2 * codebook->nof_beams.value();
    result.push_back(std::min(m0_value + 1, nof_coefficients));
    if (ri == 2) {
      result.push_back(std::min(m1_value + 1, nof_coefficients));
    }

    return result;
  }

  /// Number of beam group combinations C(N1N2,L), as per TS38.214 Table 5.2.2.2.3-1.
  static unsigned get_nof_beam_group_combinations(unsigned nof_beam_groups, unsigned nof_beams)
  {
    unsigned result = 1;
    for (unsigned i = 0; i != nof_beams; ++i) {
      result = result * (nof_beam_groups - i) / (i + 1);
    }
    return result;
  }

  static unsigned get_pmi_size(std::monostate, unsigned, const typeII_nof_amplitudes&) { return 0; }

  static unsigned get_pmi_size(pmi_codebook_one_port, unsigned, const typeII_nof_amplitudes&) { return 0; }

  static unsigned get_pmi_size(pmi_codebook_two_port, unsigned ri, const typeII_nof_amplitudes&)
  {
    return (ri == 1) ? 2 : 1;
  }

  static unsigned
  get_pmi_size(const pmi_codebook_typeI_single_panel& codebook, unsigned ri, const typeII_nof_amplitudes&)
  {
    const pmi_codebook_single_panel_info&    panel_info = get_single_panel_info(codebook.n1_n2);
    const pmi_typeI_single_panel_param_sizes sizes      = get_pmi_sizes_typeI_single_panel(panel_info, ri);
    return sizes.i_1_1 + sizes.i_1_2 + sizes.i_1_3 + sizes.i_2;
  }

  /// Type II PMI field bit-width, as per TS38.212 Table 6.3.2.1.2-1.
  static unsigned
  get_pmi_size(const pmi_codebook_typeII& codebook, unsigned, const typeII_nof_amplitudes& nof_amplitudes)
  {
    const pmi_codebook_single_panel_info& panel_info = get_single_panel_info(codebook.n1_n2);

    unsigned nof_beams        = codebook.nof_beams.value();
    unsigned nof_coefficients = 2 * nof_beams;
    unsigned nof_phase_bits   = log2_ceil(static_cast<unsigned>(codebook.phase_alphabet_size));

    // Wideband information fields, common to all the reported layers.
    unsigned size = log2_ceil(panel_info.o1 * panel_info.o2) +
                    log2_ceil(get_nof_beam_group_combinations(panel_info.n1 * panel_info.n2, nof_beams));

    for (uint8_t nof_amplitudes_layer : nof_amplitudes) {
      // Strongest coefficient indicator, plus a three-bit wideband amplitude for each coefficient other than the
      // strongest one.
      size += log2_ceil(nof_coefficients) + 3 * (nof_coefficients - 1);

      if (!codebook.subband_amplitude) {
        // All the reported coefficients other than the strongest one carry a full resolution phase.
        size += (nof_amplitudes_layer - 1) * nof_phase_bits;
        continue;
      }

      // The strongest coefficients carry a full resolution phase and an amplitude, while the weakest non-zero ones
      // carry a QPSK phase.
      unsigned nof_full_res = std::min<unsigned>(nof_amplitudes_layer, get_typeII_nof_full_res_coefficients(nof_beams));
      size += (nof_full_res - 1) * nof_phase_bits + 2 * (nof_amplitudes_layer - nof_full_res);
      size += nof_full_res - 1;
    }

    return size;
  }

  static unsigned
  get_pmi_size(const csi_report_configuration& config, unsigned ri, const typeII_nof_amplitudes& nof_amplitudes)
  {
    return std::visit([ri, &nof_amplitudes](const auto& item) { return get_pmi_size(item, ri, nof_amplitudes); },
                      config.pmi_codebook);
  }

  static unsigned get_first_tb_wideband_cqi_size() { return 4; }

  static unsigned get_second_tb_wideband_cqi_size(unsigned ri) { return (ri > 4) ? 4 : 0; }

  static unsigned fill_cri(csi_report_packed& packed, csi_report_data& unpacked, const csi_report_configuration& config)
  {
    unsigned nof_cri_bits = get_cri_size(config);

    unsigned cri = rgen() & mask_lsb_ones<unsigned>(nof_cri_bits);
    unpacked.cri.push_back(cri);

    if (nof_cri_bits > 0) {
      packed.push_back(cri, nof_cri_bits);
    }

    return nof_cri_bits;
  }

  static unsigned fill_ri(csi_report_packed& packed, csi_report_data& unpacked, const csi_report_configuration& config)
  {
    unsigned nof_ri                   = static_cast<unsigned>(config.ri_restriction.count());
    unsigned nof_csi_rs_antenna_ports = get_precoding_codebook_antenna_ports(config.pmi_codebook);

    unsigned nof_ri_bits = 0;
    if (std::holds_alternative<pmi_codebook_two_port>(config.pmi_codebook) ||
        std::holds_alternative<pmi_codebook_typeII>(config.pmi_codebook)) {
      nof_ri_bits = std::min(1U, log2_ceil(nof_ri));
    } else if (std::holds_alternative<pmi_codebook_typeI_single_panel>(config.pmi_codebook)) {
      nof_ri_bits = (nof_csi_rs_antenna_ports == 4) ? std::min(2U, log2_ceil(nof_ri)) : log2_ceil(nof_ri);
    }

    // Create a uniform distribution to select a random rank index.
    std::uniform_int_distribution<unsigned> rank_idx_dist(0, nof_ri - 1);
    unsigned                                rank_idx = rank_idx_dist(rgen);

    // Select a random rank from the allowed options given by the RI restriction bitset (see TS38.214
    // Section 5.2.2.2.1.).
    unsigned rank = config.ri_restriction.get_bit_positions()[rank_idx] + 1;

    // The unpacked RI value indicates the chosen rank.
    unpacked.ri.emplace(rank);

    // The packed RI value indicates the chosen rank index (see TS38.212 Section 6.3.1.1.2.).
    packed.push_back(rank_idx, nof_ri_bits);

    return nof_ri_bits;
  }

  static void fill_li(csi_report_packed& packed, csi_report_data& unpacked, const csi_report_configuration& config)
  {
    unsigned nof_layers  = unpacked.ri.value().value();
    unsigned nof_li_bits = get_li_size(config, nof_layers);

    unsigned li = (rgen() & mask_lsb_ones<unsigned>(nof_li_bits));
    unpacked.li.emplace(li);

    if (nof_li_bits > 0) {
      packed.push_back(li, nof_li_bits);
    }
  }

  /// Fills the indicators of the number of non-zero wideband amplitude coefficients. They are exclusive to the Type II
  /// codebook.
  static unsigned fill_nof_amplitudes(csi_report_packed&              packed,
                                      typeII_nof_amplitudes&          nof_amplitudes,
                                      const csi_report_data&          unpacked,
                                      const csi_report_configuration& config)
  {
    unsigned nof_indicators = get_nof_amplitudes_indicators(config);
    if (nof_indicators == 0) {
      return 0;
    }

    unsigned nof_layers       = unpacked.ri.value().value();
    unsigned indicator_size   = get_nof_amplitudes_indicator_size(config);
    unsigned nof_coefficients = 2 * std::get<pmi_codebook_typeII>(config.pmi_codebook).nof_beams.value();

    std::uniform_int_distribution<unsigned> nof_amplitudes_dist(1, nof_coefficients);

    for (unsigned i_layer = 0; i_layer != nof_indicators; ++i_layer) {
      unsigned nof_amplitudes_layer = (i_layer < nof_layers) ? nof_amplitudes_dist(rgen) : 1;
      packed.push_back(nof_amplitudes_layer - 1, indicator_size);

      if (i_layer < nof_layers) {
        nof_amplitudes.push_back(nof_amplitudes_layer);
      }
    }

    return nof_indicators * indicator_size;
  }

  /// Fills the Type II PMI fields in TS38.212 Table 6.3.2.1.2-1 order, from left to right.
  static void fill_pmi_typeII(csi_report_packed&           packed,
                              csi_report_data&             unpacked,
                              const pmi_codebook_typeII&   codebook,
                              const typeII_nof_amplitudes& nof_amplitudes)
  {
    const pmi_codebook_single_panel_info& panel_info = get_single_panel_info(codebook.n1_n2);

    unsigned nof_layers       = nof_amplitudes.size();
    unsigned nof_beams        = codebook.nof_beams.value();
    unsigned nof_coefficients = 2 * nof_beams;
    unsigned nof_phase_bits   = log2_ceil(static_cast<unsigned>(codebook.phase_alphabet_size));
    unsigned i_1_1_size       = log2_ceil(panel_info.o1 * panel_info.o2);
    unsigned i_1_2_size       = log2_ceil(get_nof_beam_group_combinations(panel_info.n1 * panel_info.n2, nof_beams));

    pmi_typeII type;
    type.config = codebook;
    type.i_1_1  = rgen() & mask_lsb_ones<unsigned>(i_1_1_size);
    type.i_1_2  = rgen() & mask_lsb_ones<unsigned>(i_1_2_size);
    type.layers.resize(nof_layers);

    // Wideband information fields X1.
    if (i_1_1_size > 0) {
      packed.push_back(type.i_1_1, i_1_1_size);
    }
    if (i_1_2_size > 0) {
      packed.push_back(type.i_1_2, i_1_2_size);
    }

    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      pmi_typeII::layer_coefficients& layer = type.layers[i_layer];

      // Strongest coefficient indicator.
      layer.i_1_3 = std::uniform_int_distribution<unsigned>(0, nof_coefficients - 1)(rgen);
      packed.push_back(layer.i_1_3, log2_ceil(nof_coefficients));

      // Select the coefficients that carry a non-zero wideband amplitude, other than the strongest one.
      std::vector<unsigned> nz_coefficients;
      for (unsigned i_coefficient = 0; i_coefficient != nof_coefficients; ++i_coefficient) {
        if (i_coefficient != layer.i_1_3) {
          nz_coefficients.push_back(i_coefficient);
        }
      }

      // Shuffle the coefficient index list to randomize which coefficients are non-zero.
      std::shuffle(nz_coefficients.begin(), nz_coefficients.end(), rgen);
      nz_coefficients.resize(nof_amplitudes[i_layer] - 1);

      // Wideband amplitudes.
      layer.i_1_4.assign(nof_coefficients, 0);
      layer.i_1_4[layer.i_1_3] = 7;
      for (unsigned i_coefficient : nz_coefficients) {
        layer.i_1_4[i_coefficient] = std::uniform_int_distribution<unsigned>(1, 7)(rgen);
      }
      for (unsigned i_coefficient = 0; i_coefficient != nof_coefficients; ++i_coefficient) {
        if (i_coefficient != layer.i_1_3) {
          packed.push_back(layer.i_1_4[i_coefficient], 3);
        }
      }
    }

    // Number of coefficients reported with a full resolution phase.
    unsigned nof_full_res =
        codebook.subband_amplitude ? get_typeII_nof_full_res_coefficients(nof_beams) : nof_coefficients;

    // Coefficient fields X2: the phases of all the layers followed by the subband amplitudes of all the layers.
    std::array<std::array<bool, max_nof_typeII_coefficients>, max_nof_typeII_layers> is_full_res = {};
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      pmi_typeII::layer_coefficients& layer = type.layers[i_layer];

      // The strength order selects the coefficients reported with a full resolution phase.
      static_vector<uint8_t, max_nof_typeII_coefficients> order =
          get_typeII_coefficient_strength_order(layer.i_1_4, layer.i_1_3);
      for (unsigned i = 0, i_end = std::min<unsigned>(order.size(), nof_full_res - 1); i != i_end; ++i) {
        is_full_res[i_layer][order[i]] = true;
      }

      layer.i_2_1.assign(nof_coefficients, 0);
      for (unsigned i_coefficient = 0; i_coefficient != nof_coefficients; ++i_coefficient) {
        if ((i_coefficient == layer.i_1_3) || (layer.i_1_4[i_coefficient] == 0)) {
          continue;
        }

        // The weakest non-zero coefficients fall back to a QPSK alphabet when the subband amplitude is reported.
        unsigned nof_bits          = is_full_res[i_layer][i_coefficient] ? nof_phase_bits : 2;
        layer.i_2_1[i_coefficient] = rgen() & mask_lsb_ones<unsigned>(nof_bits);
        packed.push_back(layer.i_2_1[i_coefficient], nof_bits);
      }
    }

    if (codebook.subband_amplitude) {
      for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
        pmi_typeII::layer_coefficients& layer = type.layers[i_layer];

        // The subband amplitudes that are not reported are set to the maximum value.
        layer.i_2_2.assign(nof_coefficients, 1);
        for (unsigned i_coefficient = 0; i_coefficient != nof_coefficients; ++i_coefficient) {
          if (is_full_res[i_layer][i_coefficient]) {
            layer.i_2_2[i_coefficient] = rgen() & 1U;
            packed.push_back(layer.i_2_2[i_coefficient], 1);
          }
        }
      }
    }

    precoding_matrix_indicator pmi;
    pmi.emplace<pmi_typeII>(type);
    unpacked.pmi.emplace(pmi);
  }

  static void fill_pmi(csi_report_packed&              packed,
                       csi_report_data&                unpacked,
                       const csi_report_configuration& config,
                       const typeII_nof_amplitudes&    nof_amplitudes)
  {
    unsigned ri = unpacked.ri.value().value();

    if (std::holds_alternative<pmi_codebook_two_port>(config.pmi_codebook)) {
      unsigned nof_pmi_bits = (ri == 1) ? 2 : 1;

      pmi_two_antenna_port type;
      type.pmi = rgen() & mask_lsb_ones<unsigned>(nof_pmi_bits);

      precoding_matrix_indicator pmi;
      pmi.emplace<pmi_two_antenna_port>(type);
      unpacked.pmi.emplace(pmi);

      packed.push_back(type.pmi, nof_pmi_bits);
    } else if (std::holds_alternative<pmi_codebook_typeI_single_panel>(config.pmi_codebook)) {
      const auto&                           codebook   = std::get<pmi_codebook_typeI_single_panel>(config.pmi_codebook);
      const pmi_codebook_single_panel_info& panel_info = get_single_panel_info(codebook.n1_n2);
      const pmi_typeI_single_panel_param_sizes sizes   = get_pmi_sizes_typeI_single_panel(panel_info, ri);

      uint8_t i_1_1 = rgen() & mask_lsb_ones<unsigned>(sizes.i_1_1);
      uint8_t i_1_2 = rgen() & mask_lsb_ones<unsigned>(sizes.i_1_2);
      uint8_t i_1_3 = rgen() & mask_lsb_ones<unsigned>(sizes.i_1_3);
      uint8_t i_2   = rgen() & mask_lsb_ones<unsigned>(sizes.i_2);

      // Set PMI values.
      pmi_typeI_single_panel type{codebook,
                                  i_1_1,
                                  sizes.i_1_2 > 0 ? std::make_optional(i_1_2) : std::nullopt,
                                  sizes.i_1_3 > 0 ? std::make_optional(i_1_3) : std::nullopt,
                                  i_2};

      precoding_matrix_indicator pmi;
      pmi.emplace<pmi_typeI_single_panel>(type);
      unpacked.pmi.emplace(pmi);

      // Pack PMI values in TS38.212 Section 6.3.2.1.2 order.
      packed.push_back(i_1_1, sizes.i_1_1);
      if (sizes.i_1_2 > 0) {
        packed.push_back(i_1_2, sizes.i_1_2);
      }
      if (sizes.i_1_3 > 0) {
        packed.push_back(i_1_3, sizes.i_1_3);
      }
      packed.push_back(i_2, sizes.i_2);
    } else if (std::holds_alternative<pmi_codebook_typeII>(config.pmi_codebook)) {
      fill_pmi_typeII(packed, unpacked, std::get<pmi_codebook_typeII>(config.pmi_codebook), nof_amplitudes);
    }
  }

  static unsigned
  fill_wideband_cqi_1st_tb(csi_report_packed& packed, csi_report_data& unpacked, const csi_report_configuration& config)
  {
    unsigned wideband_cqi1_size = get_first_tb_wideband_cqi_size();
    unsigned wideband_cqi1      = rgen() & mask_lsb_ones<unsigned>(4);

    unpacked.first_tb_wideband_cqi.emplace(wideband_cqi1);
    packed.push_back(wideband_cqi1, wideband_cqi1_size);

    return wideband_cqi1_size;
  }

  static void
  fill_wideband_cqi_2nd_tb(csi_report_packed& packed, csi_report_data& unpacked, const csi_report_configuration& config)
  {
    unsigned ri                 = unpacked.ri.value().value();
    unsigned wideband_cqi2_size = get_second_tb_wideband_cqi_size(ri);

    if (wideband_cqi2_size > 0) {
      unsigned wideband_cqi2 = rgen() & mask_lsb_ones<unsigned>(4);
      unpacked.second_tb_wideband_cqi.emplace(wideband_cqi2);
      packed.push_back(wideband_cqi2, 4);
    }
  }

  static std::mt19937                            rgen;
  static std::uniform_int_distribution<unsigned> nof_csi_rs_resources_dist;
  static std::uniform_int_distribution<unsigned> rank_restriction_dist;
};

std::mt19937                            CsiReportPuschFixture::rgen;
std::uniform_int_distribution<unsigned> CsiReportPuschFixture::nof_csi_rs_resources_dist(1, 16);
std::uniform_int_distribution<unsigned> CsiReportPuschFixture::rank_restriction_dist(0, 1);

} // namespace

TEST_P(CsiReportPuschFixture, csiReportPuschSize)
{
  // Get report size.
  csi_report_size csi_report_size = get_csi_report_pusch_size(configuration);

  // Assert report size.
  ASSERT_EQ(csi_report_size, expected_csi_report_size);
}

TEST_P(CsiReportPuschFixture, csiReportPuschUnpacking)
{
  // Unpack.
  ASSERT_TRUE(validate_pusch_csi_payload(csi1_packed, csi2_packed, configuration));
  csi_report_data unpacked = (csi2_packed.empty()) ? csi_report_unpack_pusch(csi1_packed, configuration)
                                                   : csi_report_unpack_pusch(csi1_packed, csi2_packed, configuration);

  // Assert unpacked CSI information.
  ASSERT_EQ(expected_unpacked_data, unpacked);
}

INSTANTIATE_TEST_SUITE_P(
    CsiReportPuschHelpersTest,
    CsiReportPuschFixture,
    ::testing::Combine(::testing::Values(pmi_codebook_one_port{},
                                         pmi_codebook_two_port{},
                                         pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::two_one,
                                                                         pmi_codebook_typeI_mode::one},
                                         pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::two_two,
                                                                         pmi_codebook_typeI_mode::one},
                                         pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::four_one,
                                                                         pmi_codebook_typeI_mode::one},
                                         pmi_codebook_typeII{pmi_codebook_single_panel_config::two_one,
                                                             2,
                                                             pmi_codebook_typeII_phase_size::qpsk,
                                                             false},
                                         pmi_codebook_typeII{pmi_codebook_single_panel_config::two_one,
                                                             2,
                                                             pmi_codebook_typeII_phase_size::psk8,
                                                             true},
                                         pmi_codebook_typeII{pmi_codebook_single_panel_config::two_two,
                                                             3,
                                                             pmi_codebook_typeII_phase_size::qpsk,
                                                             false},
                                         pmi_codebook_typeII{pmi_codebook_single_panel_config::two_two,
                                                             4,
                                                             pmi_codebook_typeII_phase_size::psk8,
                                                             true},
                                         pmi_codebook_typeII{pmi_codebook_single_panel_config::four_one,
                                                             4,
                                                             pmi_codebook_typeII_phase_size::psk8,
                                                             false},
                                         pmi_codebook_typeII{pmi_codebook_single_panel_config::four_one,
                                                             3,
                                                             pmi_codebook_typeII_phase_size::psk8,
                                                             true}),
                       ::testing::Values(csi_report_quantities::cri_ri_pmi_cqi,
                                         csi_report_quantities::cri_ri_cqi,
                                         csi_report_quantities::cri_ri_li_pmi_cqi),
                       ::testing::Range(0U, 10U)));

// Unpacks a Type II CSI report from a fixed bit stream.
TEST(csi_report_unpacking, typeII_pusch_report_from_a_fixed_bit_stream)
{
  // Type II codebook configuration.
  pmi_codebook_config pmi_codebook =
      pmi_codebook_typeII{pmi_codebook_single_panel_config::two_one, 2, pmi_codebook_typeII_phase_size::qpsk, false};

  // The maximum rank is limited by the codebook type, since the Type II codebook never reports a rank above two, as
  // per TS38.214 Section 5.2.2.2.3.
  unsigned max_rank = get_precoding_codebook_max_rank(pmi_codebook);

  // CSI report configuration. The subband configuration is absent, i.e. the report covers zero subbands.
  csi_report_configuration configuration = {.nof_csi_rs_resources = 1,
                                            .nof_reported_rs      = 1,
                                            .pmi_codebook         = pmi_codebook,
                                            .ri_restriction       = ~ri_restriction_type(max_rank),
                                            .quantities           = csi_report_quantities::cri_ri_pmi_cqi,
                                            .subband              = std::nullopt};

  // Expected CSI Part 1 fields.
  static constexpr unsigned expected_cri          = 0;
  static constexpr unsigned expected_ri           = 1;
  static constexpr unsigned expected_wideband_cqi = 15;

  // Expected numbers of non-zero wideband amplitude coefficients reported in CSI Part 1, i.e. M_0 and M_1.
  static_vector<uint8_t, max_nof_typeII_layers> expected_nof_amplitudes = {4, 1};

  // Expected Type II PMI wideband information fields X1.
  static constexpr unsigned                           expected_i_1_1 = 0;
  static constexpr unsigned                           expected_i_1_2 = 0;
  static constexpr unsigned                           expected_i_1_3 = 3;
  static_vector<uint8_t, max_nof_typeII_coefficients> expected_i_1_4 = {1, 2, 3, 7};

  // Expected Type II PMI coefficient fields X2.
  static_vector<uint8_t, max_nof_typeII_coefficients> expected_i_2_1 = {1, 1, 0, 0};
  static_vector<uint8_t, max_nof_typeII_coefficients> expected_i_2_2 = {};

  // Load the packed CSI report bit stream.
  csi_report_packed csi_packed;
  csi_packed.push_back(0x7e194d40u, 32);
  csi_packed.push_back(0x00000000u, 32);

  // The CSI Part 1 size is given by the report configuration alone.
  csi_report_size report_size = get_csi_report_pusch_size(configuration);
  unsigned        part1_size  = report_size.part1_size.value();

  // The CSI Part 2 size is signaled by the CSI Part 1 contents, as per TS38.212 Table 6.3.2.1.2-4.
  uci_payload_type csi1_payload;
  csi1_payload.push_back(csi_packed.extract(0, part1_size), part1_size);
  unsigned part2_size = uci_part2_get_size(csi1_payload, report_size.part2_correspondence).value();

  // Split the bit stream into the two report parts.
  csi_report_packed csi1_packed = csi_packed.slice(0, part1_size);
  csi_report_packed csi2_packed = csi_packed.slice(part1_size, part1_size + part2_size);

  // Unpack.
  ASSERT_TRUE(validate_pusch_csi_payload(csi1_packed, csi2_packed, configuration));
  csi_report_data unpacked = csi_report_unpack_pusch(csi1_packed, csi2_packed, configuration);

  // Assert Part 1 size.
  ASSERT_EQ(part1_size, 9);

  // Assert the position of the indicators of the number of non-zero wideband amplitude coefficients within CSI Part 1
  // by checking the value in the expected position against the correct M_l value.
  // The first indicator M_0 comes after CRI (0 bits) + RI (1 bit) + wideband CQI (4 bits).
  static constexpr unsigned nof_amplitudes_offset = 5;
  static constexpr unsigned nof_amplitudes_size   = 2;
  ASSERT_EQ(csi1_packed.extract(nof_amplitudes_offset, nof_amplitudes_size), expected_nof_amplitudes[0] - 1);
  ASSERT_EQ(csi1_packed.extract(nof_amplitudes_offset + nof_amplitudes_size, nof_amplitudes_size),
            expected_nof_amplitudes[1] - 1);

  // Assert the CSI Part 1 fields.
  ASSERT_EQ(unpacked.cri.size(), 1);
  ASSERT_EQ(unpacked.cri.front(), expected_cri);
  ASSERT_TRUE(unpacked.ri.has_value());
  ASSERT_EQ(unpacked.ri.value().value(), expected_ri);
  ASSERT_TRUE(unpacked.first_tb_wideband_cqi.has_value());
  ASSERT_EQ(unpacked.first_tb_wideband_cqi.value().value(), expected_wideband_cqi);

  // Assert that the report carries a Type II PMI with one layer per reported rank.
  ASSERT_TRUE(unpacked.pmi.has_value());
  ASSERT_TRUE(std::holds_alternative<pmi_typeII>(unpacked.pmi.value()));
  const pmi_typeII& pmi = std::get<pmi_typeII>(unpacked.pmi.value());
  ASSERT_EQ(pmi.layers.size(), expected_ri);

  // Assert the wideband information fields X1.
  ASSERT_EQ(pmi.i_1_1, expected_i_1_1);
  ASSERT_EQ(pmi.i_1_2, expected_i_1_2);
  ASSERT_EQ(pmi.layers[0].i_1_3, expected_i_1_3);
  ASSERT_EQ(pmi.layers[0].i_1_4, expected_i_1_4);

  // Assert that the unpacked wideband amplitudes match the number of non-zero coefficients reported in CSI Part 1.
  ASSERT_EQ(std::count_if(pmi.layers[0].i_1_4.begin(),
                          pmi.layers[0].i_1_4.end(),
                          [](uint8_t amplitude) { return amplitude != 0; }),
            expected_nof_amplitudes[0]);

  // Assert the coefficient fields X2.
  ASSERT_EQ(pmi.layers[0].i_2_1, expected_i_2_1);
  ASSERT_EQ(pmi.layers[0].i_2_2, expected_i_2_2);
}

// Verify that no supported CSI report configuration produces a report part larger than \c csi_report_max_size.
//
// \c csi_report_max_size bounds the packed CSI report container, hence a configuration exceeding it would be truncated.
TEST(csi_report_size, no_supported_configuration_exceeds_the_maximum_report_size)
{
  // The RI restriction is a bitmap with one bit per layer, hence the codebooks with more ports than the maximum number
  // of layers are skipped.
  constexpr unsigned max_nof_csi_rs_ports = 8;

  for (unsigned codebook_id = 0, codebook_id_end = pmi_codebook_id::max() + 1; codebook_id != codebook_id_end;
       ++codebook_id) {
    const pmi_codebook_config& pmi_codebook     = to_pmi_codebook_config(codebook_id);
    unsigned                   nof_csi_rs_ports = get_precoding_codebook_antenna_ports(pmi_codebook);
    if ((nof_csi_rs_ports == 0) || (nof_csi_rs_ports > max_nof_csi_rs_ports)) {
      continue;
    }

    // The RI restriction bitmap only selects ranks that the codebook can report.
    unsigned max_rank = get_precoding_codebook_max_rank(pmi_codebook);

    for (unsigned ri_bitmap = 1, ri_bitmap_end = 1U << max_rank; ri_bitmap != ri_bitmap_end; ++ri_bitmap) {
      for (csi_report_quantities quantities : {csi_report_quantities::cri_ri_pmi_cqi,
                                               csi_report_quantities::cri_ri_cqi,
                                               csi_report_quantities::cri_ri_li_pmi_cqi}) {
        ri_restriction_type ri_restriction(nof_csi_rs_ports);
        ri_restriction.from_uint64(ri_bitmap);

        csi_report_configuration config = {};
        config.nof_csi_rs_resources     = 1;
        config.nof_reported_rs          = 1;
        config.pmi_codebook             = pmi_codebook;
        config.ri_restriction           = ri_restriction;
        config.quantities               = quantities;

        const csi_report_size pusch_size = get_csi_report_pusch_size(config);
        ASSERT_LE(pusch_size.part1_size, csi_report_max_size)
            << "PUSCH CSI Part 1 of codebook " << to_string(pmi_codebook) << " exceeds the maximum report size.";
        ASSERT_LE(pusch_size.part2_max_size, csi_report_max_size)
            << "PUSCH CSI Part 2 of codebook " << to_string(pmi_codebook) << " exceeds the maximum report size.";

        // The Type II PMI is reported in CSI Part 2, which is only multiplexed in PUSCH.
        if (!std::holds_alternative<pmi_codebook_typeII>(pmi_codebook)) {
          ASSERT_LE(get_csi_report_pucch_size(config).part1_size, csi_report_max_size)
              << "PUCCH CSI Part 1 of codebook " << to_string(pmi_codebook) << " exceeds the maximum report size.";
        }
      }
    }
  }
}
