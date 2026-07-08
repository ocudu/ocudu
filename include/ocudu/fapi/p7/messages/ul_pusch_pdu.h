// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "dmrs_definitions.h"
#include "ocudu/fapi/p7/messages/resource_allocation_types.h"
#include "ocudu/fapi/p7/messages/uci_part1_to_part2_correspondence.h"
#include "ocudu/ran/cyclic_prefix.h"
#include "ocudu/ran/dmrs/dmrs.h"
#include "ocudu/ran/harq_id.h"
#include "ocudu/ran/pusch/pusch_context.h"
#include "ocudu/ran/pusch/pusch_mcs.h"
#include "ocudu/ran/resource_allocation/ofdm_symbol_range.h"
#include "ocudu/ran/resource_allocation/rb_interval.h"
#include "ocudu/ran/sch/ldpc_base_graph.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include "ocudu/ran/uci/uci_configuration.h"
#include "ocudu/support/units.h"

namespace ocudu {
namespace fapi {

/// Uplink PUSCH data information.
struct ul_pusch_data {
  uint8_t      rv_index;
  harq_id_t    harq_process_id;
  bool         new_data;
  units::bytes tb_size;
};

/// Uplink PUSCH UCI information.
struct ul_pusch_uci {
  units::bits       harq_ack_bit;
  units::bits       csi_part1_bit;
  alpha_scaling_opt alpha_scaling;
  uint8_t           beta_offset_harq_ack;
  uint8_t           beta_offset_csi1;
  uint8_t           beta_offset_csi2;
};

/// Uplink PUSCH PDU information.
struct ul_pusch_pdu {
  /// Holds the parameters when Transform Precoding is enabled.
  struct transform_precoding_enabled {
    uint16_t pusch_dmrs_identity;
  };

  /// Holds the parameters when Transform Precoding is disabled.
  struct transform_precoding_disabled {
    uint8_t num_dmrs_cdm_grps_no_data;
  };

  using transform_precoding_config = std::variant<transform_precoding_enabled, transform_precoding_disabled>;

  /// Holds the parameters required for TTI bundling.
  struct tti_bundling {
    uint8_t tb_over_ms_num_total_slots;
    uint8_t tb_over_ms_remaining_slots;
  };

  rnti_t                                           rnti;
  uint32_t                                         handle = 0;
  crb_interval                                     bwp;
  subcarrier_spacing                               scs;
  cyclic_prefix                                    cp;
  float                                            target_code_rate;
  modulation_scheme                                qam_mod_order;
  uint8_t                                          mcs_index;
  pusch_mcs_table                                  mcs_table;
  uint16_t                                         nid_pusch;
  uint8_t                                          num_layers;
  dmrs_symbol_mask                                 ul_dmrs_symb_pos;
  dmrs_config_type                                 dmrs_type;
  uint16_t                                         pusch_dmrs_scrambling_id;
  transform_precoding_config                       transform_precoding;
  uint8_t                                          nscid;
  dmrs_ports_mask                                  dmrs_ports;
  resource_allocation_type_1                       resource_allocation_1;
  uint16_t                                         tx_direct_current_location;
  ofdm_symbol_range                                symbols;
  std::optional<ul_pusch_data>                     pusch_data;
  std::optional<ul_pusch_uci>                      pusch_uci;
  ldpc_base_graph_type                             ldpc_base_graph;
  units::bytes                                     tb_size_lbrm_bytes;
  std::optional<uci_part1_to_part2_correspondence> uci_correspondence;
  std::optional<uint8_t>                           rapid;
  std::optional<tti_bundling>                      repetitions;
  /// Vendor specific parameters.
  std::optional<pusch_context> context;
};

} // namespace fapi
} // namespace ocudu

namespace fmt {
template <>
struct formatter<ocudu::fapi::ul_pusch_pdu> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::fapi::ul_pusch_pdu& pdu, FormatContext& ctx) const
  {
    format_to(ctx.out(),
              "\n\t- PUSCH rnti={} bwp={} scs={} cp={} target_code_rate={} modulation={} mcs_index={} "
              "mcs_table={} nid_pusch={} num_layers={} ul_dmrs_symb_pos={} dmrs_type={} pusch_dmrs_scrambling_id={} "
              "nscid={} dmrs_ports={} tx_direct_current_location={} symb={} ldpc_base_graph={} tb_size_lbrm_bytes={} "
              "Resource Allocation Type 1 vrbs={}",
              pdu.rnti,
              pdu.bwp,
              underlying(pdu.scs),
              pdu.cp,
              pdu.target_code_rate,
              to_string(pdu.qam_mod_order),
              pdu.mcs_index,
              pusch_mcs_table_to_string(pdu.mcs_table),
              pdu.nid_pusch,
              pdu.num_layers,
              pdu.ul_dmrs_symb_pos.to_uint64(),
              underlying(pdu.dmrs_type),
              pdu.pusch_dmrs_scrambling_id,
              pdu.nscid,
              pdu.dmrs_ports.to_uint64(),
              pdu.tx_direct_current_location,
              pdu.symbols,
              underlying(pdu.ldpc_base_graph),
              pdu.tb_size_lbrm_bytes.value(),
              pdu.resource_allocation_1.vrbs);

    if (const auto* tp_enabled =
            std::get_if<ocudu::fapi::ul_pusch_pdu::transform_precoding_enabled>(&pdu.transform_precoding)) {
      format_to(ctx.out(), " Transform Precoding Enabled pusch_dmrs_identity={}", tp_enabled->pusch_dmrs_identity);
    } else if (const auto* tp_disabled =
                   std::get_if<ocudu::fapi::ul_pusch_pdu::transform_precoding_disabled>(&pdu.transform_precoding)) {
      format_to(ctx.out(),
                " Transform Precoding Disabled num_dmrs_cdm_grps_no_data={}",
                tp_disabled->num_dmrs_cdm_grps_no_data);
    }

    if (pdu.pusch_data.has_value()) {
      format_to(ctx.out(),
                " CW: rv_idx={} harq_id={} new_data={} tbs={}",
                pdu.pusch_data->rv_index,
                underlying(pdu.pusch_data->harq_process_id),
                pdu.pusch_data->new_data,
                pdu.pusch_data->tb_size);
    }

    if (pdu.pusch_uci.has_value()) {
      format_to(ctx.out(),
                " UCI: harq_bit_len={} csi1_bit_len={} alpha_scaling={} beta_offset_harq_ack={} beta_offset_csi1={} "
                "beta_offset_csi2={}",
                pdu.pusch_uci->harq_ack_bit.value(),
                pdu.pusch_uci->csi_part1_bit.value(),
                alpha_scaling_to_float(pdu.pusch_uci->alpha_scaling),
                pdu.pusch_uci->beta_offset_harq_ack,
                pdu.pusch_uci->beta_offset_csi1,
                pdu.pusch_uci->beta_offset_csi2);
    }

    return ctx.out();
  }
};
} // namespace fmt
