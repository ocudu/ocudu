// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pucch.h"
#include "ocudu/adt/format.h"
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
static void fill_format0_parameters(fapi::ul_pucch_format_0_pdu_builder& builder, const pucch_info& mac_pdu)
{
  const auto& f0     = std::get<pucch_info::f0_config>(mac_pdu.format_params);
  const auto& res_f0 = std::get<pucch_resource::f0_config>(mac_pdu.res->format_params);
  builder.set_hopping_parameters(f0.n_id_hopping)
      .set_cyclic_shift_parameters(res_f0.initial_cyclic_shift)
      .set_payload_parameters(convert_sr_bits_to_unsigned(mac_pdu.uci_bits.sr_bits),
                              units::bits(mac_pdu.uci_bits.harq_ack_nof_bits));
}

/// Fills the Format 1 parameters.
static void fill_format1_parameters(fapi::ul_pucch_format_1_pdu_builder& builder, const pucch_info& mac_pdu)
{
  const auto& f1     = std::get<pucch_info::f1_config>(mac_pdu.format_params);
  const auto& res_f1 = std::get<pucch_resource::f1_config>(mac_pdu.res->format_params);
  builder.set_hopping_parameters(f1.n_id_hopping)
      .set_cyclic_shift_parameters(res_f1.initial_cyclic_shift)
      .set_time_domain_parameters(res_f1.time_domain_occ)
      .set_payload_parameters(convert_sr_bits_to_unsigned(mac_pdu.uci_bits.sr_bits),
                              units::bits(mac_pdu.uci_bits.harq_ack_nof_bits));
}

/// Fills the Format 2 parameters.
static void fill_format2_parameters(fapi::ul_pucch_format_2_pdu_builder& builder, const pucch_info& mac_pdu)
{
  const auto& f2 = std::get<pucch_info::f2_config>(mac_pdu.format_params);
  builder.set_scrambling_parameters(f2.n_id_scrambling, f2.n_id_0_scrambling)
      .set_payload_parameters(mac_pdu.uci_bits.sr_bits,
                              units::bits(mac_pdu.uci_bits.csi_part1_nof_bits),
                              units::bits(mac_pdu.uci_bits.harq_ack_nof_bits));
}

/// Fills the Format 3 parameters.
static void fill_format3_parameters(fapi::ul_pucch_format_3_pdu_builder& builder, const pucch_info& mac_pdu)
{
  const auto& f3     = std::get<pucch_info::f3_config>(mac_pdu.format_params);
  const auto& res_f3 = std::get<pucch_resource::f3_config>(mac_pdu.res->format_params);
  // FAPI parameter m0PucchDmrsCyclicShift (ref. TS 38.211 6.4.1.3.3.1) maps always to 0 for PUCCH Format 3.
  constexpr unsigned m0_pucch_dmrs_cyclic_shift = 0;

  builder.set_modulation_parameters(res_f3.pi_2_bpsk)
      .set_hopping_parameters(f3.n_id_hopping)
      .set_scrambling_parameters(f3.n_id_scrambling)
      .set_dmrs_parameters(res_f3.additional_dmrs, f3.n_id_0_scrambling, m0_pucch_dmrs_cyclic_shift)
      .set_payload_parameters(mac_pdu.uci_bits.sr_bits,
                              units::bits(mac_pdu.uci_bits.csi_part1_nof_bits),
                              units::bits(mac_pdu.uci_bits.harq_ack_nof_bits));
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
static void fill_format4_parameters(fapi::ul_pucch_format_4_pdu_builder& builder, const pucch_info& mac_pdu)
{
  const auto& f4     = std::get<pucch_info::f4_config>(mac_pdu.format_params);
  const auto& res_f4 = std::get<pucch_resource::f4_config>(mac_pdu.res->format_params);
  // Format 4 specific parameters.
  // Both FAPI parameters initialCyclicShift (ref. TS 38.211 6.3.2.2.2) and m0PucchDmrsCyclicShift
  // (ref. TS 38.211 6.4.1.3.3.1) map to the same value.
  const unsigned m0_format4 = get_pucch_format4_m0(static_cast<unsigned>(res_f4.occ_index));

  builder.set_modulation_parameters(res_f4.pi_2_bpsk)
      .set_hopping_parameters(f4.n_id_hopping)
      .set_occ_parameters(static_cast<uint8_t>(res_f4.occ_index), static_cast<uint8_t>(res_f4.occ_length))
      .set_scrambling_parameters(f4.n_id_scrambling)
      .set_dmrs_parameters(res_f4.additional_dmrs, f4.n_id_0_scrambling, m0_format4)
      .set_payload_parameters(mac_pdu.uci_bits.sr_bits,
                              units::bits(mac_pdu.uci_bits.csi_part1_nof_bits),
                              units::bits(mac_pdu.uci_bits.harq_ack_nof_bits));
}

static void fill_format_parameters(fapi::ul_pucch_pdu_builder& builder, const pucch_info& mac_pdu)
{
  switch (mac_pdu.format()) {
    case pucch_format::FORMAT_0: {
      fapi::ul_pucch_format_0_pdu_builder format_0_builder = builder.get_pucch_format_0_builder();
      fill_format0_parameters(format_0_builder, mac_pdu);
      break;
    }
    case pucch_format::FORMAT_1: {
      fapi::ul_pucch_format_1_pdu_builder format_1_builder = builder.get_pucch_format_1_builder();
      fill_format1_parameters(format_1_builder, mac_pdu);
      break;
    }
    case pucch_format::FORMAT_2: {
      fapi::ul_pucch_format_2_pdu_builder format_2_builder = builder.get_pucch_format_2_builder();
      fill_format2_parameters(format_2_builder, mac_pdu);
      break;
    }
    case pucch_format::FORMAT_3: {
      fapi::ul_pucch_format_3_pdu_builder format_3_builder = builder.get_pucch_format_3_builder();
      fill_format3_parameters(format_3_builder, mac_pdu);
      break;
    }
    case pucch_format::FORMAT_4: {
      fapi::ul_pucch_format_4_pdu_builder format_4_builder = builder.get_pucch_format_4_builder();
      fill_format4_parameters(format_4_builder, mac_pdu);
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
  builder.set_bwp_parameters(bwp_cfg.crbs, bwp_cfg.scs, bwp_cfg.cp)
      .set_frequency_allocation_parameters(mac_pdu.grant_prbs())
      .set_time_allocation_parameters(mac_pdu.res->syms)
      .set_hopping_information_parameters(mac_pdu.res->second_hop_prb.has_value()
                                              ? std::optional<uint16_t>(*mac_pdu.res->second_hop_prb)
                                              : std::nullopt)
      .set_multi_slot_tx_indicator(mac_pdu.repetition.has_value() ? mac_pdu.repetition->position
                                                                  : pucch_repetition_tx_slot::no_multi_slot);

  fill_format_parameters(builder, mac_pdu);
}
