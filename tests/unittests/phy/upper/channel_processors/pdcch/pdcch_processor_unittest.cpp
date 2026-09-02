// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "dmrs_pdcch_processor_test_doubles.h"
#include "pdcch_encoder_test_doubles.h"
#include "pdcch_modulator_test_doubles.h"
#include "resource_grid_test_doubles.h"
#include "ocudu/adt/format.h"
#include "ocudu/phy/support/precoding_formatters.h"
#include "ocudu/phy/support/resource_grid_mapper.h"
#include "ocudu/phy/upper/channel_processors/pdcch/pdcch_processor.h"
#include "ocudu/ran/precoding/precoding_codebooks.h"
#include <gtest/gtest.h>
#include <random>

static std::mt19937 rgen(0);

namespace ocudu {
struct pdcch_processor_config_t {
  std::unique_ptr<pdcch_encoder>        encoder;
  std::unique_ptr<pdcch_modulator>      modulator;
  std::unique_ptr<dmrs_pdcch_processor> dmrs;
};
std::unique_ptr<pdcch_processor> create_pdcch_processor(pdcch_processor_config_t& config);

class pdcch_processor_impl
{
public:
  static crb_bitmap compute_rb_mask(const pdcch_processor::coreset_description& coreset,
                                    const pdcch_processor::dci_description&     dci);
};

} // namespace ocudu

using namespace ocudu;

TEST(pdcch_processor_unittest, process)
{
  // Random distributions.
  std::uniform_int_distribution<unsigned> dist_numerology(0, 4);
  std::uniform_int_distribution<unsigned> dist_sfn(0, 1023);
  std::uniform_int_distribution<unsigned> dist_subframe(0, 9);
  std::uniform_int_distribution<unsigned> dist_slot(0, 15);
  std::uniform_int_distribution<unsigned> dist_n_id(0, 65535);
  std::uniform_real_distribution<float>   dist_power_offset(-6, +6);
  std::uniform_int_distribution<unsigned> dist_payload_sz(15, 127);
  std::uniform_int_distribution<uint8_t>  dist_payload(0, 1);
  std::uniform_int_distribution<unsigned> dist_port_idx(0, 15);

  // Create dummies and spies.
  resource_grid_dummy                          grid;
  std::shared_ptr<pdcch_encoder_factory_spy>   encoder_factory_spy   = std::make_shared<pdcch_encoder_factory_spy>();
  std::shared_ptr<pdcch_modulator_factory_spy> modulator_factory_spy = std::make_shared<pdcch_modulator_factory_spy>();
  std::shared_ptr<dmrs_pdcch_processor_factory_spy> dmrs_factory_spy =
      std::make_shared<dmrs_pdcch_processor_factory_spy>();

  std::shared_ptr<pdcch_processor_factory> pdcch_factory =
      create_pdcch_processor_factory_sw(encoder_factory_spy, modulator_factory_spy, dmrs_factory_spy);
  ASSERT_TRUE(pdcch_factory);

  std::unique_ptr<pdcch_processor> pdcch = pdcch_factory->create();

  pdcch_encoder_spy*        encoder   = encoder_factory_spy->get_entries().front();
  pdcch_modulator_spy*      modulator = modulator_factory_spy->get_entries().front();
  dmrs_pdcch_processor_spy* dmrs      = dmrs_factory_spy->get_entries().front();

  for (unsigned bwp_size_rb : {52, 104}) {
    for (unsigned bwp_start_rb : {0, 1}) {
      for (unsigned start_symbol_index : {0, 1}) {
        for (unsigned duration : {1, 2, 3}) {
          for (cyclic_prefix cp : {cyclic_prefix::NORMAL, cyclic_prefix::EXTENDED}) {
            for (unsigned aggregation_level_index : {0, 1, 2, 3, 4}) {
              for (auto cce_to_reg_mapping : {pdcch_processor::cce_to_reg_mapping_type::CORESET0,
                                              pdcch_processor::cce_to_reg_mapping_type::NON_INTERLEAVED,
                                              pdcch_processor::cce_to_reg_mapping_type::INTERLEAVED}) {
                aggregation_level dci_aggregation_level = aggregation_index_to_level(aggregation_level_index);

                unsigned nof_cces = to_nof_cces(dci_aggregation_level);
                // Skip if it cannot fit a candidate.
                if (duration * (bwp_size_rb / 6) < nof_cces) {
                  continue;
                }

                unsigned numerology  = dist_numerology(rgen);
                bool     is_coreset0 = (cce_to_reg_mapping == pdcch_processor::cce_to_reg_mapping_type::CORESET0);
                unsigned E = nof_cces * pdcch_constants::NOF_REG_PER_CCE * pdcch_constants::NOF_RE_PDCCH_PER_RB * 2;

                bounded_bitset<pdcch_constants::MAX_NOF_FREQ_RESOURCES, true> frequency_resources(
                    pdcch_constants::MAX_NOF_FREQ_RESOURCES);
                frequency_resources.fill(0, (12 * (bwp_size_rb / 12)) / pdcch_constants::NOF_RB_PER_FREQ_RESOURCE);

                pdcch_processor::pdu_t pdu = {};
                pdu.slot = {numerology, dist_sfn(rgen), dist_subframe(rgen), dist_slot(rgen) % (1 << numerology)};
                pdu.cp   = cp;

                pdcch_processor::coreset_description& coreset = pdu.coreset;
                coreset.bwp_size_rb                           = is_coreset0 ? (12 * (bwp_size_rb / 12)) : bwp_size_rb;
                coreset.bwp_start_rb                          = bwp_start_rb;
                coreset.start_symbol_index                    = start_symbol_index;
                coreset.duration                              = duration;
                coreset.frequency_resources                   = frequency_resources;
                coreset.cce_to_reg_mapping                    = cce_to_reg_mapping;
                coreset.reg_bundle_size                       = 6;
                coreset.interleaver_size                      = 2;
                coreset.shift_index                           = 0;

                pdcch_processor::dci_description& dci = pdu.dci;
                dci.n_id_pdcch_dmrs                   = dist_n_id(rgen);
                dci.n_id_pdcch_data                   = dist_n_id(rgen);
                dci.n_rnti                            = dist_n_id(rgen);
                dci.cce_index                         = 0;
                dci.dci_aggregation_level             = dci_aggregation_level;
                dci.dmrs_power_offset_dB              = dist_power_offset(rgen);
                dci.data_power_offset_dB              = dist_power_offset(rgen);
                dci.payload.resize(dist_payload_sz(rgen));
                for (uint8_t& bit : dci.payload) {
                  bit = dist_payload(rgen);
                }
                dci.payload.resize(dist_payload_sz(rgen));
                dci.precoding = precoding_configuration::make_wideband(make_single_port());

                // Reset spy classes.
                encoder->reset();
                modulator->reset();
                dmrs->reset();

                // Process PDU.
                pdcch->process(grid.get_writer(), pdu);

                // Calculate ideal allocation mask.
                crb_bitmap rb_mask = pdcch_processor_impl::compute_rb_mask(pdu.coreset, pdu.dci);

                // Check PDCCH encoder inputs.
                ASSERT_EQ(encoder->get_nof_entries(), 1);
                const auto& encoder_entry = encoder->get_entries().front();
                ASSERT_EQ(encoder_entry.config.rnti, dci.rnti);
                ASSERT_EQ(encoder_entry.config.E, E);
                ASSERT_EQ(const_span<uint8_t>(encoder_entry.data), const_span<uint8_t>(pdu.dci.payload));

                // Check PDCCH modulator inputs.
                ASSERT_EQ(modulator->get_nof_entries(), 1);
                const auto& modulator_entry = modulator->get_entries().front();
                ASSERT_EQ(rb_mask, modulator_entry.config.rb_mask);
                ASSERT_EQ(modulator_entry.config.start_symbol_index, start_symbol_index);
                ASSERT_EQ(modulator_entry.config.duration, duration);
                ASSERT_EQ(modulator_entry.config.n_id, dci.n_id_pdcch_data);
                ASSERT_EQ(modulator_entry.config.n_rnti, dci.n_rnti);
                ASSERT_EQ(modulator_entry.config.scaling, convert_dB_to_amplitude(dci.data_power_offset_dB));
                ASSERT_EQ(pdu.dci.precoding, modulator_entry.config.precoding);
                ASSERT_EQ(const_span<uint8_t>(modulator_entry.bits), const_span<uint8_t>(encoder_entry.encoded));
                ASSERT_EQ((void*)modulator_entry.grid, (void*)&grid.get_writer());

                // Check PDCCH DMRS inputs.
                ASSERT_EQ(dmrs->get_nof_entries(), 1);
                const auto& dmrs_entry = dmrs->get_entries().front();
                ASSERT_EQ(dmrs_entry.config.slot, pdu.slot);
                ASSERT_EQ(dmrs_entry.config.cp, cp);
                ASSERT_EQ(dmrs_entry.config.reference_point_k_rb, is_coreset0 ? coreset.bwp_start_rb : 0);
                ASSERT_EQ(dmrs_entry.config.rb_mask, rb_mask);
                ASSERT_EQ(dmrs_entry.config.start_symbol_index, coreset.start_symbol_index);
                ASSERT_EQ(dmrs_entry.config.duration, coreset.duration);
                ASSERT_EQ(dmrs_entry.config.n_id, dci.n_id_pdcch_dmrs);
                ASSERT_EQ(dmrs_entry.config.amplitude, convert_dB_to_amplitude(dci.dmrs_power_offset_dB));
                ASSERT_EQ(pdu.dci.precoding, dmrs_entry.config.precoding);
              }
            }
          }
        }
      }
    }
  }
}
