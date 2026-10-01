// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <string_view>

namespace ocudu {
namespace app_services {

/// Metrics properties.
class metrics_properties
{
public:
  /// Default destructor.
  virtual ~metrics_properties() = default;

  /// Returns the metrics name.
  virtual std::string_view name() const = 0;
};

} // namespace app_services
} // namespace ocudu
