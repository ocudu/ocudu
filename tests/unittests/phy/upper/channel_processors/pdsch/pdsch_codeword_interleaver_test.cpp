// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdsch_codeword_interleaver.h"
#include "support/compare_sequences.h"
#include <gtest/gtest.h>
#include <random>

using namespace ocudu;

namespace {

std::mt19937 rgen(4321);

/// Symbol buffer that serves the symbols in chunks of random size, as a block processor serves codeblocks.
class chunked_symbol_buffer : public resource_grid_mapper::symbol_buffer
{
public:
  chunked_symbol_buffer(span<const ci8_t> symbols_, unsigned granularity) : symbols(symbols_)
  {
    std::uniform_int_distribution<unsigned> dist(1, 64);
    for (unsigned remaining = symbols.size(); remaining != 0;) {
      unsigned chunk = std::min(remaining, dist(rgen) * granularity);
      chunks.push_back(chunk);
      remaining -= chunk;
    }
  }

  unsigned get_max_block_size() const override { return chunks.empty() ? 0 : chunks.front(); }

  bool empty() const override { return chunks.empty(); }

  span<const ci8_t> pop_symbols(unsigned block_size) override
  {
    EXPECT_LE(block_size, chunks.front());
    span<const ci8_t> result = symbols.first(block_size);
    symbols                  = symbols.last(symbols.size() - block_size);
    chunks.front() -= block_size;
    if (chunks.front() == 0) {
      chunks.erase(chunks.begin());
    }
    return result;
  }

private:
  span<const ci8_t>     symbols;
  std::vector<unsigned> chunks;
};

std::vector<ci8_t> make_symbols(unsigned nof_symbols, int8_t tag)
{
  std::vector<ci8_t> symbols(nof_symbols);
  for (unsigned i = 0; i != nof_symbols; ++i) {
    symbols[i] = ci8_t(tag, static_cast<int8_t>(i % 127));
  }
  return symbols;
}

using interleaver_params = std::tuple<unsigned, unsigned>;

class PdschCodewordInterleaverFixture : public ::testing::TestWithParam<interleaver_params>
{};

} // namespace

TEST_P(PdschCodewordInterleaverFixture, Interleave)
{
  auto [nof_layers_cw0, nof_layers_cw1] = GetParam();
  unsigned nof_layers                   = nof_layers_cw0 + nof_layers_cw1;
  unsigned nof_re                       = 5000;

  std::vector<ci8_t> symbols_cw0 = make_symbols(nof_re * nof_layers_cw0, 0);
  std::vector<ci8_t> symbols_cw1 = make_symbols(nof_re * nof_layers_cw1, 1);

  chunked_symbol_buffer      buffer_cw0(symbols_cw0, nof_layers_cw0);
  chunked_symbol_buffer      buffer_cw1(symbols_cw1, nof_layers_cw1);
  pdsch_codeword_interleaver interleaver(buffer_cw0, nof_layers_cw0, buffer_cw1, nof_layers_cw1);

  // Pop random blocks, as the resource grid mapper does with reserved resource elements.
  std::vector<ci8_t> output;
  while (!interleaver.empty()) {
    unsigned max_block_size = interleaver.get_max_block_size();
    ASSERT_GT(max_block_size, 0);
    ASSERT_EQ(max_block_size % nof_layers, 0);
    ASSERT_LE(max_block_size, pdsch_codeword_interleaver::max_block_size);

    std::uniform_int_distribution<unsigned> dist(1, max_block_size / nof_layers);
    span<const ci8_t>                       block = interleaver.pop_symbols(dist(rgen) * nof_layers);
    output.insert(output.end(), block.begin(), block.end());
  }
  ASSERT_TRUE(buffer_cw0.empty());
  ASSERT_TRUE(buffer_cw1.empty());
  ASSERT_EQ(output.size(), nof_re * nof_layers);

  // Each resource element carries the first codeword layers followed by the second codeword layers.
  std::vector<ci8_t> expected;
  expected.reserve(nof_re * nof_layers);
  for (unsigned i_re = 0; i_re != nof_re; ++i_re) {
    auto re_cw0 = symbols_cw0.begin() + i_re * nof_layers_cw0;
    auto re_cw1 = symbols_cw1.begin() + i_re * nof_layers_cw1;
    expected.insert(expected.end(), re_cw0, re_cw0 + nof_layers_cw0);
    expected.insert(expected.end(), re_cw1, re_cw1 + nof_layers_cw1);
  }

  error_type<std::string> result = compare_sequences(span<const ci8_t>(output), span<const ci8_t>(expected));
  ASSERT_TRUE(result.has_value()) << result.error();
}

INSTANTIATE_TEST_SUITE_P(PdschCodewordInterleaver,
                         PdschCodewordInterleaverFixture,
                         ::testing::Combine(::testing::Values(1, 2, 3, 4), ::testing::Values(1, 2, 3, 4)));
