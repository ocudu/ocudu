// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/memory_pool/pool_memory_region.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/error_handling.h"
#include "ocudu/support/memory_pool/memory_pool_utils.h"
#include <fstream>
#include <sstream>
#include <sys/mman.h>

using namespace ocudu;

/// Cache line size.
static constexpr std::size_t OCUDU_CACHE_LINE = 64;

/// Default huge page size.
static constexpr std::size_t OCUDU_HUGEPAGE_2MB = 2 * 1024 * 1024;

/// Get the configured huge page size.
static std::size_t system_hugepage_size()
{
  const char* meminfo_file_path = "/proc/meminfo";
  const char* target_key        = "Hugepagesize:";

  // Open the /proc/meminfo file.
  std::ifstream meminfo(meminfo_file_path);
  if (!meminfo.is_open()) {
    // Could not open the /proc/meminfo file, fallback to 2MB.
    return OCUDU_HUGEPAGE_2MB;
  }

  // Read the /proc/meminfo file line by line.
  std::string line;
  while (std::getline(meminfo, line)) {
    // Find the target key.
    if (line.rfind(target_key, 0) == 0) {
      std::stringstream ss(line);
      std::string       key;
      std::size_t       size_kb = 0;

      // Parse the hugepage size.
      if (ss >> key >> size_kb) {
        return size_kb * 1024;
      }
    }
  }

  // Did not manage to parse the value from the /proc/meminfo file, fallback to 2MB.
  return OCUDU_HUGEPAGE_2MB;
}

/// Maps the requested size in bytes to the reserved huge page region.
static uint8_t* map_to_hugepage_region(std::size_t aligned_size)
{
  // Note: MAP_POPULATE prefaults the mapped pages.
  uint8_t* ptr = static_cast<uint8_t*>(::mmap(
      nullptr, aligned_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB | MAP_POPULATE, -1, 0));

  if (ptr != MAP_FAILED) {
    return ptr;
  }

  return nullptr;
}

/// Allocates the memory of the requested size either on heap or by mapping huge pages.
static std::variant<heap_memory_resource, hugepage_memory_resource> make_storage(std::size_t size_bytes,
                                                                                 bool        use_hugepages)
{
  report_error_if_not(size_bytes > 0, "Memory region size in bytes must be greater than zero");

  ocudulog::basic_logger& memory_region_logger = ocudulog::fetch_basic_logger("ALL");

  // Huge page backing requested.
  if (use_hugepages) {
    // Get the configured huge page size.
    std::size_t hp_size = system_hugepage_size();

    // Align to the cache line boundary.
    std::size_t aligned_size = align_next(size_bytes, OCUDU_CACHE_LINE);

    // Align to the huge page boundary.
    aligned_size = align_next(aligned_size, hp_size);

    // Try to map to the huge page region.
    uint8_t* ptr = map_to_hugepage_region(aligned_size);

    // Successfully mapped to huge pages.
    if (ptr != nullptr) {
      memory_region_logger.info("Memory region successfully mapped to huge page(s).");
      return hugepage_memory_resource{ptr, size_bytes, aligned_size};
    }
    memory_region_logger.warning("Failed to map memory region to huge page(s), fallback to malloc() style.");
  }

  // Use malloc() if huge page backing was not requested or mapping the huge pages has failed.
  return heap_memory_resource(size_bytes);
}

pool_memory_region::pool_memory_region(std::size_t size_bytes, bool use_hugepages) :
  storage(make_storage(size_bytes, use_hugepages)),
  block_view(std::visit([](const auto& res) { return res.memory_block(); }, storage))
{
}

/// Constructor implementation of the hugepage_memory_resource
hugepage_memory_resource::hugepage_memory_resource(uint8_t* ptr, size_t sz, size_t mapped_sz) :
  block_view(ptr, sz), mapped_size(mapped_sz)
{
}

/// Destructor implementation of the hugepage_memory_resource
hugepage_memory_resource::~hugepage_memory_resource()
{
  uint8_t* block = static_cast<uint8_t*>(block_view.data());
  if (block != nullptr && mapped_size != 0) {
    if (::munmap(block, mapped_size) != 0) {
      ocudulog::fetch_basic_logger("ALL").warning("Failed to unmap memory block backed by huge pages");
    }
  }
}
