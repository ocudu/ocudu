// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/srs/srs_configuration_helpers.h"

using namespace ocudu;

ofdm_symbol_range ocudu::get_srs_symbol_range(const srs_config::srs_resource& res, cyclic_prefix cp)
{
  // Calculate the starting symbol.
  unsigned start_symbol = get_nsymb_per_slot(cp) - 1 - res.res_mapping.start_pos;

  return ofdm_symbol_range::start_and_len(start_symbol, res.res_mapping.nof_symb);
}

srs_resource_configuration ocudu::to_srs_resource_configuration(const srs_config::srs_resource& res, cyclic_prefix cp)
{
  // Convert the number of antenna ports.
  auto nof_antenna_ports = static_cast<srs_resource_configuration::one_two_four_enum>(res.nof_ports);

  // Calculate the symbol range.
  unsigned start_symbol = get_srs_symbol_range(res, cp).start();

  // Extract periodicity and offset.
  std::optional<srs_resource_configuration::periodicity_and_offset> periodicity_and_offset;
  if (res.periodicity_and_offset.has_value()) {
    periodicity_and_offset.emplace(srs_resource_configuration::periodicity_and_offset{
        static_cast<uint16_t>(res.periodicity_and_offset->period), res.periodicity_and_offset->offset});
  }

  // Fill SRS resource fields.
  return {.nof_antenna_ports   = nof_antenna_ports,
          .nof_symbols         = res.res_mapping.nof_symb,
          .start_symbol        = start_symbol,
          .configuration_index = res.freq_hop.c_srs,
          .sequence_id         = res.sequence_id,
          .bandwidth_index     = res.freq_hop.b_srs,
          .comb_size           = res.tx_comb.size,
          .comb_offset         = res.tx_comb.tx_comb_offset,
          .cyclic_shift        = res.tx_comb.tx_comb_cyclic_shift,
          .freq_position       = res.freq_domain_pos,
          .freq_shift          = res.freq_domain_shift,
          .freq_hopping        = res.freq_hop.b_hop,
          .hopping             = res.grp_or_seq_hop,
          .periodicity         = periodicity_and_offset};
}
