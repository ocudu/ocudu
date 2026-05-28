// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/adt/to_array.h"
#include "ocudu/ran/csi_report/csi_report_configuration.h"
#include "ocudu/ran/csi_report/csi_report_data.h"
#include "ocudu/ran/csi_report/csi_report_formatters.h"
#include "ocudu/ran/csi_report/csi_report_on_pucch_helpers.h"
#include "ocudu/ran/csi_report/csi_report_on_pusch_helpers.h"
#include "ocudu/ran/precoding/precoding_codebook_helpers.h"
#include <fmt/ostream.h>
#include <gtest/gtest.h>
#include <random>

using namespace ocudu;

namespace ocudu {

auto to_tuple(const csi_report_data& data)
{
  return std::tie(data.cri,
                  data.rsrp_dBm,
                  data.ri,
                  data.li,
                  data.pmi,
                  data.first_tb_wideband_cqi,
                  data.second_tb_wideband_cqi,
                  data.first_tb_subband_diff_cqi,
                  data.second_tb_subband_diff_cqi);
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

  return false;
}

bool operator==(const csi_report_data& left, const csi_report_data& right)
{
  return to_tuple(left) == to_tuple(right);
}

std::ostream& operator<<(std::ostream& os, csi_report_data data)
{
  fmt::print(os, "{}", data);
  return os;
}

std::ostream& operator<<(std::ostream& os, const csi_report_configuration& config)
{
  fmt::print(os, "{}", config);
  return os;
}

} // namespace ocudu

namespace {

using CsiReportUnpackingParams = csi_report_configuration;

class CsiReportPucchFixture : public ::testing::TestWithParam<CsiReportUnpackingParams>
{
protected:
  csi_report_data   expected_unpacked;
  csi_report_packed packed_pucch_data;
  csi_report_packed packed_part2_data;

  void SetUp() override
  {
    const csi_report_configuration& configuration = GetParam();

    // Pack CRI if enabled.
    if (configuration.quantities < csi_report_quantities::other) {
      fill_cri(packed_pucch_data, expected_unpacked, configuration);
    }

    // Pack RSRP if enabled.
    if ((configuration.quantities == csi_report_quantities::cri_rsrp) ||
        (configuration.quantities == csi_report_quantities::ssb_index_rsrp)) {
      fill_rsrp(packed_pucch_data, expected_unpacked, configuration);
    }

    // Pack RI if enabled.
    if ((configuration.quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
        (configuration.quantities == csi_report_quantities::cri_ri_cqi) ||
        (configuration.quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
      fill_ri(packed_pucch_data, expected_unpacked, configuration);
    }

    // If subband reports are enabled, apply TS38.212 Tables 6.3.1.1.2-9 and 6.3.1.1.2-10 for wideband
    // and subband. Otherwise, keep going with the fields in Table 6.3.1.1.2-7.
    if (configuration.subband.has_value()) {
      // Pack wideband CQI in Part 1.
      fill_wideband_cqi(packed_pucch_data, expected_unpacked, configuration);

      // Pack subband differential CQI in Part 1 if enabled.
      if (configuration.subband->cqi) {
        fill_subband_diff_cqi_first_tb(packed_pucch_data, expected_unpacked, configuration);
      }

      // Pack wideband CQI for the second TB in Part 2 (wideband).
      fill_wideband_cqi_second_tb(packed_part2_data, expected_unpacked, configuration);

      // Pack LI in Part 2 (wideband) if enabled.
      if (configuration.quantities == csi_report_quantities::cri_ri_li_pmi_cqi) {
        fill_li(packed_part2_data, expected_unpacked, configuration);
      }

      // Pack PMI in Part 2 (wideband) if enabled.
      if ((configuration.quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
          (configuration.quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
        fill_pmi(packed_part2_data, expected_unpacked, configuration);
      }

      // Pack CQI differential for even subbands in Part 2 (subband) if enabled.
      if (configuration.subband->cqi) {
        fill_even_subband_diff_cqi_second_tb(packed_part2_data, expected_unpacked, configuration);
      }

      // Pack CQI differential for odd subbands in Part 2 (subband) if enabled.
      if (configuration.subband->cqi) {
        fill_odd_subband_diff_cqi_second_tb(packed_part2_data, expected_unpacked, configuration);
      }

      return;
    }

    // Pack LI if enabled.
    if (configuration.quantities == csi_report_quantities::cri_ri_li_pmi_cqi) {
      fill_li(packed_pucch_data, expected_unpacked, configuration);
    }

    // Fill with padding.
    fill_padding(packed_pucch_data, expected_unpacked, configuration);

    // Pack PMI if enabled.
    if ((configuration.quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
        (configuration.quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
      fill_pmi(packed_pucch_data, expected_unpacked, configuration);
    }

    // Pack Wideband CQI if enabled.
    if ((configuration.quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
        (configuration.quantities == csi_report_quantities::cri_ri_cqi) ||
        (configuration.quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
      fill_wideband_cqi(packed_pucch_data, expected_unpacked, configuration);
    }
  }

private:
  static void fill_cri(csi_report_packed& packed, csi_report_data& unpacked, const csi_report_configuration& config)
  {
    unsigned nof_reported_rs = config.nof_reported_rs.value();
    unsigned nof_cri_bits    = log2_ceil(config.nof_csi_rs_resources);

    // Pack each of the CRI.
    for (unsigned i = 0; i != nof_reported_rs; ++i) {
      unsigned cri = rgen() & mask_lsb_ones<unsigned>(nof_cri_bits);
      unpacked.cri.emplace_back(cri);
      packed.push_back(cri, nof_cri_bits);
    }
  }

  static void fill_rsrp(csi_report_packed& packed, csi_report_data& unpacked, const csi_report_configuration& config)
  {
    unsigned nof_reported_rs = config.nof_reported_rs.value();

    // Pack first RSRP. The RSRP value is expressed in 1 dB steps from [-140, -44] dBm, as per TS38.214
    // Section 5.2.1.4.3.
    uint8_t rsrp = rgen() % (140 - 44);
    unpacked.rsrp_dBm.push_back(-140 + rsrp);
    packed.push_back(rsrp, 7);

    // Pack the differential RSRP.
    for (unsigned i = 1; i != nof_reported_rs; ++i) {
      uint8_t diff_rsrp = rgen() % 16;
      unpacked.rsrp_dBm.push_back(unpacked.rsrp_dBm.front() - 2 * diff_rsrp);
      packed.push_back(diff_rsrp, 4);
    }
  }

  static void fill_ri(csi_report_packed& packed, csi_report_data& unpacked, const csi_report_configuration& config)
  {
    unsigned nof_ri      = static_cast<unsigned>(config.ri_restriction.count());
    unsigned nof_ri_bits = get_nof_ri_bits(config);

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
  }

  static void fill_li(csi_report_packed& packed, csi_report_data& unpacked, const csi_report_configuration& config)
  {
    unsigned nof_layers  = unpacked.ri.value().value();
    unsigned nof_li_bits = get_nof_li_bits(config, nof_layers);

    unsigned li = (rgen() & mask_lsb_ones<unsigned>(nof_li_bits));
    unpacked.li.emplace(li);
    packed.push_back(li, nof_li_bits);
  }

  static unsigned get_nof_ri_bits(const csi_report_configuration& config)
  {
    unsigned ri_restriction_count     = static_cast<unsigned>(config.ri_restriction.count());
    unsigned nof_csi_rs_antenna_ports = get_precoding_codebook_antenna_ports(config.pmi_codebook);

    if (std::holds_alternative<pmi_codebook_two_port>(config.pmi_codebook)) {
      return std::min(1U, log2_ceil(ri_restriction_count));
    }
    if (std::holds_alternative<pmi_codebook_typeI_single_panel>(config.pmi_codebook)) {
      return (nof_csi_rs_antenna_ports == 4) ? std::min(2U, log2_ceil(ri_restriction_count))
                                             : log2_ceil(ri_restriction_count);
    }
    return 0;
  }

  static unsigned get_nof_li_bits(const csi_report_configuration& config, unsigned ri)
  {
    if (std::holds_alternative<pmi_codebook_two_port>(config.pmi_codebook)) {
      return log2_ceil(ri);
    }
    if (std::holds_alternative<pmi_codebook_typeI_single_panel>(config.pmi_codebook)) {
      return std::min(2U, log2_ceil(ri));
    }
    return 0;
  }

  static unsigned get_nof_pmi_bits(const csi_report_configuration& config, unsigned ri)
  {
    if (std::holds_alternative<pmi_codebook_two_port>(config.pmi_codebook)) {
      return (ri == 1) ? 2 : 1;
    }
    if (std::holds_alternative<pmi_codebook_typeI_single_panel>(config.pmi_codebook)) {
      const auto&                           codebook   = std::get<pmi_codebook_typeI_single_panel>(config.pmi_codebook);
      const pmi_codebook_single_panel_info& panel_info = get_single_panel_info(codebook.n1_n2);
      const pmi_typeI_single_panel_param_sizes sizes   = get_pmi_sizes_typeI_single_panel(panel_info, ri);
      return sizes.i_1_1 + sizes.i_1_2 + sizes.i_1_3 + sizes.i_2;
    }
    return 0;
  }

  static unsigned get_pucch_payload_size(const csi_report_configuration& config, unsigned ri)
  {
    // CRI.
    unsigned count = log2_ceil(config.nof_csi_rs_resources);

    // RI.
    count += get_nof_ri_bits(config);

    // LI.
    if (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi) {
      count += get_nof_li_bits(config, ri);
    }

    // PMI.
    if ((config.quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
        (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
      count += get_nof_pmi_bits(config, ri);
    }

    // CQI.
    if ((config.quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
        (config.quantities == csi_report_quantities::cri_ri_cqi) ||
        (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
      count += 4;

      // Second TB for RI > 4.
      if (ri > 4) {
        count += 4;
      }
    }
    return count;
  }

  static void fill_padding(csi_report_packed& packed, csi_report_data& unpacked, const csi_report_configuration& config)
  {
    // Padding is not applicable if the RI is not reported.
    if (!unpacked.ri.has_value()) {
      return;
    }

    csi_report_data::ri_type ri = unpacked.ri.value();
    if (std::holds_alternative<pmi_codebook_one_port>(config.pmi_codebook) ||
        std::holds_alternative<std::monostate>(config.pmi_codebook)) {
      return;
    }

    unsigned nof_csi_rs_antenna_ports = get_precoding_codebook_antenna_ports(config.pmi_codebook);
    unsigned current_size             = get_pucch_payload_size(config, ri.value());
    unsigned max_size                 = 0;
    for (unsigned i_rank = 1; i_rank <= nof_csi_rs_antenna_ports; ++i_rank) {
      max_size = std::max(max_size, get_pucch_payload_size(config, i_rank));
    }

    if (max_size > current_size) {
      packed.push_back(0U, max_size - current_size);
    }
  }

  static void fill_pmi(csi_report_packed& packed, csi_report_data& unpacked, const csi_report_configuration& config)
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
      // Obtain PMI codebook configuration.
      const pmi_codebook_typeI_single_panel pmi_codebook =
          std::get<pmi_codebook_typeI_single_panel>(config.pmi_codebook);

      // Get parameter sizes for the selected RI.
      pmi_typeI_single_panel_param_sizes param_sizes =
          get_pmi_sizes_typeI_single_panel(get_single_panel_info(pmi_codebook.n1_n2), ri);

      // Set PMI values.
      pmi_typeI_single_panel type;
      type.i_1_1 = rgen() & mask_lsb_ones<unsigned>(param_sizes.i_1_1);
      if (param_sizes.i_1_2) {
        type.i_1_2.emplace(rgen() & mask_lsb_ones<unsigned>(param_sizes.i_1_2));
      }
      if (param_sizes.i_1_3) {
        type.i_1_3.emplace(rgen() & mask_lsb_ones<unsigned>(param_sizes.i_1_3));
      }
      type.i_2 = rgen() & mask_lsb_ones<unsigned>(param_sizes.i_2);

      precoding_matrix_indicator pmi;
      pmi.emplace<pmi_typeI_single_panel>(type);
      unpacked.pmi.emplace(pmi);

      // Pack PMI values.
      packed.push_back(type.i_1_1, param_sizes.i_1_1);
      if (type.i_1_2.has_value()) {
        packed.push_back(*type.i_1_2, param_sizes.i_1_2);
      }
      if (type.i_1_3.has_value()) {
        packed.push_back(*type.i_1_3, param_sizes.i_1_3);
      }
      packed.push_back(type.i_2, param_sizes.i_2);
    }
  }

  static void
  fill_wideband_cqi(csi_report_packed& packed, csi_report_data& unpacked, const csi_report_configuration& config)
  {
    unsigned ri            = unpacked.ri.value().value();
    unsigned wideband_cqi1 = rgen() & mask_lsb_ones<unsigned>(4);
    unsigned wideband_cqi2 = rgen() & mask_lsb_ones<unsigned>(4);

    unpacked.first_tb_wideband_cqi.emplace(wideband_cqi1);
    packed.push_back(wideband_cqi1, 4);

    if (!config.subband.has_value() && (ri > 4)) {
      unpacked.second_tb_wideband_cqi.emplace(wideband_cqi2);
      packed.push_back(wideband_cqi2, 4);
    }
  }

  static void fill_wideband_cqi_second_tb(csi_report_packed&              packed,
                                          csi_report_data&                unpacked,
                                          const csi_report_configuration& config)
  {
    unsigned ri = unpacked.ri.value().value();
    if (ri <= 4) {
      return;
    }

    unsigned wideband_cqi2 = rgen() & mask_lsb_ones<unsigned>(4);
    unpacked.second_tb_wideband_cqi.emplace(wideband_cqi2);
    packed.push_back(wideband_cqi2, 4);
  }

  static void fill_subband_diff_cqi_first_tb(csi_report_packed&              packed,
                                             csi_report_data&                unpacked,
                                             const csi_report_configuration& config)
  {
    csi_report_data::subband_diff_cqi_list diff_cqi;
    for (unsigned i_subband = 0, nof_subbands = config.subband->nof_subbands.value(); i_subband != nof_subbands;
         ++i_subband) {
      unsigned value = rgen() & mask_lsb_ones<unsigned>(2);
      diff_cqi.emplace_back(value);
      packed.push_back(value, 2);
    }
    unpacked.first_tb_subband_diff_cqi.emplace(diff_cqi);
  }

  static void fill_even_subband_diff_cqi_second_tb(csi_report_packed&              packed,
                                                   csi_report_data&                unpacked,
                                                   const csi_report_configuration& config)
  {
    unsigned ri = unpacked.ri.value().value();
    if (ri <= 4) {
      return;
    }

    unsigned                               nof_subbands = config.subband->nof_subbands.value();
    csi_report_data::subband_diff_cqi_list diff_cqi;
    for (unsigned i_subband = 0; i_subband != nof_subbands; ++i_subband) {
      diff_cqi.emplace_back((rgen() + i_subband) & mask_lsb_ones<unsigned>(2));
    }

    for (unsigned i_subband = 0; i_subband < nof_subbands; i_subband += 2) {
      packed.push_back(diff_cqi[i_subband].value(), 2);
    }
    unpacked.second_tb_subband_diff_cqi.emplace(diff_cqi);
  }

  static void fill_odd_subband_diff_cqi_second_tb(csi_report_packed&              packed,
                                                  csi_report_data&                unpacked,
                                                  const csi_report_configuration& config)
  {
    unsigned ri = unpacked.ri.value().value();
    if (ri <= 4) {
      return;
    }

    const unsigned nof_subbands = config.subband->nof_subbands.value();
    const auto&    diff_cqi     = unpacked.second_tb_subband_diff_cqi.value();

    for (unsigned i_subband = 1; i_subband < nof_subbands; i_subband += 2) {
      packed.push_back(diff_cqi[i_subband].value(), 2);
    }
  }

  static std::mt19937 rgen;
};

std::mt19937 CsiReportPucchFixture::rgen;

} // namespace

TEST_P(CsiReportPucchFixture, CsiReportPucchUnpacking)
{
  const csi_report_configuration& configuration = GetParam();

  // Get report size.
  csi_report_size csi_report_size = get_csi_report_pucch_size(configuration);

  // Assert report size.
  ASSERT_EQ(csi_report_size.part1_size.value(), packed_pucch_data.size());

  // Unpack.
  ASSERT_TRUE(validate_pucch_csi_payload(packed_pucch_data, configuration));
  csi_report_data unpacked = csi_report_unpack_pucch(packed_pucch_data, configuration);
  if (configuration.subband.has_value()) {
    csi_report_packed remaining_part2 =
        csi_report_unpack_pucch_part2_wideband(unpacked, packed_part2_data, configuration);
    remaining_part2 = csi_report_unpack_pucch_part2_subband(unpacked, remaining_part2, configuration);
    ASSERT_TRUE(remaining_part2.empty());
  }

  // Assert CRI.
  ASSERT_EQ(expected_unpacked, unpacked);
}

static std::vector<csi_report_configuration> generate_test_cases()
{
  std::mt19937                           rgen;
  std::vector<csi_report_configuration>  test_cases;
  std::uniform_int_distribution<uint8_t> nof_csi_rs_resources_dist(1, 16);
  std::uniform_int_distribution<uint8_t> nof_reported_rs_dist(1, 4);
  std::uniform_int_distribution<uint8_t> nof_subbands_dist(5, csi_max_nof_subbands);
  const auto                             pmi_codebooks = to_array<pmi_codebook_config>(
      {pmi_codebook_one_port{},
                                   pmi_codebook_two_port{},
                                   pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::two_one, pmi_codebook_typeI_mode::one},
                                   pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::four_one, pmi_codebook_typeI_mode::one},
                                   pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::two_two, pmi_codebook_typeI_mode::one}});

  for (unsigned i = 0; i != 10; ++i) {
    for (const auto& quantities : {csi_report_quantities::cri_ri_pmi_cqi,
                                   csi_report_quantities::cri_ri_cqi,
                                   csi_report_quantities::cri_rsrp,
                                   csi_report_quantities::ssb_index_rsrp,
                                   csi_report_quantities::cri_ri_li_pmi_cqi}) {
      // Handle quantities for cri-RSRP and ssb-Index-RSRP.
      if ((quantities == csi_report_quantities::cri_rsrp) || (quantities == csi_report_quantities::ssb_index_rsrp)) {
        test_cases.emplace_back(csi_report_configuration{
            .nof_csi_rs_resources = nof_csi_rs_resources_dist(rgen),
            .nof_reported_rs      = nof_reported_rs_dist(rgen),
            .pmi_codebook         = std::monostate{},
            .ri_restriction       = {},
            .quantities           = quantities,
            .subband              = std::nullopt,
        });
        continue;
      }

      for (bool subband : {false, true}) {
        for (const pmi_codebook_config pmi_codebook : pmi_codebooks) {
          csi_report_configuration config;
          config.nof_csi_rs_resources = nof_csi_rs_resources_dist(rgen);
          config.nof_reported_rs      = 1;
          config.pmi_codebook         = pmi_codebook;
          config.ri_restriction       = ~ri_restriction_type(get_precoding_codebook_antenna_ports(pmi_codebook));
          config.quantities           = quantities;

          // Set a random RI restriction bit to false.
          if (config.ri_restriction.size() > 1) {
            config.ri_restriction.set(rgen() % config.ri_restriction.size(), false);
          }

          if (subband) {
            config.subband.emplace(
                csi_report_subband_configuration{.cqi = true, .pmi = false, .nof_subbands = nof_subbands_dist(rgen)});
          }

          test_cases.push_back(config);
        }
      }
    }
  }

  // Test case for the largest CSI report size on PUCCH. The current supported maximum report size is for a
  // single-panel codebook for eight ports (4x1) and the maximum number of wideband CQI subbands.
  test_cases.emplace_back(csi_report_configuration{
      .nof_csi_rs_resources = 64,
      .nof_reported_rs      = 1,
      .pmi_codebook =
          pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::four_one, pmi_codebook_typeI_mode::one},
      .ri_restriction = ~ri_restriction_type(8),
      .quantities     = csi_report_quantities::cri_ri_cqi,
      .subband = csi_report_subband_configuration{.cqi = true, .pmi = false, .nof_subbands = csi_max_nof_subbands},
  });

  return test_cases;
}

static const std::vector<CsiReportUnpackingParams> test_cases = generate_test_cases();

INSTANTIATE_TEST_SUITE_P(CsiReportPucchHelpersTest, CsiReportPucchFixture, ::testing::ValuesIn(test_cases));
