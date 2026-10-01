// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ocudulog/logger.h"
#include <optional>

namespace ocudu {

struct os_sched_affinity_bitmask;

/// \brief Check whether the CPU scaling governor is set to performance.
///
/// \param[in] logger to print warnings.
/// \return True if we were able to read the sysfs scaling governor information.
bool check_cpu_governor(ocudulog::basic_logger& logger);

/// \brief Check whether the DRM KMS polling is set.
///
/// \param[in] logger to print warnings.
/// \return True if we were able to read the sysfs for the DRM KMS polling information.
bool check_drm_kms_polling(ocudulog::basic_logger& logger);

} // namespace ocudu
