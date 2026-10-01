// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/csi_report/csi_report_configuration.h"
#include "ocudu/ran/csi_report/csi_report_packed.h"
#include "ocudu/ran/precoding/precoding_codebook_type2_helpers.h"
#include <array>
#include <utility>

namespace ocudu {
namespace test_helpers {

/// \brief Values that a packed Type II CSI report carries.
///
/// Every coefficient that the report can carry is reported with the maximum wideband amplitude, so the number of
/// non-zero amplitudes takes its maximum and the payload size does not depend on the chosen values.
struct typeii_csi_report_values {
  /// Rank indicator. The Type II codebook reports one or two layers.
  unsigned ri = 1;
  /// Wideband Channel Quality Indicator of the first transport block.
  unsigned wideband_cqi = 10;
  /// Beam rotation index \f$i_{1,1}\f$.
  unsigned i_1_1 = 0;
  /// Beam group index \f$i_{1,2}\f$.
  unsigned i_1_2 = 0;
  /// Strongest coefficient index \f$i_{1,3}\f$, one entry per layer.
  std::array<unsigned, max_nof_typeII_layers> i_1_3 = {};
  /// Phase index \f$i_{2,1}\f$ of every reported coefficient, one entry per layer.
  std::array<unsigned, max_nof_typeII_layers> phase = {};
};

/// \brief Packs a Type II CSI report on PUSCH, following TS38.212 Tables 6.3.2.1.2-3 and 6.3.2.1.2-4.
///
/// \param[in] config Report configuration. Its codebook must be Type II and its quantities must contain the PMI.
/// \param[in] values Values that the report carries.
/// \return The CSI Part 1 and CSI Part 2 payloads.
std::pair<csi_report_packed, csi_report_packed> pack_typeii_csi_report_pusch(const csi_report_configuration& config,
                                                                             const typeii_csi_report_values& values);

} // namespace test_helpers
} // namespace ocudu
