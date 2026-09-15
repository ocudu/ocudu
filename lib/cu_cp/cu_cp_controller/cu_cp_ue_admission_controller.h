// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu::ocucp {

class cu_cp_ue_admission_controller
{
public:
  virtual ~cu_cp_ue_admission_controller() = default;

  /// Determines whether the CU-CP is in a condition to accept new UEs.
  virtual bool request_ue_setup() const = 0;
};

} // namespace ocudu::ocucp
