// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <memory>

namespace ocudu {

/// Notifier of objects of type T. The interface uses a builder type to minimize the amount of copying.
template <typename T>
class zero_copy_notifier
{
protected:
  /// Deleter of builder that commits/notifies the filled object, when the builder goes out of scope.
  struct committer {
    zero_copy_notifier* parent;

    void operator()(T* ptr)
    {
      if (ptr != nullptr) {
        parent->commit(*ptr);
      }
    }
  };

public:
  /// Builder used to fill an object of type T and later commit it.
  using builder = std::unique_ptr<T, committer>;

  virtual ~zero_copy_notifier() = default;

  /// \brief This method will be called to start preparing a new object to notify.
  builder get_builder() { return builder{&get_next(), committer{this}}; }

private:
  /// Internal method that retrieves the next pooled object of type T to be filled by the caller to get_builder.
  virtual T& get_next() = 0;

  /// \brief Method called when the object is filled and it is ready to be committed. This method will be automatically
  /// called when the builder goes out of scope.
  virtual void commit(T& ptr) = 0;
};

} // namespace ocudu
