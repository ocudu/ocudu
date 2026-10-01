// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/tiny_optional.h"

int main()
{
  ocudu::tiny_optional<std::unique_ptr<int>> a;
  ocudu::tiny_optional<float>                b;
  ocudu::tiny_optional<std::unique_ptr<int>> c = {};
  ocudu::tiny_optional<float>                d;

  int a_val = 8;
  a         = &a_val;
  b         = 3.4;
  d         = {};
  abort();
}
