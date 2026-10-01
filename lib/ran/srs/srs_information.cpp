// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/srs/srs_information.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/resource_block.h"
#include "ocudu/ran/srs/srs_bandwidth_configuration.h"
#include "ocudu/ran/srs/srs_resource_configuration.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;

static constexpr unsigned get_sequence_length(unsigned m_srs_b, tx_comb_size comb_size)
{
  return (m_srs_b * NOF_SUBCARRIERS_PER_RB) / static_cast<unsigned>(comb_size);
}

srs_information ocudu::get_srs_information(const srs_resource_configuration& resource, unsigned i_antenna_port)
{
  // Select BW configuration.
  std::optional<srs_configuration> srs_bw_config =
      srs_configuration_get(resource.configuration_index.value(), resource.bandwidth_index.value());
  ocudu_assert(srs_bw_config.has_value(),
               "Invalid combination of c-SRS (i.e., {}) and b-SRS (i.e., {})",
               resource.configuration_index,
               resource.bandwidth_index);

  // Assert configuration parameters.
  ocudu_assert(resource.hopping == srs_group_or_sequence_hopping::neither, "No sequence nor group hopping supported.");
  ocudu_assert(!resource.has_frequency_hopping(), "Frequency hopping is not supported.");

  // Calculate sequence length.
  unsigned sequence_length = get_sequence_length(srs_bw_config->m_srs, resource.comb_size);

  // Extract comb size.
  unsigned comb_size = static_cast<unsigned>(resource.comb_size);

  // Calculate sequence group.
  unsigned f_gh = 0;
  unsigned u    = (f_gh + resource.sequence_id.value()) % 30;

  // Calculate sequence number.
  unsigned v = 0;

  // Maximum number of cyclic shifts depending on the comb size.
  unsigned n_cs_max = (resource.comb_size == tx_comb_size::n4) ? 12 : 8;

  // Calculate cyclic shift. Note that n_cs_max is always multiple of the number of antenna ports (one, two or four).
  unsigned cyclic_shift_port = (resource.cyclic_shift.value() +
                                (n_cs_max * i_antenna_port) / static_cast<unsigned>(resource.nof_antenna_ports)) %
                               n_cs_max;

  // Calculate initial subcarrier index.
  unsigned k_tc = resource.comb_offset.value();
  if ((resource.cyclic_shift >= n_cs_max / 2) && (resource.cyclic_shift < n_cs_max) &&
      (resource.nof_antenna_ports == srs_resource_configuration::one_two_four_enum::four) &&
      ((i_antenna_port == 1) || (i_antenna_port == 3))) {
    k_tc = (k_tc + comb_size / 2) % comb_size;
  }
  unsigned k0_bar = resource.freq_shift.value() * NOF_SUBCARRIERS_PER_RB + k_tc;
  unsigned sum    = 0;
  for (unsigned b = 0; b <= resource.bandwidth_index; ++b) {
    std::optional<srs_configuration> bw_config = srs_configuration_get(resource.configuration_index.value(), b);
    ocudu_assert(bw_config, "Invalid configuration.");

    unsigned M_srs = get_sequence_length(bw_config->m_srs, resource.comb_size);
    unsigned n_b   = ((4 * resource.freq_position.value()) / bw_config->m_srs) % bw_config->N;
    sum += comb_size * M_srs * n_b;
  }
  unsigned initial_subcarrier = k0_bar + sum;

  // Fill derived parameters.
  srs_information info = {.sequence_length            = sequence_length,
                          .sequence_group             = u,
                          .sequence_number            = v,
                          .n_cs                       = cyclic_shift_port,
                          .n_cs_max                   = n_cs_max,
                          .mapping_initial_subcarrier = initial_subcarrier,
                          .comb_size                  = comb_size};

  return info;
}
