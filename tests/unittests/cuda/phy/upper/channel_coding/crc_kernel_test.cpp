// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/cuda/adt/cuda_copy.h"
#include "ocudu/cuda/adt/cuda_stream.h"
#include "ocudu/cuda/adt/device_vector.h"
#include "ocudu/cuda/phy/upper/channel_coding/crc.h"
#include "ocudu/phy/upper/channel_coding/channel_coding_factories.h"
#include <cuda_runtime.h>
#include <gtest/gtest.h>
#include <random>
#include <vector>

using namespace ocudu;
using namespace ocudu::cuda;

namespace {

/// Returns true if a CUDA device is available to run the test.
bool has_cuda_device()
{
  int nof_devices = 0;
  return (::cudaGetDeviceCount(&nof_devices) == cudaSuccess) && (nof_devices > 0);
}

/// Payload of pseudo-random bytes, the same for a given size on every run.
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

class crc_kernel_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    if (!has_cuda_device()) {
      GTEST_SKIP() << "No CUDA device available";
    }

    // The tables the checksum kernels index are populated once per process.
    ASSERT_TRUE(crc_init_tables().has_value());

    cuda_expected<cuda_stream> created = cuda_stream::create();
    ASSERT_TRUE(created.has_value());
    stream = std::move(created.value());

    crc_factory = create_crc_calculator_factory_sw("auto");
    ASSERT_NE(crc_factory, nullptr);
  }

  /// Computes a checksum on the device for a payload held on the host.
  uint32_t compute_on_device(span<const uint8_t> data, crc_generator_poly poly)
  {
    cuda_expected<device_vector<uint8_t>> d_data = device_vector<uint8_t>::create(data.size());
    EXPECT_TRUE(d_data.has_value());
    EXPECT_TRUE(copy_to_device(d_data.value(), span<const uint8_t>(data.data(), data.size())).has_value());

    cuda_expected<device_vector<uint32_t>> d_crc = device_vector<uint32_t>::create(1);
    EXPECT_TRUE(d_crc.has_value());

    unsigned    nof_bits = static_cast<unsigned>(data.size() * 8);
    cuda_result status;
    switch (poly) {
      case crc_generator_poly::CRC24A:
        status = crc24a_compute(d_crc.value().data(), d_data.value().data(), nof_bits, stream);
        break;
      case crc_generator_poly::CRC24B:
        status = crc24b_compute(d_crc.value().data(), d_data.value().data(), nof_bits, stream);
        break;
      case crc_generator_poly::CRC16:
        status =
            crc16_compute(reinterpret_cast<uint16_t*>(d_crc.value().data()), d_data.value().data(), nof_bits, stream);
        break;
      default:
        EXPECT_TRUE(false) << "unsupported polynomial";
    }
    EXPECT_TRUE(status.has_value());
    EXPECT_EQ(::cudaDeviceSynchronize(), cudaSuccess);

    uint32_t checksum = 0;
    EXPECT_TRUE(copy_to_host(span<uint32_t>(&checksum, 1), d_crc.value()).has_value());

    unsigned nof_crc_bits = get_crc_size(poly);
    return checksum & ((nof_crc_bits >= 32) ? ~0U : ((1U << nof_crc_bits) - 1U));
  }

  cuda_stream                             stream;
  std::shared_ptr<crc_calculator_factory> crc_factory;
};

} // namespace

TEST_F(crc_kernel_test, crc24a_matches_the_generic_calculator)
{
  std::unique_ptr<crc_calculator> reference = crc_factory->create(crc_generator_poly::CRC24A);
  ASSERT_NE(reference, nullptr);

  // A transport block is a few kilobytes; the sizes below bracket that range.
  for (std::size_t nof_bytes : {64U, 256U, 1024U, 3072U, 8192U}) {
    std::vector<uint8_t> payload = make_payload(nof_bytes);
    EXPECT_EQ(compute_on_device(payload, crc_generator_poly::CRC24A), reference->calculate_byte(payload))
        << "payload of " << nof_bytes << " bytes";
  }
}

TEST_F(crc_kernel_test, crc24b_matches_the_generic_calculator)
{
  std::unique_ptr<crc_calculator> reference = crc_factory->create(crc_generator_poly::CRC24B);
  ASSERT_NE(reference, nullptr);

  for (std::size_t nof_bytes : {64U, 512U, 1024U}) {
    std::vector<uint8_t> payload = make_payload(nof_bytes);
    EXPECT_EQ(compute_on_device(payload, crc_generator_poly::CRC24B), reference->calculate_byte(payload))
        << "payload of " << nof_bytes << " bytes";
  }
}

TEST_F(crc_kernel_test, crc16_matches_the_generic_calculator)
{
  std::unique_ptr<crc_calculator> reference = crc_factory->create(crc_generator_poly::CRC16);
  ASSERT_NE(reference, nullptr);

  for (std::size_t nof_bytes : {64U, 512U, 1024U}) {
    std::vector<uint8_t> payload = make_payload(nof_bytes);
    EXPECT_EQ(compute_on_device(payload, crc_generator_poly::CRC16), reference->calculate_byte(payload))
        << "payload of " << nof_bytes << " bytes";
  }
}

TEST_F(crc_kernel_test, an_all_zero_payload_gives_a_zero_checksum)
{
  // The checksum of an all-zero message is zero for these polynomials, which catches a table that
  // was never populated: an uninitialised table also returns zero for other payloads.
  std::unique_ptr<crc_calculator> reference = crc_factory->create(crc_generator_poly::CRC24A);
  std::vector<uint8_t>            zeros(1024, 0);

  EXPECT_EQ(compute_on_device(zeros, crc_generator_poly::CRC24A), reference->calculate_byte(zeros));

  // A payload differing in one bit must not give the same checksum.
  std::vector<uint8_t> one_bit = zeros;
  one_bit[512]                 = 0x01;
  EXPECT_NE(compute_on_device(one_bit, crc_generator_poly::CRC24A),
            compute_on_device(zeros, crc_generator_poly::CRC24A));
}

TEST_F(crc_kernel_test, a_check_separates_a_mismatch_from_a_failure)
{
  // The check reports whether the checksum matches. A corrupt block gives a successful result
  // carrying false, not an error.
  std::vector<uint8_t>            payload   = make_payload(256);
  std::unique_ptr<crc_calculator> reference = crc_factory->create(crc_generator_poly::CRC24A);
  uint32_t                        checksum  = reference->calculate_byte(payload);

  std::vector<uint8_t> with_crc = payload;
  with_crc.push_back(static_cast<uint8_t>(checksum >> 16));
  with_crc.push_back(static_cast<uint8_t>(checksum >> 8));
  with_crc.push_back(static_cast<uint8_t>(checksum));

  cuda_expected<device_vector<uint8_t>> d_data = device_vector<uint8_t>::create(with_crc.size());
  ASSERT_TRUE(d_data.has_value());
  ASSERT_TRUE(copy_to_device(d_data.value(), span<const uint8_t>(with_crc.data(), with_crc.size())).has_value());

  cuda_expected<bool> matched = crc24a_check(d_data.value().data(), with_crc.size() * 8, stream);
  ASSERT_TRUE(matched.has_value());
  EXPECT_TRUE(matched.value());

  // Flipping a bit leaves the call successful and the answer false.
  with_crc[128] ^= 0x01;
  ASSERT_TRUE(copy_to_device(d_data.value(), span<const uint8_t>(with_crc.data(), with_crc.size())).has_value());

  cuda_expected<bool> corrupted = crc24a_check(d_data.value().data(), with_crc.size() * 8, stream);
  ASSERT_TRUE(corrupted.has_value());
  EXPECT_FALSE(corrupted.value());
}
