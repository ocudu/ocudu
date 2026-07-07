// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ocuduvec/fill.h"
#include "ocudu/ocuduvec/zero.h"
#include "ocudu/phy/support/resource_grid.h"
#include "ocudu/phy/support/resource_grid_reader.h"
#include "ocudu/phy/support/resource_grid_writer.h"
#include "ocudu/phy/support/support_factories.h"
#include "ocudu/support/ocudu_test.h"
#include <gtest/gtest.h>
#include <random>

using namespace ocudu;

using resource_grid_test_params = std::tuple<unsigned, unsigned>;

// Gets the tolerance from an expected value.
static float get_tolerance(cf_t expected_value)
{
  // The tolerance is calculated from the complex number based in brain float (BF16) precision.
  return std::max(std::abs(expected_value) / 256.0F, 1e-5F);
}

// Converts a subcarrier range \c [start_subc, stop_subc) into the CRB range that fully contains it.
static crb_interval subc_range_to_crb_range(unsigned start_subc, unsigned stop_subc)
{
  return {start_subc / NOF_SUBCARRIERS_PER_RB, divide_ceil(stop_subc, NOF_SUBCARRIERS_PER_RB)};
}

namespace {

class ResourceGridFixture : public ::testing::TestWithParam<resource_grid_test_params>
{
protected:
  static std::shared_ptr<resource_grid_factory> rg_factory;
  static std::mt19937                           rgen;
  static constexpr unsigned                     nof_elements = 32;
  static constexpr unsigned                     nof_symbols  = 14;

  void SetUp() override
  {
    auto params = GetParam();
    nof_ports   = std::get<0>(params);
    nof_subc    = std::get<1>(params);

    rg_factory = create_resource_grid_factory();
    ASSERT_NE(rg_factory, nullptr) << "Cannot create resource grid factory";

    grid = rg_factory->create(nof_ports, nof_symbols, nof_subc);

    grid->set_all_zero();
  }

  unsigned                       nof_ports = 0;
  unsigned                       nof_subc  = 0;
  std::unique_ptr<resource_grid> grid      = nullptr;
};

std::shared_ptr<resource_grid_factory> ResourceGridFixture::rg_factory = nullptr;
std::mt19937                           ResourceGridFixture::rgen{};

// Test per-(port, symbol) emptiness tracking, including the initial state of a freshly created grid.
TEST_P(ResourceGridFixture, PerPortSymbolEmptiness)
{
  // A newly created grid is initially empty.
  ASSERT_TRUE(grid->get_reader().is_empty());
  for (unsigned port = 0; port != nof_ports; ++port) {
    for (unsigned symbol = 0; symbol != nof_symbols; ++symbol) {
      ASSERT_TRUE(grid->get_reader().get_allocation_range(port, symbol).empty())
          << fmt::format("Expected (port={}, symbol={}) to be empty after reset", port, symbol);
    }
    ASSERT_TRUE(grid->get_reader().is_empty(port)) << fmt::format("Port {} should be empty", port);
  }

  // Write to a specific (port, symbol).
  static constexpr unsigned nof_alloc_subc = 4;
  static constexpr unsigned test_port      = 0;
  static constexpr unsigned test_symbol    = 3;
  std::vector               symbols(nof_alloc_subc, cf_t{1.0f, 2.0f});

  bounded_bitset<MAX_NOF_SUBCARRIERS> mask(nof_subc);
  mask.fill(0, nof_alloc_subc);
  grid->get_writer().put(test_port, test_symbol, 0, mask, symbols);

  // The grid as a whole must no longer be empty.
  ASSERT_FALSE(grid->get_reader().is_empty());

  // The written (port, symbol) must now be allocated.
  for (unsigned port = 0; port != nof_ports; ++port) {
    for (unsigned symbol = 0; symbol != nof_symbols; ++symbol) {
      bool expected_empty = (port != test_port) || (symbol != test_symbol);
      ASSERT_EQ(expected_empty, grid->get_reader().get_allocation_range(port, symbol).empty())
          << fmt::format("Unexpected emptiness for (port={}, symbol={})", port, symbol);
    }
  }

  // Set all grid to zero.
  grid->set_all_zero();

  // set_all_zero must clear everything, including the whole-grid emptiness flag.
  ASSERT_TRUE(grid->get_reader().is_empty());
  for (unsigned port = 0; port != nof_ports; ++port) {
    for (unsigned symbol = 0; symbol != nof_symbols; ++symbol) {
      ASSERT_TRUE(grid->get_reader().get_allocation_range(port, symbol).empty())
          << fmt::format("After reset: (port={}, symbol={}) should be empty", port, symbol);
    }
  }
}

// Test masked (bitset) access with varying numbers of elements.
TEST_P(ResourceGridFixture, MaskBitset)
{
  std::uniform_int_distribution<unsigned> port_dist(0, nof_ports - 1);
  std::uniform_int_distribution<unsigned> symbol_dist(0, nof_symbols - 1);
  std::uniform_int_distribution<unsigned> subc_dist(0, nof_subc - 1);
  std::uniform_real_distribution<float>   value_dist(-1.0, +1.0);

  // Iterate over various numbers of elements.
  unsigned                            symbol_idx = symbol_dist(rgen);
  std::vector<cf_t>                   symbols_gold(nof_elements);
  bounded_bitset<MAX_NOF_SUBCARRIERS> mask(nof_subc);

  // Fill mask and generate symbols.
  unsigned port_gold = port_dist(rgen);
  for (unsigned i = 0; i != nof_elements; ++i) {
    unsigned subc = 0;

    // Select a subcarrier that has not been set yet.
    do {
      subc = subc_dist(rgen);
    } while (mask.test(subc));

    // Create random allocation
    mask.set(subc);
    symbols_gold[i] = {value_dist(rgen), value_dist(rgen)};
  }

  // Put elements.
  span<const cf_t> symbol_buffer_put = grid->get_writer().put(port_gold, symbol_idx, 0, mask, symbols_gold);

  // Make sure all symbols are used.
  ASSERT_TRUE(symbol_buffer_put.empty());

  // Validate the allocation range matches the span covered by the mask.
  ASSERT_EQ(grid->get_reader().get_allocation_range(port_gold, symbol_idx),
            subc_range_to_crb_range(mask.find_lowest(), mask.find_highest(+1)));

  // Assert grid entries.
  unsigned count = 0;
  for (unsigned port = 0; port != nof_ports; ++port) {
    ASSERT_EQ(port != port_gold, grid->get_reader().is_empty(port));

    for (unsigned symbol = 0; symbol != nof_symbols; ++symbol) {
      // Get resource grid data for the given symbol.
      std::vector<cf_t> rg_data(nof_subc);
      grid->get_reader().get(rg_data, port, symbol, 0);

      for (unsigned subc = 0; subc != nof_subc; ++subc) {
        cf_t gold  = {0.0, 0.0};
        cf_t value = rg_data[subc];

        if (port == port_gold && symbol == symbol_idx && mask.test(subc)) {
          gold = symbols_gold[count];
          count++;
        }

        float error = std::abs(gold - value);
        ASSERT_LT(error, get_tolerance(gold));
      }
    }
  }

  // Get elements using the same mask.
  std::vector<cf_t> symbols(nof_elements);
  span<cf_t>        symbol_buffer_get = grid->get_reader().get(symbols, port_gold, symbol_idx, 0, mask);

  // Make sure all symbols are used.
  ASSERT_TRUE(symbol_buffer_get.empty());

  // Assert that symbols are equal.
  for (unsigned i = 0; i != nof_elements; ++i) {
    cf_t gold  = symbols_gold[i];
    cf_t value = symbols[i];

    float error = std::abs(gold - value);
    ASSERT_LT(error, get_tolerance(gold));
  }

  // Put/get round trip using the same (non-contiguous) mask with the cbf16_t overloads, exercising their
  // per-subcarrier iteration branches.
  std::vector<cbf16_t> symbols_cbf16_gold(nof_elements);
  for (cbf16_t& symbol : symbols_cbf16_gold) {
    symbol = to_cbf16({value_dist(rgen), value_dist(rgen)});
  }
  ASSERT_TRUE(grid->get_writer().put(port_gold, symbol_idx, 0, mask, symbols_cbf16_gold).empty());

  // Validate the allocation range matches the span covered by the mask (same mask as the cf_t put above).
  ASSERT_EQ(grid->get_reader().get_allocation_range(port_gold, symbol_idx),
            subc_range_to_crb_range(mask.find_lowest(), mask.find_highest() + 1));

  std::vector<cbf16_t> symbols_cbf16_read(nof_elements);
  ASSERT_TRUE(grid->get_reader().get(symbols_cbf16_read, port_gold, symbol_idx, 0, mask).empty());
  for (unsigned i = 0; i != nof_elements; ++i) {
    ASSERT_EQ(symbols_cbf16_gold[i], symbols_cbf16_read[i]);
  }
}

// Test consecutive and strided access.
TEST_P(ResourceGridFixture, Consecutive)
{
  static constexpr unsigned stride = 2;

  std::uniform_int_distribution<unsigned> port_dist(0, nof_ports - 1);
  std::uniform_int_distribution<unsigned> symbol_dist(0, nof_symbols - 1);
  std::uniform_int_distribution<unsigned> subc_dist(0, nof_subc - 1 - nof_elements);
  std::uniform_int_distribution<unsigned> strided_subc_dist(0, nof_subc - 1 - (nof_elements - 1) * stride);
  std::uniform_real_distribution<float>   value_dist(-1.0, +1.0);

  // Select port
  unsigned port_gold = port_dist(rgen);

  // Put elements in grid
  unsigned          symbol_idx = symbol_dist(rgen);
  std::vector<cf_t> symbols_gold(nof_elements);

  // Select initial subcarrier
  unsigned k_init = subc_dist(rgen);

  // Create random data
  for (unsigned i = 0; i != nof_elements; ++i) {
    symbols_gold[i] = {value_dist(rgen), value_dist(rgen)};
  }

  // Put element
  grid->get_writer().put(port_gold, symbol_idx, k_init, symbols_gold);

  // Validate the allocation range matches the span covered by the consecutive write.
  ASSERT_EQ(grid->get_reader().get_allocation_range(port_gold, symbol_idx),
            subc_range_to_crb_range(k_init, k_init + nof_elements));

  // Assert grid
  unsigned count = 0;
  for (unsigned port = 0; port != nof_ports; ++port) {
    ASSERT_EQ(port != port_gold, grid->get_reader().is_empty(port));

    for (unsigned symbol = 0; symbol != nof_symbols; ++symbol) {
      // Get resource grid data for the given symbol
      std::vector<cf_t> rg_data(nof_subc);
      grid->get_reader().get(rg_data, port, symbol, 0);

      for (unsigned subc = 0; subc != nof_subc; ++subc) {
        cf_t gold  = {0.0, 0.0};
        cf_t value = rg_data[subc];

        if (port == port_gold && symbol == symbol_idx && (subc >= k_init && subc < k_init + nof_elements)) {
          gold = symbols_gold[count];
          count++;
        }

        float error = std::abs(gold - value);
        ASSERT_LT(error, get_tolerance(gold));
      }
    }
  }

  // Get elements
  std::vector<cf_t> symbols(nof_elements);
  grid->get_reader().get(symbols, port_gold, symbol_idx, k_init);

  // Assert symbols
  for (unsigned i = 0; i != nof_elements; ++i) {
    cf_t gold  = symbols_gold[i];
    cf_t value = symbols[i];

    float error = std::abs(gold - value);
    ASSERT_LT(error, get_tolerance(gold));
  }

  // Test view contents
  span<const cbf16_t> view = grid->get_reader().get_view(port_gold, symbol_idx).subspan(k_init, nof_subc - k_init);
  for (unsigned i = 0; i != nof_elements; ++i) {
    cf_t gold  = symbols_gold[i];
    cf_t value = to_cf(view[i]);

    float error = std::abs(gold - value);
    ASSERT_LT(error, get_tolerance(gold));
  }

  // Write with a stride using the cbf16_t writer overload, at a different OFDM symbol so it cannot disturb the
  // consecutive allocation asserted above.
  unsigned             symbol_idx_stride = (symbol_idx + 1) % nof_symbols;
  unsigned             k_init_stride     = strided_subc_dist(rgen);
  std::vector<cbf16_t> symbols_stride_gold(nof_elements);
  for (cbf16_t& symbol : symbols_stride_gold) {
    symbol = to_cbf16({value_dist(rgen), value_dist(rgen)});
  }
  grid->get_writer().put(port_gold, symbol_idx_stride, k_init_stride, stride, symbols_stride_gold);

  // Validate the allocated range matches with the put.
  ASSERT_EQ(grid->get_reader().get_allocation_range(port_gold, symbol_idx_stride),
            subc_range_to_crb_range(k_init_stride, k_init_stride + (symbols_stride_gold.size() - 1) * stride + 1));

  // Read back with the same stride using the cf_t reader overload.
  std::vector<cf_t> symbols_stride_read(nof_elements);
  grid->get_reader().get(symbols_stride_read, port_gold, symbol_idx_stride, k_init_stride, stride);
  for (unsigned i = 0; i != nof_elements; ++i) {
    cf_t  gold  = to_cf(symbols_stride_gold[i]);
    float error = std::abs(gold - symbols_stride_read[i]);
    ASSERT_LT(error, get_tolerance(gold));
  }

  // Write consecutive resource elements directly through the writer's mutable resource grid view, at a third OFDM
  // symbol.
  unsigned             symbol_idx_view = (symbol_idx + 2) % nof_symbols;
  unsigned             k_init_view     = subc_dist(rgen);
  std::vector<cbf16_t> symbols_view_gold(nof_elements);
  span<cbf16_t>        mutable_view = grid->get_writer().get_view(port_gold, symbol_idx_view);
  for (unsigned i = 0; i != nof_elements; ++i) {
    symbols_view_gold[i]          = to_cbf16({value_dist(rgen), value_dist(rgen)});
    mutable_view[k_init_view + i] = symbols_view_gold[i];
  }

  // Read back using the reader's consecutive cbf16_t accessor.
  std::vector<cbf16_t> symbols_view_read(nof_elements);
  grid->get_reader().get(symbols_view_read, port_gold, symbol_idx_view, k_init_view);
  for (unsigned i = 0; i != nof_elements; ++i) {
    ASSERT_EQ(symbols_view_gold[i], symbols_view_read[i]);
  }
}

// Test masked (bitset) access using a contiguous mask, exercising the fast-copy branches of both the cf_t and
// cbf16_t reader/writer overloads.
TEST_P(ResourceGridFixture, MaskBitsetContiguous)
{
  std::uniform_int_distribution<unsigned> port_dist(0, nof_ports - 1);
  std::uniform_int_distribution<unsigned> symbol_dist(0, nof_symbols - 1);
  std::uniform_real_distribution<float>   value_dist(-1.0, +1.0);

  unsigned port_gold  = port_dist(rgen);
  unsigned symbol_idx = symbol_dist(rgen);

  bounded_bitset<MAX_NOF_SUBCARRIERS> mask(nof_subc);
  mask.fill(0, nof_elements);
  ASSERT_TRUE(mask.is_contiguous());

  // cf_t put/get round trip through the contiguous fast-copy branch.
  std::vector<cf_t> symbols_cf(nof_elements);
  for (cf_t& symbol : symbols_cf) {
    symbol = {value_dist(rgen), value_dist(rgen)};
  }
  ASSERT_TRUE(grid->get_writer().put(port_gold, symbol_idx, 0, mask, symbols_cf).empty());

  // Validate the allocation range matches the contiguous fill span [0, nof_elements).
  ASSERT_EQ(grid->get_reader().get_allocation_range(port_gold, symbol_idx), subc_range_to_crb_range(0, nof_elements));

  std::vector<cf_t> symbols_cf_read(nof_elements);
  ASSERT_TRUE(grid->get_reader().get(symbols_cf_read, port_gold, symbol_idx, 0, mask).empty());
  for (unsigned i = 0; i != nof_elements; ++i) {
    float error = std::abs(symbols_cf[i] - symbols_cf_read[i]);
    ASSERT_LT(error, get_tolerance(symbols_cf[i]));
  }

  // cbf16_t put/get round trip through the contiguous fast-copy branch.
  std::vector<cbf16_t> symbols_cbf16(nof_elements);
  for (cbf16_t& symbol : symbols_cbf16) {
    symbol = to_cbf16({value_dist(rgen), value_dist(rgen)});
  }
  ASSERT_TRUE(grid->get_writer().put(port_gold, symbol_idx, 0, mask, symbols_cbf16).empty());

  // Validate the allocation range matches the contiguous fill span [0, nof_elements).
  ASSERT_EQ(grid->get_reader().get_allocation_range(port_gold, symbol_idx), subc_range_to_crb_range(0, nof_elements));

  std::vector<cbf16_t> symbols_cbf16_read(nof_elements);
  ASSERT_TRUE(grid->get_reader().get(symbols_cbf16_read, port_gold, symbol_idx, 0, mask).empty());
  for (unsigned i = 0; i != nof_elements; ++i) {
    ASSERT_EQ(symbols_cbf16[i], symbols_cbf16_read[i]);
  }
}

// Parameterised tests for all parameter combinations.
INSTANTIATE_TEST_SUITE_P(AllConfigurations,
                         ResourceGridFixture,
                         ::testing::Combine(::testing::Values(1, 2, 4, 8),     // Number of ports.
                                            ::testing::Values(6 * 12, 15 * 12) // Number of subcarriers.
                                            ));

} // namespace
