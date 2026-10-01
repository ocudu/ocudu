// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/versioning/build_info.h"
#include "hashes.h"

using namespace ocudu;

const char* ocudu::get_build_hash()
{
  return build_hash;
}

const char* ocudu::get_build_info()
{
  return build_info;
}

const char* ocudu::get_build_mode()
{
  return build_mode;
}
