// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ran/cyclic_prefix.h"
#include "ocudu/ran/resource_allocation/ofdm_symbol_range.h"
#include "ocudu/ran/resource_allocation/rb_interval.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/srs/srs_configuration.h"
#include "ocudu/ran/srs/srs_resource_configuration.h"
#include "ocudu/ran/subcarrier_spacing.h"

namespace ocudu {
namespace fapi {

/// SRS PDU.
struct ul_srs_pdu {
  rnti_t                                        rnti;
  uint32_t                                      handle = 0U;
  crb_interval                                  bwp;
  subcarrier_spacing                            scs;
  cyclic_prefix                                 cp;
  srs_resource_configuration::one_two_four_enum num_ant_ports;
  ofdm_symbol_range                             ofdm_symbols;
  srs_nof_symbols                               num_repetitions;
  uint8_t                                       time_start_position;
  uint8_t                                       config_index;
  uint16_t                                      sequence_id;
  uint8_t                                       bandwidth_index;
  tx_comb_size                                  comb_size;
  uint8_t                                       comb_offset;
  uint8_t                                       cyclic_shift;
  uint8_t                                       frequency_position;
  uint16_t                                      frequency_shift;
  uint8_t                                       frequency_hopping;
  srs_group_or_sequence_hopping                 group_or_sequence_hopping;
  srs_resource_type                             resource_type;
  srs_periodicity                               t_srs;
  uint16_t                                      t_offset;
  bool                                          enable_normalized_iq_matrix_report{false};
  bool                                          enable_positioning_report{false};
};

} // namespace fapi
} // namespace ocudu

namespace fmt {
template <>
struct formatter<ocudu::fapi::ul_srs_pdu> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::fapi::ul_srs_pdu& pdu, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "\n\t- SRS rnti={} bwp={} scs={} cp={} nof_ports={} symb={}:{} config_idx={} sequence_id={} "
                     "bandwidth_index={} comb=(size={} offset={} cyclic_shift={}) freq_position={} freq_shift={} "
                     "freq_hopping={} group_or_sequence_hopping={} type={} srs_peridiocity={} slot_offset={} "
                     "normalized_channel_iq_matrix_req={} positioning_report_req={}",
                     pdu.rnti,
                     pdu.bwp,
                     to_string(pdu.scs),
                     pdu.cp.to_string(),
                     underlying(pdu.num_ant_ports),
                     pdu.time_start_position,
                     pdu.ofdm_symbols.length(),
                     pdu.config_index,
                     pdu.sequence_id,
                     pdu.bandwidth_index,
                     underlying(pdu.comb_size),
                     pdu.comb_offset,
                     pdu.cyclic_shift,
                     pdu.frequency_position,
                     pdu.frequency_shift,
                     pdu.frequency_hopping,
                     underlying(pdu.group_or_sequence_hopping),
                     to_string(pdu.resource_type),
                     underlying(pdu.t_srs),
                     pdu.t_offset,
                     pdu.enable_normalized_iq_matrix_report ? "enabled" : "disabled",
                     pdu.enable_positioning_report ? "enabled" : "disabled");
  }
};
} // namespace fmt
