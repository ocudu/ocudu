// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/bf16.h"

int main()
{
  ocudu::bf16_t a;

  a = ocudu::to_bf16(1.23F);
  abort();
}
