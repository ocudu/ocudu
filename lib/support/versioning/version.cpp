// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/versioning/version.h"
#include "version_info.h"

using namespace ocudu;

const char* ocudu::get_version()
{
  return OCUDU_VERSION_STRING;
}
