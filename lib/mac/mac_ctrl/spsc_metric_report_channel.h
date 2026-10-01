// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/noop_functor.h"
#include "ocudu/adt/spsc_queue.h"
#include "ocudu/support/zero_copy_notifier.h"

namespace ocudu {

/// Notifier using SPSC queue and internal pooling of transferred objects.
template <typename ReportType, typename RecycleFunc = noop_operation>
class spsc_metric_report_channel : public zero_copy_notifier<ReportType>
{
  using committer = typename zero_copy_notifier<ReportType>::committer;

  struct consumer {
    spsc_metric_report_channel* parent;

    void operator()(ReportType* report)
    {
      if (report != nullptr) {
        parent->dispose(*report);
      }
    }
  };

  struct noop_ctor {
    ReportType operator()() const { return ReportType{}; }
  };

public:
  using builder = typename zero_copy_notifier<ReportType>::builder;
  using reader  = std::unique_ptr<ReportType, consumer>;

  template <typename ConstructReport = noop_ctor>
  spsc_metric_report_channel(size_t                  capacity,
                             ocudulog::basic_logger& logger_,
                             const ConstructReport&  report_ctor   = noop_ctor{},
                             const RecycleFunc&      recycle_func_ = {}) :
    logger(logger_), recycle_func(recycle_func_), free_list(capacity), pending(capacity)
  {
    ocudu_assert(capacity >= 2, "Capacity must be greater than 1");

    reports.reserve(capacity);
    for (unsigned i = 0; i != capacity; ++i) {
      reports.emplace_back(report_ctor());
      bool discard = free_list.try_push(i);
      report_fatal_error_if_not(discard, "Failed to fill free list");
    }
  }
  ~spsc_metric_report_channel() override
  {
    while (pop() != nullptr) {
    }
  }
  spsc_metric_report_channel(const spsc_metric_report_channel&) = delete;
  spsc_metric_report_channel(spsc_metric_report_channel&&)      = delete;

  reader pop()
  {
    unsigned idx;
    if (not pending.try_pop(idx)) {
      return reader{nullptr, consumer{nullptr}};
    }
    ocudu_sanity_check(idx < reports.size(), "Invalid report being committed");
    return reader{&reports[idx], consumer{this}};
  }

  ReportType* peek()
  {
    unsigned* idx = pending.front();
    if (idx == nullptr) {
      return nullptr;
    }
    return &reports[*idx];
  }

  size_t capacity() const { return reports.size(); }

  size_t size() const { return pending.size(); }

private:
  using queue_type = concurrent_queue<unsigned, concurrent_queue_policy::lockfree_spsc>;

  ReportType& get_next() override
  {
    unsigned idx;
    if (not free_list.try_pop(idx)) {
      logger.warning("Metric report queue is depleted. Discarding next report...");
      return dummy_report;
    }
    return reports[idx];
  }

  void commit(ReportType& report) override
  {
    if (&report == &dummy_report) {
      // Ignored.
      recycle_func(report);
      return;
    }
    unsigned idx = &report - reports.data();
    ocudu_sanity_check(idx < reports.size(), "Invalid report being committed");
    if (not pending.try_push(idx)) {
      logger.error("Failed to push metric report. Discarding it...");
      recycle_func(report);
    }
  }

  /// Called when the report has been consumed and can be returned to the free list.
  void dispose(ReportType& report)
  {
    unsigned idx = &report - reports.data();
    ocudu_sanity_check(idx < reports.size(), "Invalid report being committed");

    // Clear the report before popping so that it can be recycled.
    recycle_func(report);

    // Make the report reusable.
    if (not free_list.try_push(idx)) {
      logger.error("Failed to recycle metric report");
    }
  }

  ocudulog::basic_logger& logger;
  RecycleFunc             recycle_func;

  ReportType dummy_report;

  std::vector<ReportType> reports;
  queue_type              free_list;
  queue_type              pending;
};

} // namespace ocudu
