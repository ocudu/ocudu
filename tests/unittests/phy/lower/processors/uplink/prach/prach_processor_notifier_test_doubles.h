// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/phy/lower/processors/uplink/prach/prach_processor_notifier.h"
#include "ocudu/phy/support/prach_buffer.h"
#include "ocudu/phy/support/prach_buffer_context.h"
#include "ocudu/phy/support/shared_prach_buffer.h"
#include <vector>

namespace ocudu {

class prach_processor_notifier_spy : public prach_processor_notifier
{
private:
  struct rx_prach_window_entry {
    prach_buffer_context context;
    shared_prach_buffer  buffer;
  };

  ocudulog::basic_logger&            logger;
  std::vector<prach_buffer_context>  request_late_entries;
  std::vector<prach_buffer_context>  request_overflow_entries;
  std::vector<rx_prach_window_entry> rx_prach_window_entries;

public:
  explicit prach_processor_notifier_spy(const std::string& log_level = "info") :
    logger(ocudulog::fetch_basic_logger("Notifier", false))
  {
    ocudulog::init();
    auto value = ocudulog::str_to_basic_level(log_level);
    logger.set_level(value.has_value() ? value.value() : ocudulog::basic_levels::none);
  }

  void on_prach_request_late(const prach_buffer_context& context) override
  {
    logger.info("PRACH request is late. Sector/Port {}/{}. Slot/Symbol {}/{}.",
                context.sector,
                context.ports,
                context.slot,
                context.start_symbol);
    request_late_entries.push_back(context);
  }

  void on_prach_request_overflow(const prach_buffer_context& context) override
  {
    logger.info("PRACH request discarded. Sector/Port {}/{}. Slot/Symbol {}/{}.",
                context.sector,
                context.ports,
                context.slot,
                context.start_symbol);
    request_overflow_entries.push_back(context);
  }

  void on_rx_prach_window(shared_prach_buffer buffer, const prach_buffer_context& context) override
  {
    logger.info("PRACH Rx Window Processed. Sector/Port {}/{}. Slot/Symbol {}/{}.",
                context.sector,
                context.ports,
                context.slot,
                context.start_symbol);
    rx_prach_window_entries.emplace_back();
    rx_prach_window_entry& entry = rx_prach_window_entries.back();
    entry.context                = context;
    entry.buffer                 = std::move(buffer);
  }

  unsigned get_nof_notifications() const
  {
    return request_late_entries.size() + request_overflow_entries.size() + rx_prach_window_entries.size();
  }

  void clear_notifications()
  {
    request_late_entries.clear();
    request_overflow_entries.clear();
    rx_prach_window_entries.clear();
  }

  const std::vector<prach_buffer_context>& get_request_late_entries() const { return request_late_entries; }

  const std::vector<prach_buffer_context>& get_request_overflow_entries() const { return request_overflow_entries; }

  const std::vector<rx_prach_window_entry>& get_rx_prach_window_entries() const { return rx_prach_window_entries; }
};

} // namespace ocudu
