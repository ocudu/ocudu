// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/async/async_event_source.h"
#include "ocudu/support/async/eager_async_task.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace ocudu;

class async_single_event_observer_test : public ::testing::Test
{
protected:
  timer_manager                    timers{1};
  manual_task_worker               worker{64};
  async_event_source<int>          event_source{timer_factory{timers, worker}};
  async_single_event_observer<int> tr;
};

TEST_F(async_single_event_observer_test, when_transaction_sink_is_created_then_it_starts_unregistered)
{
  ASSERT_FALSE(tr.connected());
  ASSERT_FALSE(tr.complete());
}

TEST_F(async_single_event_observer_test, when_no_events_have_been_set_then_sink_is_not_complete)
{
  tr.subscribe_to(event_source);
  ASSERT_FALSE(tr.complete());
}

TEST_F(async_single_event_observer_test, when_event_source_is_triggered_then_sink_receives_result)
{
  tr.subscribe_to(event_source);

  event_source.set(2);
  ASSERT_TRUE(tr.complete());
  ASSERT_EQ(tr.result(), 2);
}

TEST_F(async_single_event_observer_test, when_sink_subscribes_multiple_times_then_previous_result_auto_resets)
{
  tr.subscribe_to(event_source);
  event_source.set(2);

  tr.subscribe_to(event_source);
  event_source.set(3);

  ASSERT_TRUE(tr.complete());
  ASSERT_EQ(tr.result(), 3);
}

#ifdef ASSERTS_ENABLED
TEST_F(async_single_event_observer_test, only_one_sink_per_event_source_allowed)
{
  async_single_event_observer<int> tr2;
  tr.subscribe_to(event_source);

  eager_async_task<int> t = launch_async([this](coro_context<eager_async_task<int>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_AWAIT(tr);
    CORO_RETURN(tr.result());
  });

  ASSERT_FALSE(t.ready());
  ASSERT_DEATH(tr2.subscribe_to(event_source), ".*");
}
#endif
