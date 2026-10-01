// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>
#include <optional>

namespace ocudu::ocuup {

/// \brief Public interface to the main CU-UP class
class cu_up_interface
{
public:
  virtual ~cu_up_interface() = default;

  virtual void start() = 0;

  /// \brief Stop the CU-UP. This call is blocking and only returns once all tasks in the CU-UP are completed.
  virtual void stop() = 0;
};

} // namespace ocudu::ocuup
