// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/cuda/adt/cuda_copy.h"
#include "ocudu/cuda/adt/cuda_event.h"
#include "ocudu/cuda/adt/cuda_stream.h"
#include "ocudu/cuda/adt/device_vector.h"
#include "ocudu/cuda/adt/managed_vector.h"
#include "ocudu/cuda/support/cuda_device.h"
#include <cuda_runtime.h>
#include <gtest/gtest.h>
#include <numeric>
#include <vector>

using namespace ocudu;
using namespace ocudu::cuda;

namespace {

/// Returns true if a CUDA device is available to run the test.
bool has_cuda_device()
{
  int nof_devices = 0;
  return (cudaGetDeviceCount(&nof_devices) == cudaSuccess) && (nof_devices > 0);
}

class cuda_adt_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    if (!has_cuda_device()) {
      GTEST_SKIP() << "No CUDA device available.";
    }
  }
};

} // namespace

TEST_F(cuda_adt_test, stream_is_created_and_synchronizes)
{
  cuda_expected<cuda_stream> stream = cuda_stream::create();
  ASSERT_TRUE(stream.has_value()) << stream.error();
  ASSERT_TRUE(stream.value().is_valid());
  ASSERT_NE(stream.value().native(), nullptr);

  cuda_result result = stream.value().synchronize();
  ASSERT_TRUE(result.has_value()) << result.error();
  ASSERT_TRUE(stream.value().is_completed());
}

TEST_F(cuda_adt_test, high_priority_stream_is_created)
{
  // The priority is an optimization: a device that does not support it must still produce a stream.
  cuda_expected<cuda_stream> stream = cuda_stream::create(cuda_stream_priority::high);
  ASSERT_TRUE(stream.has_value()) << stream.error();
  ASSERT_TRUE(stream.value().is_valid());
}

TEST_F(cuda_adt_test, moved_from_stream_releases_ownership)
{
  cuda_expected<cuda_stream> stream = cuda_stream::create();
  ASSERT_TRUE(stream.has_value()) << stream.error();

  void*       native = stream.value().native();
  cuda_stream moved  = std::move(stream.value());

  ASSERT_TRUE(moved.is_valid());
  ASSERT_EQ(moved.native(), native);
  ASSERT_FALSE(stream.value().is_valid());
  // The moved-from object must not destroy the stream now owned by the destination.
  ASSERT_EQ(stream.value().native(), nullptr);
}

TEST_F(cuda_adt_test, event_records_and_synchronizes)
{
  cuda_expected<cuda_stream> stream = cuda_stream::create();
  ASSERT_TRUE(stream.has_value()) << stream.error();
  cuda_expected<cuda_event> event = cuda_event::create();
  ASSERT_TRUE(event.has_value()) << event.error();

  cuda_result recorded = event.value().record(stream.value());
  ASSERT_TRUE(recorded.has_value()) << recorded.error();

  cuda_result synchronized = event.value().synchronize();
  ASSERT_TRUE(synchronized.has_value()) << synchronized.error();
  ASSERT_TRUE(event.value().is_completed());
}

TEST_F(cuda_adt_test, event_on_an_invalid_stream_reports_an_error)
{
  cuda_expected<cuda_event> event = cuda_event::create();
  ASSERT_TRUE(event.has_value()) << event.error();

  cuda_stream invalid;
  cuda_result recorded = event.value().record(invalid);
  ASSERT_FALSE(recorded.has_value());
}

TEST_F(cuda_adt_test, device_vector_allocates_the_requested_size)
{
  cuda_expected<device_vector<float>> block = device_vector<float>::create(1024);
  ASSERT_TRUE(block.has_value()) << block.error();
  ASSERT_EQ(block.value().size(), 1024);
  ASSERT_EQ(block.value().size_bytes(), 1024 * sizeof(float));
  ASSERT_NE(block.value().data(), nullptr);
  ASSERT_FALSE(block.value().empty());
}

TEST_F(cuda_adt_test, empty_device_vector_is_valid)
{
  cuda_expected<device_vector<float>> block = device_vector<float>::create(0);
  ASSERT_TRUE(block.has_value()) << block.error();
  ASSERT_TRUE(block.value().empty());
  ASSERT_EQ(block.value().data(), nullptr);
}

TEST_F(cuda_adt_test, moved_from_device_vector_releases_ownership)
{
  cuda_expected<device_vector<int>> block = device_vector<int>::create(256);
  ASSERT_TRUE(block.has_value()) << block.error();

  int*               data  = block.value().data();
  device_vector<int> moved = std::move(block.value());

  ASSERT_EQ(moved.data(), data);
  ASSERT_EQ(moved.size(), 256);
  // Releasing the same allocation twice would be a double free.
  ASSERT_EQ(block.value().data(), nullptr);
  ASSERT_EQ(block.value().size(), 0);
}

TEST_F(cuda_adt_test, data_survives_a_round_trip_through_device_memory)
{
  std::vector<int> source(512);
  std::iota(source.begin(), source.end(), 0);

  cuda_expected<device_vector<int>> block = device_vector<int>::create(source.size());
  ASSERT_TRUE(block.has_value()) << block.error();

  cuda_result written = copy_to_device(block.value(), span<const int>(source));
  ASSERT_TRUE(written.has_value()) << written.error();

  std::vector<int> destination(source.size(), -1);
  cuda_result      read = copy_to_host(span<int>(destination), block.value());
  ASSERT_TRUE(read.has_value()) << read.error();

  ASSERT_EQ(source, destination);
}

TEST_F(cuda_adt_test, asynchronous_round_trip_completes_after_synchronization)
{
  cuda_expected<cuda_stream> stream = cuda_stream::create();
  ASSERT_TRUE(stream.has_value()) << stream.error();

  std::vector<int> source(512);
  std::iota(source.begin(), source.end(), 1000);

  cuda_expected<device_vector<int>> block = device_vector<int>::create(source.size());
  ASSERT_TRUE(block.has_value()) << block.error();

  cuda_result written = copy_to_device_async(block.value(), span<const int>(source), stream.value());
  ASSERT_TRUE(written.has_value()) << written.error();

  std::vector<int> destination(source.size(), -1);
  cuda_result      read = copy_to_host_async(span<int>(destination), block.value(), stream.value());
  ASSERT_TRUE(read.has_value()) << read.error();

  cuda_result synchronized = stream.value().synchronize();
  ASSERT_TRUE(synchronized.has_value()) << synchronized.error();

  ASSERT_EQ(source, destination);
}

TEST_F(cuda_adt_test, copy_larger_than_the_destination_reports_an_error)
{
  std::vector<int> source(64, 7);

  cuda_expected<device_vector<int>> block = device_vector<int>::create(32);
  ASSERT_TRUE(block.has_value()) << block.error();

  cuda_result written = copy_to_device(block.value(), span<const int>(source));
  ASSERT_FALSE(written.has_value());
}

TEST_F(cuda_adt_test, managed_vector_allocates_the_requested_size)
{
  cuda_expected<managed_vector<float>> block = managed_vector<float>::create(128);
  ASSERT_TRUE(block.has_value());
  EXPECT_EQ(block.value().size(), 128);
  EXPECT_EQ(block.value().size_bytes(), 128 * sizeof(float));
  EXPECT_NE(block.value().data(), nullptr);
  EXPECT_FALSE(block.value().empty());
}

TEST_F(cuda_adt_test, empty_managed_vector_is_valid)
{
  cuda_expected<managed_vector<float>> block = managed_vector<float>::create(0);
  ASSERT_TRUE(block.has_value());
  EXPECT_TRUE(block.value().empty());
  EXPECT_EQ(block.value().data(), nullptr);
}

TEST_F(cuda_adt_test, moved_from_managed_vector_releases_ownership)
{
  cuda_expected<managed_vector<int>> block = managed_vector<int>::create(16);
  ASSERT_TRUE(block.has_value());

  managed_vector<int> moved(std::move(block.value()));
  EXPECT_EQ(moved.size(), 16);
  EXPECT_NE(moved.data(), nullptr);
  EXPECT_TRUE(block.value().empty());
  EXPECT_EQ(block.value().data(), nullptr);
}

TEST_F(cuda_adt_test, managed_memory_is_addressable_from_the_host)
{
  cuda_expected<managed_vector<int>> block = managed_vector<int>::create(64);
  ASSERT_TRUE(block.has_value());

  // The point of a managed allocation: the host writes through the same pointer the device reads.
  std::iota(block.value().data(), block.value().data() + block.value().size(), 0);
  EXPECT_EQ(block.value().data()[0], 0);
  EXPECT_EQ(block.value().data()[63], 63);
}

TEST_F(cuda_adt_test, managed_memory_survives_a_prefetch_round_trip)
{
  cuda_expected<cuda_stream> stream = cuda_stream::create();
  ASSERT_TRUE(stream.has_value());
  cuda_expected<managed_vector<int>> block = managed_vector<int>::create(256);
  ASSERT_TRUE(block.has_value());

  std::iota(block.value().data(), block.value().data() + block.value().size(), 1);

  ASSERT_TRUE(block.value().prefetch(memory_location::device, 0, stream.value()).has_value());
  ASSERT_TRUE(block.value().prefetch(memory_location::host, 0, stream.value()).has_value());
  ASSERT_TRUE(stream.value().synchronize().has_value());

  EXPECT_EQ(block.value().data()[0], 1);
  EXPECT_EQ(block.value().data()[255], 256);
}

TEST_F(cuda_adt_test, managed_memory_advice_is_accepted)
{
  cuda_expected<managed_vector<int>> block = managed_vector<int>::create(64);
  ASSERT_TRUE(block.has_value());

  EXPECT_TRUE(block.value().advise_preferred_location(memory_location::device, 0).has_value());
  EXPECT_TRUE(block.value().advise_accessed_by(memory_location::host, 0).has_value());
}

TEST_F(cuda_adt_test, prefetching_an_empty_block_succeeds)
{
  cuda_expected<cuda_stream> stream = cuda_stream::create();
  ASSERT_TRUE(stream.has_value());
  managed_vector<int> block;

  EXPECT_TRUE(block.prefetch(memory_location::device, 0, stream.value()).has_value());
  EXPECT_TRUE(block.advise_accessed_by(memory_location::device, 0).has_value());
}

TEST_F(cuda_adt_test, device_properties_are_queried)
{
  cuda_device_properties properties = get_cuda_device_properties(0);

  // The values depend on the part the tests run on, so only their consistency is checked: an
  // integrated device shares its memory with the host and therefore reads managed pages directly.
  if (properties.integrated) {
    EXPECT_TRUE(properties.concurrent_managed_access);
  }
}

TEST_F(cuda_adt_test, unqueried_properties_are_all_false)
{
  // The conservative default matters: every property answers "assume nothing", so a backend that
  // never queried the device migrates the pages explicitly rather than reading them in place.
  //
  // The failed-query path is not exercised with an out-of-range device on purpose. The CUDA API
  // call it needs fails by design, and compute-sanitizer counts each such failure as an error,
  // which would hide real findings behind expected ones.
  cuda_device_properties properties;

  EXPECT_FALSE(properties.direct_managed_access_from_host);
  EXPECT_FALSE(properties.concurrent_managed_access);
  EXPECT_FALSE(properties.pageable_memory_uses_host_page_tables);
  EXPECT_FALSE(properties.integrated);
}
