// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdsch_processor_validator_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/phy/upper/dmrs_mapping.h"
#include "ocudu/ran/dmrs/dmrs.h"
#include <algorithm>

using namespace ocudu;

/// Checks that reserved elements do not collide with DM-RS.
static bool check_dmrs_and_reserved_collision(const pdsch_processor::pdu_t& config)
{
  // Create an OFDM symbol mask of all reserved elements to check it doesn't collide with the DM-RS one.
  symbol_slot_mask reserved_symbol_mask(config.dmrs_symbol_mask.size());
  for (const re_pattern& reserved_pattern : config.reserved.get_re_patterns()) {
    reserved_symbol_mask |= reserved_pattern.symbols;
  }
  reserved_symbol_mask &= config.dmrs_symbol_mask;

  return reserved_symbol_mask.any();
}

error_type<std::string> pdsch_processor_validator_impl::is_valid(const pdsch_processor::pdu_t& pdu,
                                                                 unsigned cell_bandwidth_prbs) const
{
  using namespace units::literals;

  unsigned         nof_symbols_slot = get_nsymb_per_slot(pdu.cp);
  dmrs_config_type dmrs_config      = pdu.dmrs;

  // The frequency allocation is not compatible with the BWP parameters.
  if (!pdu.freq_alloc.is_bwp_valid(pdu.bwp_start_rb, pdu.bwp_size_rb, cell_bandwidth_prbs)) {
    return make_unexpected(
        fmt::format("Invalid BWP configuration, i.e., [{}, {}) for the given RB allocation, i.e., {}.",
                    pdu.bwp_start_rb,
                    pdu.bwp_start_rb + pdu.bwp_size_rb,
                    pdu.freq_alloc));
  }

  // The size of the DM-RS symbol mask must coincide with the number of symbols in the slot.
  if (pdu.dmrs_symbol_mask.size() != nof_symbols_slot) {
    return make_unexpected(
        fmt::format("The DM-RS symbol mask size (i.e., {}) must be equal to the slot size (i.e., {}).",
                    pdu.dmrs_symbol_mask.size(),
                    nof_symbols_slot));
  }

  // The number of OFDM symbols carrying DM-RS must be greater than zero.
  if (pdu.dmrs_symbol_mask.none()) {
    return make_unexpected("The number of OFDM symbols carrying DM-RS must be greater than zero.");
  }

  // The index of the first OFDM symbol carrying DM-RS shall be equal to or greater than the first symbol allocated to
  // transmission.
  int first_dmrs_symbol_index = pdu.dmrs_symbol_mask.find_lowest(true);
  if (static_cast<unsigned>(first_dmrs_symbol_index) < pdu.start_symbol_index) {
    return make_unexpected(fmt::format("The index of the first OFDM symbol carrying DM-RS (i.e., {}) must be equal to "
                                       "or greater than the first symbol allocated to transmission (i.e., {}).",
                                       first_dmrs_symbol_index,
                                       pdu.start_symbol_index));
  }

  // The index of the last OFDM symbol carrying DM-RS shall not be larger than the last symbol allocated to
  // transmission.
  int last_dmrs_symbol_index = pdu.dmrs_symbol_mask.find_highest(true);
  if (static_cast<unsigned>(last_dmrs_symbol_index) >= (pdu.start_symbol_index + pdu.nof_symbols)) {
    return make_unexpected(fmt::format("The index of the last OFDM symbol carrying DM-RS (i.e., {}) must be less than "
                                       "or equal to the last symbol allocated to transmission (i.e., {}).",
                                       last_dmrs_symbol_index,
                                       (pdu.start_symbol_index + pdu.nof_symbols - 1)));
  }

  // None of the occupied symbols must exceed the slot size.
  if (nof_symbols_slot < (pdu.start_symbol_index + pdu.nof_symbols)) {
    return make_unexpected(fmt::format("The symbol allocation (i.e., [{}, {})) exceeds the slot size (i.e., {}).",
                                       pdu.start_symbol_index,
                                       pdu.start_symbol_index + pdu.nof_symbols,
                                       nof_symbols_slot));
  }

  // Only DM-RS Type 1 is supported.
  if (pdu.dmrs != dmrs_config_type::type1) {
    return make_unexpected("Only DM-RS Type 1 is currently supported.");
  }

  // Each layer must be carried by a different DM-RS port.
  unsigned nof_layers = pdu.precoding_and_beamforming.get_nof_layers();
  if (pdu.dmrs_ports.count() != nof_layers) {
    return make_unexpected(
        fmt::format("The number of DM-RS ports (i.e., {}) must be equal to the number of layers (i.e., {}).",
                    pdu.dmrs_ports.count(),
                    nof_layers));
  }

  // The DM-RS ports must be supported by the DM-RS type.
  if (static_cast<unsigned>(pdu.dmrs_ports.find_highest()) >= dmrs_max_ports_type(dmrs_config)) {
    return make_unexpected(fmt::format("The DM-RS ports (i.e., {}) exceed the maximum number of ports for DM-RS {} "
                                       "(i.e., {}).",
                                       pdu.dmrs_ports,
                                       dmrs_config,
                                       dmrs_max_ports_type(dmrs_config)));
  }

  // List of DM-RS ports used by the transmission.
  const static_vector<size_t, DMRS_MAX_NPORTS> dmrs_ports = pdu.dmrs_ports.get_bit_positions();

  // The CDM groups of the DM-RS ports must not carry data.
  if (!std::all_of(dmrs_ports.begin(), dmrs_ports.end(), [&](size_t i_dmrs_port) {
        return get_dmrs_cdm_group_index(dmrs_config, i_dmrs_port) < pdu.nof_cdm_groups_without_data;
      })) {
    return make_unexpected(fmt::format(
        "The DM-RS ports (i.e., {}) belong to CDM groups that carry data (number of CDM groups without data is {}).",
        pdu.dmrs_ports,
        pdu.nof_cdm_groups_without_data));
  }

  // DM-RS ports with time-domain OCC require double-symbol DM-RS, as per TS38.211 Tables 7.4.1.1.2-1 and 7.4.1.1.2-2.
  if (std::any_of(dmrs_ports.begin(), dmrs_ports.end(), [dmrs_config](size_t i_dmrs_port) {
        return is_dmrs_port_double_symbol(dmrs_config, i_dmrs_port);
      })) {
    for (unsigned i_symbol = 0, e = pdu.dmrs_symbol_mask.size(); i_symbol != e;) {
      if (!pdu.dmrs_symbol_mask.test(i_symbol)) {
        ++i_symbol;
        continue;
      }

      // The DM-RS symbol must be immediately followed by another DM-RS symbol to form a double-symbol pair.
      if ((i_symbol + 1 >= e) || !pdu.dmrs_symbol_mask.test(i_symbol + 1)) {
        return make_unexpected(
            fmt::format("The DM-RS ports (i.e., {}) require double-symbol DM-RS, but the DM-RS symbol mask (i.e., {}) "
                        "is not composed of adjacent symbol pairs.",
                        pdu.dmrs_ports,
                        pdu.dmrs_symbol_mask));
      }

      // Skip the pair.
      i_symbol += 2;
    }
  }

  // PT-RS is only supported when it is associated with the DM-RS port 1000.
  if (pdu.ptrs.has_value() && (pdu.dmrs_ports.find_lowest() != 0)) {
    return make_unexpected(fmt::format(
        "PT-RS is only supported if the DM-RS port mask (i.e., {}) includes port index 0.", pdu.dmrs_ports));
  }

  if (pdu.precoding_and_beamforming.get_nof_beams() % pdu.codewords.size() != 0) {
    return make_unexpected(
        fmt::format("The number of beams (i.e., {}) must be divisible by the number of codewords (i.e., {}).",
                    pdu.precoding_and_beamforming.get_nof_beams(),
                    pdu.codewords.size()));
  }

  // The number of CDM groups without data must not exceed the maximum supported by the DM-RS type.
  if (pdu.nof_cdm_groups_without_data > get_max_nof_cdm_groups_without_data(dmrs_config)) {
    return make_unexpected(fmt::format(
        "The number of CDM groups without data (i.e., {}) must not exceed the maximum supported by the DM-RS "
        "type (i.e., {}).",
        pdu.nof_cdm_groups_without_data,
        get_max_nof_cdm_groups_without_data(dmrs_config)));
  }

  // The limited buffer for rate matching size must not be zero.
  if (pdu.tbs_lbrm == 0_bytes) {
    return make_unexpected("Invalid LBRM size (0 bytes).");
  }

  // Make sure the DM-RS and the reserved resource elements do not collide.
  if (check_dmrs_and_reserved_collision(pdu)) {
    return make_unexpected("Reserved REs collide with DM-RS.");
  }

  return default_success_t();
}
