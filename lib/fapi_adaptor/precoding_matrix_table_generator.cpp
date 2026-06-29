// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/fapi_adaptor/precoding_matrix_table_generator.h"
#include "precoding_matrix_mapper_functions.h"
#include "precoding_matrix_repository_builder.h"
#include "ocudu/adt/slotted_array.h"
#include "ocudu/fapi_adaptor/precoding_matrix_mapper.h"
#include "ocudu/ran/precoding/precoding_codebook_helpers.h"
#include "ocudu/ran/precoding/precoding_codebooks.h"

using namespace ocudu;
using namespace fapi_adaptor;

/// Returns the maximum number of codebooks for the given number of antenna ports.
static unsigned get_max_num_codebooks(unsigned nof_ports)
{
  static const slotted_array<unsigned, 9> max_num_codebooks = [] {
    slotted_array<unsigned, 9> result;
    result.insert(1, 6);
    result.insert(2, 11);
    result.insert(4, 261);
    result.insert(8, 901);
    return result;
  }();

  ocudu_assert(max_num_codebooks.contains(nof_ports), "Unsupported number of antenna ports={}", nof_ports);
  return max_num_codebooks[nof_ports];
}

/// Generates SSB codebooks and precoding matrices for the given number of ports.
static unsigned generate_ssb(unsigned offset, unsigned nof_ports, precoding_matrix_repository_builder& repo_builder)
{
  precoding_weight_matrix precoding = make_one_layer_one_port(nof_ports, 0);
  unsigned                pm_index  = offset + get_ssb_precoding_matrix_index();
  repo_builder.add(pm_index, precoding);

  return ++offset;
}

/// Generates PDCCH codebooks and precoding matrices for the given number of ports.
static unsigned generate_pdcch(unsigned offset, unsigned nof_ports, precoding_matrix_repository_builder& repo_builder)
{
  precoding_weight_matrix precoding = make_one_layer_one_port(nof_ports, 0);
  unsigned                pm_index  = offset + get_pdcch_precoding_matrix_index();
  repo_builder.add(pm_index, precoding);

  return ++offset;
}

/// Generates CSI-RS codebooks and precoding matrices for the given number of ports.
static unsigned generate_csi_rs(unsigned offset, unsigned nof_ports, precoding_matrix_repository_builder& repo_builder)
{
  precoding_weight_matrix precoding = make_identity(nof_ports);
  unsigned                pm_index  = offset + get_csi_rs_precoding_matrix_index();
  repo_builder.add(pm_index, precoding);

  return ++offset;
}

/// Generates PDSCH omnidirectional codebook and precoding matrices for the given number of ports.
static unsigned
generate_pdsch_omnidirectional(unsigned offset, unsigned nof_ports, precoding_matrix_repository_builder& repo_builder)
{
  precoding_weight_matrix precoding = make_one_layer_one_port(nof_ports, 0);
  unsigned                pm_index  = offset + get_pdsch_omnidirectional_precoding_matrix_index();
  repo_builder.add(pm_index, precoding);

  return ++offset;
}

/// Generates one-port PDSCH codebooks and precoding matrices.
static unsigned generate_pdsch_one_port(unsigned offset, precoding_matrix_repository_builder& repo_builder)
{
  precoding_weight_matrix precoding = make_single_port();
  unsigned                pm_index  = offset + get_pdsch_one_port_precoding_matrix_index();
  repo_builder.add(pm_index, precoding);

  return ++offset;
}

/// Generates the identity matrix.
static unsigned
generate_identity_matrix(unsigned offset, precoding_matrix_repository_builder& repo_builder, unsigned nof_layers)
{
  precoding_weight_matrix precoding = make_identity(nof_layers);
  repo_builder.add(0, precoding);

  return ++offset;
}

/// Generates two-port PDSCH codebooks and precoding matrices for one layer.
static unsigned generate_pdsch_2_ports_1_layer(unsigned offset, precoding_matrix_repository_builder& repo_builder)
{
  unsigned base_offset = offset;
  for (unsigned i = 0, e = 4; i != e; ++i) {
    precoding_weight_matrix precoding = make_one_layer_two_ports(i);
    unsigned                pm_index  = base_offset + get_pdsch_two_port_precoding_matrix_index(i);
    repo_builder.add(pm_index, precoding);
    offset = pm_index;
  }
  return ++offset;
}

/// Generates two-port PDSCH codebooks and precoding matrices for two layers.
static unsigned generate_pdsch_2_ports_2_layers(unsigned offset, precoding_matrix_repository_builder& repo_builder)
{
  unsigned base_offset = offset;
  for (unsigned i = 0, e = 2; i != e; ++i) {
    precoding_weight_matrix precoding = make_two_layer_two_ports(i);
    unsigned                pm_index  = base_offset + get_pdsch_two_port_precoding_matrix_index(i);
    repo_builder.add(pm_index, precoding);
    offset = pm_index;
  }

  return ++offset;
}

/// Generates PDSCH single-panel type 1 precoding matrices codebook for a number of layers.
static unsigned generate_pdsch_sp_type1(unsigned                              offset,
                                        const pmi_codebook_typeI_single_panel panel,
                                        unsigned                              nof_layers,
                                        precoding_matrix_repository_builder&  repo_builder)
{
  unsigned base_offset = offset;

  // Get parameter ranges.
  pmi_typeI_single_panel_param_ranges param_ranges = get_pmi_ranges_typeI_single_panel(panel, nof_layers);

  unsigned nof_i_1_1 = param_ranges.i_1_1;
  unsigned nof_i_1_2 = param_ranges.i_1_2;
  unsigned nof_i_1_3 = param_ranges.i_1_3;
  unsigned nof_i_2   = param_ranges.i_2;

  for (unsigned i_1_1 = 0; i_1_1 != nof_i_1_1; ++i_1_1) {
    for (unsigned i_1_2 = 0; i_1_2 != nof_i_1_2; ++i_1_2) {
      for (unsigned i_1_3 = 0; i_1_3 != nof_i_1_3; ++i_1_3) {
        for (unsigned i_2 = 0; i_2 != nof_i_2; ++i_2) {
          pmi_typeI_single_panel pmi = {.panel_config = panel,
                                        .i_1_1        = i_1_1,
                                        .i_1_2        = (param_ranges.i_1_2 > 0) ? std::optional(i_1_2) : std::nullopt,
                                        .i_1_3        = (param_ranges.i_1_3 > 0) ? std::optional(i_1_3) : std::nullopt,
                                        .i_2          = i_2};

          precoding_weight_matrix precoding = make_type1_sp_mode1(pmi, nof_layers);
          unsigned pm_index = base_offset + get_pdsch_single_panel_type1_precoding_matrix_index(param_ranges, pmi);
          repo_builder.add(pm_index, precoding);

          offset = pm_index;
        }
      }
    }
  }

  return ++offset;
}

namespace {

/// Dispatches codebook generation to the correct handler for the PMI codebook type.
struct codebook_table_generator {
  precoding_matrix_mapper_codebook_offset_configuration& mapper_offsets;
  precoding_matrix_repository_builder&                   repo_builder;

  void operator()(std::monostate) const { ocudu_assertion_failure("Unsupported PMI codebook configuration"); }

  void operator()(const pmi_codebook_one_port&) const
  {
    unsigned                  offset    = 0U;
    static constexpr unsigned nof_ports = 1U;

    offset = generate_identity_matrix(offset, repo_builder, nof_ports);
    mapper_offsets.ssb_codebook_offsets.push_back(offset);
    offset                           = generate_ssb(offset, nof_ports, repo_builder);
    mapper_offsets.pdsch_omni_offset = offset;
    offset                           = generate_pdsch_omnidirectional(offset, nof_ports, repo_builder);
    mapper_offsets.pdsch_codebook_offsets.push_back(offset);
    offset = generate_pdsch_one_port(offset, repo_builder);
    mapper_offsets.pdcch_codebook_offsets.push_back(offset);
    offset = generate_pdcch(offset, nof_ports, repo_builder);
    mapper_offsets.csi_rs_codebook_offsets.push_back(offset);
    generate_csi_rs(offset, nof_ports, repo_builder);
  }

  void operator()(const pmi_codebook_two_port&) const
  {
    unsigned                  offset    = 0U;
    static constexpr unsigned nof_ports = 2U;

    offset = generate_identity_matrix(offset, repo_builder, nof_ports);
    mapper_offsets.ssb_codebook_offsets.push_back(offset);
    offset = generate_ssb(offset, nof_ports, repo_builder);
    mapper_offsets.pdcch_codebook_offsets.push_back(offset);
    offset                           = generate_pdcch(offset, nof_ports, repo_builder);
    mapper_offsets.pdsch_omni_offset = offset;
    offset                           = generate_pdsch_omnidirectional(offset, nof_ports, repo_builder);
    mapper_offsets.pdsch_codebook_offsets.push_back(offset);
    offset = generate_pdsch_2_ports_1_layer(offset, repo_builder);
    mapper_offsets.pdsch_codebook_offsets.push_back(offset);
    offset = generate_pdsch_2_ports_2_layers(offset, repo_builder);
    mapper_offsets.csi_rs_codebook_offsets.push_back(offset);
    generate_csi_rs(offset, nof_ports, repo_builder);
  }

  void operator()(const pmi_codebook_typeI_single_panel& codebook_config) const
  {
    unsigned nof_ports = get_precoding_codebook_antenna_ports(codebook_config);
    unsigned offset    = 0U;

    offset = generate_identity_matrix(offset, repo_builder, nof_ports);
    mapper_offsets.ssb_codebook_offsets.push_back(offset);
    offset = generate_ssb(offset, nof_ports, repo_builder);
    mapper_offsets.pdcch_codebook_offsets.push_back(offset);
    offset                           = generate_pdcch(offset, nof_ports, repo_builder);
    mapper_offsets.pdsch_omni_offset = offset;
    offset                           = generate_pdsch_omnidirectional(offset, nof_ports, repo_builder);
    for (unsigned nof_layers = 1; nof_layers <= nof_ports; ++nof_layers) {
      mapper_offsets.pdsch_codebook_offsets.push_back(offset);
      offset = generate_pdsch_sp_type1(offset, codebook_config, nof_layers, repo_builder);
    }
    mapper_offsets.csi_rs_codebook_offsets.push_back(offset);
    generate_csi_rs(offset, nof_ports, repo_builder);
  }
};

} // namespace

std::pair<std::unique_ptr<precoding_matrix_mapper>, std::unique_ptr<precoding_matrix_repository>>
ocudu::fapi_adaptor::generate_precoding_matrix_tables(const pmi_codebook_config& codebook_config, unsigned sector_id)
{
  precoding_matrix_mapper_codebook_offset_configuration mapper_offsets;
  precoding_matrix_repository_builder                   repo_builder(
      get_max_num_codebooks(get_precoding_codebook_antenna_ports(codebook_config)));

  std::visit(codebook_table_generator{mapper_offsets, repo_builder}, codebook_config);

  return {std::make_unique<precoding_matrix_mapper>(sector_id, mapper_offsets), repo_builder.build()};
}
