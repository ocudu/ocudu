// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/du_types.h"
#include "ocudu/support/executors/task_executor.h"

namespace ocudu::odu {

/// Interface used by the F1AP to access UE-dedicated task executors.
class f1ap_ue_executor_mapper
{
public:
  virtual ~f1ap_ue_executor_mapper() = default;

  /// \brief Returns the executor currently registered for F1-C Rx SDU handling for a given UE.
  /// \param ue_index Index of the UE.
  /// \return task executor of the UE with given UE Index.
  virtual task_executor& f1c_dl_sdu_executor(du_ue_index_t ue_index) = 0;
};

} // namespace ocudu::odu
