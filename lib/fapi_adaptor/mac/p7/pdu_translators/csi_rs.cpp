// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "csi_rs.h"
#include "ocudu/adt/format.h"

using namespace ocudu;
using namespace fapi_adaptor;

void ocudu::fapi_adaptor::convert_csi_rs_mac_to_fapi(fapi::dl_tti_request_builder& builder,
                                                     const csi_rs_info&            csi_rs_pdu)
{
  fapi::dl_csi_rs_pdu_builder csi_builder = builder.add_csi_rs_pdu();

  csi_builder
      .set_csi_resource_config_parameters(
          csi_rs_pdu.type, csi_rs_pdu.row, csi_rs_pdu.cdm_type, csi_rs_pdu.scrambling_id)
      .set_frequency_domain_parameters(csi_rs_pdu.freq_domain, csi_rs_pdu.freq_density)
      .set_time_domain_parameters(csi_rs_pdu.symbol0, csi_rs_pdu.symbol1)
      .set_resource_block_parameters(csi_rs_pdu.crbs)
      .set_bwp_parameters(csi_rs_pdu.bwp_cfg->scs, csi_rs_pdu.bwp_cfg->cp, csi_rs_pdu.bwp_cfg->crbs)
      .set_profile_nr_tx_power_info_parameters(csi_rs_pdu.power_ctrl_offset,
                                               fapi::to_power_control_offset_ss(csi_rs_pdu.power_ctrl_offset_ss));
}
