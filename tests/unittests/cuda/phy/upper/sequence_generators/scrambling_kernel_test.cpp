// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/cuda/adt/cuda_copy.h"
#include "ocudu/cuda/adt/cuda_stream.h"
#include "ocudu/cuda/adt/device_vector.h"
#include "ocudu/cuda/phy/upper/sequence_generators/scrambling.h"
#include "ocudu/phy/upper/sequence_generators/sequence_generator_factories.h"
#include <cuda_fp16.h>
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

/// Sequence initialisers the tests run over, covering a small value, a large one and a realistic
/// one built from an RNTI and a cell identifier.
const std::vector<uint32_t> initialisers = {0x0, 0x1, 0x5eed, (0x4601U << 15U) + 321U, 0x7ffffffe};

/// Returns bit \c index of a sequence the device packed, which places bit \c n at position
/// <tt>31 - n modulo 32</tt> of word <tt>n / 32</tt>.
uint8_t device_sequence_bit(const std::vector<uint32_t>& words, unsigned index)
{
  return static_cast<uint8_t>((words[index / 32] >> (31U - (index % 32U))) & 1U);
}

class scrambling_kernel_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    if (!has_cuda_device()) {
      GTEST_SKIP() << "No CUDA device available";
    }

    cuda_expected<cuda_stream> created_stream = cuda_stream::create();
    ASSERT_TRUE(created_stream.has_value());
    stream = std::move(created_stream.value());

    cuda_expected<scrambler> created = scrambler::create();
    ASSERT_TRUE(created.has_value()) << created.error();
    gpu = std::move(created.value());

    reference = create_pseudo_random_generator_sw_factory()->create();
    ASSERT_NE(reference, nullptr);
  }

  /// Fills a bit buffer with pseudo-random bits, the same for a given size on every run.
  static dynamic_bit_buffer make_bits(unsigned nof_bits)
  {
    dynamic_bit_buffer                      bits(nof_bits);
    std::mt19937                            rgen(0x5eed);
    std::uniform_int_distribution<unsigned> dist(0, 1);
    for (unsigned i = 0; i != nof_bits; ++i) {
      bits.insert(dist(rgen), i, 1);
    }
    return bits;
  }

  /// Packs a bit buffer the way the scrambling kernels read it, bit \c n at position
  /// <tt>31 - n modulo 32</tt> of word <tt>n / 32</tt>.
  static std::vector<uint32_t> pack_for_device(const bit_buffer& bits)
  {
    std::vector<uint32_t> words((bits.size() + 31) / 32, 0);
    for (unsigned i = 0, end = bits.size(); i != end; ++i) {
      if (bits.extract(i, 1) != 0) {
        words[i / 32] |= (1U << (31U - (i % 32U)));
      }
    }
    return words;
  }

  /// Reads the sequence the scrambler holds back from the device.
  std::vector<uint32_t> read_sequence(unsigned nof_bits)
  {
    unsigned              nof_words = (nof_bits + 31) / 32;
    std::vector<uint32_t> words(nof_words);
    EXPECT_EQ(::cudaMemcpy(words.data(), gpu.sequence(), nof_words * sizeof(uint32_t), cudaMemcpyDeviceToHost),
              cudaSuccess);
    return words;
  }

  cuda_stream                              stream;
  scrambler                                gpu;
  std::unique_ptr<pseudo_random_generator> reference;
};

} // namespace

TEST_F(scrambling_kernel_test, the_sequence_matches_the_software_generator)
{
  for (uint32_t c_init : initialisers) {
    // 8192 bits is a few code blocks at 256QAM, and not a multiple of the 256 bit block the kernel
    // covers per thread block, so the tail is exercised too.
    unsigned nof_bits = 8192 + 37;

    gpu.init(c_init);
    ASSERT_TRUE(gpu.generate(nof_bits, stream).has_value());
    ASSERT_EQ(::cudaDeviceSynchronize(), cudaSuccess);
    std::vector<uint32_t> actual = read_sequence(nof_bits);

    reference->init(c_init);
    dynamic_bit_buffer expected(nof_bits);
    reference->generate(expected);

    for (unsigned i = 0; i != nof_bits; ++i) {
      ASSERT_EQ(device_sequence_bit(actual, i), expected.extract(i, 1))
          << "c_init 0x" << std::hex << c_init << std::dec << ", bit " << i;
    }
  }
}

TEST_F(scrambling_kernel_test, an_offset_matches_advancing_the_software_generator)
{
  // A code block that is not the first in a transport block starts part way into the sequence.
  for (unsigned offset : {1U, 31U, 32U, 33U, 1024U, 65537U}) {
    uint32_t c_init   = 0x5eed;
    unsigned nof_bits = 4096;

    gpu.init(c_init);
    gpu.set_offset(offset);
    ASSERT_TRUE(gpu.generate(nof_bits, stream).has_value());
    ASSERT_EQ(::cudaDeviceSynchronize(), cudaSuccess);
    std::vector<uint32_t> actual = read_sequence(nof_bits);

    reference->init(c_init);
    reference->advance(offset);
    dynamic_bit_buffer expected(nof_bits);
    reference->generate(expected);

    for (unsigned i = 0; i != nof_bits; ++i) {
      ASSERT_EQ(device_sequence_bit(actual, i), expected.extract(i, 1)) << "offset " << offset << ", bit " << i;
    }
  }
}

TEST_F(scrambling_kernel_test, scrambling_matches_the_software_generator)
{
  uint32_t           c_init   = (0x4601U << 15U) + 321U;
  unsigned           nof_bits = 8192;
  dynamic_bit_buffer input    = make_bits(nof_bits);

  std::vector<uint32_t>                  words = pack_for_device(input);
  cuda_expected<device_vector<uint32_t>> d_in  = device_vector<uint32_t>::create(words.size());
  cuda_expected<device_vector<uint32_t>> d_out = device_vector<uint32_t>::create(words.size());
  ASSERT_TRUE(d_in.has_value());
  ASSERT_TRUE(d_out.has_value());
  ASSERT_TRUE(copy_to_device(d_in.value(), span<const uint32_t>(words.data(), words.size())).has_value());

  gpu.init(c_init);
  ASSERT_TRUE(gpu.apply_xor(d_out.value().data(), d_in.value().data(), nof_bits, stream).has_value());
  ASSERT_EQ(::cudaDeviceSynchronize(), cudaSuccess);

  std::vector<uint32_t> actual(words.size());
  ASSERT_TRUE(copy_to_host(span<uint32_t>(actual.data(), actual.size()), d_out.value()).has_value());

  reference->init(c_init);
  dynamic_bit_buffer expected(nof_bits);
  reference->apply_xor(expected, input);

  for (unsigned i = 0; i != nof_bits; ++i) {
    ASSERT_EQ(device_sequence_bit(actual, i), expected.extract(i, 1)) << "bit " << i;
  }
}

TEST_F(scrambling_kernel_test, scrambling_in_place_gives_the_same_bits)
{
  // The in place kernel exists because the out of place one marks both buffers restrict, so the 2
  // must be shown to agree.
  uint32_t           c_init   = 0x5eed;
  unsigned           nof_bits = 4096;
  dynamic_bit_buffer input    = make_bits(nof_bits);

  std::vector<uint32_t>                  words = pack_for_device(input);
  cuda_expected<device_vector<uint32_t>> d_in  = device_vector<uint32_t>::create(words.size());
  cuda_expected<device_vector<uint32_t>> d_out = device_vector<uint32_t>::create(words.size());
  cuda_expected<device_vector<uint32_t>> d_ip  = device_vector<uint32_t>::create(words.size());
  ASSERT_TRUE(d_in.has_value());
  ASSERT_TRUE(d_out.has_value());
  ASSERT_TRUE(d_ip.has_value());
  ASSERT_TRUE(copy_to_device(d_in.value(), span<const uint32_t>(words.data(), words.size())).has_value());
  ASSERT_TRUE(copy_to_device(d_ip.value(), span<const uint32_t>(words.data(), words.size())).has_value());

  gpu.init(c_init);
  ASSERT_TRUE(gpu.apply_xor(d_out.value().data(), d_in.value().data(), nof_bits, stream).has_value());
  ASSERT_TRUE(gpu.apply_xor(d_ip.value().data(), nof_bits, stream).has_value());
  ASSERT_EQ(::cudaDeviceSynchronize(), cudaSuccess);

  std::vector<uint32_t> out_of_place(words.size());
  std::vector<uint32_t> in_place(words.size());
  ASSERT_TRUE(copy_to_host(span<uint32_t>(out_of_place.data(), out_of_place.size()), d_out.value()).has_value());
  ASSERT_TRUE(copy_to_host(span<uint32_t>(in_place.data(), in_place.size()), d_ip.value()).has_value());

  EXPECT_EQ(out_of_place, in_place);
}

TEST_F(scrambling_kernel_test, descrambling_flips_the_sign_where_the_sequence_carries_a_one)
{
  uint32_t c_init   = 0x5eed;
  unsigned nof_bits = 4096;

  std::vector<float> llrs(nof_bits);
  for (unsigned i = 0; i != nof_bits; ++i) {
    llrs[i] = 1.0F + static_cast<float>(i % 17);
  }

  cuda_expected<device_vector<float>> d_llrs = device_vector<float>::create(nof_bits);
  ASSERT_TRUE(d_llrs.has_value());
  ASSERT_TRUE(copy_to_device(d_llrs.value(), span<const float>(llrs.data(), llrs.size())).has_value());

  gpu.init(c_init);
  ASSERT_TRUE(gpu.apply_xor(d_llrs.value().data(), nof_bits, stream).has_value());
  ASSERT_EQ(::cudaDeviceSynchronize(), cudaSuccess);

  std::vector<float> actual(nof_bits);
  ASSERT_TRUE(copy_to_host(span<float>(actual.data(), actual.size()), d_llrs.value()).has_value());

  reference->init(c_init);
  dynamic_bit_buffer sequence(nof_bits);
  reference->generate(sequence);

  for (unsigned i = 0; i != nof_bits; ++i) {
    float expected = (sequence.extract(i, 1) != 0) ? -llrs[i] : llrs[i];
    ASSERT_FLOAT_EQ(actual[i], expected) << "ratio " << i;
  }
}

TEST_F(scrambling_kernel_test, the_half_precision_descrambler_matches_the_single_precision_one)
{
  uint32_t c_init   = 0x5eed;
  unsigned nof_bits = 4096;

  std::vector<float>  single(nof_bits);
  std::vector<__half> half_llrs(nof_bits);
  for (unsigned i = 0; i != nof_bits; ++i) {
    single[i]    = 1.0F + static_cast<float>(i % 17);
    half_llrs[i] = __float2half(single[i]);
  }

  cuda_expected<device_vector<float>>  d_single = device_vector<float>::create(nof_bits);
  cuda_expected<device_vector<__half>> d_half   = device_vector<__half>::create(nof_bits);
  ASSERT_TRUE(d_single.has_value());
  ASSERT_TRUE(d_half.has_value());
  ASSERT_TRUE(copy_to_device(d_single.value(), span<const float>(single.data(), single.size())).has_value());
  ASSERT_TRUE(copy_to_device(d_half.value(), span<const __half>(half_llrs.data(), half_llrs.size())).has_value());

  gpu.init(c_init);
  ASSERT_TRUE(gpu.apply_xor(d_single.value().data(), nof_bits, stream).has_value());
  ASSERT_TRUE(gpu.apply_xor(d_half.value().data(), nof_bits, stream).has_value());
  ASSERT_EQ(::cudaDeviceSynchronize(), cudaSuccess);

  std::vector<float>  single_out(nof_bits);
  std::vector<__half> half_out(nof_bits);
  ASSERT_TRUE(copy_to_host(span<float>(single_out.data(), single_out.size()), d_single.value()).has_value());
  ASSERT_TRUE(copy_to_host(span<__half>(half_out.data(), half_out.size()), d_half.value()).has_value());

  for (unsigned i = 0; i != nof_bits; ++i) {
    ASSERT_FLOAT_EQ(__half2float(half_out[i]), single_out[i]) << "ratio " << i;
  }
}

TEST_F(scrambling_kernel_test, a_second_scrambler_at_the_same_initialiser_gives_the_same_sequence)
{
  // The sequences live in a cache shared by every scrambler in the process. A second scrambler
  // asking for the same initialiser and offset must see the same bits, not a half written buffer.
  uint32_t c_init   = 0x1234abcd;
  unsigned nof_bits = 4096;

  gpu.init(c_init);
  ASSERT_TRUE(gpu.generate(nof_bits, stream).has_value());
  ASSERT_EQ(::cudaDeviceSynchronize(), cudaSuccess);
  std::vector<uint32_t> first = read_sequence(nof_bits);

  cuda_expected<scrambler> other = scrambler::create();
  ASSERT_TRUE(other.has_value());
  other.value().init(c_init);
  ASSERT_TRUE(other.value().generate(nof_bits, stream).has_value());
  ASSERT_EQ(::cudaDeviceSynchronize(), cudaSuccess);

  unsigned              nof_words = (nof_bits + 31) / 32;
  std::vector<uint32_t> second(nof_words);
  ASSERT_EQ(::cudaMemcpy(second.data(), other.value().sequence(), nof_words * sizeof(uint32_t), cudaMemcpyDeviceToHost),
            cudaSuccess);

  EXPECT_EQ(first, second);
}

TEST_F(scrambling_kernel_test, an_empty_request_is_reported)
{
  gpu.init(0x5eed);

  cuda_result status = gpu.generate(0, stream);
  ASSERT_FALSE(status.has_value());
  EXPECT_FALSE(status.error().empty());
}
