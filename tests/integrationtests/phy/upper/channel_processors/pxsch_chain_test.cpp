// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/adt/format.h"
#include "ocudu/ocuduvec/bit.h"
#include "ocudu/phy/upper/channel_coding/channel_coding_factories.h"
#include "ocudu/phy/upper/channel_processors/pdsch/factories.h"
#include "ocudu/phy/upper/channel_processors/pusch/factories.h"
#include "ocudu/phy/upper/channel_processors/pusch/pusch_decoder_buffer.h"
#include "ocudu/phy/upper/channel_processors/pusch/pusch_decoder_notifier.h"
#include "ocudu/phy/upper/channel_processors/pusch/pusch_decoder_result.h"
#include "ocudu/phy/upper/rx_buffer_pool.h"
#include "ocudu/phy/upper/unique_rx_buffer.h"
#include "ocudu/ran/dmrs/dmrs.h"
#include "ocudu/ran/pdsch/dlsch_info.h"
#include "ocudu/ran/pdsch/pdsch_mcs.h"
#include "ocudu/ran/pusch/pusch_constants.h"
#include "ocudu/ran/resource_allocation/rb_interval.h"
#include "ocudu/ran/sch/sch_mcs.h"
#include "ocudu/ran/sch/tbs_calculator.h"
#include "ocudu/support/math/complex_normal_random.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>
#include <random>
#include <thread>
#include <tuple>

using namespace ocudu;

static constexpr pdsch_mcs_table mcs_table           = pdsch_mcs_table::qam64;
static constexpr unsigned        nof_layers          = 1;
static constexpr unsigned        nof_ofdm_symbols    = 14;
static const symbol_slot_mask    dmrs_symbols        = {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0};
static const float               noise_var           = convert_dB_to_power(-30);
static constexpr unsigned        nof_ldpc_iterations = 6;
static constexpr uint64_t        seed_begin          = 0;
static constexpr uint64_t        seed_end            = 128;

using ldpc_decoder_early_stop = bool;
using pxsch_chain_params      = std::tuple<sch_mcs_index, prb_interval, ldpc_decoder_early_stop>;

namespace ocudu {

std::ostream& operator<<(std::ostream& os, const sch_mcs_index& index)
{
  fmt::print(os, "mcs={}", index.value());
  return os;
}

std::ostream& operator<<(std::ostream& os, const prb_interval& prb_mapping)
{
  fmt::print(os, "prb=from{}to{}", prb_mapping.start(), prb_mapping.stop());
  return os;
}

} // namespace ocudu

namespace {

class PxschChainFixture : public ::testing::TestWithParam<pxsch_chain_params>
{
private:
  static std::shared_ptr<pdsch_encoder_factory> create_sw_pdsch_encoder_factory()
  {
    std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw("auto");
    EXPECT_TRUE(crc_calc_factory);

    std::shared_ptr<ldpc_encoder_factory> ldpc_encoder_factory = create_ldpc_encoder_factory_sw("auto");
    EXPECT_TRUE(ldpc_encoder_factory);

    std::shared_ptr<ldpc_rate_matcher_factory> ldpc_rate_matcher_factory = create_ldpc_rate_matcher_factory_sw();
    EXPECT_TRUE(ldpc_rate_matcher_factory);

    std::shared_ptr<ldpc_segmenter_tx_factory> segmenter_factory =
        create_ldpc_segmenter_tx_factory_sw(crc_calc_factory);
    EXPECT_TRUE(segmenter_factory);

    pdsch_encoder_factory_sw_configuration encoder_factory_config;
    encoder_factory_config.encoder_factory      = ldpc_encoder_factory;
    encoder_factory_config.rate_matcher_factory = ldpc_rate_matcher_factory;
    encoder_factory_config.segmenter_factory    = segmenter_factory;
    return create_pdsch_encoder_factory_sw(encoder_factory_config);
  }

  static std::shared_ptr<pusch_decoder_factory> create_sw_pusch_decoder_factory()
  {
    std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw("auto");
    EXPECT_TRUE(crc_calc_factory);

    ldpc_decoder_factory::ldpc_decoder_factory_configuration ldpc_dec_cfg = {.force_decoding      = false,
                                                                             .early_stop_syndrome = false};
    std::shared_ptr<ldpc_decoder_factory> ldpc_decoder_factory = create_ldpc_decoder_factory_sw("auto", ldpc_dec_cfg);
    EXPECT_TRUE(ldpc_decoder_factory);

    std::shared_ptr<ldpc_rate_dematcher_factory> ldpc_rate_dematcher_factory =
        create_ldpc_rate_dematcher_factory_sw("auto");
    EXPECT_TRUE(ldpc_rate_dematcher_factory);

    std::shared_ptr<ldpc_segmenter_rx_factory> segmenter_rx_factory = create_ldpc_segmenter_rx_factory_sw();
    EXPECT_TRUE(segmenter_rx_factory);

    pusch_decoder_factory_sw_configuration pusch_decoder_factory_sw_config;
    pusch_decoder_factory_sw_config.crc_factory               = crc_calc_factory;
    pusch_decoder_factory_sw_config.decoder_factory           = ldpc_decoder_factory;
    pusch_decoder_factory_sw_config.dematcher_factory         = ldpc_rate_dematcher_factory;
    pusch_decoder_factory_sw_config.segmenter_factory         = segmenter_rx_factory;
    pusch_decoder_factory_sw_config.nof_pusch_decoder_threads = 1;
    pusch_decoder_factory_sw_config.executor                  = nullptr;
    pusch_decoder_factory_sw_config.nof_prb                   = MAX_NOF_PRBS;
    pusch_decoder_factory_sw_config.nof_layers                = pusch_constants::MAX_NOF_LAYERS;

    return create_pusch_decoder_factory_sw(pusch_decoder_factory_sw_config);
  }

protected:
  /// Implements a PUSCH decoder notifier adaptor for synchronizing the decoder results.
  class pusch_decoder_notifier_adaptor : public pusch_decoder_notifier
  {
  public:
    void on_sch_data(const pusch_decoder_result& result_) override
    {
      completed = true;
      result    = result_;
    }

    void reset() { completed = false; }

    const pusch_decoder_result& wait_for_completion() const
    {
      while (!completed.load()) {
        std::this_thread::sleep_for(std::chrono::microseconds(10));
      }

      return result;
    }

  private:
    pusch_decoder_result result;
    std::atomic<bool>    completed = {false};
  };

  void SetUp() override
  {
    std::shared_ptr<pdsch_encoder_factory> pdsch_enc_factory = create_sw_pdsch_encoder_factory();
    ASSERT_TRUE(pdsch_enc_factory);

    std::shared_ptr<pusch_decoder_factory> pusch_dec_factory = create_sw_pusch_decoder_factory();
    ASSERT_TRUE(pusch_dec_factory);

    std::shared_ptr<modulation_mapper_factory> modulation_factory = create_modulation_mapper_factory();
    ASSERT_TRUE(modulation_factory);

    std::shared_ptr<demodulation_mapper_factory> demodulation_factory = create_demodulation_mapper_factory();
    ASSERT_TRUE(demodulation_factory);

    encoder = pdsch_enc_factory->create();
    ASSERT_TRUE(encoder);

    decoder = pusch_dec_factory->create();
    ASSERT_TRUE(decoder);

    modulator = modulation_factory->create();
    ASSERT_TRUE(modulator);

    demodulator = demodulation_factory->create();
    ASSERT_TRUE(demodulator);
  }

  std::unique_ptr<pdsch_encoder>       encoder;
  std::unique_ptr<pusch_decoder>       decoder;
  std::unique_ptr<modulation_mapper>   modulator;
  std::unique_ptr<demodulation_mapper> demodulator;
};
} // namespace

TEST_P(PxschChainFixture, Ideal)
{
  // Extract test parameters.
  sch_mcs_index mcs_index      = std::get<0>(GetParam());
  prb_interval  rb_mapping     = std::get<1>(GetParam());
  bool          use_early_stop = std::get<2>(GetParam());

  // Get modulation and code scheme.
  sch_mcs_description mcs_descr = pdsch_mcs_get_config(mcs_table, mcs_index);

  // Calculate TBS.
  tbs_calculator_configuration tbs_params;
  tbs_params.nof_symb_sh      = nof_ofdm_symbols;
  tbs_params.nof_dmrs_prb     = NOF_SUBCARRIERS_PER_RB * dmrs_symbols.count();
  tbs_params.nof_oh_prb       = 0;
  tbs_params.mcs_descr        = mcs_descr;
  tbs_params.nof_layers       = nof_layers;
  tbs_params.tb_scaling_field = 0;
  tbs_params.n_prb            = rb_mapping.length();
  units::bits tbs             = tbs_calculator_calculate(tbs_params).to_bits();

  // Get PxSCH information.
  dlsch_configuration dlsch_params;
  dlsch_params.tbs                         = tbs;
  dlsch_params.mcs_descr                   = mcs_descr;
  dlsch_params.nof_rb                      = rb_mapping.length();
  dlsch_params.start_symbol_index          = 0;
  dlsch_params.nof_symbols                 = nof_ofdm_symbols;
  dlsch_params.dmrs_type                   = dmrs_config_type::type1;
  dlsch_params.dmrs_symbol_mask            = dmrs_symbols;
  dlsch_params.nof_cdm_groups_without_data = 2;
  dlsch_params.nof_layers                  = nof_layers;
  dlsch_params.contains_dc                 = true;
  dlsch_information info                   = get_dlsch_information(dlsch_params);
  ASSERT_LT(info.get_effective_code_rate(), 0.95);

  // Create HARQ receive buffer pool.
  rx_buffer_pool_config rx_buffer_cfg;
  rx_buffer_cfg.max_codeblock_size                                = ldpc::MAX_CODEBLOCK_SIZE;
  rx_buffer_cfg.nof_buffers                                       = 1;
  rx_buffer_cfg.nof_codeblocks                                    = info.sch.nof_cb;
  rx_buffer_cfg.expire_timeout_slots                              = 1;
  rx_buffer_cfg.external_soft_bits                                = false;
  std::unique_ptr<rx_buffer_pool_controller> rx_bufffer_pool_ctrl = create_rx_buffer_pool(rx_buffer_cfg);
  ASSERT_TRUE(rx_bufffer_pool_ctrl);

  // Prepare encoder configuration.
  pdsch_encoder::configuration encoder_config;
  encoder_config.base_graph     = get_ldpc_base_graph(mcs_descr.get_normalised_target_code_rate(), tbs);
  encoder_config.rv             = 0;
  encoder_config.mod            = mcs_descr.modulation;
  encoder_config.Nref           = 0;
  encoder_config.nof_layers     = nof_layers;
  encoder_config.nof_ch_symbols = info.nof_dl_sch_bits.value() / get_bits_per_symbol(mcs_descr.modulation);

  // Prepare decoder configuration.
  pusch_decoder::configuration decoder_config;
  decoder_config.base_graph          = get_ldpc_base_graph(mcs_descr.get_normalised_target_code_rate(), tbs);
  decoder_config.rv                  = 0;
  decoder_config.mod                 = mcs_descr.modulation;
  decoder_config.Nref                = 0;
  decoder_config.nof_layers          = nof_layers;
  decoder_config.nof_ldpc_iterations = nof_ldpc_iterations;
  decoder_config.use_early_stop      = use_early_stop;
  decoder_config.new_data            = true;

  // Prepare transmit and receive buffers.
  dynamic_bit_buffer   tx_data(tbs.value());
  std::vector<uint8_t> tx_encoded_bits(info.nof_dl_sch_bits.value());
  dynamic_bit_buffer   tx_encoded_packed(info.nof_dl_sch_bits.value());
  dynamic_bit_buffer   rx_data(tbs.value());
  std::vector<cf_t>    modulated_symbols(info.nof_dl_sch_bits.value() / get_bits_per_symbol(mcs_descr.modulation));

  // Prepare modulated symbols noise variance.
  std::vector<float> noise_vars(info.nof_dl_sch_bits.value() / get_bits_per_symbol(mcs_descr.modulation));
  std::fill(noise_vars.begin(), noise_vars.end(), noise_var);

  for (uint64_t seed = seed_begin; seed != seed_end; ++seed) {
    // Create random generator with the seed.
    std::mt19937 rgen(seed);

    // Generate random data.
    span<uint8_t> byte_buffer = tx_data.get_buffer();
    std::generate(byte_buffer.begin(), byte_buffer.end(), [&rgen]() { return static_cast<uint8_t>(rgen()); });

    // Get RM buffer.
    unique_rx_buffer rx_buffer =
        rx_bufffer_pool_ctrl->get_pool().reserve({}, trx_buffer_identifier(to_rnti(0), 0), info.sch.nof_cb, true);

    // Encode data.
    encoder->encode(tx_encoded_bits, tx_data.get_buffer(), encoder_config);
    ocuduvec::bit_pack(tx_encoded_packed, tx_encoded_bits);

    // Modulate.
    modulator->modulate(modulated_symbols, tx_encoded_packed, mcs_descr.modulation);

    // Add noise.
    complex_normal_distribution<cf_t> dist(0, std::sqrt(noise_var));
    std::transform(modulated_symbols.begin(),
                   modulated_symbols.end(),
                   modulated_symbols.begin(),
                   [&rgen, &dist](cf_t in) { return in + dist(rgen); });

    // Simulate DC.
    unsigned dc_position_begin = rgen() % (NOF_SUBCARRIERS_PER_RB * rb_mapping.length());
    for (unsigned dc_position = dc_position_begin; dc_position < modulated_symbols.size();
         dc_position += NOF_SUBCARRIERS_PER_RB * rb_mapping.length()) {
      modulated_symbols[dc_position] = 0;
    }

    // Prepare decoder for new data.
    pusch_decoder_notifier_adaptor notifier;
    pusch_decoder_buffer&          decoder_buffer =
        decoder->new_data(rx_data.get_buffer(), std::move(rx_buffer), notifier, decoder_config);

    // Transform data into softbits.
    span<log_likelihood_ratio> rx_softbit_buffer = decoder_buffer.get_next_block_view(info.nof_dl_sch_bits.value());
    demodulator->demodulate_soft(rx_softbit_buffer, modulated_symbols, noise_vars, mcs_descr.modulation);

    // Write softbits.
    decoder_buffer.on_new_softbits(rx_softbit_buffer);
    decoder_buffer.on_end_softbits();

    // Wait for decoder to complete.
    const pusch_decoder_result& decoder_result = notifier.wait_for_completion();

    // Verify CRC.
    ASSERT_TRUE(decoder_result.tb_crc_ok);

    // Verify decoded data matches the transmitted data.
    ASSERT_EQ(tx_data, rx_data);
  }
}

INSTANTIATE_TEST_SUITE_P(PxschChain,
                         PxschChainFixture,
                         testing::Combine(testing::Range(sch_mcs_index(26), sch_mcs_index(28), 3),
                                          testing::Values(prb_interval{0, 3}, prb_interval{0, 4}, prb_interval{0, 5}),
                                          testing::Values(true, false)));
