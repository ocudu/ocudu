// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/expected.h"
#include "fmt/format.h"
#include <cerrno>
#include <charconv>

namespace ocudu {
namespace detail {

/// \brief Parses a floating point value from the given string into a floating type.
///
/// Note: Takes std::string as an argument as std::strod requires the array to be null-termitated and std::string_view
/// does not guarantee it.
template <typename Float>
expected<Float, std::string> parse_floating_point(const std::string& value)
{
  // Reset a possible previous error.
  errno                = 0;
  const char* str_init = value.c_str();
  char*       str_end  = nullptr;

  Float out_value;
  if constexpr (std::is_same_v<Float, float>) {
    out_value = std::strtof(str_init, &str_end);
  } else if constexpr (std::is_same_v<Float, double>) {
    out_value = std::strtod(str_init, &str_end);
  } else {
    out_value = std::strtold(str_init, &str_end);
  }

  if (errno == ERANGE || str_end == str_init || *str_end != '\0') {
    return make_unexpected(fmt::format("Could not convert '{}' to double", value));
  }

  return out_value;
}

} // namespace detail

/// Parses integer values from a console command.
template <typename Integer>
inline expected<Integer, std::string> parse_int(std::string_view value)
{
  static_assert(std::is_integral_v<Integer>, "Template type is not an integral");

  Integer out_value;
  auto [ptr, errorcode] = std::from_chars(value.begin(), value.end(), out_value);

  if (errorcode == std::errc() && ptr == value.end()) {
    return out_value;
  }

  return make_unexpected(fmt::format("Could not convert '{}' to integer", value));
}

/// Parses hex integer values from a console command.
template <typename Integer>
inline expected<Integer, std::string> parse_unsigned_hex(std::string_view value)
{
  static_assert(std::is_integral_v<Integer>, "Template type is not an integral");

  // Skip '0x' or '0X' as std::from_chars does not manage it.
  auto start_pos = value.find('x');
  if (start_pos == value.npos) {
    start_pos = value.find('X');
  }

  Integer out_value;
  auto [ptr, errorcode] = std::from_chars(
      (start_pos == value.npos) ? value.begin() : value.begin() + (start_pos + 1), value.end(), out_value, 16);

  if (errorcode == std::errc() && ptr == value.end()) {
    return out_value;
  }

  return make_unexpected(fmt::format("Could not convert '{}' to integer", value));
}

/// Parses a floating point value from the given string into a double.
inline expected<double, std::string> parse_double(const std::string& value)
{
  return detail::parse_floating_point<double>(value);
}

/// Parses a floating point value from the given string into a float.
inline expected<float, std::string> parse_float(const std::string& value)
{
  return detail::parse_floating_point<float>(value);
}

} // namespace ocudu
