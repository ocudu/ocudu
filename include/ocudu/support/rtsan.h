// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// \brief Defines helper macros to use RTSAN, if supported.

#pragma once

#ifdef __has_feature
#if __has_feature(realtime_sanitizer)
#define OCUDU_RTSAN_ENABLED
#endif
#endif

#ifdef OCUDU_RTSAN_ENABLED
#include <sanitizer/rtsan_interface.h>

#define OCUDU_RTSAN_NONBLOCKING [[clang::nonblocking]]
#define OCUDU_RTSAN_SCOPED_DISABLER(VAR) __rtsan::ScopedDisabler(VAR);

namespace ocudu {
namespace detail {
class scoped_enabler
{
public:
  scoped_enabler() { __rtsan_enable(); }
  ~scoped_enabler() { __rtsan_disable(); }
  scoped_enabler(const scoped_enabler&)            = delete;
  scoped_enabler& operator=(const scoped_enabler&) = delete;
  scoped_enabler(scoped_enabler&&)                 = delete;
  scoped_enabler& operator=(scoped_enabler&&)      = delete;
};
} // namespace detail
} // namespace ocudu

#define OCUDU_RTSAN_SCOPED_ENABLER ::ocudu::detail::scoped_enabler rtsan_enabler##__LINE__

#else
#define OCUDU_RTSAN_NONBLOCKING
#define OCUDU_RTSAN_SCOPED_DISABLER(VAR)
#define OCUDU_RTSAN_SCOPED_ENABLER
#endif
