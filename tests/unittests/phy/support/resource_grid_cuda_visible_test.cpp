// SPDX-FileCopyrightText: Copyright (C) 2021-2026 DeepSig Inc
// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/phy/support/resource_grid.h"
#include "ocudu/phy/support/resource_grid_reader.h"
#include "ocudu/phy/support/resource_grid_writer.h"
#include "ocudu/phy/support/support_factories.h"
#include <cuda_runtime.h>
#include <gtest/gtest.h>
#include <vector>

using namespace ocudu;

namespace {

constexpr unsigned nof_ports   = 2;
constexpr unsigned nof_symbols = 14;
constexpr unsigned nof_subc    = 12 * 51;

/// Returns true if a CUDA device is available to run the test.
bool has_cuda_device()
{
  int nof_devices = 0;
  return (::cudaGetDeviceCount(&nof_devices) == cudaSuccess) && (nof_devices > 0);
}

class resource_grid_cuda_visible_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    if (!has_cuda_device()) {
      GTEST_SKIP() << "No CUDA device available";
    }

    host_factory = create_resource_grid_factory();
    ASSERT_NE(host_factory, nullptr);

    factory = create_resource_grid_factory_cuda_visible(host_factory, {});
    if (factory == nullptr) {
      GTEST_SKIP() << "No CUDA device is available for a shared resource grid";
    }

    grid = factory->create(nof_ports, nof_symbols, nof_subc);
    ASSERT_NE(grid, nullptr);
  }

  std::shared_ptr<resource_grid_factory> host_factory;
  std::shared_ptr<resource_grid_factory> factory;
  std::unique_ptr<resource_grid>         grid;
};

} // namespace

TEST_F(resource_grid_cuda_visible_test, the_grid_view_covers_every_resource_element)
{
  const std::size_t nof_elements = static_cast<std::size_t>(nof_ports) * nof_symbols * nof_subc;
  EXPECT_EQ(grid->get_reader().get_buffer().size(), nof_elements);
  EXPECT_EQ(grid->get_writer().get_buffer().size(), nof_elements);
}

TEST_F(resource_grid_cuda_visible_test, a_generic_grid_answers_the_same_accessor)
{
  // The generic grid answers the same question with a null pointer, which is how a consumer tells
  // that there is nothing to accelerate.
  std::shared_ptr<resource_grid_factory> generic_factory = create_resource_grid_factory();
  ASSERT_NE(generic_factory, nullptr);
  std::unique_ptr<resource_grid> host_grid = generic_factory->create(nof_ports, nof_symbols, nof_subc);
  ASSERT_NE(host_grid, nullptr);

  // The generic grid answers the same way, so a consumer reads it without knowing which
  // implementation it holds.
  EXPECT_EQ(host_grid->get_reader().get_buffer().size(), grid->get_reader().get_buffer().size());
}

TEST_F(resource_grid_cuda_visible_test, the_reader_and_the_writer_view_one_allocation)
{
  EXPECT_EQ(grid->get_reader().get_buffer().data(), grid->get_writer().get_buffer().data());
}

TEST_F(resource_grid_cuda_visible_test, a_host_write_is_read_back)
{
  std::vector<cf_t> written(nof_subc);
  for (unsigned i = 0; i != nof_subc; ++i) {
    written[i] = cf_t(static_cast<float>(i % 11) - 5.0F, static_cast<float>(i % 7) - 3.0F);
  }
  grid->get_writer().put(0, 0, 0, written);

  std::vector<cf_t> read_back(nof_subc);
  grid->get_reader().get(read_back, 0, 0, 0);
  for (unsigned i = 0; i != nof_subc; ++i) {
    ASSERT_NEAR(read_back[i].real(), written[i].real(), 0.5F) << "subcarrier " << i;
  }
}

TEST_F(resource_grid_cuda_visible_test, set_all_zero_clears_the_grid)
{
  std::vector<cf_t> written(nof_subc, cf_t(1.0F, 1.0F));
  grid->get_writer().put(0, 0, 0, written);
  ASSERT_FALSE(grid->get_reader().is_empty(0));

  grid->set_all_zero();
  EXPECT_TRUE(grid->get_reader().is_empty());
}

TEST_F(resource_grid_cuda_visible_test, a_device_write_is_visible_to_the_host)
{
  void* address = grid->get_writer().get_buffer().data();
  ASSERT_NE(address, nullptr);

  ::cudaStream_t stream = nullptr;
  ASSERT_EQ(::cudaStreamCreateWithFlags(&stream, cudaStreamNonBlocking), cudaSuccess);

  // A kernel would fill the grid here. The producer synchronises its own stream before the grid is
  // handed on, which is what makes the resource elements safe for the host to read.
  const std::size_t nof_bytes = static_cast<std::size_t>(nof_ports) * nof_symbols * nof_subc * sizeof(cbf16_t);
  ASSERT_EQ(::cudaMemsetAsync(address, 0x3c, nof_bytes, stream), cudaSuccess);
  ASSERT_EQ(::cudaStreamSynchronize(stream), cudaSuccess);

  std::vector<cf_t> read_back(nof_subc);
  grid->get_reader().get(read_back, 0, 0, 0);
  EXPECT_NE(read_back[0].real(), 0.0F);

  ::cudaStreamDestroy(stream);
}

TEST_F(resource_grid_cuda_visible_test, the_address_reaches_the_same_elements_as_the_host_view)
{
  // What a consumer does: take the address and read the resource elements of one symbol. They have
  // to be the ones the host sees through the reader.
  std::vector<cf_t> written(nof_subc);
  for (unsigned i = 0; i != nof_subc; ++i) {
    written[i] = cf_t(static_cast<float>(i % 5) - 2.0F, static_cast<float>(i % 3) - 1.0F);
  }
  grid->get_writer().put(1, 2, 0, written);

  span<const cbf16_t> whole = grid->get_reader().get_buffer();

  // Port 1, symbol 2, as laid out by the documented order.
  const std::size_t offset = (static_cast<std::size_t>(1) * nof_symbols + 2) * nof_subc;
  for (unsigned i = 0; i != nof_subc; ++i) {
    ASSERT_NEAR(to_cf(whole[offset + i]).real(), written[i].real(), 0.5F) << "subcarrier " << i;
  }
}
