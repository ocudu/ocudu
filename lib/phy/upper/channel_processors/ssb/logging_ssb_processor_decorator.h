// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ocuduvec/bit.h"
#include "ocudu/ocuduvec/zero.h"
#include "ocudu/phy/support/support_formatters.h"
#include "ocudu/phy/upper/channel_processors/ssb/formatters.h"
#include "ocudu/phy/upper/channel_processors/ssb/ssb_processor.h"

namespace ocudu {

class logging_ssb_processor_decorator : public ssb_processor
{
  template <typename Func>
  static std::chrono::nanoseconds time_execution(Func&& func)
  {
    auto start = std::chrono::steady_clock::now();
    func();
    auto end = std::chrono::steady_clock::now();

    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  }

public:
  logging_ssb_processor_decorator(ocudulog::basic_logger& logger_, std::unique_ptr<ssb_processor> processor_) :
    logger(logger_), processor(std::move(processor_))
  {
    ocudu_assert(processor, "Invalid processor.");
  }

  void process(resource_grid_writer& grid, const pdu_t& pdu) override
  {
    const auto&& func = [&]() { processor->process(grid, pdu); };

    std::chrono::nanoseconds time_ns = time_execution(func);

    static_bit_buffer<MIB_PAYLOAD_SIZE> data(pdu.mib_payload.size());
    ocuduvec::zero(data.get_buffer());
    ocuduvec::bit_pack(data, pdu.mib_payload);

    if (logger.debug.enabled()) {
      // Detailed log information, including a list of all SSB PDU fields.
      logger.debug(pdu.slot.sfn(),
                   pdu.slot.slot_index(),
                   data.get_buffer().data(),
                   divide_ceil(data.size(), 8),
                   "SSB: {:s} {}\n  {:n}\n  {}",
                   pdu,
                   time_ns,
                   pdu,
                   time_ns);

      return;
    }
    // Single line log entry.
    logger.info(pdu.slot.sfn(),
                pdu.slot.slot_index(),
                data.get_buffer().data(),
                divide_ceil(data.size(), 8),
                "SSB: {:s} {}",
                pdu,
                time_ns);
  }

private:
  ocudulog::basic_logger&        logger;
  std::unique_ptr<ssb_processor> processor;
};

} // namespace ocudu
