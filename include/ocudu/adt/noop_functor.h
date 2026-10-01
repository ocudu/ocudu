// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {

/// Do nothing operation functor.
struct noop_operation {
  template <typename T>
  void operator()(T&& t) const
  {
    // Do nothing.
  }
  void operator()() const
  {
    // Do nothing.
  }
};

} // namespace ocudu
