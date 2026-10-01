// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/du_types.h"
#include "ocudu/support/executors/task_executor.h"

namespace ocudu {

/// \brief Mapper of task executors used by the MAC and MAC scheduler for low-latency tasks. The task
/// executors can be mapped based on cell index or type of task (slot indication vs others).
class mac_cell_executor_mapper
{
public:
  virtual ~mac_cell_executor_mapper() = default;

  /// \brief Default executor to handle MAC events for a given cell.
  virtual task_executor& mac_cell_executor(du_cell_index_t cell_index) = 0;

  /// \brief Executor to handle slot_indication events for a given cell. These events are of high-priority.
  virtual task_executor& slot_ind_executor(du_cell_index_t cell_index) = 0;
};

/// This interface is used to allow the MAC to choose between different UE-specific task executors.
class mac_ue_executor_mapper
{
public:
  virtual ~mac_ue_executor_mapper() = default;

  /// Method to signal the detection of a new UE and potentially change its executors based on its
  /// parameters (e.g. PCell).
  /// \param ue_index Index of the UE.
  /// \param pcell_index Primary Cell of the new UE.
  virtual void rebind_executors(du_ue_index_t ue_index, du_cell_index_t pcell_index) = 0;

  /// \brief Returns the executor for a given UE that handles control tasks.
  ///
  /// This executor should be used for tasks that affect the UE state and that we are not too frequent.
  /// \param ue_index Index of the UE.
  /// \return task executor of the UE with given UE Index.
  virtual task_executor& ctrl_executor(du_ue_index_t ue_index) = 0;

  /// Returns the executor currently registered for MAC Rx PDU handling for a given UE.
  /// \param ue_index Index of the UE.
  /// \return task executor of the UE with given UE Index.
  virtual task_executor& mac_ul_pdu_executor(du_ue_index_t ue_index) = 0;
};

} // namespace ocudu
