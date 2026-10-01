// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <type_traits>
#include <utility>

namespace ocudu {

namespace detail {

template <typename Callable>
struct scope_exit {
  template <typename C>
  explicit scope_exit(C&& f_) : exit_function(std::forward<C>(f_))
  {
  }
  scope_exit(scope_exit&& rhs) noexcept : exit_function(std::move(rhs.exit_function)), active(rhs.active)
  {
    rhs.release();
  }
  scope_exit(const scope_exit&)                = delete;
  scope_exit& operator=(const scope_exit&)     = delete;
  scope_exit& operator=(scope_exit&&) noexcept = delete;
  ~scope_exit()
  {
    if (active) {
      exit_function();
    }
  }

  void release() { active = false; }

private:
  Callable exit_function;
  bool     active = true;
};

} // namespace detail

/// \brief Defers callable call to scope exit
/// \tparam Callable Any type with a call operator
/// \param callable function that is called at scope exit
/// \return object that has to be stored in a local variable
template <typename Callable>
[[nodiscard]] detail::scope_exit<std::decay_t<Callable>> make_scope_exit(Callable&& callable)
{
  return detail::scope_exit<std::decay_t<Callable>>{std::forward<Callable>(callable)};
}

#define DEFER(FUNC) auto on_exit_call##__LINE__ = ocudu::make_scope_exit([&]() { FUNC })

} // namespace ocudu
