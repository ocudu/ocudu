// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/radio/radio_event_notifier.h"

namespace ocudu {

/// Radio Unit radio event logger.
class ru_radio_logger_event_handler : public radio_event_notifier
{
public:
  explicit ru_radio_logger_event_handler(ocudulog::basic_logger& logger_) : logger(logger_) {}

  // See interface for documentation.
  void on_radio_rt_event(const event_description& description) override
  {
    if (description.type == radio_event_type::LATE || description.type == radio_event_type::UNDERFLOW ||
        description.type == radio_event_type::OVERFLOW) {
      logger.warning("Real-time failure in RF: {}", to_string(description.type));
    }

    static const auto& log_format_debug = "Real-time failure in RF: Type={} Source={} Timestamp={}";

    if (description.timestamp.has_value()) {
      logger.debug(
          log_format_debug, to_string(description.type), to_string(description.source), *description.timestamp);
    } else {
      logger.debug(log_format_debug, to_string(description.type), to_string(description.source), "na");
    }
  }

private:
  ocudulog::basic_logger& logger;
};

/// Radio event dispatcher.
class ru_radio_event_dispatcher : public radio_event_notifier
{
  std::vector<radio_event_notifier*> handlers;

public:
  explicit ru_radio_event_dispatcher(std::vector<radio_event_notifier*> handlers_) : handlers(std::move(handlers_))
  {
    ocudu_assert(!handlers.empty(), "Empty list of radio event notifiers");
    ocudu_assert(std::all_of(handlers.begin(), handlers.end(), [](auto* handler) { return handler != nullptr; }),
                 "Invalid radio event notifier");
  }

  // See interface for documentation.
  void on_radio_rt_event(const event_description& description) override
  {
    for (auto* handler : handlers) {
      handler->on_radio_rt_event(description);
    }
  }
};

} // namespace ocudu
