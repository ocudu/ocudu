// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

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
  return std::tie(data.cri, data.ri, data.li, data.pmi, data.first_tb_wideband_cqi, data.second_tb_wideband_cqi);
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

using CsiReportUnpackingParams = std::tuple<pmi_codebook_config, csi_report_quantities, Repetitions>;

class CsiReportPucchFixture : public ::testing::TestWithParam<CsiReportUnpackingParams>
{
protected:
  csi_report_configuration configuration;
  csi_report_data          unpacked_data;
  csi_report_packed        packed_pucch_data;

  void SetUp() override
  {
    const pmi_codebook_config&   pmi_codebook = std::get<0>(GetParam());
    const csi_report_quantities& quantities   = std::get<1>(GetParam());

    unsigned nof_csi_rs_antenna_ports = get_precoding_codebook_antenna_ports(pmi_codebook);

    configuration.nof_csi_rs_resources = nof_csi_rs_resources_dist(rgen);
    configuration.pmi_codebook         = pmi_codebook;
    configuration.ri_restriction       = ~ri_restriction_type(nof_csi_rs_antenna_ports);
    configuration.quantities           = quantities;

    if (configuration.ri_restriction.count() > 2) {
      // Set a random RI restriction element to false.
      std::uniform_int_distribution<unsigned> ri_restriction_dist(1, nof_csi_rs_antenna_ports - 1);
      configuration.ri_restriction.set(ri_restriction_dist(rgen), false);
    }

    // Pack CRI if enabled.
    if (configuration.quantities < csi_report_quantities::other) {
      fill_cri(packed_pucch_data, unpacked_data, configuration);
    }

    // Pack RI if enabled.
    if (configuration.quantities < csi_report_quantities::other) {
      fill_ri(packed_pucch_data, unpacked_data, configuration);
    }

    // Pack LI if enabled.
    if (quantities == csi_report_quantities::cri_ri_li_pmi_cqi) {
      fill_li(packed_pucch_data, unpacked_data, configuration);
    }

    // Fill with padding.
    fill_padding(packed_pucch_data, unpacked_data, configuration);

    // Pack PMI if enabled.
    if ((quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
        (quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
      fill_pmi(packed_pucch_data, unpacked_data, configuration);
    }

    // Pack Wideband CQI if enabled.
    if ((quantities == csi_report_quantities::cri_ri_pmi_cqi) || (quantities == csi_report_quantities::cri_ri_cqi) ||
        (quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
      fill_wideband_cqi(packed_pucch_data, unpacked_data, configuration);
    }
  }

private:
  static void fill_cri(csi_report_packed& packed, csi_report_data& unpacked, const csi_report_configuration& config)
  {
    unsigned nof_cri_bits = 0;
    if (!std::holds_alternative<std::monostate>(config.pmi_codebook)) {
      nof_cri_bits = log2_ceil(config.nof_csi_rs_resources);
    }

    unsigned cri = rgen() & mask_lsb_ones<unsigned>(nof_cri_bits);
    unpacked.cri.emplace(cri);
    packed.push_back(cri, nof_cri_bits);
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
    if (std::holds_alternative<pmi_codebook_one_port>(config.pmi_codebook) ||
        std::holds_alternative<std::monostate>(config.pmi_codebook)) {
      return;
    }

    unsigned ri                       = unpacked.ri.value().value();
    unsigned nof_csi_rs_antenna_ports = get_precoding_codebook_antenna_ports(config.pmi_codebook);
    unsigned current_size             = get_pucch_payload_size(config, ri);
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
      const auto&                           codebook   = std::get<pmi_codebook_typeI_single_panel>(config.pmi_codebook);
      const pmi_codebook_single_panel_info& panel_info = get_single_panel_info(codebook.n1_n2);
      const pmi_typeI_single_panel_param_sizes sizes   = get_pmi_sizes_typeI_single_panel(panel_info, ri);

      unsigned i_1_1 = rgen() & mask_lsb_ones<unsigned>(sizes.i_1_1);
      unsigned i_1_2 = rgen() & mask_lsb_ones<unsigned>(sizes.i_1_2);
      unsigned i_1_3 = rgen() & mask_lsb_ones<unsigned>(sizes.i_1_3);
      unsigned i_2   = rgen() & mask_lsb_ones<unsigned>(sizes.i_2);

      // Set PMI values.
      pmi_typeI_single_panel type;
      type.panel_config = codebook.n1_n2;
      type.i_1_1        = i_1_1;
      if (sizes.i_1_2 > 0) {
        type.i_1_2.emplace(i_1_2);
      }
      if (sizes.i_1_3 > 0) {
        type.i_1_3.emplace(i_1_3);
      }
      type.i_2 = i_2;

      precoding_matrix_indicator pmi;
      pmi.emplace<pmi_typeI_single_panel>(type);
      unpacked.pmi.emplace(pmi);

      // Pack PMI values in TS38.212 Section 6.3.1.1.2 order.
      packed.push_back(i_1_1, sizes.i_1_1);
      if (sizes.i_1_2 > 0) {
        packed.push_back(i_1_2, sizes.i_1_2);
      }
      if (sizes.i_1_3 > 0) {
        packed.push_back(i_1_3, sizes.i_1_3);
      }
      packed.push_back(i_2, sizes.i_2);
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

    if (ri > 4) {
      unpacked.second_tb_wideband_cqi.emplace(wideband_cqi2);
      packed.push_back(wideband_cqi2, 4);
    }
  }

  static std::mt19937                            rgen;
  static std::uniform_int_distribution<unsigned> nof_csi_rs_resources_dist;
};

std::mt19937                            CsiReportPucchFixture::rgen;
std::uniform_int_distribution<unsigned> CsiReportPucchFixture::nof_csi_rs_resources_dist(1, 16);

} // namespace

TEST_P(CsiReportPucchFixture, CsiReportPucchUnpacking)
{
  // Get report size.
  csi_report_size csi_report_size = get_csi_report_pucch_size(configuration);

  // Assert report size.
  ASSERT_EQ(csi_report_size.part1_size, units::bits(packed_pucch_data.size()));

  // Unpack.
  ASSERT_TRUE(validate_pucch_csi_payload(packed_pucch_data, configuration));
  csi_report_data unpacked = csi_report_unpack_pucch(packed_pucch_data, configuration);

  // Assert CRI.
  ASSERT_EQ(unpacked_data, unpacked);
}

INSTANTIATE_TEST_SUITE_P(
    CsiReportPucchHelpersTest,
    CsiReportPucchFixture,
    ::testing::Combine(::testing::Values(pmi_codebook_one_port{},
                                         pmi_codebook_two_port{},
                                         pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::two_one,
                                                                         pmi_codebook_typeI_mode::one},
                                         pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::two_two,
                                                                         pmi_codebook_typeI_mode::one},
                                         pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::four_one,
                                                                         pmi_codebook_typeI_mode::one}),
                       ::testing::Values(csi_report_quantities::cri_ri_pmi_cqi,
                                         csi_report_quantities::cri_ri_cqi,
                                         csi_report_quantities::cri_ri_li_pmi_cqi),
                       ::testing::Range(0U, 10U)));
