// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ue_security_manager/ue_security_manager_impl.h"
#include "ocudu/nrppa/nrppa.h"
#include "ocudu/ran/aggregate_maximum_bit_rate.h"
#include "ocudu/ran/cu_cp_types.h"

namespace ocudu::ocucp {

// Forward declarations.
class up_resource_manager;
class ue_task_scheduler;
class ue_security_manager;
class ngap_rrc_ue_notifier;

/// Common UE interface.
class cu_cp_ue_impl_interface
{
public:
  virtual ~cu_cp_ue_impl_interface() = default;

  /// \brief Get the UE index of the UE.
  virtual cu_cp_ue_index_t get_ue_index() const = 0;

  /// \brief Get the index of the DU where the UE is connected.
  virtual cu_cp_du_index_t get_du_index() const = 0;

  /// \brief Get the CU-UP index of the UE.
  virtual cu_cp_cu_up_index_t get_cu_up_index() const = 0;

  /// \brief Get the XN-C peer index of the UE.
  virtual xnc_peer_index_t get_xnc_peer_index() const = 0;

  /// \brief Get the UP resource manager of the UE.
  virtual up_resource_manager& get_up_resource_manager() = 0;

  /// \brief Get the task scheduler of the UE.
  virtual ue_task_scheduler& get_task_sched() = 0;

  /// \brief Get the security manager of the UE.
  virtual ue_security_manager& get_security_manager() = 0;

  /// \brief Get the RRC UE notifier of the UE.
  virtual ngap_rrc_ue_notifier& get_ngap_rrc_ue_notifier() = 0;

  /// \brief Get the measurement results of the UE.
  virtual std::optional<cell_measurement_positioning_info>& get_measurement_results() = 0;

  /// \brief Set UE AMBR.
  /// \param[in] ue_ambr The AMBR to set for the UE.
  virtual void set_ue_ambr(aggregate_maximum_bit_rate_t ue_ambr) = 0;

  /// \brief Get UE AMBR.
  virtual aggregate_maximum_bit_rate_t get_ue_ambr() const = 0;
};

} // namespace ocudu::ocucp
