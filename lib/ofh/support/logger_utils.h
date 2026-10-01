// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ocudulog/logger.h"

namespace ocudu {
namespace ofh {

/// Logs the given message as a warning if \c enable_warnings is true, otherwise logs it in info.
template <typename... Args>
void log_conditional_warning(ocudulog::basic_logger& logger, bool enable_warnings, const char* fmt, Args&&... args)
{
  if (enable_warnings) {
    logger.warning(fmt, std::forward<Args>(args)...);
    return;
  }
  logger.info(fmt, std::forward<Args>(args)...);
}

} // namespace ofh
} // namespace ocudu
