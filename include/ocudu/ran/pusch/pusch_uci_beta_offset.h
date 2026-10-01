// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

/// \brief Maximum CSI payload size, in bits, that selects the first beta offset index.
///
/// As per Section 9.3, TS 38.213, a CSI Part 1 or CSI Part 2 payload larger than this value selects the second beta
/// offset index, i.e. \e betaOffsetCSI-Part1-Index2 or \e betaOffsetCSI-Part2-Index2.
constexpr unsigned MAX_NOF_CSI_BITS_BETA_OFFSET_IDX_1 = 11;

/// \brief Maps the integer \c beta_offset value for HARQ-ACK reporting into the corresponding float value.
///
/// The integer \c beta_offset value is passed by the \c PUSCH-Config, TS 38.331, and the mapping into the corresponding
/// float value is determined as per Table 9.3-1, TS 38.213.
/// \param[in] beta_uint_val is the integer value as per \c BetaOffsets, TS 38.331.
/// \remark The caller must ensure \c beta_uint_val < 16, so as not to incur in reserved values, as per Table 9.3-1,
/// TS 38.213.
/// \return The corresponding float value as per Table 9.3-1, TS 38.213.
float beta_harq_ack_to_float(unsigned beta_uint_val);

/// \brief Maps the integer \c beta_offset value for CSI reporting into the corresponding float value.
///
/// The integer \c beta_offset value is passed by the \c PUSCH-Config, TS 38.331, and the mapping into the corresponding
/// float value is determined as per Table 9.3-2, TS 38.213.
/// \param[in] beta_uint_val is the integer value as per \c BetaOffsets, TS 38.331.
/// \remark The caller must ensure \c beta_uint_val < 19, so as not to incur in reserved values, as per Table 9.3-2,
/// TS 38.213.
/// \return The corresponding float value as per Table 9.3-2, TS 38.213.
float beta_csi_to_float(unsigned beta_uint_val);

} // namespace ocudu
