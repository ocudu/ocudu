// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ssb.h"
#include "ocudu/ocuduvec/bit.h"

using namespace ocudu;
using namespace fapi_adaptor;

/// Unpacks the contents of the BCH payload.
static void unpack_bch_payload(span<uint8_t> dest, const fapi::dl_ssb_pdu& fapi_pdu)
{
  ocuduvec::bit_unpack(dest, fapi_pdu.bch_payload, dest.size());
}

/// Returns the coefficient \f$\beta_{PSS}\f$ from the given SSB PDU (see TS38.213, Section 4.1).
static ssb_pss_to_sss_epre convert_to_beta_pss(const fapi::dl_ssb_pdu& fapi_pdu)
{
  if (const auto* profile_nr = std::get_if<fapi::dl_ssb_pdu::power_profile_nr>(&fapi_pdu.power_config)) {
    return profile_nr->beta_pss;
  }

  if (const auto* profile_sss = std::get_if<fapi::dl_ssb_pdu::power_profile_sss>(&fapi_pdu.power_config)) {
    float beta_pss_db = profile_sss->beta_pss_db;
    ocudu_assert((beta_pss_db == 3.0F) || (beta_pss_db = 0.0F),
                 "Invalid beta PSS value {} dB, expected {{{} dB, {} dB}}.",
                 beta_pss_db,
                 0.0F,
                 3.0F);
    return (profile_sss->beta_pss_db == 3.0F) ? ssb_pss_to_sss_epre::dB_3 : ssb_pss_to_sss_epre::dB_0;
  }

  return ssb_pss_to_sss_epre::dB_0;
}

void ocudu::fapi_adaptor::convert_ssb_fapi_to_phy(ssb_processor::pdu_t&   proc_pdu,
                                                  const fapi::dl_ssb_pdu& fapi_pdu,
                                                  slot_point              slot,
                                                  subcarrier_spacing      scs_common)
{
  proc_pdu.slot              = slot;
  proc_pdu.phys_cell_id      = fapi_pdu.phys_cell_id;
  proc_pdu.beta_pss          = convert_to_beta_pss(fapi_pdu);
  proc_pdu.ssb_idx           = fapi_pdu.ssb_block_index;
  proc_pdu.L_max             = fapi_pdu.L_max;
  proc_pdu.subcarrier_offset = fapi_pdu.subcarrier_offset;
  proc_pdu.offset_to_pointA  = fapi_pdu.ssb_offset_pointA;
  proc_pdu.pattern_case      = fapi_pdu.case_type;
  proc_pdu.common_scs        = scs_common;

  unpack_bch_payload(proc_pdu.mib_payload, fapi_pdu);

  // Use only a single port for SSB.
  proc_pdu.ports = {0};
}
