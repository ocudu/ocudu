// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/cuda/adt/cuda_copy.h"
#include "ocudu/cuda/adt/cuda_stream.h"
#include "ocudu/cuda/adt/device_vector.h"
#include "ocudu/cuda/phy/upper/channel_processors/pdsch/transport_block_encode.h"
#include "ocudu/ran/sch/sch_segmentation.h"
#include <cuda_runtime.h>
#include <gtest/gtest.h>
#include <numeric>
#include <random>
#include <vector>

using namespace ocudu;

namespace {

/// Returns true if a CUDA device is available to run the test.
bool has_cuda_device()
{
  int nof_devices = 0;
  return (::cudaGetDeviceCount(&nof_devices) == cudaSuccess) && (nof_devices > 0);
}

/// Fills a payload with pseudo-random bytes, the same for a given size on every run.
std::vector<uint8_t> make_payload(std::size_t nof_bytes)
{
  std::vector<uint8_t>                    data(nof_bytes);
  std::mt19937                            rgen(0x5eed);
  std::uniform_int_distribution<unsigned> dist(0, 255);
  for (uint8_t& byte : data) {
    byte = static_cast<uint8_t>(dist(rgen));
  }
  return data;
}

class transport_block_encode_kernel_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    ASSERT_TRUE(has_cuda_device()) << "No CUDA device available";

    cuda::cuda_expected<cuda::cuda_stream> created_stream = cuda::cuda_stream::create();
    ASSERT_TRUE(created_stream.has_value());
    stream = std::move(created_stream.value());

    cuda::cuda_expected<cuda::transport_block_encoder> created = cuda::transport_block_encoder::create();
    ASSERT_TRUE(created.has_value()) << created.error();
    gpu = std::move(created.value());
  }

  cuda::cuda_stream             stream;
  cuda::transport_block_encoder gpu;
};

} // namespace

TEST_F(transport_block_encode_kernel_test, the_encoder_is_configured_and_reports_its_geometry)
{
  // A 273 PRB, 4 layer, 64QAM allocation, which is the shape the downlink runs at full rate.
  cuda::transport_block_encoder_config config = {};
  config.nof_tb_bits                          = 24576;
  config.nof_layers                           = 4;
  config.modulation                           = modulation_scheme::QAM64;
  config.nof_allocated_bits                   = 49152;
  config.redundancy_version                   = 0;
  config.code_rate                            = 0.5F;
  config.rnti                                 = 0x1234;
  config.n_id                                 = 1;
  config.codeword_index                       = 0;
  config.enable_scrambling                    = true;

  ASSERT_TRUE(gpu.configure(config).has_value());

  // The geometry the encoder derives has to agree with the allocation it was given.
  EXPECT_EQ(gpu.get_nof_output_bits(), config.nof_allocated_bits);
  EXPECT_GT(gpu.get_nof_codeblocks(), 0U);
  EXPECT_GT(static_cast<unsigned>(gpu.get_lifting_size()), 0U);
}

TEST_F(transport_block_encode_kernel_test, a_limited_buffer_changes_the_bits_a_retransmission_carries)
{
  // A transport block large enough that the limited circular buffer of TS 38.212, Section 5.4.2.1
  // is shorter than the code blocks, at a redundancy version whose starting position sits late in
  // the buffer. The encoder has to hand the limit to the rate matcher: without it the selection
  // wraps at the whole buffer and takes bits the limited one does not hold.
  cuda::transport_block_encoder_config config = {};
  config.nof_tb_bits                          = 24576;
  config.nof_layers                           = 4;
  config.modulation                           = modulation_scheme::QAM64;
  config.nof_allocated_bits                   = 49152;
  config.redundancy_version                   = 2;
  config.code_rate                            = 0.5F;
  config.rnti                                 = 0x1234;
  config.n_id                                 = 1;
  config.codeword_index                       = 0;
  config.enable_scrambling                    = true;

  std::vector<uint8_t> payload = make_payload(config.nof_tb_bits / 8);

  cuda::cuda_expected<cuda::device_vector<uint8_t>> d_input = cuda::device_vector<uint8_t>::create(payload.size());
  ASSERT_TRUE(d_input.has_value());
  ASSERT_TRUE(cuda::copy_to_device(d_input.value(), span<const uint8_t>(payload)).has_value());

  // Encode once with the whole buffer and once with it limited.
  std::vector<std::vector<uint8_t>> encoded;
  for (unsigned nof_buffer_bits : {0U, 12288U}) {
    config.nof_buffer_bits = nof_buffer_bits;
    ASSERT_TRUE(gpu.configure(config).has_value()) << "buffer of " << nof_buffer_bits;

    const std::size_t                                 nof_output_bytes = gpu.get_nof_output_bits() / 8;
    cuda::cuda_expected<cuda::device_vector<uint8_t>> d_out = cuda::device_vector<uint8_t>::create(nof_output_bytes);
    ASSERT_TRUE(d_out.has_value());
    ASSERT_TRUE(gpu.encode(d_out.value().data(), d_input.value().data(), stream).has_value());
    ASSERT_EQ(::cudaDeviceSynchronize(), cudaSuccess);

    std::vector<uint8_t> bits(nof_output_bytes);
    ASSERT_TRUE(cuda::copy_to_host(span<uint8_t>(bits), d_out.value()).has_value());
    encoded.push_back(std::move(bits));
  }

  // The limit reaches the selection, so the two transmissions cannot carry the same bits. An
  // encoder that ignored it would produce the first sequence twice.
  EXPECT_NE(encoded[0], encoded[1]);

  // Neither is empty, which the comparison above would not catch on its own.
  EXPECT_NE(std::accumulate(encoded[1].begin(), encoded[1].end(), 0U), 0U);
}

TEST_F(transport_block_encode_kernel_test, encoding_the_same_transport_block_twice_gives_the_same_bits)
{
  cuda::transport_block_encoder_config config = {};
  config.nof_tb_bits                          = 8448;
  config.nof_layers                           = 1;
  config.modulation                           = modulation_scheme::QPSK;
  config.nof_allocated_bits                   = 16896;
  config.redundancy_version                   = 0;
  config.code_rate                            = 0.5F;
  config.rnti                                 = 0x4321;
  config.n_id                                 = 3;
  config.codeword_index                       = 0;
  config.enable_scrambling                    = true;
  ASSERT_TRUE(gpu.configure(config).has_value());

  std::vector<uint8_t> payload = make_payload(config.nof_tb_bits / 8);

  cuda::cuda_expected<cuda::device_vector<uint8_t>> d_input = cuda::device_vector<uint8_t>::create(payload.size());
  ASSERT_TRUE(d_input.has_value());
  ASSERT_TRUE(cuda::copy_to_device(d_input.value(), span<const uint8_t>(payload)).has_value());

  const std::size_t nof_output_bytes = gpu.get_nof_output_bits() / 8;

  cuda::cuda_expected<cuda::device_vector<uint8_t>> d_first  = cuda::device_vector<uint8_t>::create(nof_output_bytes);
  cuda::cuda_expected<cuda::device_vector<uint8_t>> d_second = cuda::device_vector<uint8_t>::create(nof_output_bytes);
  ASSERT_TRUE(d_first.has_value());
  ASSERT_TRUE(d_second.has_value());

  ASSERT_TRUE(gpu.encode(d_first.value().data(), d_input.value().data(), stream).has_value());
  ASSERT_TRUE(gpu.encode(d_second.value().data(), d_input.value().data(), stream).has_value());
  ASSERT_EQ(::cudaDeviceSynchronize(), cudaSuccess);

  std::vector<uint8_t> first(nof_output_bytes);
  std::vector<uint8_t> second(nof_output_bytes);
  ASSERT_TRUE(cuda::copy_to_host(span<uint8_t>(first), d_first.value()).has_value());
  ASSERT_TRUE(cuda::copy_to_host(span<uint8_t>(second), d_second.value()).has_value());

  // The encoder holds state between calls, so a repeated encode is the cheapest check that none of
  // it leaks from one transmission into the next.
  EXPECT_EQ(first, second);

  // An encoder that produced nothing would also pass the comparison above.
  EXPECT_NE(std::accumulate(first.begin(), first.end(), 0U), 0U);
}

TEST_F(transport_block_encode_kernel_test, a_filler_count_of_zero_is_taken_as_given)
{
  // A transport block whose segmentation leaves filler bits, so that a count of zero from the
  // caller and the encoder's own count cannot agree.
  cuda::transport_block_encoder_config config = {};
  config.nof_tb_bits                          = 280;
  config.nof_layers                           = 1;
  config.modulation                           = modulation_scheme::QPSK;
  config.nof_allocated_bits                   = 1120;
  config.code_rate                            = 0.25F;
  config.enable_scrambling                    = false;

  // Derive the shape once, so the lifting size handed back below is the one the encoder picked.
  ASSERT_TRUE(gpu.configure(config).has_value());
  config.lifting_size = gpu.get_lifting_size();
  config.base_graph   = gpu.get_base_graph();

  std::vector<uint8_t> payload = make_payload(config.nof_tb_bits / 8);

  cuda::cuda_expected<cuda::device_vector<uint8_t>> d_input = cuda::device_vector<uint8_t>::create(payload.size());
  ASSERT_TRUE(d_input.has_value());
  ASSERT_TRUE(cuda::copy_to_device(d_input.value(), span<const uint8_t>(payload)).has_value());

  // Encode once letting the encoder keep its own filler count, and once with a count of zero.
  std::vector<std::vector<uint8_t>> encoded;
  for (bool give_zero_filler : {false, true}) {
    if (give_zero_filler) {
      config.nof_filler_bits = 0;
    }
    ASSERT_TRUE(gpu.configure(config).has_value()) << "zero filler: " << give_zero_filler;

    const std::size_t                                 nof_output_bytes = gpu.get_nof_output_bits() / 8;
    cuda::cuda_expected<cuda::device_vector<uint8_t>> d_out = cuda::device_vector<uint8_t>::create(nof_output_bytes);
    ASSERT_TRUE(d_out.has_value());
    ASSERT_TRUE(gpu.encode(d_out.value().data(), d_input.value().data(), stream).has_value());
    ASSERT_EQ(::cudaDeviceSynchronize(), cudaSuccess);

    std::vector<uint8_t> bits(nof_output_bytes);
    ASSERT_TRUE(cuda::copy_to_host(span<uint8_t>(bits), d_out.value()).has_value());
    encoded.push_back(std::move(bits));
  }

  // The information length differs between the two, so the codewords cannot match. An encoder that
  // read the zero as a request to derive the count would produce the first sequence twice.
  EXPECT_NE(encoded[0], encoded[1]);
}

TEST_F(transport_block_encode_kernel_test, a_filler_count_outside_the_code_block_is_refused)
{
  // A transport block of 1000 bits carries its 16 bit checksum in one code block of 1016 bits.
  // The encoder places it on BG2 with a lifting size of 104, so K' is 1040 and 24 bits are left
  // for filler.
  cuda::transport_block_encoder_config config = {};
  config.nof_tb_bits                          = 1000;
  config.nof_layers                           = 1;
  config.modulation                           = modulation_scheme::QPSK;
  config.nof_allocated_bits                   = 2048;
  config.code_rate                            = 0.5F;
  config.enable_scrambling                    = false;

  ASSERT_TRUE(gpu.configure(config).has_value());
  ASSERT_EQ(gpu.get_base_graph(), ldpc_base_graph_type::BG2);
  ASSERT_EQ(static_cast<unsigned>(gpu.get_lifting_size()), 104U);

  // A filler count above K' leaves an information length below zero.
  config.nof_filler_bits = 2000;
  EXPECT_FALSE(gpu.configure(config).has_value());
  config.nof_filler_bits.reset();

  // A lifting size of 96 holds 960 bits of a code block that carries 1016, so the count the
  // encoder derives for it is below zero.
  config.lifting_size = ldpc::LS96;
  EXPECT_FALSE(gpu.configure(config).has_value());
}

TEST_F(transport_block_encode_kernel_test, the_largest_transport_block_is_accepted)
{
  // The reservation holds the codeword buffer the LDPC encoder writes, which is all 68 BG1 columns
  // per code block. A reservation sized for 66 refuses the transport blocks with the most code
  // blocks, which are the largest downlink grants.
  cuda::transport_block_encoder_config config = {};
  config.nof_tb_bits                          = 1277992;
  config.nof_layers                           = 4;
  config.modulation                           = modulation_scheme::QAM256;
  config.nof_allocated_bits                   = 2000000;
  config.code_rate                            = 0.85F;
  config.enable_scrambling                    = true;

  cuda::cuda_result configured = gpu.configure(config);
  ASSERT_TRUE(configured.has_value()) << configured.error();
  EXPECT_GT(gpu.get_nof_codeblocks(), 147U);
}

TEST_F(transport_block_encode_kernel_test, a_base_graph_from_the_caller_carries_the_lifting_size_with_it)
{
  // A transport block the encoder places on BG2 on its own.
  cuda::transport_block_encoder_config config = {};
  config.nof_tb_bits                          = 1000;
  config.nof_layers                           = 1;
  config.modulation                           = modulation_scheme::QPSK;
  config.nof_allocated_bits                   = 2048;
  config.code_rate                            = 0.5F;
  config.enable_scrambling                    = false;

  ASSERT_TRUE(gpu.configure(config).has_value());
  ASSERT_EQ(gpu.get_base_graph(), ldpc_base_graph_type::BG2);
  ASSERT_EQ(static_cast<unsigned>(gpu.get_lifting_size()), 104U);

  // BG1 holds 22 information columns where BG2 holds 10, so a code block of the same length needs
  // a smaller lifting size. A base graph that arrives on its own has to carry the rest of the
  // geometry with it.
  config.base_graph = ldpc_base_graph_type::BG1;
  ASSERT_TRUE(gpu.configure(config).has_value());
  EXPECT_EQ(gpu.get_base_graph(), ldpc_base_graph_type::BG1);
  EXPECT_EQ(static_cast<unsigned>(gpu.get_lifting_size()), 48U);
}

TEST_F(transport_block_encode_kernel_test, a_base_graph_the_encoder_derives_needs_a_code_rate)
{
  // A configuration that leaves the base graph and the code rate as they come. The base graph is
  // derived from the rate alone, so this one gives nothing to derive it from.
  cuda::transport_block_encoder_config config = {};
  config.nof_tb_bits                          = 1000;
  config.nof_layers                           = 1;
  config.modulation                           = modulation_scheme::QPSK;
  config.nof_allocated_bits                   = 2048;
  config.enable_scrambling                    = false;

  EXPECT_FALSE(gpu.configure(config).has_value());

  // The rate is read for the base graph alone. A caller that gives the base graph owes no rate.
  config.base_graph            = ldpc_base_graph_type::BG2;
  cuda::cuda_result configured = gpu.configure(config);
  EXPECT_TRUE(configured.has_value()) << configured.error();
}

TEST_F(transport_block_encode_kernel_test, a_code_block_count_from_the_caller_carries_the_lifting_size_with_it)
{
  // A transport block the encoder segments into three code blocks on its own.
  cuda::transport_block_encoder_config config = {};
  config.nof_tb_bits                          = 20000;
  config.nof_layers                           = 1;
  config.modulation                           = modulation_scheme::QPSK;
  config.nof_allocated_bits                   = 40960;
  config.code_rate                            = 0.5F;
  config.enable_scrambling                    = false;

  ASSERT_TRUE(gpu.configure(config).has_value());
  ASSERT_EQ(gpu.get_nof_codeblocks(), 3U);
  ASSERT_EQ(static_cast<unsigned>(gpu.get_lifting_size()), 320U);

  // Spreading the same transport block over four code blocks makes each of them shorter, and a
  // shorter code block needs a smaller lifting size.
  config.nof_codeblocks = 4;
  ASSERT_TRUE(gpu.configure(config).has_value());
  EXPECT_EQ(gpu.get_nof_codeblocks(), 4U);
  EXPECT_EQ(static_cast<unsigned>(gpu.get_lifting_size()), 240U);
}

TEST_F(transport_block_encode_kernel_test, the_geometry_matches_the_segmentation_of_the_generic_path)
{
  // Transport block sizes from the smallest the standard allows up to the largest, at the rates a
  // downlink grant reaches. A receiver derives the geometry from the transport block size and the
  // rate alone, so a geometry the encoder derives differently is one that receiver cannot decode.
  const std::vector<unsigned> sizes = {56,   120,   184,   280,   408,   552,    696,   888,  1160,
                                       1480, 1864,  2280,  2792,  3496,  3824,   3840,  4096, 6272,
                                       8192, 12296, 20000, 25600, 51024, 102048, 204800};
  const std::vector<float>    rates = {0.25F, 0.5F, 0.85F};

  for (float rate : rates) {
    for (unsigned tbs : sizes) {
      cuda::transport_block_encoder_config config = {};
      config.nof_tb_bits                          = tbs;
      config.nof_layers                           = 1;
      config.modulation                           = modulation_scheme::QPSK;
      config.nof_allocated_bits                   = ((static_cast<unsigned>(tbs / rate) + 15U) / 16U) * 16U;
      config.code_rate                            = rate;
      config.enable_scrambling                    = false;

      cuda::cuda_result configured = gpu.configure(config);
      ASSERT_TRUE(configured.has_value()) << "rate " << rate << ", size " << tbs << ": " << configured.error();

      ldpc_base_graph_type base_graph     = get_ldpc_base_graph(rate, units::bits(tbs));
      unsigned             nof_codeblocks = compute_nof_codeblocks(units::bits(tbs), base_graph);
      unsigned             lifting_size   = compute_lifting_size(units::bits(tbs), base_graph, nof_codeblocks);

      EXPECT_EQ(gpu.get_base_graph(), base_graph) << "rate " << rate << ", size " << tbs;
      EXPECT_EQ(gpu.get_nof_codeblocks(), nof_codeblocks) << "rate " << rate << ", size " << tbs;
      EXPECT_EQ(static_cast<unsigned>(gpu.get_lifting_size()), lifting_size) << "rate " << rate << ", size " << tbs;
    }
  }
}
