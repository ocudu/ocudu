// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Tests the entire LDPC chain.

#include "ocudu/adt/to_array.h"
#include "ocudu/ocuduvec/bit.h"
#include "ocudu/phy/upper/channel_coding/channel_coding_factories.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc_decoder.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc_encoder.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc_encoder_buffer.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc_segmenter_buffer.h"
#include <gtest/gtest.h>
#include <random>

using namespace ocudu;
using namespace ocudu::ldpc;

// Random generator for bits.
static std::mt19937                           rgen(0);
static std::uniform_int_distribution<uint8_t> byte_gen(0, 255);

// Fixed log-likelihood ratio amplitude.
static constexpr log_likelihood_ratio LLRS_AMPL = 10;

// Returns the message length (encoder input size) and the number of data bits for the given codeblock.
static std::pair<unsigned, unsigned> get_nof_data_bits(const codeblock_metadata& cb_meta)
{
  unsigned cb_length    = cb_meta.cb_specific.full_length;
  unsigned inverse_rate = (cb_meta.tb_common.base_graph == ldpc_base_graph_type::BG1) ? 3 : 5;
  unsigned msg_length   = cb_length / inverse_rate;

  return {msg_length, msg_length - cb_meta.cb_specific.nof_crc_bits - cb_meta.cb_specific.nof_filler_bits};
}

// Copies data bit_buffer to the container bit_buffer, starting from offset.
static void fill_bit_buffer(bit_buffer container, bit_buffer_reader data, unsigned offset)
{
  unsigned container_size = container.size();
  unsigned data_size      = data.size();
  ocudu_assert(container_size >= data_size + offset,
               "Cannot feed {} data bits to a container of {} bits from offset {}.",
               data_size,
               container_size,
               offset);

  static constexpr unsigned word_size   = 8;
  unsigned                  offset_read = 0;

  for (unsigned i_word = 0, nof_words = data_size / word_size; i_word != nof_words; ++i_word) {
    container.insert(data.extract(offset_read, word_size), offset_read + offset, word_size);
    offset_read += word_size;
  }

  unsigned leftover = data_size - offset_read;
  if (leftover > 0) {
    container.insert(data.extract(offset_read, leftover), offset_read + offset, leftover);
  }
}

struct test_parameters {
  std::string       implementation;
  unsigned          nof_rb;
  unsigned          nof_layers;
  float             code_rate;
  modulation_scheme modulation;

  test_parameters(std::string i, unsigned r, unsigned l, float cr, modulation_scheme m) :
    implementation(std::move(i)), nof_rb(r), nof_layers(l), code_rate(cr), modulation(m)
  {
  }
};

std::ostream& operator<<(std::ostream& os, const test_parameters& tp)
{
  return os << fmt::format("{}, RB={}, layers={}, rate={}, modulation={}",
                           tp.implementation,
                           tp.nof_rb,
                           tp.nof_layers,
                           tp.code_rate,
                           to_string(tp.modulation));
}

class LDPCChainFixture : public ::testing::TestWithParam<test_parameters>
{
public:
  // Creates the factories just once.
  static void SetUpTestSuite()
  {
    if (!enc_factory_generic) {
      enc_factory_generic = create_ldpc_encoder_factory_sw("generic");
      ASSERT_NE(enc_factory_generic, nullptr);
    }
    if (!enc_factory_avx2) {
      enc_factory_avx2 = create_ldpc_encoder_factory_sw("avx2");
      ASSERT_NE(enc_factory_avx2, nullptr);
    }
    if (!enc_factory_neon) {
      enc_factory_neon = create_ldpc_encoder_factory_sw("neon");
      ASSERT_NE(enc_factory_neon, nullptr);
    }

    ldpc_decoder_factory::ldpc_decoder_factory_configuration cfg = {.force_decoding      = false,
                                                                    .early_stop_syndrome = false};
    if (!dec_factory_generic) {
      dec_factory_generic = create_ldpc_decoder_factory_sw("generic", cfg);
      ASSERT_NE(dec_factory_generic, nullptr);
    }
    if (!dec_factory_avx2) {
      dec_factory_avx2 = create_ldpc_decoder_factory_sw("avx2", cfg);
      ASSERT_NE(dec_factory_avx2, nullptr);
    }
    if (!dec_factory_avx512) {
      dec_factory_avx512 = create_ldpc_decoder_factory_sw("avx512", cfg);
      ASSERT_NE(dec_factory_avx512, nullptr);
    }
    if (!dec_factory_neon) {
      dec_factory_neon = create_ldpc_decoder_factory_sw("neon", cfg);
      ASSERT_NE(dec_factory_neon, nullptr);
    }
    if (!segmenter_tx_factory) {
      std::shared_ptr<crc_calculator_factory> crc_factory = create_crc_calculator_factory_sw("auto");
      ASSERT_NE(crc_factory, nullptr);

      segmenter_tx_factory = create_ldpc_segmenter_tx_factory_sw(crc_factory);
      ASSERT_NE(segmenter_tx_factory, nullptr);
    }

    if (!segmenter_rx_factory) {
      segmenter_rx_factory = create_ldpc_segmenter_rx_factory_sw();
      ASSERT_NE(segmenter_rx_factory, nullptr);
    }

    if (!rate_matcher_factory) {
      rate_matcher_factory = create_ldpc_rate_matcher_factory_sw();
      ASSERT_NE(rate_matcher_factory, nullptr);
    }

    if (!rate_dematcher_factory_generic) {
      rate_dematcher_factory_generic = create_ldpc_rate_dematcher_factory_sw("generic");
      ASSERT_NE(rate_dematcher_factory_generic, nullptr);
    }

    if (!rate_dematcher_factory_avx2) {
      rate_dematcher_factory_avx2 = create_ldpc_rate_dematcher_factory_sw("avx2");
      ASSERT_NE(rate_dematcher_factory_avx2, nullptr);
    }

    if (!rate_dematcher_factory_avx512) {
      rate_dematcher_factory_avx512 = create_ldpc_rate_dematcher_factory_sw("avx512");
      ASSERT_NE(rate_dematcher_factory_avx512, nullptr);
    }

    if (!rate_dematcher_factory_neon) {
      rate_dematcher_factory_neon = create_ldpc_rate_dematcher_factory_sw("neon");
      ASSERT_NE(rate_dematcher_factory_neon, nullptr);
    }
  }

protected:
  // Finalizes the test setup by asserting the creation of encoder and decoder as well as double-checking some of the
  // quantities computed in the constructor.
  void SetUp() override
  {
    // Check the factories again, since ASSERT_* is not fatal in SetUpTestSuite in old googletest releases.
    ASSERT_NE(enc_factory_generic, nullptr);
    ASSERT_NE(enc_factory_avx2, nullptr);
    ASSERT_NE(enc_factory_neon, nullptr);
    ASSERT_NE(dec_factory_generic, nullptr);
    ASSERT_NE(dec_factory_avx2, nullptr);
    ASSERT_NE(dec_factory_avx512, nullptr);
    ASSERT_NE(dec_factory_neon, nullptr);
    ASSERT_NE(segmenter_tx_factory, nullptr);
    ASSERT_NE(segmenter_rx_factory, nullptr);
    ASSERT_NE(rate_matcher_factory, nullptr);
    ASSERT_NE(rate_dematcher_factory_generic, nullptr);
    ASSERT_NE(rate_dematcher_factory_avx2, nullptr);
    ASSERT_NE(rate_dematcher_factory_avx512, nullptr);
    ASSERT_NE(rate_dematcher_factory_neon, nullptr);

    const std::string& implementation = GetParam().implementation;
    if (implementation == "generic") {
      encoder_test = enc_factory_generic->create();
      ASSERT_NE(encoder_test, nullptr);
      decoder_test = dec_factory_generic->create();
      ASSERT_NE(decoder_test, nullptr);
      rate_dematcher = rate_dematcher_factory_generic->create();
      ASSERT_NE(rate_dematcher, nullptr);
    } else if (implementation == "avx2") {
      encoder_test = enc_factory_avx2->create();
      ASSERT_NE(encoder_test, nullptr);
      decoder_test = dec_factory_avx2->create();
      ASSERT_NE(decoder_test, nullptr);
      rate_dematcher = rate_dematcher_factory_avx2->create();
      ASSERT_NE(rate_dematcher, nullptr);
    } else if (implementation == "avx512") {
      encoder_test = enc_factory_avx2->create();
      ASSERT_NE(encoder_test, nullptr);
      decoder_test = dec_factory_avx512->create();
      ASSERT_NE(decoder_test, nullptr);
      rate_dematcher = rate_dematcher_factory_avx512->create();
      ASSERT_NE(rate_dematcher, nullptr);
    } else if (implementation == "neon") {
      encoder_test = enc_factory_neon->create();
      ASSERT_NE(encoder_test, nullptr);
      decoder_test = dec_factory_neon->create();
      ASSERT_NE(decoder_test, nullptr);
      rate_dematcher = rate_dematcher_factory_neon->create();
      ASSERT_NE(rate_dematcher, nullptr);
    } else {
      FAIL() << fmt::format("Invalid implementation type {}.", implementation);
    }

    segmenter_tx = segmenter_tx_factory->create();
    ASSERT_NE(segmenter_tx, nullptr);
    segmenter_rx = segmenter_rx_factory->create();
    ASSERT_NE(segmenter_rx, nullptr);
    rate_matcher = rate_matcher_factory->create();
    ASSERT_NE(rate_matcher, nullptr);
  }

  static std::shared_ptr<ldpc_encoder_factory>        enc_factory_generic;
  static std::shared_ptr<ldpc_decoder_factory>        dec_factory_generic;
  static std::shared_ptr<ldpc_encoder_factory>        enc_factory_avx2;
  static std::shared_ptr<ldpc_decoder_factory>        dec_factory_avx2;
  static std::shared_ptr<ldpc_decoder_factory>        dec_factory_avx512;
  static std::shared_ptr<ldpc_encoder_factory>        enc_factory_neon;
  static std::shared_ptr<ldpc_decoder_factory>        dec_factory_neon;
  static std::shared_ptr<ldpc_segmenter_tx_factory>   segmenter_tx_factory;
  static std::shared_ptr<ldpc_segmenter_rx_factory>   segmenter_rx_factory;
  static std::shared_ptr<ldpc_rate_matcher_factory>   rate_matcher_factory;
  static std::shared_ptr<ldpc_rate_dematcher_factory> rate_dematcher_factory_generic;
  static std::shared_ptr<ldpc_rate_dematcher_factory> rate_dematcher_factory_avx2;
  static std::shared_ptr<ldpc_rate_dematcher_factory> rate_dematcher_factory_avx512;
  static std::shared_ptr<ldpc_rate_dematcher_factory> rate_dematcher_factory_neon;

  std::unique_ptr<ldpc_encoder>        encoder_test;
  std::unique_ptr<ldpc_decoder>        decoder_test;
  std::unique_ptr<ldpc_segmenter_tx>   segmenter_tx;
  std::unique_ptr<ldpc_segmenter_rx>   segmenter_rx;
  std::unique_ptr<ldpc_rate_matcher>   rate_matcher;
  std::unique_ptr<ldpc_rate_dematcher> rate_dematcher;
};

std::shared_ptr<ldpc_encoder_factory>        LDPCChainFixture::enc_factory_generic            = nullptr;
std::shared_ptr<ldpc_decoder_factory>        LDPCChainFixture::dec_factory_generic            = nullptr;
std::shared_ptr<ldpc_encoder_factory>        LDPCChainFixture::enc_factory_avx2               = nullptr;
std::shared_ptr<ldpc_decoder_factory>        LDPCChainFixture::dec_factory_avx2               = nullptr;
std::shared_ptr<ldpc_decoder_factory>        LDPCChainFixture::dec_factory_avx512             = nullptr;
std::shared_ptr<ldpc_encoder_factory>        LDPCChainFixture::enc_factory_neon               = nullptr;
std::shared_ptr<ldpc_decoder_factory>        LDPCChainFixture::dec_factory_neon               = nullptr;
std::shared_ptr<ldpc_segmenter_tx_factory>   LDPCChainFixture::segmenter_tx_factory           = nullptr;
std::shared_ptr<ldpc_segmenter_rx_factory>   LDPCChainFixture::segmenter_rx_factory           = nullptr;
std::shared_ptr<ldpc_rate_matcher_factory>   LDPCChainFixture::rate_matcher_factory           = nullptr;
std::shared_ptr<ldpc_rate_dematcher_factory> LDPCChainFixture::rate_dematcher_factory_generic = nullptr;
std::shared_ptr<ldpc_rate_dematcher_factory> LDPCChainFixture::rate_dematcher_factory_avx2    = nullptr;
std::shared_ptr<ldpc_rate_dematcher_factory> LDPCChainFixture::rate_dematcher_factory_avx512  = nullptr;
std::shared_ptr<ldpc_rate_dematcher_factory> LDPCChainFixture::rate_dematcher_factory_neon    = nullptr;

TEST_P(LDPCChainFixture, LDPCChainTest)
{
  const test_parameters& tp = GetParam();

  // Number of REs used for transmission.
  constexpr unsigned nof_scs_per_rb = 12;
  constexpr unsigned nof_ofdm_syms  = 12;
  unsigned           nof_res        = tp.nof_rb * nof_scs_per_rb * nof_ofdm_syms * tp.nof_layers;

  // Pick a reasonable transport block size from the parameters.
  auto     tbs_bits  = static_cast<unsigned>(nof_res * get_bits_per_symbol(tp.modulation) * tp.code_rate);
  unsigned tbs_bytes = tbs_bits / 8;

  // The base graph depends on the TBS and the coding rate.
  ldpc_base_graph_type bg = ldpc_base_graph_type::BG1;
  if ((tbs_bytes <= 36) || ((tbs_bytes <= 478) && (tp.code_rate <= 0.67)) || (tp.code_rate <= 0.25)) {
    bg = ldpc_base_graph_type::BG2;
  }

  // Configure the segmenters.
  segmenter_config cfg_seg = {.transport_block_size = units::bytes(tbs_bytes),
                              .base_graph           = bg,
                              .rv                   = 0,
                              .mod                  = tp.modulation,
                              .nof_layers           = tp.nof_layers,
                              .nof_ch_symbols       = nof_res};

  constexpr unsigned   nof_tbs = 1;
  std::vector<uint8_t> transport_block(tbs_bytes);

  for (unsigned i_tb = 0; i_tb != nof_tbs; ++i_tb) {
    // Fill the transport block with random bits.
    std::generate(transport_block.begin(), transport_block.end(), []() { return byte_gen(rgen); });

    // Initialize the segmenter.
    const ldpc_segmenter_buffer& segment_buffer = segmenter_tx->new_transmission(cfg_seg);

    unsigned             cw_length = segment_buffer.get_cw_length().value();
    std::vector<uint8_t> codeword_tx(cw_length);
    span<uint8_t>        codeword_view(codeword_tx);

    for (unsigned i_cb = 0, nof_cb = segment_buffer.get_nof_codeblocks(); i_cb != nof_cb; ++i_cb) {
      codeblock_metadata metadata = segment_buffer.get_cb_metadata(i_cb);

      // Get the message (data + CRC + zero padding + filler bits) corresponding to the i_cb codeblock.
      dynamic_bit_buffer message_packed(segment_buffer.get_segment_length().value());
      segment_buffer.read_codeblock(message_packed, transport_block, i_cb);

      // Configure the encoder and encode the message.
      ldpc_encoder::configuration cfg_enc   = {.base_graph   = metadata.tb_common.base_graph,
                                               .lifting_size = metadata.tb_common.lifting_size};
      const ldpc_encoder_buffer&  rm_buffer = encoder_test->encode(message_packed, cfg_enc);

      // Match the rate.
      unsigned           rm_length = segment_buffer.get_rm_length(i_cb);
      dynamic_bit_buffer output_buffer(rm_length);
      rate_matcher->rate_match(output_buffer, rm_buffer, metadata);

      // Append the codeblock at the end of the codeword.
      ocuduvec::bit_unpack(codeword_view.first(rm_length), output_buffer);
      codeword_view = codeword_view.last(codeword_view.size() - rm_length);
    }

    ASSERT_TRUE(codeword_view.empty()) << "Codeword view should be empty.";

    // Convert to LLR (BPSK modulation, even if this is not the configured modulation).
    std::vector<log_likelihood_ratio> llr(codeword_tx.size());
    std::transform(codeword_tx.begin(), codeword_tx.end(), llr.begin(), [](uint8_t b) {
      return log_likelihood_ratio::copysign(LLRS_AMPL, 1 - 2 * b);
    });

    // Segment the received codeword.
    static_vector<described_rx_codeblock, MAX_NOF_SEGMENTS> codeblocks;
    segmenter_rx->segment(codeblocks, llr, cfg_seg);

    dynamic_bit_buffer decoded_tbs_packed(tbs_bytes * 8);
    unsigned           offset           = 0;
    unsigned           nof_missing_bits = tbs_bytes * 8;

    for (const auto& cb : codeblocks) {
      span<const log_likelihood_ratio> cb_data = cb.first;
      const codeblock_metadata&        cb_meta = cb.second;

      // Recover the full codeblock.
      std::vector<log_likelihood_ratio> llr_dematched(cb_meta.cb_specific.full_length);
      rate_dematcher->rate_dematch(llr_dematched, cb_data, /*new_data=*/true, cb_meta);

      // Configure and run the decoder.
      ldpc_decoder::configuration cfg_dec = {.base_graph   = cb_meta.tb_common.base_graph,
                                             .lifting_size = cb_meta.tb_common.lifting_size};
      auto [msg_length, data_length]      = get_nof_data_bits(cb_meta);
      dynamic_bit_buffer decoded_bits(msg_length);
      decoder_test->decode(decoded_bits, llr_dematched, nullptr, cfg_dec);

      // Append the information bits (there can be zero padding at the end of the last message).
      unsigned nof_useful_bits = std::min(data_length, nof_missing_bits);
      fill_bit_buffer(decoded_tbs_packed, decoded_bits.first(nof_useful_bits).get_reader(), offset);
      offset += data_length;
      nof_missing_bits -= nof_useful_bits;
    }
    ASSERT_EQ(nof_missing_bits, 0) << "decoded_tbs_packed should be full.";

    ASSERT_EQ(decoded_tbs_packed.get_buffer(), span<uint8_t>(transport_block))
        << "Sent and received transport blocks do not match.";
  }
}

static std::vector<test_parameters> generate_cases()
{
  std::vector<test_parameters> out;
  static const auto            implementation = to_array<std::string>({"generic"
#ifdef __AVX2__
                                                            ,
                                                            "avx2"
#endif // __AVX2__
#if defined(__AVX512F__) && defined(__AVX512BW__) && defined(__AVX512VBMI__)
                                                            ,
                                                            "avx512"
#endif // defined(__AVX512F__) && defined(__AVX512BW__) && defined(__AVX512VBMI__)
#ifdef __ARM_NEON
                                                            ,
                                                            "neon"
#endif // __ARM_NEON
  });
  for (const auto& i : implementation) {
    // Resource blocks.
    for (auto rb : {2U, 20U, 50U}) {
      // Layers.
      for (auto l : {1U, 2U, 4U}) {
        // Code rate.
        for (auto cr : {0.2F, 0.4F, 0.6F, 0.9F}) {
          for (auto m : {modulation_scheme::QPSK, modulation_scheme::QAM64}) {
            out.emplace_back(i, rb, l, cr, m);
          }
        }
      }
    }
  }
  return out;
}

static std::vector<test_parameters> all_tests = generate_cases();

INSTANTIATE_TEST_SUITE_P(LDPC, LDPCChainFixture, ::testing::ValuesIn(all_tests));
