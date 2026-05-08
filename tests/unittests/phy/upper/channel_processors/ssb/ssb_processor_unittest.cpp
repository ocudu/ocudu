// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../../../support/resource_grid_test_doubles.h"
#include "../../signal_processors/ssb/dmrs_pbch_processor_doubles.h"
#include "../../signal_processors/ssb/pss_processor_doubles.h"
#include "../../signal_processors/ssb/sss_processor_doubles.h"
#include "pbch_encoder_doubles.h"
#include "pbch_modulator_doubles.h"
#include "ocudu/adt/to_array.h"
#include "ocudu/ocuduvec/compare.h"
#include "ocudu/phy/phys_cell_id.h"
#include "ocudu/phy/upper/channel_processors/ssb/ssb_processor.h"
#include "ocudu/ran/cyclic_prefix.h"
#include "ocudu/ran/ssb/ssb_mapping.h"
#include <gtest/gtest.h>
#include <random>

using namespace ocudu;

namespace {

struct test_parameters {
  ssb_pattern_case    pattern_case;
  unsigned            L_max;
  ssb_pss_to_sss_epre beta_pss;
  subcarrier_spacing  scs;

  test_parameters(ssb_pattern_case pc, unsigned l, ssb_pss_to_sss_epre b, subcarrier_spacing scs_) :
    pattern_case(pc), L_max(l), beta_pss(b), scs(scs_)
  {
  }
};

std::ostream& operator<<(std::ostream& os, const test_parameters& tp)
{
  return os << fmt::format("SSB pattern case = {}, L_max = {}, beta_pss = {}, SCS = {}",
                           to_string(tp.pattern_case),
                           tp.L_max,
                           ssb_pss_to_sss_epre_to_dB(tp.beta_pss),
                           to_string(tp.scs));
}

class SsbProcessorFixture : public testing::TestWithParam<test_parameters>
{
protected:
  void SetUp() override
  {
    // Extract parameters.
    const test_parameters& params = GetParam();
    pattern_case                  = params.pattern_case;
    L_max                         = params.L_max;
    beta_pss                      = params.beta_pss;
    common_scs                    = params.scs;

    std::shared_ptr<pbch_encoder_factory_spy> encoder_factory_spy = std::make_shared<pbch_encoder_factory_spy>();
    ASSERT_TRUE(encoder_factory_spy) << "Failed to create PBCH encoder factory.";

    std::shared_ptr<pbch_modulator_factory_spy> modulator_factory_spy = std::make_shared<pbch_modulator_factory_spy>();
    ASSERT_TRUE(modulator_factory_spy) << "Failed to create PBCH modulator factory.";

    std::shared_ptr<dmrs_pbch_processor_factory_spy> dmrs_factory_spy =
        std::make_shared<dmrs_pbch_processor_factory_spy>();
    ASSERT_TRUE(dmrs_factory_spy) << "Failed to create DMRS PBCH processor factory.";

    std::shared_ptr<pss_processor_factory_spy> pss_factory_spy = std::make_shared<pss_processor_factory_spy>();
    ASSERT_TRUE(pss_factory_spy) << "Failed to create PSS processor factory.";

    std::shared_ptr<sss_processor_factory_spy> sss_factory_spy = std::make_shared<sss_processor_factory_spy>();
    ASSERT_TRUE(sss_factory_spy) << "Failed to create SSS processor factory.";

    ssb_processor_factory_sw_configuration ssb_factory_config;
    ssb_factory_config.encoder_factory                      = encoder_factory_spy;
    ssb_factory_config.modulator_factory                    = modulator_factory_spy;
    ssb_factory_config.dmrs_factory                         = dmrs_factory_spy;
    ssb_factory_config.pss_factory                          = pss_factory_spy;
    ssb_factory_config.sss_factory                          = sss_factory_spy;
    std::shared_ptr<ssb_processor_factory> ssb_proc_factory = create_ssb_processor_factory_sw(ssb_factory_config);
    ASSERT_TRUE(ssb_proc_factory) << "Failed to create SSB processor factory.";

    pbch = ssb_proc_factory->create();
    ASSERT_TRUE(pbch) << "Failed to create SSB processor.";

    pbch_validator = ssb_proc_factory->create_validator();
    ASSERT_TRUE(pbch_validator) << "Failed to create SSB PDU validator.";

    encoder = encoder_factory_spy->get_entries().front();
    ASSERT_TRUE(encoder) << "Failed to create PBCH encoder.";

    modulator = modulator_factory_spy->get_entries().front();
    ASSERT_TRUE(modulator) << "Failed to create PBCH modulator.";

    dmrs = dmrs_factory_spy->get_entries().front();
    ASSERT_TRUE(dmrs) << "Failed to create DMRS PBCH processor.";

    pss = pss_factory_spy->get_entries().front();
    ASSERT_TRUE(pss) << "Failed to create PSS processor.";

    sss = sss_factory_spy->get_entries().front();
    ASSERT_TRUE(sss) << "Failed to create SSS processor.";
  }

  ssb_pattern_case    pattern_case;
  unsigned            L_max;
  ssb_pss_to_sss_epre beta_pss;
  subcarrier_spacing  common_scs;

  std::unique_ptr<ssb_processor>     pbch           = nullptr;
  std::unique_ptr<ssb_pdu_validator> pbch_validator = nullptr;

  pbch_encoder_spy*        encoder   = nullptr;
  pbch_modulator_spy*      modulator = nullptr;
  dmrs_pbch_processor_spy* dmrs      = nullptr;
  pss_processor_spy*       pss       = nullptr;
  sss_processor_spy*       sss       = nullptr;

  static std::mt19937                            rgen;
  static std::uniform_int_distribution<unsigned> sfn_dist;
  static std::uniform_int_distribution<unsigned> pci_dist;
  static std::uniform_int_distribution<unsigned> bit_dist;
  static std::uniform_int_distribution<unsigned> port_dist;
  static std::uniform_int_distribution<unsigned> offset_to_pointA_dist;
  static std::uniform_int_distribution<unsigned> ssb_subcarrier_offset_dist;
};

std::mt19937                            SsbProcessorFixture::rgen;
std::uniform_int_distribution<unsigned> SsbProcessorFixture::sfn_dist(0, 1023);
std::uniform_int_distribution<unsigned> SsbProcessorFixture::pci_dist(0, phys_cell_id::NOF_NID - 1);
std::uniform_int_distribution<unsigned> SsbProcessorFixture::bit_dist(0, 1);
std::uniform_int_distribution<unsigned> SsbProcessorFixture::port_dist(0, 63);
std::uniform_int_distribution<unsigned> SsbProcessorFixture::offset_to_pointA_dist(0, 191);
std::uniform_int_distribution<unsigned> SsbProcessorFixture::ssb_subcarrier_offset_dist(0, 11);

TEST_P(SsbProcessorFixture, UnitTest)
{
  resource_grid_writer_spy grid = resource_grid_writer_spy(MAX_PORTS, MAX_NSYMB_PER_SLOT, MAX_NOF_PRBS);

  // Obtain the frequency range from the pattern case.
  frequency_range freq_range = to_frequency_range(pattern_case);

  // Select numerology from case.
  subcarrier_spacing ssb_scs = to_subcarrier_spacing(pattern_case);

  // Iterate over all possible SS/PBCH block candidates.
  for (unsigned ssb_idx = 0; ssb_idx != L_max; ++ssb_idx) {
    ssb_offset_to_pointA offset_pointA = offset_to_pointA_dist(rgen);

    // Make even the offset to PointA if common SCS is 30 or 120 kHz.
    if ((common_scs == subcarrier_spacing::kHz30) || (common_scs == subcarrier_spacing::kHz120)) {
      offset_pointA = ssb_offset_to_pointA(2 * (offset_pointA.value() / 2));
    }

    ssb_subcarrier_offset subcarrier_offset = ssb_subcarrier_offset_dist(rgen);

    // Round the SS/PBCH block subcarrier offset to avoid incompatible subcarrier offsets.
    if ((freq_range == frequency_range::FR1) && (ssb_scs != subcarrier_spacing::kHz15)) {
      subcarrier_offset = 2 * (subcarrier_offset.value() / 2);
    } else if (freq_range == frequency_range::FR2) {
      subcarrier_offset = 4 * (subcarrier_offset.value() / 4);
    }

    // Compute secondary variables.
    unsigned ssb_first_symbol_burst = ssb_get_l_first(pattern_case, ssb_idx);
    unsigned nslots_in_subframe     = get_nof_slots_per_subframe(ssb_scs);
    unsigned slot_in_burst          = ssb_first_symbol_burst / get_nsymb_per_slot(cyclic_prefix::NORMAL);
    unsigned subframe_in_burst      = slot_in_burst / nslots_in_subframe;
    unsigned slot_in_subframe       = slot_in_burst % nslots_in_subframe;

    // Compute secondary assertion values.
    unsigned ssb_first_subcarrier  = ssb_get_k_first(freq_range, ssb_scs, common_scs, offset_pointA, subcarrier_offset);
    unsigned ssb_first_symbol_slot = ssb_first_symbol_burst % get_nsymb_per_slot(cyclic_prefix::NORMAL);

    // Iterate half frames.
    for (unsigned subframe : {subframe_in_burst, 5 + subframe_in_burst}) {
      // Generate PBCH PDU.
      ssb_processor::pdu_t pdu = {};
      pdu.slot                 = {to_numerology_value(ssb_scs), sfn_dist(rgen), subframe, slot_in_subframe};
      pdu.phys_cell_id         = pci_dist(rgen);
      pdu.beta_pss             = beta_pss;
      pdu.ssb_idx              = ssb_idx;
      pdu.L_max                = L_max;
      pdu.common_scs           = common_scs;
      pdu.subcarrier_offset    = subcarrier_offset;
      pdu.offset_to_pointA     = offset_pointA;
      pdu.pattern_case         = pattern_case;
      pdu.ports.emplace_back(port_dist(rgen));
      for (uint8_t& bit : pdu.mib_payload) {
        bit = bit_dist(rgen);
      }

      // Reset spies.
      encoder->reset();
      modulator->reset();
      dmrs->reset();
      pss->reset();
      sss->reset();

      error_type<std::string> is_valid = pbch_validator->is_valid(pdu, MAX_NOF_PRBS);
      ASSERT_TRUE(is_valid.has_value()) << is_valid.error();

      // Process PDU.
      pbch->process(grid, pdu);

      // Assert modules number of entries.
      ASSERT_EQ(encoder->get_nof_entries(), 1);
      ASSERT_EQ(modulator->get_nof_entries(), 1);
      ASSERT_EQ(dmrs->get_nof_entries(), 1);
      ASSERT_EQ(pss->get_nof_entries(), 1);
      ASSERT_EQ(sss->get_nof_entries(), 1);

      // Assert encoder.
      const auto& encoder_entry = encoder->get_entries()[0];
      ASSERT_TRUE(encoder_entry.msg.N_id == pdu.phys_cell_id);
      ASSERT_TRUE(encoder_entry.msg.ssb_idx == pdu.ssb_idx);
      ASSERT_TRUE(encoder_entry.msg.L_max == L_max);
      ASSERT_TRUE(encoder_entry.msg.hrf == pdu.slot.is_odd_hrf());
      ASSERT_TRUE(ocuduvec::equal(encoder_entry.msg.payload, encoder_entry.msg.payload));
      ASSERT_TRUE(encoder_entry.msg.sfn == pdu.slot.sfn());
      ASSERT_TRUE(encoder_entry.msg.k_ssb == pdu.subcarrier_offset);
      ASSERT_TRUE(encoder_entry.encoded.size() == pbch_encoder::E);

      // Assert modulator.
      const auto& modulator_entry = modulator->get_entries()[0];
      ASSERT_TRUE(modulator_entry.config.phys_cell_id == pdu.phys_cell_id);
      ASSERT_TRUE(modulator_entry.config.ssb_idx == pdu.ssb_idx);
      ASSERT_EQ(ssb_first_subcarrier, modulator_entry.config.ssb_first_subcarrier);
      ASSERT_TRUE(modulator_entry.config.ssb_first_symbol == ssb_first_symbol_slot);
      ASSERT_TRUE(modulator_entry.config.amplitude == 1.0F);
      ASSERT_TRUE(ocuduvec::equal(modulator_entry.config.ports, pdu.ports));
      ASSERT_TRUE(ocuduvec::equal(modulator_entry.bits, encoder_entry.encoded));
      ASSERT_TRUE(modulator_entry.grid_ptr == &grid);

      // Assert DM-RS for PBCH.
      const auto& dmrs_entry = dmrs->get_entries()[0];
      ASSERT_TRUE(dmrs_entry.config.phys_cell_id == pdu.phys_cell_id);
      ASSERT_TRUE(dmrs_entry.config.ssb_idx == pdu.ssb_idx);
      ASSERT_TRUE(dmrs_entry.config.L_max == pdu.L_max);
      ASSERT_TRUE(dmrs_entry.config.ssb_first_subcarrier == ssb_first_subcarrier);
      ASSERT_TRUE(dmrs_entry.config.ssb_first_symbol == ssb_first_symbol_slot);
      ASSERT_TRUE(dmrs_entry.config.hrf == pdu.slot.is_odd_hrf());
      ASSERT_TRUE(dmrs_entry.config.amplitude == 1.0F);
      ASSERT_TRUE(ocuduvec::equal(dmrs_entry.config.ports, pdu.ports));

      // Assert PSS.
      const auto& pss_entry = pss->get_entries()[0];
      ASSERT_TRUE(pss_entry.config.phys_cell_id == pdu.phys_cell_id);
      ASSERT_TRUE(pss_entry.config.ssb_first_subcarrier == ssb_first_subcarrier);
      ASSERT_TRUE(pss_entry.config.ssb_first_symbol == ssb_first_symbol_slot);
      ASSERT_TRUE(pss_entry.config.amplitude == ssb_pss_to_sss_epre_to_amplitude(beta_pss));
      ASSERT_TRUE(ocuduvec::equal(pss_entry.config.ports, pdu.ports));

      // Assert SSS.
      const auto& sss_entry = sss->get_entries()[0];
      ASSERT_TRUE(sss_entry.config.phys_cell_id == pdu.phys_cell_id);
      ASSERT_TRUE(sss_entry.config.ssb_first_subcarrier == ssb_first_subcarrier);
      ASSERT_TRUE(sss_entry.config.ssb_first_symbol == ssb_first_symbol_slot);
      ASSERT_TRUE(sss_entry.config.amplitude == 1.0F);
      ASSERT_TRUE(ocuduvec::equal(sss_entry.config.ports, pdu.ports));
    }
  }
}

constexpr auto ssb_pattern_case_values = to_array<ssb_pattern_case>(
    {ssb_pattern_case::A, ssb_pattern_case::B, ssb_pattern_case::C, ssb_pattern_case::D, ssb_pattern_case::E});

constexpr auto L_max_values = to_array<unsigned>({4, 8, 64});

constexpr auto beta_pss_values = to_array<ssb_pss_to_sss_epre>({ssb_pss_to_sss_epre::dB_0, ssb_pss_to_sss_epre::dB_3});

constexpr auto subcarrier_spacing_values = to_array<subcarrier_spacing>(
    {subcarrier_spacing::kHz15, subcarrier_spacing::kHz30, subcarrier_spacing::kHz60, subcarrier_spacing::kHz120});

std::vector<test_parameters> generate_cases()
{
  std::vector<test_parameters> out;

  for (const auto& pattern_case : ssb_pattern_case_values) {
    for (const auto& L_max : L_max_values) {
      for (const auto& beta_pss : beta_pss_values) {
        for (const auto& scs : subcarrier_spacing_values) {
          frequency_range freq_range = to_frequency_range(pattern_case);

          // Skip invalid pattern and L_max combinations.
          if (pattern_case != ssb_pattern_case::E && pattern_case != ssb_pattern_case::D && L_max == 64) {
            continue;
          }

          // Skip invalid frequency range and common SCS combinations.
          if (!is_scs_valid(scs, freq_range)) {
            continue;
          }

          out.emplace_back(pattern_case, L_max, beta_pss, scs);
        }
      }
    }
  }

  return out;
}

std::vector<test_parameters> all_tests = generate_cases();

INSTANTIATE_TEST_SUITE_P(SsbProcessorUnittest, SsbProcessorFixture, testing::ValuesIn(all_tests));
} // namespace
