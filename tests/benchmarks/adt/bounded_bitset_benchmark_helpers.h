// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include <random>

namespace ocudu {

template <size_t N>
std::array<bool, N> create_contiguous_array(unsigned number_of_ones)
{
  std::array<bool, N> ar = {};
  std::fill(ar.begin(), ar.begin() + number_of_ones, true);
  return ar;
}

template <size_t N>
std::array<bool, N> shuffle_array(const std::array<bool, N>& ar)
{
  static std::mt19937 rgen;
  std::array<bool, N> ar2 = ar;
  std::shuffle(ar2.begin(), ar2.end(), rgen);
  return ar;
}

} // namespace ocudu
