// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_integer.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/e1ap/common/e1ap_types.h"
#include "ocudu/ran/cu_cp_types.h"
#include <chrono>
#include <optional>

namespace ocudu::ocucp {

/// Granularity of the CHO window duration field. Each integer step = 100 ms (TS 38.331 Section 6.3.2).
constexpr std::chrono::milliseconds cho_window_duration_step{100};

/// Maximum CHO window duration: INTEGER (1..6000) steps x 100 ms = 600 s (TS 38.331 Section 6.3.2).
constexpr std::chrono::milliseconds cho_window_duration_max{6000 * 100};

/// Conditional Reconfiguration ID used by CHO procedures. Range is 1..8.
using cond_recfg_id_t = bounded_integer<uint8_t, 1, 8>;

/// \brief Single target candidate for CHO orchestration.
struct cu_cp_cho_target_candidate {
  pci_t               pci = INVALID_PCI;
  nr_cell_global_id_t cgi;
  cu_cp_du_index_t    du_index = cu_cp_du_index_t::invalid; ///< Invalid for inter-CU (Xn) candidates.
  /// Xn-C peer index for inter-CU candidates. When set, the target is served by a remote CU-CP.
  std::optional<xnc_peer_index_t> xnc_index;
};

/// \brief Request for intra-CU CHO orchestration.
struct cu_cp_intra_cu_cho_request {
  cu_cp_ue_index_t                        source_ue_index = cu_cp_ue_index_t::invalid;
  cu_cp_du_index_t                        source_du_index = cu_cp_du_index_t::invalid;
  std::vector<cu_cp_cho_target_candidate> targets;
  std::chrono::milliseconds               timeout = std::chrono::milliseconds{10000};
  /// Runtime override for the T1 conditional event threshold. Replaces the configured value when set.
  std::optional<std::chrono::system_clock::time_point> t1_thres_override;
};

/// \brief Response from intra-CU CHO orchestration.
struct cu_cp_intra_cu_cho_response {
  bool success = false;
};

/// \brief Parameters for a single CHO candidate preparation request.
struct cu_cp_cho_preparation_request {
  cond_recfg_id_t cond_recfg_id = 1; ///< CHO conditional reconfiguration ID (valid range 1..8).
};

/// \brief Result of a single CHO candidate preparation.
/// Only populated when cu_cp_intra_cu_handover_request::cho_preparation is set.
struct cu_cp_cho_preparation_result {
  cu_cp_ue_index_t target_ue_index = cu_cp_ue_index_t::invalid; ///< Target UE allocated/prepared for this candidate.
  byte_buffer      packed_rrc_recfg;   ///< Packed RRCReconfiguration for deferred CHO execution.
  unsigned         transaction_id = 0; ///< RRC transaction ID of packed_rrc_recfg.
  /// CU-UP bearer update payload collected during target preparation.
  std::optional<e1ap_ng_ran_bearer_context_mod_request> ng_ran_bearer_context_mod_request;
};

} // namespace ocudu::ocucp
