// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/expected.h"
#include <string>

namespace ocudu {
namespace ofh {

/// \brief Warn of unreceived Radio Unit frames type.
///
/// never: does not log unreceived frames.
/// always: logs all unreceived frames.
/// after_traffic_detection: logs all the unreceived frames after the first received frame.
enum class warn_unreceived_ru_frames { never, always, after_traffic_detection };

/// Converts the given warn unreceived RU frames value to string.
inline const char* to_string(warn_unreceived_ru_frames value)
{
  switch (value) {
    case warn_unreceived_ru_frames::never:
      return "never";
      break;
    case warn_unreceived_ru_frames::always:
      return "always";
      break;
    case warn_unreceived_ru_frames::after_traffic_detection:
      return "after_traffic_detection";
      break;
  }

  return "always";
}

/// Converts the given value to warn unreceived RU frames.
inline expected<warn_unreceived_ru_frames> to_warn_unreceived_ru_frames(std::string value)
{
  std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return std::tolower(c); });

  if (value == "never") {
    return warn_unreceived_ru_frames::never;
  }

  if (value == "always") {
    return warn_unreceived_ru_frames::always;
  }

  if (value == "after_traffic_detection") {
    return warn_unreceived_ru_frames::after_traffic_detection;
  }

  return make_unexpected(default_error_t());
}

} // namespace ofh
} // namespace ocudu
