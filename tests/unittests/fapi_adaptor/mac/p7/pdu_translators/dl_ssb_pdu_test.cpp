// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "helpers.h"
#include "ssb.h"
#include "ocudu/mac/mac_cell_result.h"
#include "ocudu/ran/ssb/pbch_mib_pack.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi_adaptor;
using namespace unittests;

static uint32_t generate_bch_payload(const dl_ssb_pdu& mac_pdu, uint32_t sfn, bool hrf, subcarrier_spacing scs_common)
{
  // Prepare message parameters.
  pbch_mib_message msg = {.sfn                   = sfn,
                          .hrf                   = hrf,
                          .scs_common            = scs_common,
                          .subcarrier_offset     = mac_pdu.subcarrier_offset,
                          .dmrs_typeA_pos        = mac_pdu.mib_data.dmrs_typeA_pos,
                          .pdcch_config_sib1     = mac_pdu.mib_data.pdcch_config_sib1,
                          .cell_barred           = mac_pdu.mib_data.cell_barred,
                          .intrafreq_reselection = mac_pdu.mib_data.intra_freq_reselection,
                          .ssb_block_index       = mac_pdu.ssb_index};

  // Generate payload.
  return pbch_mib_pack(msg);
}

TEST(mac_fapi_ssb_pdu_conversor_test, valid_pdu_should_pass)
{
  ocudu::dl_ssb_pdu pdu = build_valid_dl_ssb_pdu();

  fapi::dl_ssb_pdu         fapi_pdu;
  fapi::dl_ssb_pdu_builder builder(fapi_pdu);
  slot_point               slot(1, 0);
  convert_ssb_mac_to_fapi(builder, pdu, slot_point(1, 0));

  ASSERT_EQ(pdu.pci, fapi_pdu.phys_cell_id);
  const auto* profile_nr = std::get_if<fapi::dl_ssb_pdu::power_profile_nr>(&fapi_pdu.power_config);
  ASSERT_TRUE(profile_nr != nullptr);
  ASSERT_EQ(static_cast<unsigned>(pdu.pss_to_sss_epre), static_cast<unsigned>(profile_nr->beta_pss));
  ASSERT_EQ(pdu.ssb_index, fapi_pdu.ssb_block_index);
  ASSERT_EQ(pdu.subcarrier_offset, fapi_pdu.subcarrier_offset);
  ASSERT_EQ(pdu.offset_to_pointA.value(), fapi_pdu.ssb_offset_pointA.value());
  ASSERT_EQ(static_cast<unsigned>(pdu.ssb_case), static_cast<unsigned>(fapi_pdu.case_type));
  ASSERT_EQ(pdu.L_max, fapi_pdu.L_max);
  ASSERT_EQ(static_cast<unsigned>(pdu.scs), static_cast<unsigned>(fapi_pdu.scs));
  ASSERT_EQ(std::get<beam_identifier>(pdu.precoding_and_beamforming), fapi_pdu.beam_id);

  // MIB.
  ASSERT_EQ(generate_bch_payload(pdu, slot.sfn(), slot.is_odd_hrf(), slot.scs()) >> 8, fapi_pdu.bch_payload);
}
