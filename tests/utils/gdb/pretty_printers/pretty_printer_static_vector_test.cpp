// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/static_vector.h"

int main()
{
  ocudu::static_vector<int, 5> a;

  for (unsigned i = 0; i != 3; ++i) {
    a.push_back(i);
  }

  abort();
}
