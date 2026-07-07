// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "split6_o_du_low_metrics_collector_impl.h"
#include "metrics/split6_flexible_o_du_low_metrics.h"
#include "ocudu/du/du_low/o_du_low_metrics_collector.h"
#include "ocudu/ru/ru_metrics_collector.h"

using namespace ocudu;

namespace {

/// Radio Unit metrics collector dummy implementation.
class ru_metrics_collector_dummy : public ru_metrics_collector
{
public:
  void collect_metrics(ru_metrics& metrics) override {}
};

/// O-DU low metrics collector dummy implementation.
class o_du_low_metrics_collector_dummy : public odu::o_du_low_metrics_collector
{
public:
  void collect_metrics(odu::o_du_low_metrics& metrics) override {}
};

} // namespace

static o_du_low_metrics_collector_dummy dummy_odu_low;
static ru_metrics_collector_dummy       dummy_ru;

split6_o_du_low_metrics_collector_impl::split6_o_du_low_metrics_collector_impl(
    odu::o_du_low_metrics_collector*           odu_low_,
    ru_metrics_collector*                      ru_,
    split6_flexible_o_du_low_metrics_notifier* notifier_,
    unique_timer                               timer_,
    std::chrono::milliseconds                  report_period_) :
  timer(std::move(timer_)),
  report_period(report_period_),
  notifier(notifier_),
  odu_low(odu_low_ ? odu_low_ : &dummy_odu_low),
  ru(ru_ ? ru_ : &dummy_ru)
{
  // When no notifier is configured, do not start timer to collect metrics.
  if (!notifier || report_period.count() == 0) {
    return;
  }

  ocudu_assert(timer.is_valid(), "Invalid timer passed to metrics controller");
  timer.set(report_period, [this]() { collect_metrics(); });

  stopped.store(false, std::memory_order_relaxed);
  timer.run();
}

split6_o_du_low_metrics_collector_impl&
split6_o_du_low_metrics_collector_impl::operator=(split6_o_du_low_metrics_collector_impl&& other) noexcept
{
  // Stop others timer to put it into known state.
  other.stopped.store(true, std::memory_order_relaxed);
  other.timer.stop();

  // Copy/move members.
  timer         = std::move(other.timer);
  report_period = other.report_period;
  notifier      = other.notifier;
  odu_low       = other.odu_low;
  ru            = other.ru;

  // Set to nullptr other.
  other.notifier = nullptr;
  other.odu_low  = nullptr;
  other.ru       = nullptr;

  if (!notifier || report_period.count() == 0) {
    return *this;
  }

  // Configure and start timer again.
  timer.set(report_period, [this]() { collect_metrics(); });

  stopped.store(false, std::memory_order_relaxed);
  timer.run();

  return *this;
}

split6_o_du_low_metrics_collector_impl::~split6_o_du_low_metrics_collector_impl()
{
  stopped.store(true, std::memory_order_relaxed);
  timer.stop();
}

void split6_o_du_low_metrics_collector_impl::collect_metrics()
{
  if (stopped.load(std::memory_order_relaxed)) {
    return;
  }

  split6_flexible_o_du_low_metrics metrics;
  odu_low->collect_metrics(metrics.du_low);
  ru->collect_metrics(metrics.ru);

  notifier->on_new_metrics(metrics);

  timer.run();
}
