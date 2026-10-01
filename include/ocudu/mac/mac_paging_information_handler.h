// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/paging_information.h"

namespace ocudu {

/// \brief Interface to handle paging information from the CU-CP.
class mac_paging_information_handler
{
public:
  virtual ~mac_paging_information_handler() = default;

  /// \brief Handles Paging information.
  ///
  /// \param msg Information of the paging message to schedule.
  virtual void handle_paging_information(const paging_information& msg) = 0;
};

} // namespace ocudu
