// SPDX-FileCopyrightText: Copyright (C) 2021-2026 DeepSig Inc
// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/cuda/support/cuda_device.h"
#include <cuda_runtime.h>

using namespace ocudu;
using namespace ocudu::cuda;

/// Reads one device attribute, reporting false when the query fails.
static bool query_attribute(::cudaDeviceAttr attribute, int device_id)
{
  int value = 0;
  if (::cudaDeviceGetAttribute(&value, attribute, device_id) != cudaSuccess) {
    // The failed query leaves the error latched on the context, where it would surface at an
    // unrelated call.
    (void)::cudaGetLastError();
    return false;
  }

  return value != 0;
}

cuda_device_properties ocudu::cuda::get_cuda_device_properties(int device_id)
{
  cuda_device_properties properties;
  properties.direct_managed_access_from_host = query_attribute(cudaDevAttrDirectManagedMemAccessFromHost, device_id);
  properties.concurrent_managed_access       = query_attribute(cudaDevAttrConcurrentManagedAccess, device_id);
  properties.pageable_memory_uses_host_page_tables =
      query_attribute(cudaDevAttrPageableMemoryAccessUsesHostPageTables, device_id);
  properties.integrated = query_attribute(cudaDevAttrIntegrated, device_id);

  return properties;
}

bool cuda::is_cuda_device_available()
{
  int nof_devices = 0;
  if ((::cudaGetDeviceCount(&nof_devices) != cudaSuccess) || (nof_devices == 0)) {
    // The query leaves the error latched on the context, where it would surface at an unrelated
    // call.
    (void)::cudaGetLastError();
    return false;
  }
  return true;
}
