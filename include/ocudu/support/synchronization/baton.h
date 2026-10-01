// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <condition_variable>
#include <mutex>

namespace ocudu {

/// \brief Baton synchronization primitive.
///
/// It is used to synchronize two threads, where one waits for the other to finish.
class baton
{
public:
  void post()
  {
    std::lock_guard<std::mutex> lock(mutex);
    is_set = true;
    cvar.notify_one();
  }

  void wait()
  {
    std::unique_lock<std::mutex> lock(mutex);
    cvar.wait(lock, [this] { return is_set; });
  }

private:
  std::mutex              mutex;
  std::condition_variable cvar;
  bool                    is_set = false;
};

/// RAII helper for baton synchronization primitive.
class scoped_baton_sender
{
  struct deleter {
    void operator()(baton* p)
    {
      if (p != nullptr) {
        p->post();
      }
    }
  };

public:
  explicit scoped_baton_sender(baton& parent_) : parent(&parent_) {}

  void post() { parent = nullptr; }

private:
  /// Use of unique_ptr for RAII
  std::unique_ptr<baton, deleter> parent;
};

} // namespace ocudu
