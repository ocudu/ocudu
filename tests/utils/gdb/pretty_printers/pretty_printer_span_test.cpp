// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/span.h"
#include <array>
#include <cstdlib>

int main()
{
  std::array<int, 3> arr = {10, 20, 30};
  ocudu::span<int>   a(arr);

  ocudu::span<int> b;

  std::abort();
}
