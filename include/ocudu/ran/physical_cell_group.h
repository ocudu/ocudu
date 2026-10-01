// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/rnti.h"
#include <optional>

namespace ocudu {

/// Options for \c pdsch-HARQ-ACK-Codebook, in \c PhysicalCellGroupConfig, TS 38.331.
enum class pdsch_harq_ack_codebook { semistatic, dynamic };

/// \c PhysicalCellGroupConfig, TS 38.331.
struct physical_cell_group_config {
  /// Instead of using an std::optional<>, \c not_set stands for not present.
  enum class x_scale_opt { not_set, db0, db6 };

  /// Optional field with 1 possible value. False = not present.
  bool harq_ack_spatial_bundling_pucch;
  /// Optional field with 1 possible value. False = not present.
  bool harq_ack_spatial_bundling_pusch;
  /// Values {-30,...,33}.
  std::optional<int>      p_nr_fr1;
  pdsch_harq_ack_codebook pdsch_harq_codebook;
  std::optional<rnti_t>   tpc_srs_rnti;
  std::optional<rnti_t>   tpc_pucch_rnti;
  std::optional<rnti_t>   tpc_pusch_rnti;
  std::optional<rnti_t>   sp_csi_rnti;
  std::optional<rnti_t>   cs_rnti;
  std::optional<rnti_t>   mcs_rnti;
  /// Values {-30,...,33}.
  std::optional<int> p_ue_fr1;
  x_scale_opt        x_scale;
  /// Values {1,...,15}.
  std::optional<unsigned> pdcch_blind_detection;
};

} // namespace ocudu
