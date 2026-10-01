// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/interval.h"
#include "ocudu/ran/resource_block.h"

namespace ocudu::pucch_constants {

namespace f0 {

/// Minimum number of OFDM symbols for a PUCCH Format 0.
constexpr unsigned MIN_NOF_SYMS = 1;
/// Maximum number of OFDM symbols for a PUCCH Format 0.
constexpr unsigned MAX_NOF_SYMS = 2;
/// Number of RBs for a PUCCH Format 0.
constexpr unsigned NOF_RBS = 1;
/// Number of initial cyclic shift values for a PUCCH Format 0.
constexpr unsigned NOF_ICS = 12;

/// Minimum number of HARQ-ACK feedback bits for a PUCCH Format 0.
constexpr unsigned MIN_NOF_HARQ_ACK_BITS = 0;
/// Maximum number of HARQ-ACK feedback bits for a PUCCH Format 0.
constexpr unsigned MAX_NOF_HARQ_ACK_BITS = 2;

} // namespace f0

namespace f1 {

/// Minimum number of OFDM symbols for a PUCCH Format 1.
constexpr unsigned MIN_NOF_SYMS = 4;
/// Maximum number of OFDM symbols for a PUCCH Format 1.
constexpr unsigned MAX_NOF_SYMS = 14;
/// Number of RBs for a PUCCH Format 1.
constexpr unsigned NOF_RBS = 1;
/// Number of initial cyclic shift values for a PUCCH Format 1.
constexpr unsigned NOF_ICS = 12;
/// Number of time domain OCC values for a PUCCH Format 1.
constexpr unsigned NOF_TD_OCC = 7;

/// Minimum number of symbols in a PUCCH Format 1 used for the UCI payload (i.e., excluding DM-RS symbols).
constexpr unsigned MIN_NOF_DATA_SYMS = 1;
/// Maximum number of symbols in a PUCCH Format 1 used for the UCI payload (i.e., excluding DM-RS symbols).
constexpr unsigned MAX_NOF_DATA_SYMS = 7;
/// Minimum number of HARQ-ACK feedback bits for a PUCCH Format 1.
constexpr unsigned MIN_NOF_HARQ_ACK_BITS = 0;
/// Maximum number of HARQ-ACK feedback bits for a PUCCH Format 1.
constexpr unsigned MAX_NOF_HARQ_ACK_BITS = 2;

} // namespace f1

namespace f2 {

/// Minimum number of OFDM symbols for a PUCCH Format 2.
constexpr unsigned MIN_NOF_SYMS = 1;
/// Maximum number of OFDM symbols for a PUCCH Format 2.
constexpr unsigned MAX_NOF_SYMS = 2;
/// Minimum number of RBs for a PUCCH Format 2.
constexpr unsigned MIN_NOF_RBS = 1;
/// Maximum number of RBs for a PUCCH Format 2.
constexpr unsigned MAX_NOF_RBS = 16;

/// Number of subcarriers per RB that are used for the UCI payload in a PUCCH Format 2.
constexpr unsigned NOF_DATA_SUBC_PER_RB = 8;
/// Maximum effective code rate for a PUCCH Format 2, as per Table 9.2.5.2-1, TS 38.213.
constexpr float MAX_CODE_RATE = 0.80F;
/// Minimum UCI payload size in bits for a PUCCH Format 2.
constexpr unsigned MIN_NOF_DATA_BITS = 3;
/// Maximum UCI payload size in bits for a PUCCH Format 2.
constexpr unsigned MAX_NOF_DATA_BITS = 1706;

} // namespace f2

namespace f3 {

/// Minimum number of OFDM symbols for a PUCCH Format 3.
constexpr unsigned MIN_NOF_SYMS = 4;
/// Maximum number of OFDM symbols for a PUCCH Format 3.
constexpr unsigned MAX_NOF_SYMS = 14;
/// Minimum number of RBs for a PUCCH Format 3.
constexpr unsigned MIN_NOF_RBS = 1;
/// Maximum number of RBs for a PUCCH Format 3.
constexpr unsigned MAX_NOF_RBS = 16;

/// Maximum effective code rate for a PUCCH Format 3, as per Table 9.2.5.2-1, TS 38.213.
constexpr float MAX_CODE_RATE = 0.80F;
/// Minimum UCI payload size in bits for a PUCCH Format 3.
constexpr unsigned MIN_NOF_DATA_BITS = 3;
/// Maximum UCI payload size in bits for a PUCCH Format 3.
constexpr unsigned MAX_NOF_DATA_BITS = 1706;

} // namespace f3

namespace f4 {

/// Minimum number of OFDM symbols for a PUCCH Format 4.
constexpr unsigned MIN_NOF_SYMS = 4;
/// Maximum number of OFDM symbols for a PUCCH Format 4.
constexpr unsigned MAX_NOF_SYMS = 14;
/// Number of RBs for a PUCCH Format 4.
constexpr unsigned NOF_RBS = 1;

/// Maximum effective code rate for a PUCCH Format 4, as per Table 9.2.5.2-1, TS 38.213.
constexpr float MAX_CODE_RATE = 0.80F;
/// Minimum UCI payload size in bits for a PUCCH Format 4.
constexpr unsigned MIN_NOF_DATA_BITS = 3;
/// Maximum UCI payload size in bits for a PUCCH Format 4.
constexpr unsigned MAX_NOF_DATA_BITS = 1706;

} // namespace f4

/// Number of layers for PUCCH transmission (PUCCH doesn't use spatial multiplexing).
constexpr unsigned NOF_LAYERS = 1;

/// PUCCH hopping identifier, parameter \f$n_{ID}\f$ range.
constexpr interval<unsigned, false> N_ID(0, 1024);

/// \brief Maximum number of resource elements used by PUCCH.
///
/// It corresponds to PUCCH Format 3 with a maximum number of OFDM symbols and RBs, and 2 DM-RS symbols.
constexpr unsigned MAX_NOF_RE = NOF_SUBCARRIERS_PER_RB * f3::MAX_NOF_RBS * (f3::MAX_NOF_SYMS - 2);

/// \brief Maximum number of LLRs corresponding to a PUCCH.
///
/// Derives from \ref MAX_NOF_RE assuming QPSK modulation.
constexpr unsigned MAX_NOF_LLR = MAX_NOF_RE * 2;

/// Number of common PUCCH resources in a cell.
/// \remark See TS 38.331, section 9.2.1, maximum value is given by the number of possible values of r_PUCCH, which
/// is 16.
constexpr size_t MAX_NOF_CELL_COMMON_PUCCH_RESOURCES = 16;

/// [Implementation-defined] Maximum number of dedicated PUCCH resources in a cell.
constexpr size_t MAX_NOF_CELL_DED_RESOURCES = 256;

/// [Implementation-defined] Maximum number of total PUCCH resources in a cell (common + dedicated).
constexpr size_t MAX_NOF_TOT_CELL_RESOURCES = MAX_NOF_CELL_COMMON_PUCCH_RESOURCES + MAX_NOF_CELL_DED_RESOURCES;

} // namespace ocudu::pucch_constants
