// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/adt/to_array.h"
#include "ocudu/phy/upper/channel_coding/channel_coding_factories.h"
#include "ocudu/support/cpu_features.h"
#include "fmt/ostream.h"
#include <functional>
#include <getopt.h>
#include <gtest/gtest.h>
#include <random>

using namespace ocudu;

namespace {

struct test_parameters {
  std::string        factory_type;
  crc_generator_poly polynomial;
  units::bytes       nbytes;
};

std::ostream& operator<<(std::ostream& os, const test_parameters& params)
{
  fmt::print(os,
             "type={} poly=0x{:x} order={} nbytes={}",
             params.factory_type,
             to_uint(params.polynomial),
             get_crc_size(params.polynomial),
             params.nbytes);
  return os;
}

class CRCCalculatorFixture : public ::testing::TestWithParam<test_parameters>
{
protected:
  void SetUp() override
  {
    const test_parameters& params = GetParam();

    std::shared_ptr<crc_calculator_factory> factory = create_crc_calculator_factory_sw(params.factory_type);
    ASSERT_NE(factory, nullptr) << "Failed to create factory";

    crc_calc = factory->create(params.polynomial);
    ASSERT_NE(factory, nullptr) << "Failed to create CRC calculator.";
  }

  /// Generic CRC calculation for unpacked bits (one bit per byte).
  static crc_calculator_checksum_t crc_generic_calculator_bit(const span<uint8_t>& data, crc_generator_poly polynomial)
  {
    unsigned poly  = to_uint(polynomial);
    unsigned order = get_crc_size(polynomial);

    uint64_t highbit   = 1U << order;
    uint64_t remainder = 0;

    for (uint8_t bit : data) {
      remainder = (remainder << 1U) | bit;

      if ((remainder & highbit) != 0) {
        remainder ^= poly;
      }
    }

    for (unsigned bit_idx = 0; bit_idx != order; ++bit_idx) {
      remainder = (remainder << 1U);

      if ((remainder & highbit) != 0) {
        remainder ^= poly;
      }
    }

    return static_cast<crc_calculator_checksum_t>(remainder & (highbit - 1));
  }

  /// Generic CRC calculation for packed bits (eight bits per byte).
  static crc_calculator_checksum_t crc_generic_calculator_byte(span<const uint8_t> data, crc_generator_poly polynomial)
  {
    unsigned poly  = to_uint(polynomial);
    unsigned order = get_crc_size(polynomial);

    uint64_t highbit   = 1U << order;
    uint64_t remainder = 0;

    for (uint8_t v : data) {
      for (unsigned bit_idx = 0; bit_idx != 8; ++bit_idx) {
        uint64_t bit = (static_cast<uint64_t>(v) >> (7UL - bit_idx)) & 1UL;

        remainder = (remainder << 1U) | bit;

        if ((remainder & highbit) != 0) {
          remainder ^= poly;
        }
      }
    }

    for (unsigned bit_idx = 0; bit_idx != order; ++bit_idx) {
      remainder = (remainder << 1U);

      if ((remainder & highbit) != 0) {
        remainder ^= poly;
      }
    }

    return static_cast<crc_calculator_checksum_t>(remainder & (highbit - 1));
  }

  /// Generic CRC calculation for packed bits using a reference to a bit buffer.
  static crc_calculator_checksum_t crc_generic_calculator(const bit_buffer& data, crc_generator_poly polynomial)
  {
    return crc_generic_calculator_byte(data.get_buffer(), polynomial);
  }

  std::unique_ptr<crc_calculator> crc_calc;
  static std::mt19937             rgen;
};

std::mt19937 CRCCalculatorFixture::rgen;

} // namespace

TEST_P(CRCCalculatorFixture, TestCRCBit)
{
  const test_parameters& params = GetParam();

  // Create data buffer.
  std::vector<uint8_t> data;
  data.reserve(params.nbytes.value());
  std::generate_n(std::back_inserter(data), params.nbytes.value(), []() { return rgen() & 1; });

  // Calculate checksum in library.
  crc_calculator_checksum_t checksum = crc_calc->calculate_bit(data);

  // Calculate ideal CRC checksum.
  crc_calculator_checksum_t checksum_gold = crc_generic_calculator_bit(data, params.polynomial);

  ASSERT_EQ(checksum, checksum_gold) << "Packed CRC calculator checksum failed";
}

TEST_P(CRCCalculatorFixture, TestCRCByte)
{
  const test_parameters& params = GetParam();

  // Create data buffer.
  std::vector<uint8_t> data;
  data.reserve(params.nbytes.value());
  std::generate_n(std::back_inserter(data), params.nbytes.value(), std::ref(rgen));

  // Calculate checksum in library.
  crc_calculator_checksum_t checksum = crc_calc->calculate_byte(data);

  // Calculate ideal CRC checksum.
  crc_calculator_checksum_t checksum_gold = crc_generic_calculator_byte(data, params.polynomial);

  ASSERT_EQ(checksum, checksum_gold) << "Unpacked CRC calculator checksum failed";
}

TEST_P(CRCCalculatorFixture, TestCRCBitBuffer)
{
  const test_parameters& params = GetParam();

  // Create data buffer.
  dynamic_bit_buffer data(params.nbytes.to_bits().value());
  std::generate(data.get_buffer().begin(), data.get_buffer().end(), std::ref(rgen));

  // Calculate checksum in library.
  crc_calculator_checksum_t checksum = crc_calc->calculate(data);

  // Calculate ideal CRC checksum.
  crc_calculator_checksum_t checksum_gold = crc_generic_calculator(data, params.polynomial);

  ASSERT_EQ(checksum, checksum_gold) << "Bitbuffer CRC calculator checksum failed";
}

static std::vector<test_parameters> generate_cases()
{
  using namespace units::literals;

  std::vector<test_parameters> out;

  // Types of CRC calculators.
  for (std::string factory_type : {"generic", "lut", "clmul", "neon"}) {
    // Skip factory types that are not supported by the CPU.
#ifdef __x86_64__
    if ((factory_type == "neon") || ((factory_type == "clmul") && !cpu_supports_feature(cpu_feature::pclmul))) {
      continue;
    }
#elif defined(__aarch64__)
    if ((factory_type == "clmul") || ((factory_type == "neon") && !cpu_supports_feature(cpu_feature::pmull))) {
      continue;
    }
#endif

    // 5G NR CRC polynomials and some arbitrary polynomials for verifying non 5G NR standard polynomial sizes.
    for (crc_generator_poly polynomial : {crc_generator_poly::CRC24A,
                                          crc_generator_poly::CRC24B,
                                          crc_generator_poly::CRC24C,
                                          crc_generator_poly::CRC16,
                                          crc_generator_poly::CRC11,
                                          crc_generator_poly::CRC6,
                                          crc_polynomial_from_uint(0x1fff),
                                          crc_polynomial_from_uint(0x1ff),
                                          crc_polynomial_from_uint(0xf)}) {
      // Message length in bytes. Selected arbitrary values that execute all execution branches in the different
      // implementations that have SIMD.
      for (units::bytes nbytes : {1_bytes, 8_bytes, 16_bytes, 32_bytes, 257_bytes, 997_bytes, 6012_bytes}) {
        out.emplace_back(test_parameters{.factory_type = factory_type, .polynomial = polynomial, .nbytes = nbytes});
      }
    }
  }
  return out;
}

static std::vector<test_parameters> all_tests = generate_cases();

INSTANTIATE_TEST_SUITE_P(CRC, CRCCalculatorFixture, ::testing::ValuesIn(all_tests));
