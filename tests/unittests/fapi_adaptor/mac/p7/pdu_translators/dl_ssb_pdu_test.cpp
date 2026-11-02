// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "helpers.h"
#include "ssb.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi_adaptor;
using namespace unittests;

/// Number of PRBs of the cell, which is also the size of the precoding resource block groups.
static constexpr unsigned nof_prbs = 51;

/// Converts the given MAC SSB PDU, transmitted in the given slot, into a FAPI one.
static fapi::dl_ssb_pdu convert_ssb(const ocudu::dl_ssb_pdu& mac_pdu, slot_point slot)
{
  fapi::dl_ssb_pdu         fapi_pdu;
  fapi::dl_ssb_pdu_builder builder(fapi_pdu);
  convert_ssb_mac_to_fapi(builder, mac_pdu, slot, nof_prbs);
  return fapi_pdu;
}

TEST(mac_fapi_ssb_pdu_conversor_test, valid_pdu_should_pass)
{
  ocudu::dl_ssb_pdu pdu = build_valid_dl_ssb_pdu();
  slot_point        slot(1, 0);

  fapi::dl_ssb_pdu fapi_pdu = convert_ssb(pdu, slot);

  EXPECT_EQ(pdu.pci, fapi_pdu.phys_cell_id);
  const auto* profile_nr = std::get_if<fapi::dl_ssb_pdu::power_profile_nr>(&fapi_pdu.power_config);
  ASSERT_NE(profile_nr, nullptr);
  EXPECT_EQ(static_cast<unsigned>(pdu.pss_to_sss_epre), static_cast<unsigned>(profile_nr->beta_pss));
  EXPECT_EQ(pdu.ssb_index, fapi_pdu.ssb_block_index);
  EXPECT_EQ(pdu.subcarrier_offset, fapi_pdu.subcarrier_offset);
  EXPECT_EQ(pdu.offset_to_pointA.value(), fapi_pdu.ssb_offset_pointA.value());
  EXPECT_EQ(static_cast<unsigned>(pdu.ssb_case), static_cast<unsigned>(fapi_pdu.case_type));
  EXPECT_EQ(pdu.L_max, fapi_pdu.L_max);
  EXPECT_EQ(static_cast<unsigned>(pdu.scs), static_cast<unsigned>(fapi_pdu.scs));
  EXPECT_EQ(precoding_beam_list({std::get<beam_identifier>(pdu.precoding_and_beamforming)}),
            fapi_pdu.precoding_and_beamforming.prg.beams);
  EXPECT_EQ(nof_prbs, fapi_pdu.precoding_and_beamforming.prg_size);
}

TEST(mac_fapi_ssb_pdu_conversor_test, bch_payload_packs_the_mib_fields)
{
  ocudu::dl_ssb_pdu pdu               = build_valid_dl_ssb_pdu();
  pdu.subcarrier_offset               = 5;
  pdu.mib_data.dmrs_typeA_pos         = dmrs_typeA_position::pos3;
  pdu.mib_data.pdcch_config_sib1      = 0xa5;
  pdu.mib_data.cell_barred            = false;
  pdu.mib_data.intra_freq_reselection = true;

  // SFN 688 has the 6 MSBs 0b101011 and the 4 LSBs set to zero, and the common SCS is 30 kHz.
  fapi::dl_ssb_pdu fapi_pdu = convert_ssb(pdu, slot_point(subcarrier_spacing::kHz30, 688, 0));

  // The 24 bits of the MIB, as per TS 38.331 Section 6.2.2, from the MSB:
  //   - Message type: 0.
  //   - SFN 6 MSBs: 101011.
  //   - Common SCS of 30 kHz: 1.
  //   - SSB subcarrier offset 4 LSBs: 0101.
  //   - DMRS type A position 3: 1.
  //   - PDCCH-ConfigSIB1: 10100101.
  //   - Cell not barred: 1.
  //   - Intra-frequency reselection allowed: 0.
  //   - Spare: 0.
  static constexpr uint32_t expected_mib = 0b0'101011'1'0101'1'10100101'1'0'0;
  EXPECT_EQ(expected_mib, fapi_pdu.bch_payload);
}

/// Returns the BCH payload that the translator generates for the given SS/PBCH block transmitted in the given slot.
static uint32_t convert_bch_payload(const ocudu::dl_ssb_pdu& pdu, slot_point slot)
{
  return convert_ssb(pdu, slot).bch_payload;
}

TEST(mac_fapi_ssb_pdu_conversor_test, bch_payload_carries_the_sfn_msbs_and_not_the_phy_timing_bits)
{
  // The BCH payload sent in FAPI holds the 24 bits of the MIB, with the 6 MSBs of the SFN, and leaves out the 8 PHY
  // timing bits, which hold the 4 LSBs of the SFN and the half frame bit. SCS 30kHz has 10 slots per half frame.
  static constexpr unsigned first_slot_of_second_half_frame = 10;

  ocudu::dl_ssb_pdu pdu       = build_valid_dl_ssb_pdu();
  uint32_t          reference = convert_bch_payload(pdu, slot_point(subcarrier_spacing::kHz30, 0, 0));

  // The SFN LSBs and the half frame do not change the payload.
  for (unsigned sfn = 1; sfn != 16; ++sfn) {
    SCOPED_TRACE(sfn);
    EXPECT_EQ(reference, convert_bch_payload(pdu, slot_point(subcarrier_spacing::kHz30, sfn, 0)));
  }
  EXPECT_EQ(reference,
            convert_bch_payload(pdu, slot_point(subcarrier_spacing::kHz30, 0, first_slot_of_second_half_frame)));

  // The SFN MSBs do.
  EXPECT_NE(reference, convert_bch_payload(pdu, slot_point(subcarrier_spacing::kHz30, 16, 0)));
  EXPECT_NE(reference, convert_bch_payload(pdu, slot_point(subcarrier_spacing::kHz30, 512, 0)));
  EXPECT_NE(convert_bch_payload(pdu, slot_point(subcarrier_spacing::kHz30, 16, 0)),
            convert_bch_payload(pdu, slot_point(subcarrier_spacing::kHz30, 512, 0)));
}
