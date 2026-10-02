// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {
namespace cuda {

/// \brief Properties of a CUDA device that decide how managed memory has to be handled.
///
/// The differences below are not performance tuning: reading device-resident managed pages from the
/// host is coherent on some GPUs and unsupported on others, so the same code either works or
/// returns wrong data depending on where it runs. A backend queries the device once and keeps the
/// answer.
struct cuda_device_properties {
  /// \brief True when the host may read managed pages that live in device memory.
  ///
  /// Reported by \c cudaDevAttrDirectManagedMemAccessFromHost. Where it is false, the pages have to
  /// be migrated back before the host touches them.
  bool direct_managed_access_from_host = false;
  /// \brief True when host and device may access a managed allocation at the same time.
  ///
  /// Reported by \c cudaDevAttrConcurrentManagedAccess.
  bool concurrent_managed_access = false;
  /// \brief True when the device walks the host page tables to reach pageable memory.
  ///
  /// Reported by \c cudaDevAttrPageableMemoryAccessUsesHostPageTables.
  bool pageable_memory_uses_host_page_tables = false;
  /// \brief True when the device shares its memory with the host.
  ///
  /// Reported by \c cudaDevAttrIntegrated, which is set on the Tegra devices.
  bool integrated = false;
};

/// \brief Queries the managed memory properties of a device.
///
/// \param[in] device_id Device to query.
/// \return The properties of the device, or all of them false when the query fails, which is the
/// conservative answer: it makes every caller migrate the pages explicitly.
cuda_device_properties get_cuda_device_properties(int device_id);

/// \brief Returns true if a CUDA device is present and usable.
///
/// \return True when the runtime reports at least one device, false when it reports none or the
/// query itself fails.
bool is_cuda_device_available();

} // namespace cuda
} // namespace ocudu
