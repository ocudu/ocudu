// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/expected.h"
#include <cstdlib>

enum class my_error { err1 = 42 };

int main()
{
  ocudu::expected<int, my_error> a{5};
  ocudu::expected<int, my_error> b{ocudu::make_unexpected(my_error::err1)};

  std::abort();
}
