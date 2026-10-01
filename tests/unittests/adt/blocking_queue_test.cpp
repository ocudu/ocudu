// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/adt/blocking_queue.h"
#include "ocudu/support/test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;

TEST(blocking_queue_test, blocking_push_from_main_thread_and_pop_from_another_thread)
{
  unsigned            qsize = test_rng::uniform_int<unsigned>(1, 10000);
  blocking_queue<int> queue(qsize);

  std::atomic<int> count{0};
  std::thread      t([&queue, &count]() {
    while (true) {
      int val = queue.pop_blocking();
      if (queue.is_stopped()) {
        break;
      }
      ASSERT_EQ(val, count);
      count++;
    }
  });

  unsigned nof_objs = test_rng::uniform_int<unsigned>(1, 100000);
  for (unsigned i = 0; i < nof_objs; ++i) {
    queue.push_blocking(i);
  }

  while ((unsigned)count != nof_objs) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  queue.stop();
  t.join();
}

TEST(blocking_queue_test, blocking_push_from_another_thread_and_pop_in_main_thread)
{
  std::thread t;

  unsigned            qsize = test_rng::uniform_int<unsigned>(1, 10000);
  blocking_queue<int> queue(qsize);

  unsigned nof_objs = test_rng::uniform_int<unsigned>(1, 100000);
  t                 = std::thread([&queue, nof_objs]() {
    int count = 0;
    while ((unsigned)count != nof_objs and queue.push_blocking(count++)) {
    }
  });

  for (unsigned i = 0; i < nof_objs; ++i) {
    ASSERT_EQ(queue.pop_blocking(), (int)i);
  }

  ASSERT_TRUE(queue.empty());

  queue.stop();
  t.join();
}

TEST(blocking_queue_test, blocking_push_and_pop_in_batches_in_separate_threads)
{
  std::thread t;

  std::vector<int> vec(test_rng::uniform_int<unsigned>(1, 10000));
  for (unsigned i = 0; i != vec.size(); ++i) {
    vec[i] = i;
  }

  blocking_queue<int> queue(test_rng::uniform_int<unsigned>(100, 1000));
  t = std::thread([&queue, &vec]() {
    for (unsigned i = 0; i < vec.size();) {
      unsigned batch_size = test_rng::uniform_int<unsigned>(1, vec.size() - i);
      unsigned n          = queue.push_blocking(span<int>(vec).subspan(i, batch_size));
      EXPECT_LE(n, batch_size);
      i += n;
    }
  });

  std::vector<int> vec2(vec.size());
  for (unsigned i = 0; i < vec.size();) {
    unsigned batch_size = test_rng::uniform_int<unsigned>(1, vec.size() - i);
    unsigned n          = queue.pop_blocking(vec2.begin() + i, vec2.begin() + i + batch_size);
    i += n;
  }

  ASSERT_EQ(vec2, vec);

  queue.stop();
  t.join();
}
