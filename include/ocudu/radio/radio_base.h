// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {

/// Describes the base interface for radio classes.
class radio_base
{
protected:
  /// Allow default constructor for child.
  radio_base() = default;

public:
  /// Provides default destructor.
  virtual ~radio_base() = default;

  /// Forbid copy constructor.
  radio_base(const radio_base& other) = delete;

  /// Forbid move constructor.
  radio_base(radio_base&& other) = delete;

  /// Forbid copy assignment operator.
  radio_base& operator=(const radio_base&) = delete;

  /// Forbid move assignment operator.
  radio_base& operator=(radio_base&&) = delete;
};
} // namespace ocudu
