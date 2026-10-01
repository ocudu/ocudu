// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/bounded_bitset.h"

int main()
{
  ocudu::bounded_bitset<5> a(4);

  a.flip(2);

  abort();
}
