// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/fapi/p7/messages/ul_pusch_pdu.h"
#include "ocudu/ran/dmrs/dmrs.h"

namespace ocudu {
namespace fapi {

/// PUSCH PDU builder that helps to fill in the parameters specified in SCF-222 v4.0 section 3.4.3.2.
class ul_pusch_pdu_builder
{
  ul_pusch_pdu& pdu;

public:
  explicit ul_pusch_pdu_builder(ul_pusch_pdu& pdu_) : pdu(pdu_) { pdu.ul_dmrs_symb_pos.reset(); }

  /// \brief Sets the PUSCH PDU UE specific parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.2 in table PUSCH PDU.
  ul_pusch_pdu_builder& set_ue_specific_parameters(rnti_t rnti)
  {
    pdu.rnti = rnti;

    return *this;
  }

  /// \brief Sets the PUSCH PDU BWP parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.2 in table PUSCH PDU.
  ul_pusch_pdu_builder& set_bwp_parameters(crb_interval bwp, subcarrier_spacing scs, cyclic_prefix cp)
  {
    pdu.bwp = bwp;
    pdu.scs = scs;
    pdu.cp  = cp;

    return *this;
  }

  /// \brief Sets the PUSCH PDU information parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.2 in table PUSCH PDU.
  ul_pusch_pdu_builder& set_information_parameters(float             target_code_rate,
                                                   modulation_scheme qam_mod_order,
                                                   uint8_t           mcs_index,
                                                   pusch_mcs_table   mcs_table,
                                                   uint16_t          nid_pusch,
                                                   uint8_t           num_layers)
  {
    pdu.target_code_rate = target_code_rate;
    pdu.qam_mod_order    = qam_mod_order;
    pdu.mcs_index        = mcs_index;
    pdu.mcs_table        = mcs_table;
    pdu.nid_pusch        = nid_pusch;
    pdu.num_layers       = num_layers;

    return *this;
  }

  /// \brief Sets the PUSCH PDU transforn precoding enabled parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.2 in table PUSCH PDU.
  ul_pusch_pdu_builder& set_transform_precoding_enabled_parameters(uint16_t pusch_dmrs_identity)
  {
    auto& transform_precoding = pdu.transform_precoding.emplace<ul_pusch_pdu::transform_precoding_enabled>();
    transform_precoding.pusch_dmrs_identity = pusch_dmrs_identity;

    return *this;
  }

  /// \brief Sets the PUSCH PDU transform precoding disabled parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.2 in table PUSCH PDU.
  ul_pusch_pdu_builder& set_transform_precoding_disabled_parameters(uint8_t num_dmrs_cdm_grps_no_data)
  {
    auto& transform_precoding = pdu.transform_precoding.emplace<ul_pusch_pdu::transform_precoding_disabled>();
    transform_precoding.num_dmrs_cdm_grps_no_data = num_dmrs_cdm_grps_no_data;

    return *this;
  }

  /// \brief Sets the PUSCH PDU DMRS parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.2 in table PUSCH PDU.
  ul_pusch_pdu_builder& set_dmrs_parameters(dmrs_symbol_mask ul_dmrs_symb_pos,
                                            dmrs_config_type dmrs_type,
                                            uint16_t         pusch_dmrs_scrambling_id,
                                            uint8_t          nscid,
                                            dmrs_ports_mask  dmrs_ports)
  {
    pdu.ul_dmrs_symb_pos         = ul_dmrs_symb_pos;
    pdu.dmrs_type                = dmrs_type;
    pdu.pusch_dmrs_scrambling_id = pusch_dmrs_scrambling_id;
    pdu.nscid                    = nscid;
    pdu.dmrs_ports               = dmrs_ports;

    return *this;
  }

  /// \brief Sets the PUSCH PDU time allocation parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.2 in table PUSCH PDU.
  ul_pusch_pdu_builder& set_time_allocation_parameters(ofdm_symbol_range symbols)
  {
    pdu.symbols = symbols;

    return *this;
  }

  /// \brief Sets the PUSCH PDU frequency domain allocation parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.2 in table PUSCH maintenance FAPIv3.
  ul_pusch_pdu_builder& set_frequency_allocation_parameters(vrb_interval         vrbs,
                                                            uint16_t             tx_direct_current_location,
                                                            ldpc_base_graph_type ldpc_graph,
                                                            units::bytes         tb_size_lbrm_bytes)
  {
    pdu.resource_allocation_1.vrbs = vrbs;
    pdu.tx_direct_current_location = tx_direct_current_location;
    pdu.ldpc_base_graph            = ldpc_graph;
    pdu.tb_size_lbrm_bytes         = tb_size_lbrm_bytes;

    return *this;
  }

  /// \brief Adds a UCI part1 to part2 correspondence to the PUSCH PDU and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.2 in table UCI information for determining UCI Part1
  /// to PArt2 correspondence, added in FAPIv3.
  ul_pusch_pdu_builder& add_uci_part1_part2_correspondence(span<const uint16_t> param_offset,
                                                           span<const uint8_t>  param_sizes,
                                                           uint16_t             part2_size_map_index)
  {
    ocudu_assert(param_offset.size() == param_sizes.size(),
                 "Mismatching span sizes for param offset ({}) and param sizes ({})",
                 param_offset.size(),
                 param_sizes.size());

    if (!pdu.uci_correspondence.has_value()) {
      pdu.uci_correspondence.emplace();
    }
    auto& part2 = pdu.uci_correspondence->part2.emplace_back();

    part2.part2_size_map_index = part2_size_map_index;

    part2.param_offsets.assign(param_offset.begin(), param_offset.end());
    part2.param_sizes.assign(param_sizes.begin(), param_sizes.end());

    return *this;
  }

  /// \brief Adds optional PUSCH data information to the PUSCH PDU and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.2 in table optional PUSCH data information.
  ul_pusch_pdu_builder&
  add_optional_pusch_data(uint8_t rv_index, harq_id_t harq_process_id, bool new_data, units::bytes tb_size)
  {
    pdu.pusch_data = std::make_optional(ul_pusch_data{
        .rv_index = rv_index, .harq_process_id = harq_process_id, .new_data = new_data, .tb_size = tb_size});

    return *this;
  }

  /// \brief Adds optional PUSCH UCI information to the PUSCH PDU and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.2 in table optional PUSCH UCI information.
  ul_pusch_pdu_builder& add_optional_pusch_uci(units::bits       harq_ack_bit,
                                               units::bits       csi_part1_bit,
                                               alpha_scaling_opt alpha_scaling,
                                               uint8_t           beta_offset_harq_ack,
                                               uint8_t           beta_offset_csi1,
                                               uint8_t           beta_offset_csi2)
  {
    pdu.pusch_uci = std::make_optional(ul_pusch_uci{.harq_ack_bit         = harq_ack_bit,
                                                    .csi_part1_bit        = csi_part1_bit,
                                                    .alpha_scaling        = alpha_scaling,
                                                    .beta_offset_harq_ack = beta_offset_harq_ack,
                                                    .beta_offset_csi1     = beta_offset_csi1,
                                                    .beta_offset_csi2     = beta_offset_csi2});

    return *this;
  }

  /// \brief Adds optional PUSCH RAPID to the PUSCH PDU and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.2 in table optional PUSCH data information.
  ul_pusch_pdu_builder& add_optional_rapid(uint8_t rapid)
  {
    pdu.rapid = rapid;

    return *this;
  }

  /// \brief Adds optional TTI Bundling parameters to the PUSCH PDU and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.2 in table 3.4.3.2-10.
  ul_pusch_pdu_builder& add_tti_bundling_parameters(uint8_t tb_over_ms_num_total_slots,
                                                    uint8_t tb_over_ms_remaining_slots)
  {
    pdu.repetitions.emplace(ul_pusch_pdu::tti_bundling{.tb_over_ms_num_total_slots = tb_over_ms_num_total_slots,
                                                       .tb_over_ms_remaining_slots = tb_over_ms_remaining_slots});

    return *this;
  }

  /// Sets the PUSCH context as vendor specific.
  ul_pusch_pdu_builder& set_context_vendor_specific(rnti_t rnti, harq_id_t harq_id)
  {
    pdu.context = pusch_context(rnti, harq_id);
    return *this;
  }
};

} // namespace fapi
} // namespace ocudu
