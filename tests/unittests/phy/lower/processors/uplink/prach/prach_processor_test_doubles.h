// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../../../../gateways/baseband/baseband_gateway_buffer_test_doubles.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/phy/lower/processors/uplink/prach/prach_processor_baseband.h"
#include "ocudu/phy/lower/processors/uplink/prach/prach_processor_factories.h"
#include "ocudu/phy/lower/processors/uplink/prach/prach_processor_notifier.h"
#include "ocudu/phy/lower/processors/uplink/prach/prach_processor_request_handler.h"
#include "ocudu/phy/support/prach_buffer.h"
#include "ocudu/phy/support/prach_buffer_context.h"
#include <vector>

namespace ocudu {

class prach_processor_baseband_spy : public prach_processor_baseband
{
public:
  struct entry_t {
    baseband_gateway_buffer_read_only samples;
    symbol_context                    context;
  };

  void process_symbol(const baseband_gateway_buffer_reader& samples, const symbol_context& context) override
  {
    entries.emplace_back();
    entry_t& entry = entries.back();
    entry.samples  = samples;
    entry.context  = context;
  }

  const std::vector<entry_t>& get_entries() const { return entries; }

  void clear() { entries.clear(); }

private:
  std::vector<entry_t> entries;
};

class prach_processor_request_handler_spy : public prach_processor_request_handler
{
public:
  struct entry_t {
    prach_buffer_context context;
    shared_prach_buffer  buffer;
  };

  void handle_request(shared_prach_buffer buffer, const prach_buffer_context& context) override
  {
    entries.emplace_back();
    entry_t& entry = entries.back();
    entry.context  = context;
    entry.buffer   = std::move(buffer);
  }

  const std::vector<entry_t>& get_entries() const { return entries; }

  void clear() { entries.clear(); }

private:
  std::vector<entry_t> entries;
};

class prach_processor_spy : public prach_processor
{
public:
  void                             connect(prach_processor_notifier& notifier) override {}
  void                             stop() override {}
  prach_processor_request_handler& get_request_handler() override { return request_handler; }
  prach_processor_baseband&        get_baseband() override { return baseband; }

  const std::vector<prach_processor_baseband_spy::entry_t>& get_baseband_entries() const
  {
    return baseband.get_entries();
  }

  void clear() { baseband.clear(); }

private:
  prach_processor_request_handler_spy request_handler;
  prach_processor_baseband_spy        baseband;
};

class prach_processor_factory_spy : public prach_processor_factory
{
public:
  std::unique_ptr<prach_processor> create() override
  {
    std::unique_ptr<prach_processor_spy> ptr = std::make_unique<prach_processor_spy>();
    instance                                 = ptr.get();
    return std::move(ptr);
  }

  prach_processor_spy& get_spy() const { return *instance; }

private:
  prach_processor_spy* instance = nullptr;
};

} // namespace ocudu
