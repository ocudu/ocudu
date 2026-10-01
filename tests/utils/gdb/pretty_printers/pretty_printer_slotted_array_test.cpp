// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/slotted_array.h"

int main()
{
  ocudu::slotted_array<int, 5> a;

  a.insert(1, 3);
  a.insert(3, -5);

  abort();
}
