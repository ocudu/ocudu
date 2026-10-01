// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/expected.h"
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <regex>
#include <string>

namespace ocudu {

/// Returns true if \p s is a non-empty string of decimal digits.
inline bool is_number(const std::string& s)
{
  return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

/// Returns true if \p s matches the ISO 8601 timestamp format YYYY-MM-DDTHH:MM:SS[.mmm].
inline bool is_valid_timestamp(const std::string& s)
{
  static const std::regex ts_regex{R"(^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(\.\d{1,3})?$)"};
  return std::regex_match(s, ts_regex);
}

/// Parses a timestamp from either a Unix-ms integer string or ISO 8601 (YYYY-MM-DDTHH:MM:SS[.mmm]).
/// Returns the parsed time_point on success, or an error description on failure.
inline expected<std::chrono::system_clock::time_point, std::string> parse_timestamp_ms(const std::string& datetime)
{
  if (is_number(datetime)) {
    int64_t ms = std::stoll(datetime);
    return std::chrono::system_clock::time_point{std::chrono::milliseconds{ms}};
  }
  // Try ISO 8601: YYYY-MM-DDTHH:MM:SS[.mmm]
  std::tm     tm_val  = {};
  int         frac_ms = 0;
  const char* endptr  = strptime(datetime.c_str(), "%Y-%m-%dT%H:%M:%S", &tm_val);
  if (endptr == nullptr) {
    return make_unexpected(std::string{"Expected Unix ms integer or YYYY-MM-DDTHH:MM:SS[.mmm]"});
  }
  if (*endptr == '.') {
    ++endptr;
    char* end2    = nullptr;
    long  frac    = strtol(endptr, &end2, 10);
    int   ndigits = static_cast<int>(end2 - endptr);
    while (ndigits < 3) {
      frac *= 10;
      ++ndigits;
    }
    while (ndigits > 3) {
      frac /= 10;
      --ndigits;
    }
    frac_ms = static_cast<int>(frac);
  }
  time_t t = timegm(&tm_val);
  if (t == static_cast<time_t>(-1)) {
    return make_unexpected(std::string{"Failed to convert timestamp to UTC"});
  }
  return std::chrono::system_clock::from_time_t(t) + std::chrono::milliseconds{frac_ms};
}

} // namespace ocudu
