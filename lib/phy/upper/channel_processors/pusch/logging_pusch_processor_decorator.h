// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/phy/support/support_formatters.h"
#include "ocudu/phy/upper/channel_processors/pusch/formatters.h"
#include "ocudu/phy/upper/unique_rx_buffer.h"
#include "fmt/std.h"
#include <algorithm>
#include <atomic>

namespace fmt {

struct pusch_results_wrapper {
  std::optional<ocudu::pusch_processor_result_control> uci;
  std::optional<ocudu::pusch_processor_result_data>    sch;
};

/// \brief Custom formatter for \c pusch_results_wrapper.
template <>
struct formatter<pusch_results_wrapper> {
  /// Helper used to parse formatting options and format fields.
  ocudu::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const pusch_results_wrapper& result, FormatContext& ctx) const
  {
    // Format SCH message.
    if (result.sch.has_value()) {
      helper.format_always(ctx, *result.sch);
    }

    // Format UCI message.
    if (result.uci.has_value()) {
      helper.format_always(ctx, *result.uci);
    }

    // Format channel state information.
    if (result.sch.has_value()) {
      helper.format_always(ctx, result.sch->csi);
    } else if (result.uci.has_value()) {
      helper.format_always(ctx, result.uci->csi);
    }

    return ctx.out();
  }
};

} // namespace fmt

namespace ocudu {

class logging_pusch_processor_decorator : public pusch_processor, private pusch_processor_result_notifier
{
public:
  logging_pusch_processor_decorator(ocudulog::basic_logger& logger_, std::unique_ptr<pusch_processor> processor_) :
    logger(logger_), processor(std::move(processor_))
  {
    ocudu_assert(processor, "Invalid processor.");
  }

  void process(pusch_processor_result_notifier& notifier_,
               const resource_grid_reader&      grid,
               const common_parameters&         common_,
               span<ue_specific_parameters>     ues_) override
  {
    notifier    = &notifier_;
    common      = common_;
    time_start  = std::chrono::steady_clock::now();
    time_return = 0;

    // Save the UE parameters and clear the processor results.
    ues.clear();
    for (const ue_specific_parameters& ue : ues_) {
      ues.push_back({.grant = ue.grant, .data = ue.data, .results = {}, .time_uci = {}});
    }

    processor->process(*this, grid, common_, ues_);
    time_return = std::chrono::steady_clock::now().time_since_epoch().count();
  }

private:
  /// Collects the parameters and the results of a UE.
  struct ue_entry {
    /// UE grant.
    ue_grant grant;
    /// Received transport block data.
    span<uint8_t> data;
    /// Processor results.
    fmt::pusch_results_wrapper results;
    /// Time of the UCI notification.
    std::chrono::time_point<std::chrono::steady_clock> time_uci;
  };

  /// Gets the UE entry that matches the given context.
  ue_entry& get_ue(const ue_context& context)
  {
    auto* ue = std::find_if(ues.begin(), ues.end(), [&context](const ue_entry& entry) {
      return (entry.grant.context.rnti == context.rnti) && (entry.grant.context.harq_id == context.harq_id);
    });
    ocudu_assert(ue != ues.end(), "No UE matches rnti={} h_id={}.", context.rnti, context.harq_id);
    return *ue;
  }

  void on_uci(const pusch_processor_result_control& uci) override
  {
    ocudu_assert(notifier, "Invalid notifier");
    ue_entry& ue   = get_ue(uci.context);
    ue.time_uci    = std::chrono::steady_clock::now();
    ue.results.uci = uci;
    notifier->on_uci(uci);
  }

  void on_sch(const pusch_processor_result_data& sch) override
  {
    ocudu_assert(notifier, "Invalid notifier");

    // Save SCH results.
    ue_entry& ue   = get_ue(sch.context);
    ue.results.sch = sch;

    // Data size in bytes for printing hex dump only if SCH is present and CRC is passed.
    unsigned data_size = 0;
    if (ue.results.sch.has_value() && ue.results.sch->data.tb_crc_ok) {
      data_size = ue.data.size();
    }

    std::chrono::time_point<std::chrono::steady_clock> time_end = std::chrono::steady_clock::now();

    // Calculate the UCI report latency if available.
    std::chrono::nanoseconds time_uci_ns(0);
    if (ue.time_uci != std::chrono::time_point<std::chrono::steady_clock>()) {
      time_uci_ns = ue.time_uci - time_start;
    }

    // Calculate the return latency if available.
    std::chrono::nanoseconds                           time_return_ns(0);
    std::chrono::time_point<std::chrono::steady_clock> time_return_local =
        std::chrono::time_point<std::chrono::steady_clock>(std::chrono::steady_clock::duration(time_return));
    if (time_return_local != std::chrono::time_point<std::chrono::steady_clock>()) {
      time_return_ns = time_return_local - time_start;
    }

    // Calculate the final time.
    std::chrono::nanoseconds time_ns = time_end - time_start;

    if (logger.debug.enabled()) {
      // Detailed log information, including a list of all PDU fields.
      logger.debug(common.slot.sfn(),
                   common.slot.slot_index(),
                   ue.data.data(),
                   data_size,
                   "PUSCH: {:s} {:s} tbs={} {:s} {} uci_{} ret_{}\n  {:n}\n  {:n}\n  {:n}",
                   ue.grant,
                   common,
                   ue.data.size(),
                   ue.results,
                   time_ns,
                   time_uci_ns,
                   time_return_ns,
                   ue.grant,
                   common,
                   ue.results);
    } else {
      // Single line log entry.
      logger.info(common.slot.sfn(),
                  common.slot.slot_index(),
                  ue.data.data(),
                  data_size,
                  "PUSCH: {:s} {:s} tbs={} {:s} {} uci_{} ret_{}",
                  ue.grant,
                  common,
                  ue.data.size(),
                  ue.results,
                  time_ns,
                  time_uci_ns,
                  time_return_ns);
    }

    // Notify the SCH reception.
    notifier->on_sch(sch);
  }

  ocudulog::basic_logger&                                       logger;
  std::unique_ptr<pusch_processor>                              processor;
  common_parameters                                             common;
  static_vector<ue_entry, pusch_constants::MAX_NOF_MU_MIMO_UES> ues;
  pusch_processor_result_notifier*                              notifier;
  std::chrono::time_point<std::chrono::steady_clock>            time_start;
  std::atomic<uint64_t>                                         time_return;

  // Makes sure atomics are lock free.
  static_assert(std::atomic<decltype(time_return)>::is_always_lock_free);
};

} // namespace ocudu
