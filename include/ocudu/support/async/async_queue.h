// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/ring_buffer.h"
#include "ocudu/support/async/coroutine.h"

namespace ocudu {

template <typename T>
class async_queue
{
public:
  async_queue(size_t queue_size) : queue(queue_size) {}

  async_queue(const async_queue&)            = delete;
  async_queue& operator=(const async_queue&) = delete;

  /// Pushing interface.
  bool try_push(const T& t)
  {
    if (queue.try_push(t)) {
      notify_one_awaiter();
      return true;
    }
    return false;
  }
  bool try_push(T&& t)
  {
    if (queue.try_push(std::move(t))) {
      notify_one_awaiter();
      return true;
    }
    return false;
  }

  void clear() { queue.clear(); }

  size_t size() const { return queue.size(); }

  struct awaiter_type {
    explicit awaiter_type(async_queue* parent_) : parent(parent_) {}

    bool await_ready() { return not parent->queue.empty(); }

    void await_suspend(coro_handle<> ch)
    {
      ocudu_assert(this->next == nullptr, "Trying to suspend already suspended coroutine");
      suspended_handle = ch;
      // Enqueue awaiter.
      if (parent->last == nullptr) {
        // Queue of awaiters is empty.
        parent->last  = this;
        parent->front = this;
      } else {
        parent->last->next = this;
        parent->last       = this;
      }
    }

    T await_resume()
    {
      ocudu_sanity_check(not parent->queue.empty(), "Callback being resumed but queue is still empty");
      T ret = std::move(parent->queue.top());
      parent->queue.pop();
      return ret;
    }

    async_queue*  parent;
    awaiter_type* next = nullptr;
    coro_handle<> suspended_handle;
  };

  awaiter_type get_awaiter() { return awaiter_type{this}; }

private:
  void notify_one_awaiter()
  {
    if (front != nullptr) {
      awaiter_type* to_resume = front;
      if (front == last) {
        front = nullptr;
        last  = nullptr;
      } else {
        front = front->next;
      }
      to_resume->suspended_handle.resume();
    }
  }

  awaiter_type*  front = nullptr;
  awaiter_type*  last  = nullptr;
  ring_buffer<T> queue;
};

} // namespace ocudu
