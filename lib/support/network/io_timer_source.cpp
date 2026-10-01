// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/io/io_timer_source.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/error_handling.h"
#include "ocudu/support/io/io_broker.h"
#include "ocudu/support/timers.h"
#include <sys/timerfd.h>

using namespace ocudu;

static unique_fd create_timer_fd(std::chrono::milliseconds tick_period)
{
  using namespace std::chrono;

  auto timer_fd = unique_fd{::timerfd_create(CLOCK_MONOTONIC, 0)};
  report_fatal_error_if_not(timer_fd.is_open(), "Failed to create timer source (errno={})", ::strerror(errno));

  auto         tsecs     = duration_cast<seconds>(tick_period);
  auto         tnsecs    = duration_cast<nanoseconds>(tick_period) - duration_cast<nanoseconds>(tsecs);
  ::timespec   period    = {tsecs.count(), tnsecs.count()};
  ::itimerspec timerspec = {period, period};
  ::timerfd_settime(timer_fd.value(), 0, &timerspec, nullptr);

  return timer_fd;
}

io_timer_source::io_timer_source(timer_manager&            tick_sink_,
                                 io_broker&                broker_,
                                 task_executor&            executor,
                                 std::chrono::milliseconds tick_period_,
                                 bool                      auto_start) :
  tick_period(tick_period_),
  tick_sink(tick_sink_),
  broker(broker_),
  tick_exec(executor),
  logger(ocudulog::fetch_basic_logger("IO-EPOLL"))
{
  if (auto_start) {
    running.store(true, std::memory_order_relaxed);
    create_subscriber(shutdown_flag.get_token());
  }
}

io_timer_source::~io_timer_source()
{
  request_stop();
  // The dtor of stop_flag will block until all tasks using the token have completed.
}

void io_timer_source::resume()
{
  auto prev = running.exchange(true, std::memory_order_acq_rel);
  if (prev) {
    // Already started. No need to dispatch task. Early exit.
    return;
  }

  // Dispatch task to start ticking.
  // Note: Token is used to protect the asynchronous callback from outliving this object.
  while (not tick_exec.defer(
      [this, token = shutdown_flag.get_token()]() mutable { create_subscriber(std::move(token)); })) {
    // We cannot allow the command to be lost. Retry until we succeed.
    std::this_thread::sleep_for(std::chrono::milliseconds{1});
  }
}

void io_timer_source::request_stop()
{
  // Request in a non-blocking fashion, the stop of the timer.
  running.store(false, std::memory_order_release);
}

void io_timer_source::create_subscriber(scoped_sync_token token)
{
  // Note: Called inside the ticking executor, except for in the ctor.

  if (io_sub.registered()) {
    // Already created.
    return;
  }

  if (not running.load(std::memory_order_acquire)) {
    // The state has changed in the meantime and the subscriber wasn't created.
    return;
  }

  logger.info("Starting IO timer ticking source...");
  auto      fd     = create_timer_fd(tick_period);
  const int raw_fd = fd.value();
  io_sub = broker.register_fd(std::move(fd), tick_exec, [this, raw_fd, token]() mutable { read_time(raw_fd, token); });
  report_fatal_error_if_not(io_sub.registered(), "Failed to create timer source");
}

void io_timer_source::destroy_subscriber(scoped_sync_token& token)
{
  if (not io_sub.registered()) {
    // Already destroyed.
    return;
  }
  logger.info("Stopping IO timer ticking source...");
  // Unregister the fd from the broker.
  // Note: Destroying the subscriber will destroy the token saved in its callback capture in the io_broker backend.
  // We want to destroy the token here though to have deterministic time of destruction, so we move the token out first.
  auto token_tmp = std::move(token);
  io_sub.reset();
  logger.info("IO timer source stopped.");
  token_tmp.reset();
}

void io_timer_source::read_time(int raw_fd, scoped_sync_token& token)
{
  // Note: Called inside the ticking executor.

  if (not running.load(std::memory_order_acquire)) {
    // Destroy subscriber and signal the completion of the stop by resetting the token.
    destroy_subscriber(token);
    // Note: Do not touch any variable here as the ~io_timer_source() might be running concurrently.
    return;
  }

  uint64_t nof_expirations = 0;
  int      n               = ::read(raw_fd, &nof_expirations, sizeof(nof_expirations));
  if (n < 0) {
    logger.error("Failed to read timerfd (errno={})", ::strerror(errno));
    return;
  }
  if (n == 0) {
    logger.warning("Timerfd read returned 0");
    return;
  }

  while (nof_expirations-- > 0) {
    // Tick timers.
    tick_sink.tick();
  }
}
