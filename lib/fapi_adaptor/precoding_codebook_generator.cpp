// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/fapi_adaptor/precoding_codebook_generator.h"
#include "precoding_codebook_repository_builder.h"
#include "precoding_matrix_mapper_functions.h"
#include "ocudu/adt/slotted_array.h"
#include "ocudu/fapi_adaptor/precoding_matrix_mapper.h"
#include "ocudu/ran/antenna_topology.h"
#include "ocudu/ran/precoding/precoding_codebook_type1_helpers.h"
#include "ocudu/ran/precoding/precoding_codebooks.h"

using namespace ocudu;
using namespace fapi_adaptor;

/// \brief Builds the composite precoding that the given PMI selects for the given antenna topology.
///
/// The precoding of a PMI is the product of two matrices, as per TS38.214 Section 5.2.2.2:
/// \f$W_{PMI} = W_{BF} \cdot W_{MIMO}\f$. \f$W_{BF}\f$ points the spatial beams. \f$W_{MIMO}\f$ maps the
/// transmission layers onto those beams.
///
/// A topology with a beam grid keeps the two matrices separate. The configuration contains \f$W_{MIMO}\f$ and a
/// list of beams. The channel processors apply \f$W_{MIMO}\f$ at the upper physical layer. The radio unit applies
/// \f$W_{BF}\f$.
///
/// A topology with no beam grid cannot steer a beam. The configuration contains the complete \f$W_{PMI}\f$. The
/// channel processors apply it at the upper physical layer. The radio unit transmits on the antenna ports.
static precoding_beamforming_composite
make_composite(const precoding_matrix_indicator& pmi, unsigned nof_layers, antenna_topology topology)
{
  if (has_beam_grid(topology)) {
    return get_mimo_matrix_from_pmi(pmi, nof_layers);
  }

  precoding_weight_matrix precoding = make_precoding(pmi, nof_layers);

  return {precoding, get_default_beam_list(precoding.get_nof_ports())};
}

/// Returns the maximum number of codebooks for the given number of antenna ports.
static unsigned get_max_num_codebooks(unsigned nof_ports)
{
  static const slotted_array<unsigned, 9> max_num_codebooks = [] {
    slotted_array<unsigned, 9> result;
    result.insert(1, 7);
    result.insert(2, 12);
    result.insert(4, 262);
    result.insert(8, 902);
    return result;
  }();

  ocudu_assert(max_num_codebooks.contains(nof_ports), "Unsupported number of antenna ports={}", nof_ports);
  return max_num_codebooks[nof_ports];
}

/// Generates SSB codebooks and precoding matrices for the given number of ports.
static unsigned generate_ssb(unsigned offset, unsigned nof_ports, precoding_codebook_repository_builder& repo_builder)
{
  precoding_weight_matrix precoding = make_one_layer_one_port(nof_ports, 0);
  unsigned                pm_index  = offset + get_ssb_precoding_matrix_index();
  repo_builder.add(pm_index, precoding);

  return ++offset;
}

/// Generates PDCCH codebooks and precoding matrices for the given number of ports.
static unsigned generate_pdcch(unsigned offset, unsigned nof_ports, precoding_codebook_repository_builder& repo_builder)
{
  precoding_weight_matrix precoding = make_one_layer_one_port(nof_ports, 0);
  unsigned                pm_index  = offset + get_pdcch_precoding_matrix_index();
  repo_builder.add(pm_index, precoding);

  return ++offset;
}

/// Generates DL-PRS codebooks and precoding matrices for the given number of ports.
static unsigned generate_prs(unsigned offset, unsigned nof_ports, precoding_codebook_repository_builder& repo_builder)
{
  precoding_weight_matrix precoding = make_one_layer_one_port(nof_ports, 0);
  unsigned                pm_index  = offset + get_prs_precoding_matrix_index();
  repo_builder.add(pm_index, precoding);

  return ++offset;
}

/// Generates CSI-RS codebooks and precoding matrices for the given number of ports.
static unsigned
generate_csi_rs(unsigned offset, unsigned nof_ports, precoding_codebook_repository_builder& repo_builder)
{
  precoding_weight_matrix precoding = make_identity(nof_ports);
  unsigned                pm_index  = offset + get_csi_rs_precoding_matrix_index();
  repo_builder.add(pm_index, precoding);

  return ++offset;
}

/// Generates PDSCH omnidirectional codebook and precoding matrices for the given number of ports.
static unsigned
generate_pdsch_omnidirectional(unsigned offset, unsigned nof_ports, precoding_codebook_repository_builder& repo_builder)
{
  precoding_weight_matrix precoding = make_one_layer_one_port(nof_ports, 0);
  unsigned                pm_index  = offset + get_pdsch_omnidirectional_precoding_matrix_index();
  repo_builder.add(pm_index, precoding);

  return ++offset;
}

/// Generates one-port PDSCH codebooks and precoding matrices.
static unsigned generate_pdsch_one_port(unsigned offset, precoding_codebook_repository_builder& repo_builder)
{
  precoding_weight_matrix precoding = make_single_port();
  unsigned                pm_index  = offset + get_pdsch_one_port_precoding_matrix_index();
  repo_builder.add(pm_index, precoding);

  return ++offset;
}

/// Generates the identity matrix.
static unsigned
generate_identity_matrix(unsigned offset, precoding_codebook_repository_builder& repo_builder, unsigned nof_layers)
{
  precoding_weight_matrix precoding = make_identity(nof_layers);
  repo_builder.add(0, precoding);

  return ++offset;
}

/// Generates two-port PDSCH codebooks and precoding matrices for one layer.
static unsigned generate_pdsch_2_ports_1_layer(unsigned offset, precoding_codebook_repository_builder& repo_builder)
{
  unsigned base_offset = offset;
  for (unsigned i = 0, e = 4; i != e; ++i) {
    precoding_weight_matrix precoding = make_precoding(pmi_two_antenna_port{.pmi = static_cast<uint8_t>(i)}, 1);
    unsigned                pm_index  = base_offset + get_pdsch_two_port_precoding_matrix_index(i);
    repo_builder.add(pm_index, precoding);
    offset = pm_index;
  }
  return ++offset;
}

/// Generates two-port PDSCH codebooks and precoding matrices for two layers.
static unsigned generate_pdsch_2_ports_2_layers(unsigned offset, precoding_codebook_repository_builder& repo_builder)
{
  unsigned base_offset = offset;
  for (unsigned i = 0, e = 2; i != e; ++i) {
    precoding_weight_matrix precoding = make_precoding(pmi_two_antenna_port{.pmi = static_cast<uint8_t>(i)}, 2);
    unsigned                pm_index  = base_offset + get_pdsch_two_port_precoding_matrix_index(i);
    repo_builder.add(pm_index, precoding);
    offset = pm_index;
  }

  return ++offset;
}

/// Generates PDSCH single-panel type 1 precoding matrices codebook for a number of layers.
static unsigned generate_pdsch_sp_type1(unsigned                               offset,
                                        const pmi_codebook_typeI_single_panel  panel,
                                        unsigned                               nof_layers,
                                        antenna_topology                       topology,
                                        precoding_codebook_repository_builder& repo_builder)
{
  unsigned base_offset = offset;

  // Get parameter ranges.
  pmi_typeI_single_panel_param_ranges param_ranges = get_pmi_ranges_typeI_single_panel(panel, nof_layers);

  unsigned nof_i_1_1 = param_ranges.i_1_1;
  unsigned nof_i_1_2 = param_ranges.i_1_2;
  unsigned nof_i_1_3 = param_ranges.i_1_3;
  unsigned nof_i_2   = param_ranges.i_2;

  for (uint8_t i_1_1 = 0; i_1_1 != nof_i_1_1; ++i_1_1) {
    for (uint8_t i_1_2 = 0; i_1_2 != nof_i_1_2; ++i_1_2) {
      for (uint8_t i_1_3 = 0; i_1_3 != nof_i_1_3; ++i_1_3) {
        for (uint8_t i_2 = 0; i_2 != nof_i_2; ++i_2) {
          pmi_typeI_single_panel pmi = {.panel_config = panel,
                                        .i_1_1        = i_1_1,
                                        .i_1_2        = (param_ranges.i_1_2 > 0) ? std::optional(i_1_2) : std::nullopt,
                                        .i_1_3        = (param_ranges.i_1_3 > 0) ? std::optional(i_1_3) : std::nullopt,
                                        .i_2          = i_2};

          unsigned pm_index = base_offset + get_pdsch_single_panel_type1_precoding_matrix_index(param_ranges, pmi);
          repo_builder.add(pm_index, make_composite(pmi, nof_layers, topology));

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
  precoding_codebook_repository_builder&                 repo_builder;
  antenna_topology                                       topology;

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
    offset = generate_csi_rs(offset, nof_ports, repo_builder);
    mapper_offsets.prs_codebook_offsets.push_back(offset);
    generate_prs(offset, nof_ports, repo_builder);
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
    offset = generate_csi_rs(offset, nof_ports, repo_builder);
    mapper_offsets.prs_codebook_offsets.push_back(offset);
    generate_prs(offset, nof_ports, repo_builder);
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
      offset = generate_pdsch_sp_type1(offset, codebook_config, nof_layers, topology, repo_builder);
    }
    mapper_offsets.csi_rs_codebook_offsets.push_back(offset);
    offset = generate_csi_rs(offset, nof_ports, repo_builder);
    mapper_offsets.prs_codebook_offsets.push_back(offset);
    generate_prs(offset, nof_ports, repo_builder);
  }

  void operator()(const pmi_codebook_typeII&) const
  {
    report_fatal_error("Static generation and mapping of Type II precoding matrices is not supported.");
  }
};

} // namespace

std::pair<std::unique_ptr<precoding_matrix_mapper>, std::unique_ptr<precoding_codebook_repository>>
ocudu::fapi_adaptor::generate_precoding_codebooks(const pmi_codebook_config& codebook_config,
                                                  antenna_topology           topology,
                                                  unsigned                   sector_id)
{
  unsigned nof_ports = get_precoding_codebook_antenna_ports(codebook_config);

  report_fatal_error_if_not(get_total_nof_ports(topology) == nof_ports,
                            "The antenna topology has {} ports, but the codebook has {}.",
                            get_total_nof_ports(topology),
                            nof_ports);

  precoding_matrix_mapper_codebook_offset_configuration mapper_offsets;
  precoding_codebook_repository_builder                 repo_builder(get_max_num_codebooks(nof_ports));

  std::visit(codebook_table_generator{mapper_offsets, repo_builder, topology}, codebook_config);

  return {std::make_unique<precoding_matrix_mapper>(sector_id, mapper_offsets), repo_builder.build()};
}
