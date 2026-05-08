// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ssb.h"
#include "ocudu/fapi/p7/builders/dl_ssb_pdu_builder.h"
#include "ocudu/ocuduvec/bit.h"
#include "ocudu/ocuduvec/compare.h"
#include "ocudu/ran/ssb/pbch_mib_pack.h"
#include <gtest/gtest.h>
#include <random>

using namespace ocudu;
using namespace fapi_adaptor;

static std::mt19937 gen(0);

static uint32_t generate_bch_payload(ssb_subcarrier_offset subcarrier_offset,
                                     dmrs_typeA_position   dmrs_type_a_position,
                                     uint8_t               pdcch_config_sib1,
                                     bool                  cell_barred,
                                     bool                  intra_freq_reselection,
                                     uint32_t              sfn,
                                     bool                  hrf,
                                     subcarrier_spacing    scs_common)
{
  // Prepare message parameters.
  pbch_mib_message msg = {.sfn                   = sfn,
                          .hrf                   = hrf,
                          .scs_common            = scs_common,
                          .subcarrier_offset     = subcarrier_offset,
                          .dmrs_typeA_pos        = dmrs_type_a_position,
                          .pdcch_config_sib1     = pdcch_config_sib1,
                          .cell_barred           = cell_barred,
                          .intrafreq_reselection = intra_freq_reselection,
                          .ssb_block_index       = 0};

  // Generate payload.
  return pbch_mib_pack(msg);
}

TEST(fapi_to_phy_ssb_conversion_test, valid_pdu_conversion_success)
{
  // Random generators.
  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> slot_dist(0, 160);
  std::uniform_int_distribution<unsigned> pci_dist(0, 3000);
  std::uniform_int_distribution<uint8_t>  eight_bit_dist(0, UINT8_MAX);
  std::uniform_int_distribution<unsigned> binary_dist(0, 1);
  std::uniform_int_distribution<unsigned> dmrs_type_A_dist(2, 3);
  std::uniform_int_distribution<unsigned> subcarrier_offset_dist(0, 23);
  std::uniform_int_distribution<unsigned> offset_pointA_dist(0, 2199);
  std::uniform_int_distribution<unsigned> sib1_dist(0, 255);
  std::uniform_real_distribution<float>   power_scaling_ss_pbch_dist(-110, 120);
  std::uniform_real_distribution<float>   pss_to_sss_ratio_dist(-32.767, 32.767);

  for (unsigned lmax : {4u, 8u, 64u}) {
    for (unsigned ssb_idx = 0; ssb_idx != lmax; ++ssb_idx) {
      for (subcarrier_spacing scs : {subcarrier_spacing::kHz15,
                                     subcarrier_spacing::kHz30,
                                     subcarrier_spacing::kHz120,
                                     subcarrier_spacing::kHz240}) {
        for (subcarrier_spacing common_scs : {subcarrier_spacing::kHz15,
                                              subcarrier_spacing::kHz30,
                                              subcarrier_spacing::kHz60,
                                              subcarrier_spacing::kHz120}) {
          for (ssb_pss_to_sss_epre beta_pss : {ssb_pss_to_sss_epre::dB_0, ssb_pss_to_sss_epre::dB_3}) {
            for (ssb_pattern_case pattern_case : {ssb_pattern_case::A,
                                                  ssb_pattern_case::B,
                                                  ssb_pattern_case::C,
                                                  ssb_pattern_case::D,
                                                  ssb_pattern_case::E}) {
              slot_point slot(
                  static_cast<uint32_t>(scs), sfn_dist(gen), slot_dist(gen) % (10 << static_cast<uint32_t>(scs)));

              unsigned              sfn                    = slot.sfn();
              unsigned              pci                    = pci_dist(gen);
              ssb_subcarrier_offset subcarrier_offset      = subcarrier_offset_dist(gen);
              unsigned              offset_pointA          = offset_pointA_dist(gen);
              dmrs_typeA_position   dmrs_type_a_position   = static_cast<dmrs_typeA_position>(dmrs_type_A_dist(gen));
              uint8_t               pdcch_config_sib1      = eight_bit_dist(gen);
              bool                  cell_barred            = binary_dist(gen);
              bool                  intra_freq_reselection = binary_dist(gen);

              fapi::dl_ssb_pdu         fapi_pdu;
              fapi::dl_ssb_pdu_builder builder(fapi_pdu);
              builder.set_carrier_parameters(scs)
                  .set_cell_parameters(pci)
                  .set_nr_power_parameters(beta_pss)
                  .set_ssb_parameters(ssb_idx, subcarrier_offset, offset_pointA, pattern_case, lmax);

              uint32_t mib_payload = generate_bch_payload(subcarrier_offset,
                                                          dmrs_type_a_position,
                                                          pdcch_config_sib1,
                                                          cell_barred,
                                                          intra_freq_reselection,
                                                          sfn,
                                                          slot.is_odd_hrf(),
                                                          slot.scs()) >>
                                     8;
              builder.set_bch_payload_phy_timing_info(mib_payload);

              // PHY processor PDU.
              ssb_processor::pdu_t pdu;

              // Conversion block.
              convert_ssb_fapi_to_phy(pdu, fapi_pdu, slot, common_scs);

              // Assert contents.
              ASSERT_EQ(pdu.slot.sfn(), sfn);
              ASSERT_EQ(pdu.slot.slot_index(), slot.slot_index());
              ASSERT_EQ(pdu.phys_cell_id, pci);
              ASSERT_EQ(pdu.beta_pss, beta_pss);
              ASSERT_EQ(pdu.ssb_idx, ssb_idx);
              ASSERT_EQ(pdu.L_max, lmax);
              ASSERT_EQ(pdu.subcarrier_offset, subcarrier_offset);
              ASSERT_EQ(pdu.offset_to_pointA.value(), offset_pointA);
              ASSERT_EQ(pdu.pattern_case, pattern_case);
              ASSERT_TRUE(ocuduvec::equal(pdu.ports, std::vector<uint8_t>{0}));

              std::array<uint8_t, ssb_processor::MIB_PAYLOAD_SIZE> dest;
              ocuduvec::bit_unpack(dest, mib_payload, dest.size());
              ASSERT_EQ(pdu.mib_payload, dest);
            }
          }
        }
      }
    }
  }
}
