// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "csi_rs.h"
#include "ocudu/adt/format.h"
#include "ocudu/ocuduvec/bit.h"
#include "ocudu/ran/csi_rs/csi_rs_config_helpers.h"
#include "ocudu/ran/csi_rs/frequency_allocation_type.h"
#include "ocudu/ran/precoding/precoding_codebooks.h"

using namespace ocudu;
using namespace fapi_adaptor;

/// Translates the \c power_control_offset_ss enum to a linear amplitude value.
static float translate_amplitude(fapi::power_control_offset_ss power)
{
  float power_dB = to_int(power);
  return convert_dB_to_amplitude(power_dB);
}

void ocudu::fapi_adaptor::convert_csi_rs_fapi_to_phy(nzp_csi_rs_generator::config_t& proc_pdu,
                                                     const fapi::dl_csi_rs_pdu&      fapi_pdu,
                                                     slot_point                      slot,
                                                     uint16_t                        cell_bandwidth_prb)
{
  proc_pdu.slot = slot;
  proc_pdu.cp   = fapi_pdu.cp;

  proc_pdu.start_rb                 = fapi_pdu.crbs.start();
  proc_pdu.nof_rb                   = std::min(static_cast<uint16_t>(fapi_pdu.crbs.length()),
                             static_cast<uint16_t>(cell_bandwidth_prb - fapi_pdu.crbs.start()));
  proc_pdu.csi_rs_mapping_table_row = fapi_pdu.row;
  csi_rs::convert_freq_domain(proc_pdu.freq_allocation_ref_idx, fapi_pdu.freq_domain, fapi_pdu.row);

  proc_pdu.symbol_l0     = fapi_pdu.symb_L0;
  proc_pdu.symbol_l1     = fapi_pdu.symb_L1;
  proc_pdu.cdm           = fapi_pdu.cdm_type;
  proc_pdu.freq_density  = fapi_pdu.freq_density;
  proc_pdu.scrambling_id = fapi_pdu.scramb_id;

  if (const auto* profile_nr = std::get_if<fapi::dl_csi_rs_pdu::power_profile_nr>(&fapi_pdu.power_config)) {
    proc_pdu.amplitude = translate_amplitude(profile_nr->pwr_control_offset_ss);
  } else if (const auto* profile_sss = std::get_if<fapi::dl_csi_rs_pdu::power_profile_sss>(&fapi_pdu.power_config)) {
    proc_pdu.amplitude = convert_dB_to_amplitude(profile_sss->pwr_offset_db);
  }

  unsigned nof_ports                 = csi_rs::get_nof_csi_rs_ports(fapi_pdu.row);
  proc_pdu.precoding_and_beamforming = precoding_beamforming_configuration::make_wideband(make_identity(nof_ports));
}

void ocudu::fapi_adaptor::get_csi_rs_pattern_from_fapi_pdu(csi_rs_pattern&            pattern,
                                                           const fapi::dl_csi_rs_pdu& fapi_pdu,
                                                           uint16_t                   cell_bandwidth_prb)
{
  // Fill the CSI-RS pattern configuration.
  csi_rs_pattern_configuration config;

  config.start_rb                 = fapi_pdu.crbs.start();
  config.nof_rb                   = std::min(static_cast<uint16_t>(fapi_pdu.crbs.length()),
                           static_cast<uint16_t>(cell_bandwidth_prb - fapi_pdu.crbs.start()));
  config.csi_rs_mapping_table_row = fapi_pdu.row;
  csi_rs::convert_freq_domain(config.freq_allocation_ref_idx, fapi_pdu.freq_domain, fapi_pdu.row);

  config.symbol_l0    = fapi_pdu.symb_L0;
  config.symbol_l1    = fapi_pdu.symb_L1;
  config.cdm          = fapi_pdu.cdm_type;
  config.freq_density = fapi_pdu.freq_density;

  // Get the CSI-RS pattern.
  pattern = get_csi_rs_pattern(config);
}
