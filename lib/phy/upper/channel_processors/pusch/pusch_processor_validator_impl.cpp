// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pusch_processor_validator_impl.h"
#include "pusch_processor_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/phy/upper/dmrs_mapping.h"
#include "ocudu/ran/transform_precoding/transform_precoding_helpers.h"
#include "ocudu/ran/uci/uci_constants.h"
#include "ocudu/ran/uci/uci_formatters.h"
#include <algorithm>

using namespace ocudu;

pusch_processor_validator_impl::pusch_processor_validator_impl(const pusch_processor::channel_size& ce_dims_) :
  ce_dims(ce_dims_)
{
  static constexpr interval<unsigned, true> nof_tx_layers_range(1, pusch_processor_impl::max_nof_layers);

  ocudu_assert(
      (ce_dims.nof_prb > 0) && (ce_dims.nof_prb <= MAX_NOF_PRBS), "Invalid number of PRB (i.e. {}).", ce_dims.nof_prb);
  ocudu_assert((ce_dims.nof_symbols > 0) && (ce_dims.nof_symbols <= MAX_NSYMB_PER_SLOT),
               "Invalid number of OFDM symbols.");
  ocudu_assert(ce_dims.nof_rx_ports > 0, "The number of receive ports cannot be zero.");
  ocudu_assert(nof_tx_layers_range.contains(ce_dims.nof_tx_layers),
               "Number of transmit layers (i.e. {}) is out of the range {}.",
               ce_dims.nof_tx_layers,
               nof_tx_layers_range);
}

/// Validates the parameters of a single UE grant.
static error_type<std::string> is_ue_valid(const pusch_processor::ue_grant&          ue,
                                           const pusch_processor::common_parameters& common)
{
  // Each UE must transmit between one and the maximum number of layers.
  static constexpr interval<unsigned, true> nof_layers_range(1, pusch_constants::MAX_NOF_LAYERS);
  unsigned                                  nof_layers = ue.dmrs_ports.count();
  if (!nof_layers_range.contains(nof_layers)) {
    return make_unexpected(fmt::format("The number of DM-RS ports of rnti={} (i.e., {}) is out of the range {}.",
                                       ue.context.rnti,
                                       nof_layers,
                                       nof_layers_range));
  }

  // The DM-RS ports are currently mapped to the first ports, starting at port 0. The channel estimator and the
  // demodulator do not support other port selections yet.
  dmrs_port_mask expected_dmrs_ports(DMRS_MAX_NPORTS);
  expected_dmrs_ports.fill(0, nof_layers);
  if (ue.dmrs_ports != expected_dmrs_ports) {
    return make_unexpected(fmt::format("The DM-RS ports of rnti={} (i.e., {}) must be consecutive, starting at port 0.",
                                       ue.context.rnti,
                                       ue.dmrs_ports));
  }

  if (const auto* dmrs_config = std::get_if<pusch_processor::dmrs_configuration>(&common.dmrs)) {
    // The DM-RS ports must be supported by the DM-RS type.
    if (static_cast<unsigned>(ue.dmrs_ports.find_highest()) >= dmrs_max_ports_type(dmrs_config->dmrs)) {
      return make_unexpected(fmt::format("The DM-RS ports of rnti={} (i.e., {}) exceed the maximum number of ports for "
                                         "DM-RS {} (i.e., {}).",
                                         ue.context.rnti,
                                         ue.dmrs_ports,
                                         dmrs_config->dmrs,
                                         dmrs_max_ports_type(dmrs_config->dmrs)));
    }

    // The CDM groups of the DM-RS ports must not carry data.
    for (size_t i_dmrs_port : ue.dmrs_ports.get_bit_positions()) {
      if (get_dmrs_cdm_group_index(dmrs_config->dmrs, i_dmrs_port) >= dmrs_config->nof_cdm_groups_without_data) {
        return make_unexpected(fmt::format("The DM-RS ports of rnti={} (i.e., {}) belong to CDM groups that carry data "
                                           "(number of CDM groups without data is {}).",
                                           ue.context.rnti,
                                           ue.dmrs_ports,
                                           dmrs_config->nof_cdm_groups_without_data));
      }
    }
  } else if (nof_layers != 1) {
    // Transform precoding only supports a single layer.
    return make_unexpected("Transform precoding is only possible with one layer.");
  }

  // CSI Part 2 size parameters must be compatible with the CSI Part 1 number of bits.
  if (!ue.uci.csi_part2_size.is_valid(ue.uci.nof_csi_part1)) {
    return make_unexpected(
        fmt::format("CSI Part 1 UCI field length (i.e., {}) does not correspond with the CSI Part 2 (i.e., {}).",
                    ue.uci.nof_csi_part1,
                    ue.uci.csi_part2_size));
  }

  // Compute the total CSI Part 2 payload as the sum of maximum values from each entry's map.
  unsigned max_csi_part2_bits = 0;
  for (const auto& entry : ue.uci.csi_part2_size.entries) {
    if (!entry.map.empty()) {
      max_csi_part2_bits += *std::max_element(entry.map.begin(), entry.map.end());
    }
  }

  // Total UCI payload must not exceed the global bit limit.
  unsigned nof_uci_bits = ue.uci.nof_harq_ack + ue.uci.nof_csi_part1 + max_csi_part2_bits;
  if (nof_uci_bits > uci_constants::MAX_NOF_PAYLOAD_BITS) {
    return make_unexpected(fmt::format("Total UCI size (i.e., {}) exceeds the maximum size, i.e., {}.",
                                       nof_uci_bits,
                                       uci_constants::MAX_NOF_PAYLOAD_BITS));
  }

  return default_success_t();
}

error_type<std::string>
pusch_processor_validator_impl::is_valid(const pusch_processor::common_parameters&           common,
                                         span<const pusch_processor::ue_specific_parameters> ues) const
{
  unsigned nof_symbols_slot = get_nsymb_per_slot(common.cp);

  // The BWP size exceeds the grid size.
  if ((common.bwp_start_rb + common.bwp_size_rb) > ce_dims.nof_prb) {
    return make_unexpected(fmt::format(
        "The sum of the BWP start (i.e., {}) and size (i.e., {}) exceeds the maximum grid size (i.e., {} PRB).",
        common.bwp_start_rb,
        common.bwp_size_rb,
        ce_dims.nof_prb));
  }

  // The number of receive ports cannot exceed the maximum dimensions.
  if (common.rx_ports.size() > ce_dims.nof_rx_ports) {
    return make_unexpected(
        fmt::format("The number of receive ports (i.e., {}) exceeds the maximum number of receive ports (i.e., {}).",
                    common.rx_ports.size(),
                    ce_dims.nof_rx_ports));
  }

  // The frequency allocation is not compatible with the BWP parameters.
  if (!common.freq_alloc.is_bwp_valid(common.bwp_start_rb, common.bwp_size_rb)) {
    return make_unexpected(
        fmt::format("Invalid BWP configuration, i.e., [{}, {}) for the given RB allocation, i.e., {}.",
                    common.bwp_start_rb,
                    common.bwp_start_rb + common.bwp_size_rb,
                    common.freq_alloc));
  }

  // The number of UEs must be within the supported range.
  static constexpr interval<unsigned, true> nof_ues_range(1, pusch_constants::MAX_NOF_MU_MIMO_UES);
  if (!nof_ues_range.contains(ues.size())) {
    return make_unexpected(
        fmt::format("The number of UEs (i.e., {}) is out of the range {}.", ues.size(), nof_ues_range));
  }

  // Only one UE is currently supported.
  if (ues.size() != 1) {
    return make_unexpected(
        fmt::format("Multiple UEs per PUSCH transmission (i.e., {}) are not currently supported.", ues.size()));
  }

  // Validate each UE and accumulate the DM-RS ports. Each DM-RS port can only be used by one UE.
  unsigned       total_nof_layers = 0;
  dmrs_port_mask used_dmrs_ports(DMRS_MAX_NPORTS);
  for (const pusch_processor::ue_specific_parameters& ue : ues) {
    if (error_type<std::string> ue_valid = is_ue_valid(ue.grant, common); !ue_valid.has_value()) {
      return ue_valid;
    }

    dmrs_port_mask overlap_ports = used_dmrs_ports & ue.grant.dmrs_ports;
    if (overlap_ports.any()) {
      return make_unexpected(fmt::format("The DM-RS ports {} are present in two UEs.", overlap_ports));
    }

    used_dmrs_ports |= ue.grant.dmrs_ports;

    total_nof_layers += ue.grant.dmrs_ports.count();
  }

  // The total number of layers must be within the channel estimate limits.
  unsigned max_nof_layers = std::min(pusch_constants::MAX_NOF_MU_MIMO_LAYERS, ce_dims.nof_tx_layers);
  if (total_nof_layers > max_nof_layers) {
    return make_unexpected(fmt::format("The total number of transmit layers (i.e., {}) is out of the range {}.",
                                       total_nof_layers,
                                       interval<unsigned, true>(1, max_nof_layers)));
  }

  // The number of OFDM symbols in the DM-RS mask must be equal to the number of OFDM symbols in a slot.
  if (common.dmrs_symbol_mask.size() != nof_symbols_slot) {
    return make_unexpected(
        fmt::format("The DM-RS symbol mask size (i.e., {}) must be equal to the slot size (i.e., {}).",
                    common.dmrs_symbol_mask.size(),
                    nof_symbols_slot));
  }

  // The number of symbols carrying DM-RS must be greater than zero.
  if (common.dmrs_symbol_mask.none()) {
    return make_unexpected("The number of OFDM symbols carrying DM-RS must be greater than zero.");
  }

  // The index of the first OFDM symbol carrying DM-RS shall be equal to or greater than the first symbol allocated to
  // transmission.
  int first_dmrs_symbol_index = common.dmrs_symbol_mask.find_lowest(true);
  if (static_cast<unsigned>(first_dmrs_symbol_index) < common.start_symbol_index) {
    return make_unexpected(fmt::format("The index of the first OFDM symbol carrying DM-RS (i.e., {}) must be equal to "
                                       "or greater than the first symbol allocated to transmission (i.e., {}).",
                                       first_dmrs_symbol_index,
                                       common.start_symbol_index));
  }

  // The index of the last OFDM symbol carrying DM-RS shall not be larger than the last symbol allocated to
  // transmission.
  int last_dmrs_symbol_index = common.dmrs_symbol_mask.find_highest(true);
  if (static_cast<unsigned>(last_dmrs_symbol_index) >= (common.start_symbol_index + common.nof_symbols)) {
    return make_unexpected(fmt::format("The index of the last OFDM symbol carrying DM-RS (i.e., {}) must be less than "
                                       "or equal to the last symbol allocated to transmission (i.e., {}).",
                                       last_dmrs_symbol_index,
                                       (common.start_symbol_index + common.nof_symbols - 1)));
  }

  // None of the occupied symbols must exceed the slot size.
  if (nof_symbols_slot < (common.start_symbol_index + common.nof_symbols)) {
    return make_unexpected(fmt::format("The symbol allocation (i.e., [{}, {})) exceeds the slot size (i.e., {}).",
                                       common.start_symbol_index,
                                       common.start_symbol_index + common.nof_symbols,
                                       nof_symbols_slot));
  }

  // Check if transform precoding is enabled.
  if (std::holds_alternative<pusch_processor::dmrs_configuration>(common.dmrs)) {
    const auto& dmrs_config = std::get<pusch_processor::dmrs_configuration>(common.dmrs);
    // Only DM-RS Type 1 is supported.
    if (dmrs_config.dmrs != dmrs_config_type::type1) {
      return make_unexpected("Only DM-RS Type 1 is currently supported.");
    }

    // Only two CDM groups without data is supported.
    if (dmrs_config.nof_cdm_groups_without_data != 2) {
      return make_unexpected("Only two CDM groups without data are currently supported.");
    }
  } else {
    // Frequency allocation must be contiguous.
    if (!common.freq_alloc.is_contiguous()) {
      return make_unexpected("Transform precoding is only possible with contiguous allocations.");
    }

    // Number of PRB must be valid.
    if (!transform_precoding::is_nof_prbs_valid(common.freq_alloc.get_nof_rb())) {
      return make_unexpected("Transform precoding is only possible with a valid number of PRB.");
    }
  }

  // DC position is outside the channel estimate dimensions.
  interval<unsigned> dc_position_range(0, ce_dims.nof_prb * NOF_SUBCARRIERS_PER_RB);
  if (common.dc_position.has_value() && !dc_position_range.contains(*common.dc_position)) {
    return make_unexpected(
        fmt::format("DC position (i.e., {}) is out of range {}.", *common.dc_position, dc_position_range));
  }

  return default_success_t();
}
