// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/signal_handling.h"
#include "fmt/core.h"
#include <atomic>
#include <csignal>
#include <cstdio>
#include <unistd.h>

using namespace ocudu;

/// Termination timeout value measured in seconds.
static constexpr unsigned TERMINATION_TIMEOUT_S =
#ifndef TERM_TIMEOUT_S
    5;
#else
    TERM_TIMEOUT_S;
#endif

/// Handler called after the user interrupts the program.
static std::atomic<ocudu_signal_handler> interrupt_handler = nullptr;
/// Handler called just before forcing application exit.
static std::atomic<ocudu_signal_handler> cleanup_handler = nullptr;

static void signal_handler(int signal)
{
  switch (signal) {
    case SIGALRM:
      fmt::print(stderr, "Could not stop application after {} seconds. Forcing exit.\n", TERMINATION_TIMEOUT_S);
      if (auto handler = cleanup_handler.exchange(nullptr)) {
        handler(signal);
      }
      std::raise(SIGKILL);
      [[fallthrough]];
    default:
      // All other registered signals try to stop the application gracefully.
      // Call the user handler, if present, and remove it so that further signals are treated by the default handler.
      if (auto handler = interrupt_handler.exchange(nullptr)) {
        handler(signal);
      } else {
        return;
      }
      ::alarm(TERMINATION_TIMEOUT_S);
      break;
  }
}

void ocudu::register_interrupt_signal_handler(ocudu_signal_handler handler)
{
  interrupt_handler.store(handler);

  std::signal(SIGINT, signal_handler);
  std::signal(SIGTERM, signal_handler);
  std::signal(SIGHUP, signal_handler);
  std::signal(SIGALRM, signal_handler);
}

void ocudu::register_cleanup_signal_handler(ocudu_signal_handler handler)
{
  cleanup_handler.store(handler);
}
