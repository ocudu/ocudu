// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/slotted_vector.h"

int main()
{
  ocudu::slotted_vector<int> a;

  a.insert(1, 3);
  a.insert(3, -5);

  abort();
}
