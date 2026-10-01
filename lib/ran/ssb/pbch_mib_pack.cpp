// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/ssb/pbch_mib_pack.h"
#include "ocudu/ran/ssb/pbch_mib_message.h"
#include "ocudu/ran/ssb/ssb_properties.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include <type_traits>

using namespace ocudu;

uint8_t ocudu::pbch_timing_pack(uint32_t                     sfn,
                                bool                         hrf,
                                uint8_t                      ssb_block_index,
                                ocudu::ssb_subcarrier_offset subcarrier_offset,
                                unsigned                     L_max)
{
  uint8_t payload = 0;

  // Add the SFN. - 4 bit LSB.
  payload |= ((sfn & 0xfU) << 4U);

  // Half-radio frame bit.
  payload |= (hrf << 3U);

  if (L_max == 64) {
    // Pack the 6th, 5th and 4th bits of SS/PBCH block index.
    payload |= ((ssb_block_index >> 3U) & 7U);
  } else {
    // 3rd LSB set to MSB of the SSB subcarrier offset. 2nd and 1st bits are reserved.
    payload |= (((subcarrier_offset.value() >> 4U) & 1U) << 2U);
  }

  return payload;
}

uint32_t ocudu::pbch_mib_pack(const pbch_mib_message& msg)
{
  uint32_t payload = 0;

  // CHOICE in BCCH-BCH-MessageType - 1 bit.
  // Set to zero.

  // Pack msg.

  // systemFrameNumber - 6 bits MSB.
  payload |= (((msg.sfn >> 4U) & 0x3fU) << 25U);

  // subCarrierSpacingCommon - 1 bit.
  bool scs_flag = (msg.scs_common == subcarrier_spacing::kHz30 || msg.scs_common == subcarrier_spacing::kHz120);
  payload |= (scs_flag << 24U);

  // ssb-SubcarrierOffset - 4 bits.
  payload |= ((msg.subcarrier_offset.value() & 0xfU) << 20U);

  // dmrs-TypeA-Position - 1 bit.
  payload |= ((static_cast<std::underlying_type_t<dmrs_typeA_position>>(msg.dmrs_typeA_pos) & 0x1U) << 19U);

  // pdcch-ConfigSIB1 - 8 bits.
  payload |= ((msg.pdcch_config_sib1 & 0xffU) << 11U);

  // Barred - 1 bit.
  payload |= ((msg.cell_barred ? 0U : 1U) << 10U);

  // intraFreqReselection - 1 bit.
  payload |= ((msg.intrafreq_reselection ? 0U : 1U) << 9U);

  // Spare - 1 bit.
  // Leave to zero.

  return payload;
}

uint32_t ocudu::pbch_mib_and_timing_pack(const pbch_mib_message& msg, unsigned L_max)
{
  return (pbch_mib_pack(msg) | pbch_timing_pack(msg.sfn, msg.hrf, msg.ssb_block_index, msg.subcarrier_offset, L_max));
}
