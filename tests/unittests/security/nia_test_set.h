// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>
#include <ostream>

struct nia_test_set {
  const char* name;
  uint32_t    count_i;
  uint8_t     bearer;
  uint8_t     direction;
  const char* ik_cstr;
  uint32_t    length;
  const char* message_cstr;
  const char* mact_cstr;
};

//// Dummy operator to avoid Valgrind warnings.
inline std::ostream& operator<<(std::ostream& os, const nia_test_set& params)
{
  return os;
}
