// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

template <typename T>
class config_object_pool;

/// \brief View of a configuration object.
///
/// The configuration object pool will try to ensure that each config_ptr<T> is unique within the whole cell group.
/// For this reason, config_ptr<T> objects can be compared via pointer comparison.
template <typename T>
class config_ptr
{
public:
  config_ptr() = default;
  /// Construct using pooled object reference.
  explicit config_ptr(const T& obj_) : obj(&obj_) {}

  /// If a configuration object is stored.
  bool has_value() const { return obj != nullptr; }

  /// Reset the stored object.
  void reset() { obj = nullptr; }

  /// Access stored config object.
  const T& operator*() const { return *obj; }
  const T& value() const { return *obj; }
  const T* operator->() const { return obj; }
  const T* get() const { return obj; }

  // Note: Comparison made via pointer comparison. The assumption is that the object is unique.
  bool operator==(const config_ptr& rhs) const { return obj == rhs.obj; }
  bool operator!=(const config_ptr& rhs) const { return not(*this == rhs); }

private:
  friend class config_object_pool<T>;

  const T* obj = nullptr;
};

} // namespace ocudu
