// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/cuda/adt/cuda_copy.h"
#include "ocudu/cuda/adt/cuda_stream.h"
#include "ocudu/cuda/adt/device_vector.h"
#include "ocudu/cuda/phy/upper/channel_processors/pdsch/transport_block_encode.h"
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
