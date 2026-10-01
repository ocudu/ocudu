// SPDX-FileCopyrightText: Copyright (C) 2021-2026 DeepSig Inc
// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/phy/support/prach_buffer.h"
#include "ocudu/phy/support/support_factories.h"
#include "ocudu/ran/prach/prach_constants.h"
#include <cuda_runtime.h>
#include <gtest/gtest.h>
#include <vector>

using namespace ocudu;

namespace {

constexpr unsigned nof_antennas     = 2;
constexpr unsigned nof_td_occasions = 1;
constexpr unsigned nof_fd_occasions = 1;

/// Returns true if a CUDA device is available to run the test.
bool has_cuda_device()
{
  int nof_devices = 0;
  return (::cudaGetDeviceCount(&nof_devices) == cudaSuccess) && (nof_devices > 0);
}

class prach_buffer_cuda_visible_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    if (!has_cuda_device()) {
      GTEST_SKIP() << "No CUDA device available";
    }

    buffer = create_prach_buffer_cuda_visible(nof_antennas, nof_td_occasions, nof_fd_occasions, true);
    if (buffer == nullptr) {
      GTEST_SKIP() << "No CUDA device is available for a shared PRACH buffer";
    }
  }

  std::unique_ptr<prach_buffer> buffer;
};

} // namespace

TEST_F(prach_buffer_cuda_visible_test, the_buffer_view_covers_every_sample)
{
  const unsigned nof_samples = nof_antennas * nof_td_occasions * nof_fd_occasions * buffer->get_max_nof_symbols() *
                               buffer->get_sequence_length();
  EXPECT_EQ(buffer->get_buffer().size(), nof_samples);
}

TEST_F(prach_buffer_cuda_visible_test, a_generic_buffer_answers_the_same_accessors)
{
  // The generic buffer answers the same question with a null pointer, which is how a consumer tells
  // that there is nothing to accelerate.
  std::unique_ptr<prach_buffer> host_buffer = create_prach_buffer_long(nof_antennas, nof_td_occasions);
  ASSERT_NE(host_buffer, nullptr);

  // The generic buffer answers both the same way, so a consumer reads it without knowing which
  // implementation it holds.
  EXPECT_EQ(host_buffer->get_buffer().size(), buffer->get_buffer().size());
  EXPECT_EQ(host_buffer->get_symbol_offset(0, 0, 0, 1), host_buffer->get_sequence_length());
}

TEST_F(prach_buffer_cuda_visible_test, dimensions_match_a_long_preamble)
{
  EXPECT_EQ(buffer->get_max_nof_ports(), nof_antennas);
  EXPECT_EQ(buffer->get_max_nof_td_occasions(), nof_td_occasions);
  EXPECT_EQ(buffer->get_max_nof_fd_occasions(), nof_fd_occasions);
  EXPECT_EQ(buffer->get_sequence_length(), prach_constants::LONG_SEQUENCE_LENGTH);
  EXPECT_EQ(buffer->get_max_nof_symbols(), prach_constants::LONG_SEQUENCE_MAX_NOF_SYMBOLS);
}

TEST_F(prach_buffer_cuda_visible_test, symbol_offsets_address_the_same_samples_as_the_host_view)
{
  // The first symbol starts at the beginning, and consecutive symbols are one sequence apart.
  EXPECT_EQ(buffer->get_symbol_offset(0, 0, 0, 0), 0U);
  EXPECT_EQ(buffer->get_symbol_offset(0, 0, 0, 1), buffer->get_sequence_length());

  // A port is as far from the next as all of its symbols and occasions together.
  const unsigned port_stride =
      buffer->get_sequence_length() * buffer->get_max_nof_symbols() * nof_fd_occasions * nof_td_occasions;
  EXPECT_EQ(buffer->get_symbol_offset(1, 0, 0, 0), port_stride);
}

TEST_F(prach_buffer_cuda_visible_test, a_host_write_is_read_back)
{
  span<cbf16_t> symbol = buffer->get_symbol(0, 0, 0, 0);
  ASSERT_EQ(symbol.size(), prach_constants::LONG_SEQUENCE_LENGTH);
  symbol[0]                 = to_cbf16(cf_t(0.5F, -0.5F));
  symbol[symbol.size() - 1] = to_cbf16(cf_t(0.25F, 0.75F));

  span<const cbf16_t> read_back = static_cast<const prach_buffer&>(*buffer).get_symbol(0, 0, 0, 0);
  EXPECT_EQ(to_cf(read_back[0]).real(), 0.5F);
  EXPECT_EQ(to_cf(read_back[read_back.size() - 1]).imag(), 0.75F);
}

TEST_F(prach_buffer_cuda_visible_test, a_device_write_is_visible_to_the_host)
{
  void* address = const_cast<cbf16_t*>(buffer->get_buffer().data());
  ASSERT_NE(address, nullptr);

  ::cudaStream_t stream = nullptr;
  ASSERT_EQ(::cudaStreamCreateWithFlags(&stream, cudaStreamNonBlocking), cudaSuccess);

  // A decompression kernel would fill the buffer here. A pattern written with the runtime stands in
  // for it. The producer synchronises its own stream before the buffer is handed on, which is what
  // makes the samples safe for the host to read.
  const std::size_t nof_bytes = static_cast<std::size_t>(nof_antennas) * nof_td_occasions * nof_fd_occasions *
                                buffer->get_max_nof_symbols() * buffer->get_sequence_length() * sizeof(cbf16_t);
  ASSERT_EQ(::cudaMemsetAsync(address, 0x3c, nof_bytes, stream), cudaSuccess);
  ASSERT_EQ(::cudaStreamSynchronize(stream), cudaSuccess);

  span<const cbf16_t> read_back = static_cast<const prach_buffer&>(*buffer).get_symbol(0, 0, 0, 0);
  EXPECT_NE(to_cf(read_back[0]).real(), 0.0F);

  ::cudaStreamDestroy(stream);
}

TEST_F(prach_buffer_cuda_visible_test, the_address_reaches_the_same_samples_as_the_host_view)
{
  // What a consumer does: take the address, take the offset of the symbol it wants, and read there.
  // The samples it finds have to be the ones the host sees through get_symbol().
  span<cbf16_t> symbol = buffer->get_symbol(0, 0, 1, 0);
  symbol[0]            = to_cbf16(cf_t(-1.5F, 2.5F));

  span<const cbf16_t> whole  = buffer->get_buffer();
  const unsigned      offset = buffer->get_symbol_offset(0, 0, 1, 0);
  ASSERT_LT(offset, whole.size());

  EXPECT_EQ(to_cf(whole[offset]).real(), -1.5F);
  EXPECT_EQ(to_cf(whole[offset]).imag(), 2.5F);
}

TEST_F(prach_buffer_cuda_visible_test, a_short_preamble_buffer_is_dimensioned_differently)
{
  std::unique_ptr<prach_buffer> short_buffer =
      create_prach_buffer_cuda_visible(nof_antennas, nof_td_occasions, nof_fd_occasions, false);
  ASSERT_NE(short_buffer, nullptr);

  EXPECT_EQ(short_buffer->get_sequence_length(), prach_constants::SHORT_SEQUENCE_LENGTH);
  EXPECT_EQ(short_buffer->get_max_nof_symbols(), prach_constants::SHORT_SEQUENCE_MAX_NOF_SYMBOLS);
}
