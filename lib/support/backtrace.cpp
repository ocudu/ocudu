// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/backtrace.h"

#ifdef HAVE_BACKWARD
#include "Backward/backward.hpp"

void ocudu::enable_backtrace()
{
  static backward::SignalHandling sh;
}

#else // HAVE_BACKWARD

void ocudu::enable_backtrace()
{
  // Ignore.
}

#endif // HAVE_BACKWARD
