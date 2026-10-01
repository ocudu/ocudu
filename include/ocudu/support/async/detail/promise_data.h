// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/compiler.h"
#include "ocudu/support/ocudu_assert.h"
#include <memory>

namespace ocudu {
namespace detail {

/// Class that holds promise result value.
template <typename R, typename Base>
struct promise_data : public Base {
  promise_data() = default;

  promise_data(const promise_data& other)            = delete;
  promise_data(promise_data&& other)                 = delete;
  promise_data& operator=(const promise_data& other) = delete;
  promise_data& operator=(promise_data&& other)      = delete;

  ~promise_data()
  {
    if (Base::has_value) {
      std::launder(reinterpret_cast<R*>(&mem))->~R();
    }
  }

  /// Set Return value.
  template <typename U>
  void return_value(U&& u)
  {
    ocudu_sanity_check(not Base::has_value, "Return called more than once.");
    new (&mem) R(std::forward<U>(u));
    Base::has_value = true;
  }

  /// Get Return value.
  const R& get() const&
  {
    ocudu_assert(Base::has_value, "Trying to extract result from unset Promise");
    return *std::launder(reinterpret_cast<const R*>(&mem));
  }

  /// Get Return value.
  R get() &&
  {
    ocudu_assert(Base::has_value, "Trying to extract result from unset Promise");
    return std::move(*std::launder(reinterpret_cast<R*>(&mem)));
  }

private:
  /// Result storage.
  std::aligned_storage_t<sizeof(R), alignof(R)> mem;
};

/// Specialization for when Return type is void.
template <typename Base>
struct promise_data<void, Base> : public Base {
  void return_value() { Base::has_value = true; }
};

} // namespace detail
} // namespace ocudu
