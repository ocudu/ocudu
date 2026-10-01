// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#ifdef TRACY_ENABLE
#include "tracy/Tracy.hpp"

#define OCUDU_ZONE_SCOPED_NC ZoneScopedNC
#else
#define OCUDU_ZONE_SCOPED_NC(name, color)                                                                              \
  do {                                                                                                                 \
  } while (0)
#endif
