// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {
namespace app_services {

class metrics_properties;

/// General purpouse metrics set.
class metrics_set
{
public:
  /// Default destructor.
  virtual ~metrics_set() = default;

  /// Returns this metrics properties.
  virtual const metrics_properties& get_properties() const = 0;
};

} // namespace app_services
} // namespace ocudu
