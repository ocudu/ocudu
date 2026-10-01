// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {

/// Describes the base interface for baseband gateway interfaces.
class baseband_gateway_base
{
protected:
  /// Allow default constructor for child.
  baseband_gateway_base() = default;

public:
  /// Provides default destructor.
  virtual ~baseband_gateway_base() = default;

  /// Forbid copy constructor.
  baseband_gateway_base(const baseband_gateway_base& other) = delete;

  /// Forbid move constructor.
  baseband_gateway_base(baseband_gateway_base&& other) = delete;

  /// Forbid copy assignment operator.
  baseband_gateway_base& operator=(const baseband_gateway_base&) = delete;

  /// Forbid move assignment operator.
  baseband_gateway_base& operator=(baseband_gateway_base&&) = delete;
};

} // namespace ocudu
