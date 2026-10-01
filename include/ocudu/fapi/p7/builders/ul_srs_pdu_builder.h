// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/p7/messages/ul_srs_pdu.h"

namespace ocudu {
namespace fapi {

/// Uplink SRS PDU builder that helps to fill in the parameters specified in SCF-222 v4.0 section 3.4.3.3.
class ul_srs_pdu_builder
{
  ul_srs_pdu& pdu;

public:
  explicit ul_srs_pdu_builder(ul_srs_pdu& pdu_) : pdu(pdu_) {}

  /// \brief Sets the SRS PDU UE specific parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.3 in table SRS PDU.
  ul_srs_pdu_builder& set_ue_specific_parameters(rnti_t rnti)
  {
    pdu.rnti = rnti;

    return *this;
  }

  /// \brief Sets the SRS PDU BWP parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.3 in table SRS PDU.
  ul_srs_pdu_builder& set_bwp_parameters(crb_interval bwp, subcarrier_spacing scs, cyclic_prefix cp)
  {
    pdu.bwp = bwp;
    pdu.scs = scs;
    pdu.cp  = cp;

    return *this;
  }

  /// \brief Sets the SRS PDU timing parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.3 in table SRS PDU.
  ul_srs_pdu_builder& set_timing_params(uint8_t time_start_position, srs_periodicity t_srs, uint16_t t_offset)
  {
    pdu.time_start_position = time_start_position;
    pdu.t_srs               = t_srs;
    pdu.t_offset            = t_offset;

    return *this;
  }

  /// \brief Sets the SRS PDU comb parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.3 in table SRS PDU.
  ul_srs_pdu_builder& set_comb_params(tx_comb_size comb_size, uint8_t comb_offset)
  {
    pdu.comb_size   = comb_size;
    pdu.comb_offset = comb_offset;

    return *this;
  }

  /// \brief Sets the SRS report types and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v8.0 section 3.4.3.4 in table SRS PDU.
  ul_srs_pdu_builder& set_report_params(bool enable_normalized_iq_matrix_report, bool enable_positioning_report)
  {
    pdu.enable_normalized_iq_matrix_report = enable_normalized_iq_matrix_report;
    pdu.enable_positioning_report          = enable_positioning_report;

    return *this;
  }

  /// \brief Sets the SRS PDU frequency parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.3 in table SRS PDU.
  ul_srs_pdu_builder& set_frequency_params(uint8_t                       frequency_position,
                                           uint16_t                      frequency_shift,
                                           uint8_t                       frequency_hopping,
                                           srs_group_or_sequence_hopping group_or_sequence_hopping)
  {
    pdu.frequency_position        = frequency_position;
    pdu.frequency_shift           = frequency_shift;
    pdu.frequency_hopping         = frequency_hopping;
    pdu.group_or_sequence_hopping = group_or_sequence_hopping;

    return *this;
  }

  /// \brief Sets the SRS PDU parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.3 in table SRS PDU.
  ul_srs_pdu_builder& set_srs_params(srs_resource_configuration::one_two_four_enum nof_antenna_ports,
                                     ofdm_symbol_range                             ofdm_symbols,
                                     srs_nof_symbols                               nof_repetitions,
                                     uint8_t                                       config_index,
                                     uint16_t                                      sequence_id,
                                     uint8_t                                       bandwidth_index,
                                     uint8_t                                       cyclic_shift,
                                     srs_resource_type                             resource_type)
  {
    pdu.num_ant_ports   = nof_antenna_ports;
    pdu.ofdm_symbols    = ofdm_symbols;
    pdu.num_repetitions = nof_repetitions;
    pdu.config_index    = config_index;
    pdu.sequence_id     = sequence_id;
    pdu.bandwidth_index = bandwidth_index;
    pdu.cyclic_shift    = cyclic_shift;
    pdu.resource_type   = resource_type;

    return *this;
  }
};

} // namespace fapi
} // namespace ocudu
