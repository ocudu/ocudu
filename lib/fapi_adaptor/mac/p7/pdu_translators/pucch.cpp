// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pucch.h"
#include "ocudu/mac/mac_cell_result.h"
#include "ocudu/scheduler/result/pucch_info.h"

using namespace ocudu;
using namespace fapi_adaptor;

/// Returns the number of SR bits as unsigned.
static unsigned convert_sr_bits_to_unsigned(sr_nof_bits value)
{
  switch (value) {
    case sr_nof_bits::no_sr:
      return 0U;
    case sr_nof_bits::one:
      return 1U;
    case sr_nof_bits::two:
      return 2U;
    case sr_nof_bits::three:
      return 3U;
    case sr_nof_bits::four:
      return 4U;
  }
  return 0U;
}

/// Fills the Format 0 parameters.
static void fill_format0_parameters(fapi::ul_pucch_format_0_pdu_builder& builder,
                                    const pucch_format_0&                mac_pdu,
                                    sr_nof_bits                          sr_bits,
                                    units::bits                          bit_len_harq)
{
  builder.set_hopping_parameters(mac_pdu.n_id_hopping)
      .set_cyclic_shift_parameters(mac_pdu.initial_cyclic_shift)
      .set_payload_parameters(convert_sr_bits_to_unsigned(sr_bits), bit_len_harq);
}

/// Fills the Format 1 parameters.
static void fill_format1_parameters(fapi::ul_pucch_format_1_pdu_builder& builder,
                                    const pucch_format_1&                mac_pdu,
                                    sr_nof_bits                          sr_bits,
                                    units::bits                          bit_len_harq)
{
  builder.set_hopping_parameters(mac_pdu.n_id_hopping)
      .set_cyclic_shift_parameters(mac_pdu.initial_cyclic_shift)
      .set_time_domain_parameters(mac_pdu.time_domain_occ)
      .set_payload_parameters(convert_sr_bits_to_unsigned(sr_bits), bit_len_harq);
}

/// Fills the Format 2 parameters.
static void fill_format2_parameters(fapi::ul_pucch_format_2_pdu_builder& builder,
                                    const pucch_format_2&                mac_pdu,
                                    const pucch_uci_bits&                uci_bits,
                                    units::bits                          bit_len_harq)
{
  builder.set_scrambling_parameters(mac_pdu.n_id_scrambling, mac_pdu.n_id_0_scrambling)
      .set_payload_parameters(uci_bits.sr_bits, units::bits(uci_bits.csi_part1_nof_bits), bit_len_harq);
}

/// Fills the Format 3 parameters.
static void fill_format3_parameters(fapi::ul_pucch_format_3_pdu_builder& builder,
                                    const pucch_format_3&                mac_pdu,
                                    const pucch_uci_bits&                uci_bits,
                                    units::bits                          bit_len_harq)
{
  // FAPI parameter m0PucchDmrsCyclicShift (ref. TS 38.211 6.4.1.3.3.1) maps always to 0 for PUCCH Format 3.
  constexpr unsigned m0_pucch_dmrs_cyclic_shift = 0;

  builder.set_modulation_parameters(mac_pdu.pi_2_bpsk)
      .set_hopping_parameters(mac_pdu.n_id_hopping)
      .set_scrambling_parameters(mac_pdu.n_id_scrambling)
      .set_dmrs_parameters(mac_pdu.additional_dmrs, mac_pdu.n_id_0_scrambling, m0_pucch_dmrs_cyclic_shift)
      .set_payload_parameters(uci_bits.sr_bits, units::bits(uci_bits.csi_part1_nof_bits), bit_len_harq);
}

/// Gets the cyclic shift index (m0) for PUCCH Format 4, as per TS 38.211 Table 6.4.1.3.3.1-1.
static unsigned get_pucch_format4_m0(unsigned occ_index)
{
  switch (occ_index) {
    case 0:
      return 0;
    case 1:
      return 6;
    case 2:
      return 3;
    case 3:
      return 9;
    default:
      return 0;
  }
}

/// Fills the Format 4 parameters.
static void fill_format4_parameters(fapi::ul_pucch_format_4_pdu_builder& builder,
                                    const pucch_format_4&                mac_pdu,
                                    const pucch_uci_bits&                uci_bits,
                                    units::bits                          bit_len_harq)
{
  // Format 4 specific parameters.
  // Both FAPI parameters initialCyclicShift (ref. TS 38.211 6.3.2.2.2) and m0PucchDmrsCyclicShift
  // (ref. TS 38.211 6.4.1.3.3.1) map to the same value.
  const unsigned m0_format4 = get_pucch_format4_m0(static_cast<unsigned>(mac_pdu.occ_index));

  builder.set_modulation_parameters(mac_pdu.pi_2_bpsk)
      .set_hopping_parameters(mac_pdu.n_id_hopping)
      .set_occ_parameters(static_cast<uint8_t>(mac_pdu.occ_index), static_cast<uint8_t>(mac_pdu.occ_length))
      .set_scrambling_parameters(mac_pdu.n_id_scrambling)
      .set_dmrs_parameters(mac_pdu.additional_dmrs, mac_pdu.n_id_0_scrambling, m0_format4)
      .set_payload_parameters(uci_bits.sr_bits, units::bits(uci_bits.csi_part1_nof_bits), bit_len_harq);
}

static void fill_format_parameters(fapi::ul_pucch_pdu_builder& builder, const pucch_info& mac_pdu)
{
  switch (mac_pdu.format()) {
    case pucch_format::FORMAT_0: {
      const auto&                         mac_pdu_f0       = std::get<pucch_format_0>(mac_pdu.format_params);
      fapi::ul_pucch_format_0_pdu_builder format_0_builder = builder.get_pucch_format_0_builder();
      fill_format0_parameters(
          format_0_builder, mac_pdu_f0, mac_pdu.uci_bits.sr_bits, units::bits(mac_pdu.uci_bits.harq_ack_nof_bits));
      break;
    }
    case pucch_format::FORMAT_1: {
      const auto&                         mac_pdu_f1       = std::get<pucch_format_1>(mac_pdu.format_params);
      fapi::ul_pucch_format_1_pdu_builder format_1_builder = builder.get_pucch_format_1_builder();
      fill_format1_parameters(
          format_1_builder, mac_pdu_f1, mac_pdu.uci_bits.sr_bits, units::bits(mac_pdu.uci_bits.harq_ack_nof_bits));
      break;
    }
    case pucch_format::FORMAT_2: {
      const auto&                         mac_pdu_f2       = std::get<pucch_format_2>(mac_pdu.format_params);
      fapi::ul_pucch_format_2_pdu_builder format_2_builder = builder.get_pucch_format_2_builder();
      fill_format2_parameters(
          format_2_builder, mac_pdu_f2, mac_pdu.uci_bits, units::bits(mac_pdu.uci_bits.harq_ack_nof_bits));
      break;
    }
    case pucch_format::FORMAT_3: {
      const auto&                         mac_pdu_f3       = std::get<pucch_format_3>(mac_pdu.format_params);
      fapi::ul_pucch_format_3_pdu_builder format_3_builder = builder.get_pucch_format_3_builder();
      fill_format3_parameters(
          format_3_builder, mac_pdu_f3, mac_pdu.uci_bits, units::bits(mac_pdu.uci_bits.harq_ack_nof_bits));
      break;
    }
    case pucch_format::FORMAT_4: {
      const auto&                         mac_pdu_f4       = std::get<pucch_format_4>(mac_pdu.format_params);
      fapi::ul_pucch_format_4_pdu_builder format_4_builder = builder.get_pucch_format_4_builder();
      fill_format4_parameters(
          format_4_builder, mac_pdu_f4, mac_pdu.uci_bits, units::bits(mac_pdu.uci_bits.harq_ack_nof_bits));
      break;
    }
    default:
      ocudu_assert(0, "Invalid PUCCH format={}", fmt::underlying(mac_pdu.format()));
  }
}

void ocudu::fapi_adaptor::convert_pucch_mac_to_fapi(fapi::ul_pucch_pdu_builder& builder, const pucch_info& mac_pdu)
{
  builder.set_ue_specific_parameters(mac_pdu.crnti);

  const bwp_configuration& bwp_cfg = *mac_pdu.bwp_cfg;
  builder.set_bwp_parameters(bwp_cfg.crbs, bwp_cfg.scs, bwp_cfg.cp);

  const prb_interval& freq_prbs = mac_pdu.resources.prbs;
  builder.set_frequency_allocation_parameters(freq_prbs);

  const ofdm_symbol_range& symbols = mac_pdu.resources.symbols;
  builder.set_time_allocation_parameters(symbols);

  builder.set_time_allocation_parameters(mac_pdu.resources.symbols)
      .set_hopping_information_parameters(mac_pdu.resources.second_hop_prb.has_value()
                                              ? std::optional<uint16_t>(*mac_pdu.resources.second_hop_prb)
                                              : std::nullopt);

  fill_format_parameters(builder, mac_pdu);
}
