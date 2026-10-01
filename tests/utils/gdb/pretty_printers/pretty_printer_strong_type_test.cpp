// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/bounded_integer.h"
#include "ocudu/adt/strong_type.h"
#include <cstdlib>

struct tag;

int main()
{
  ocudu::strong_type<int, tag, ocudu::strong_equality> a{42};
  ocudu::bounded_integer<unsigned char, 0, 28>         b{8};

  std::abort();
}
