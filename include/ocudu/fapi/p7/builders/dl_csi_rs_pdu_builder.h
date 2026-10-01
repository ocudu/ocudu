// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/p7/messages/dl_csi_rs_pdu.h"

namespace ocudu {
namespace fapi {

/// CSI-RS PDU builder that helps to fill in the parameters specified in SCF-222 v4.0 section 3.4.2.3.
class dl_csi_rs_pdu_builder
{
  dl_csi_rs_pdu& pdu;

public:
  explicit dl_csi_rs_pdu_builder(dl_csi_rs_pdu& pdu_) : pdu(pdu_) {}

  /// \brief Sets the CSI-RS PDU resource configuration parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.3 in table CSI-RS PDU.
  dl_csi_rs_pdu_builder&
  set_csi_resource_config_parameters(csi_rs_type type, uint8_t row, csi_rs_cdm_type cdm_type, uint16_t scrambling_id)
  {
    pdu.type      = type;
    pdu.row       = row;
    pdu.cdm_type  = cdm_type;
    pdu.scramb_id = scrambling_id;

    return *this;
  }

  /// \brief Sets the CSI-RS PDU frequency domain parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.3 in table CSI-RS PDU.
  dl_csi_rs_pdu_builder& set_frequency_domain_parameters(csi_rs::freq_allocation_mask_type freq_domain,
                                                         csi_rs_freq_density_type          freq_density)
  {
    pdu.freq_domain  = freq_domain;
    pdu.freq_density = freq_density;

    return *this;
  }

  /// \brief Sets the CSI-RS PDU time domain parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.3 in table CSI-RS PDU.
  dl_csi_rs_pdu_builder& set_time_domain_parameters(uint8_t symb_l0, uint8_t symb_l1)
  {
    pdu.symb_L0 = symb_l0;
    pdu.symb_L1 = symb_l1;

    return *this;
  }

  /// \brief Sets the CSI-RS PDU resource block parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.3 in table CSI-RS PDU.
  dl_csi_rs_pdu_builder& set_resource_block_parameters(crb_interval crbs)
  {
    pdu.crbs = crbs;

    return *this;
  }

  /// \brief Sets the CSI-RS PDU BWP parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.3 in table CSI-RS PDU.
  dl_csi_rs_pdu_builder& set_bwp_parameters(subcarrier_spacing scs, cyclic_prefix cp, crb_interval bwp)
  {
    pdu.scs = scs;
    pdu.cp  = cp;
    pdu.bwp = bwp;

    return *this;
  }

  /// \brief Sets the CSI-RS PDU NR tx power info parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.3 in table CSI-RS PDU.
  dl_csi_rs_pdu_builder& set_profile_nr_tx_power_info_parameters(int                     pwr_control_offset,
                                                                 power_control_offset_ss pwr_control_offset_ss)
  {
    auto& power = pdu.power_config.emplace<dl_csi_rs_pdu::power_profile_nr>();

    power.pwr_control_offset_db = pwr_control_offset;
    power.pwr_control_offset_ss = pwr_control_offset_ss;

    return *this;
  }

  /// \brief Sets the CSI-RS PDU SSS tx power info parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.3 in table CSI-RS PDU.
  dl_csi_rs_pdu_builder& set_profile_sss_tx_power_info_parameters(float pwr_offset_db)
  {
    auto& power         = pdu.power_config.emplace<dl_csi_rs_pdu::power_profile_sss>();
    power.pwr_offset_db = pwr_offset_db;

    return *this;
  }
};

} // namespace fapi
} // namespace ocudu
