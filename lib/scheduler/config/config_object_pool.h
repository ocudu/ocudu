// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "config_ptr.h"
#include <algorithm>
#include <deque>

namespace ocudu {

/// Pool of configuration objects that ensures uniqueness of the stored objects.
template <typename T>
class config_object_pool
{
public:
  config_object_pool()                                     = default;
  config_object_pool(config_object_pool&&)                 = default;
  config_object_pool(const config_object_pool&)            = delete;
  config_object_pool& operator=(config_object_pool&&)      = delete;
  config_object_pool& operator=(const config_object_pool&) = delete;

  /// \brief If an object exists in the pool that it is equal to the provided \c obj, then the existing object is
  /// returned. Otherwise, a new entry is added to the pool and returned.
  config_ptr<T> create(const T& obj)
  {
    for (const T& existing_obj : objs) {
      if (existing_obj == obj) {
        return config_ptr<T>{existing_obj};
      }
    }
    objs.push_back(obj);
    return config_ptr<T>{objs.back()};
  }

  template <typename Equals>
  config_ptr<T> find_if(const Equals& eq) const
  {
    auto it = std::find_if(objs.begin(), objs.end(), eq);
    if (it == objs.end()) {
      return config_ptr<T>{};
    }
    return config_ptr<T>{*it};
  }

private:
  // Note: We use a deque to maintain pointer validity.
  std::deque<T> objs;
};

} // namespace ocudu
