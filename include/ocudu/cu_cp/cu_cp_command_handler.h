// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ran/cu_cp_ue_context_release.h"
#include "ocudu/ran/pci.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/ran/rnti.h"
#include <chrono>
#include <optional>

namespace ocudu::ocucp {

class cu_cp_mobility_command_handler
{
public:
  virtual ~cu_cp_mobility_command_handler() = default;

  /// \brief Trigger handover of a given UE to a target cell.
  ///
  /// The UE is uniquely identified in the CU-CP through the serving Cell PCI
  /// and RNTI. The target is identified through the Target PCI.
  virtual void
  trigger_handover(pci_t source_pci, rnti_t rnti, pci_t target_pci, plmn_identity target_plmn, tac_t target_tac) = 0;

  /// \brief Trigger Conditional Handover (CHO) with one or more target cells.
  ///
  /// Prepares CHO candidate cell configurations and sends them to the UE.
  /// The UE is uniquely identified in the CU-CP through the serving Cell PCI and RNTI.
  ///
  /// \param[in] source_pci Serving cell PCI.
  /// \param[in] rnti UE RNTI on the serving cell.
  /// \param[in] target_pcis Target cell PCIs (1-8 candidates supported per 3GPP).
  /// \param[in] timeout Maximum time to wait for CHO completion.
  /// \param[in] t1_thres_override Optional runtime override for the T1 conditional event threshold.
  virtual void trigger_conditional_handover(
      pci_t                                                source_pci,
      rnti_t                                               rnti,
      span<const pci_t>                                    target_pcis,
      std::chrono::milliseconds                            timeout,
      std::optional<std::chrono::system_clock::time_point> t1_thres_override = std::nullopt) = 0;
};

/// Handler for external UE release commands to the CU-CP.
class cu_cp_ue_release_command_handler
{
public:
  virtual ~cu_cp_ue_release_command_handler() = default;

  /// \brief Trigger RRC Release for a UE, with optional NR redirection.
  ///
  /// The UE is released to RRC_IDLE. If redirect_info is set, the RRCRelease message will
  /// include redirectedCarrierInfo pointing to the given NR carrier (TS 38.331 Sec. 5.3.8.3).
  ///
  /// \param[in] source_pci    Serving cell PCI.
  /// \param[in] rnti          UE RNTI on the serving cell.
  /// \param[in] redirect_info Optional NR carrier to redirect the UE to on entry to RRC_IDLE.
  virtual void trigger_release(pci_t                                         source_pci,
                               rnti_t                                        rnti,
                               std::optional<cu_cp_release_redirect_nr_info> redirect_info = std::nullopt) = 0;
};

/// Handler for external commands to the CU-CP.
class cu_cp_command_handler
{
public:
  virtual ~cu_cp_command_handler() = default;

  /// Get handler for mobility commands.
  virtual cu_cp_mobility_command_handler& get_mobility_command_handler() = 0;

  /// Get handler for UE release commands.
  virtual cu_cp_ue_release_command_handler& get_ue_release_command_handler() = 0;
};

} // namespace ocudu::ocucp
