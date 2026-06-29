// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/fapi_adaptor/precoding_matrix_table_generator.h"
#include "ocudu/ran/precoding/precoding_codebook_configuration.h"
#include "ocudu/ran/precoding/precoding_codebook_helpers.h"
#include "ocudu/ran/precoding/precoding_codebooks.h"
#include "ocudu/ran/precoding/precoding_weight_matrix_formatters.h"
#include "fmt/ostream.h"
#include "fmt/std.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi_adaptor;

namespace ocudu {

std::ostream& operator<<(std::ostream& os, const precoding_weight_matrix& matrix)
{
  fmt::print(os, "{}", matrix);
  return os;
}

std::ostream& operator<<(std::ostream& os, const pmi_codebook_typeI_single_panel& config)
{
  pmi_codebook_single_panel_info info = get_single_panel_info(config.n1_n2);
  fmt::print(os, "N1={} N2={} mode={}", info.n1, info.n2, fmt::underlying(config.mode));
  return os;
}

} // namespace ocudu

TEST(precoding_matrix_table_generator, one_port)
{
  std::unique_ptr<precoding_matrix_mapper>     mapper;
  std::unique_ptr<precoding_matrix_repository> repository;
  std::tie(mapper, repository) = generate_precoding_matrix_tables(pmi_codebook_one_port{}, 0);

  mac_pdsch_precoding_info info;
  info.report.reset();

  unsigned index = mapper->map(info, 1);

  precoding_weight_matrix matrix = repository->get_precoding_matrix(index);

  precoding_weight_matrix expected_matrix = make_single_port();

  ASSERT_EQ(matrix, expected_matrix);
}

TEST(precoding_matrix_table_generator, two_port_one_layer)
{
  std::unique_ptr<precoding_matrix_mapper>     mapper;
  std::unique_ptr<precoding_matrix_repository> repository;
  std::tie(mapper, repository) = generate_precoding_matrix_tables(pmi_codebook_two_port{}, 0);

  // Iterate over all possible PMI.
  for (unsigned pmi = 0; pmi != 4; ++pmi) {
    mac_pdsch_precoding_info info;
    info.report.emplace(precoding_matrix_indicator{pmi_two_antenna_port{pmi}});

    unsigned index = mapper->map(info, 1);

    precoding_weight_matrix matrix = repository->get_precoding_matrix(index);

    precoding_weight_matrix expected_matrix = make_one_layer_two_ports(pmi);

    ASSERT_EQ(matrix, expected_matrix);
  }
}

TEST(precoding_matrix_table_generator, two_port_two_layer)
{
  std::unique_ptr<precoding_matrix_mapper>     mapper;
  std::unique_ptr<precoding_matrix_repository> repository;
  std::tie(mapper, repository) = generate_precoding_matrix_tables(pmi_codebook_two_port{}, 0);

  // Iterate over all possible PMI.
  for (unsigned pmi = 0; pmi != 2; ++pmi) {
    mac_pdsch_precoding_info info;
    info.report.emplace(precoding_matrix_indicator{pmi_two_antenna_port{pmi}});

    unsigned index = mapper->map(info, 2);

    precoding_weight_matrix matrix = repository->get_precoding_matrix(index);

    precoding_weight_matrix expected_matrix = make_two_layer_two_ports(pmi);

    ASSERT_EQ(matrix, expected_matrix);
  }
}

class typeI_single_panel_fixture : public ::testing::TestWithParam<pmi_codebook_typeI_single_panel>
{};

TEST_P(typeI_single_panel_fixture, TypeI_single_panel)
{
  const pmi_codebook_typeI_single_panel& codebook_config = GetParam();

  std::unique_ptr<precoding_matrix_mapper>     mapper;
  std::unique_ptr<precoding_matrix_repository> repository;
  std::tie(mapper, repository) = generate_precoding_matrix_tables(codebook_config, 0);

  unsigned nof_ports = get_precoding_codebook_antenna_ports(codebook_config);

  for (unsigned nof_layers = 1; nof_layers <= nof_ports; ++nof_layers) {
    pmi_typeI_single_panel_param_ranges param_ranges = get_pmi_ranges_typeI_single_panel(codebook_config, nof_layers);

    unsigned nof_i_1_1 = param_ranges.i_1_1;
    unsigned nof_i_1_2 = param_ranges.i_1_2;
    unsigned nof_i_1_3 = param_ranges.i_1_3;
    unsigned nof_i_2   = param_ranges.i_2;

    for (unsigned i_1_1 = 0; i_1_1 != nof_i_1_1; ++i_1_1) {
      for (unsigned i_1_2 = 0; i_1_2 != nof_i_1_2; ++i_1_2) {
        for (unsigned i_1_3 = 0; i_1_3 != nof_i_1_3; ++i_1_3) {
          for (unsigned i_2 = 0; i_2 != nof_i_2; ++i_2) {
            mac_pdsch_precoding_info info;
            std::optional<unsigned>  i_1_2_opt = (param_ranges.i_1_2 > 0) ? std::optional(i_1_2) : std::nullopt;
            std::optional<unsigned>  i_1_3_opt = (param_ranges.i_1_3 > 0) ? std::optional(i_1_3) : std::nullopt;
            pmi_typeI_single_panel   pmi       = {
                        .panel_config = codebook_config, .i_1_1 = i_1_1, .i_1_2 = i_1_2_opt, .i_1_3 = i_1_3_opt, .i_2 = i_2};
            info.report.emplace(pmi);

            unsigned index = mapper->map(info, nof_layers);

            precoding_weight_matrix matrix = repository->get_precoding_matrix(index);

            precoding_weight_matrix expected_matrix = make_type1_sp_mode1(pmi, nof_layers);

            ASSERT_EQ(matrix, expected_matrix) << fmt::format("nof_layers={} i_1_1={} i_1_2={} i_1_3={} i_2={}",
                                                              nof_layers,
                                                              pmi.i_1_1,
                                                              pmi.i_1_2,
                                                              pmi.i_1_3,
                                                              pmi.i_2);
          }
        }
      }
    }
  }
}

INSTANTIATE_TEST_SUITE_P(SinglePanelConfigs,
                         typeI_single_panel_fixture,
                         ::testing::Values(pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::two_one,
                                                                           pmi_codebook_typeI_mode::one},
                                           pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::two_two,
                                                                           pmi_codebook_typeI_mode::one},
                                           pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::four_one,
                                                                           pmi_codebook_typeI_mode::one}));
