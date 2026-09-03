// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/executors/unique_thread.h"
#include "ocudu/support/test_utils.h"
#include "fmt/std.h"
#include <gtest/gtest.h>
#include <mutex>
#include <unistd.h>

using namespace ocudu;

TEST(unique_thread_test, uninit_unique_thread)
{
  test_delimit_logger delimit{"Uninitialized unique thread"};

  unique_thread t;
  ASSERT_TRUE(not t.running());
  ASSERT_EQ(std::string(""), t.get_name());
  ASSERT_NE(std::string(this_thread_name()), t.get_name());

  t.print_priority();

  // Join is a NO-OP.
  t.join();
  ASSERT_TRUE(not t.running());
}

TEST(unique_thread_test, init_unique_thread)
{
  test_delimit_logger delimit{"Initialized unique thread"};

  unique_thread t;
  ASSERT_TRUE(not t.running());

  // Action: start thread.
  std::atomic<bool> running{true};
  std::string       t_name;
  t = unique_thread("T1", [&running]() {
    report_fatal_error_if_not((std::string("T1")) == (this_thread_name()), "std::string(\"T1\") != this_thread_name()");

    print_this_thread_priority();

    fmt::print("Thread [{}:{}] completed.\n", this_thread_name(), std::this_thread::get_id());
    running = false;
  });
  ASSERT_EQ(std::string(t.get_name()), "T1");
  ASSERT_NE(std::string(t.get_name()), this_thread_name());

  while (running) {
    ::usleep(100);
  }
  fmt::print("Main Thread [{}:{}] completed.\n", this_thread_name(), std::this_thread::get_id());
}

// Note: The thread priority is only set if this thread runs in sudo mode.
TEST(unique_thread_test, init_unique_thread_prio)
{
  test_delimit_logger delimit{"Initialized unique thread with priority"};

  unique_thread t("T1", ocudu::os_thread_realtime_priority::max(), []() { print_this_thread_priority(); });
}
