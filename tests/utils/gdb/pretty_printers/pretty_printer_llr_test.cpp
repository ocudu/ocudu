// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/phy/upper/log_likelihood_ratio.h"

int main()
{
  ocudu::log_likelihood_ratio a;
  ocudu::log_likelihood_ratio b;

  a = 33;
  b = ocudu::log_likelihood_ratio::infinity();

  abort();
}
