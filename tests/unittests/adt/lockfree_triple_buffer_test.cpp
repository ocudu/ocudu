// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/lockfree_triple_buffer.h"
#include <gtest/gtest.h>
#include <thread>

using namespace ocudu;

TEST(lockfree_triple_buffer, basic)
{
  lockfree_triple_buffer<int> trio{};

  constexpr unsigned N               = 100;
  auto               producer_thread = std::thread([&]() {
    for (unsigned i = 1; i <= N; ++i) {
      trio.write_and_commit(i);
    }
  });

  int prev_read = 0;
  while (prev_read != N) {
    const auto& val = trio.read();
    ASSERT_GE(val, prev_read);
    prev_read = val;
  }
  producer_thread.join();

  const auto& val = trio.read();
  ASSERT_EQ(val, N);
}
