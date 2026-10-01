// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief LDPC constants, utilities and graphs.
///
/// This file defines types, constants and classes to represent an LDPC Tanner graph
/// according to TS38.212 Section 5.3.2.
#pragma once

#include "ocudu/ran/sch/ldpc_base_graph.h"
#include "ocudu/support/math/math_utils.h"
#include "ocudu/support/units.h"
#include <array>

namespace ocudu {

/// LDPC constants and helper functions.
namespace ldpc {

/// Available lifting sizes.
enum lifting_size_t {
  LS2   = 2,
  LS3   = 3,
  LS4   = 4,
  LS5   = 5,
  LS6   = 6,
  LS7   = 7,
  LS8   = 8,
  LS9   = 9,
  LS10  = 10,
  LS11  = 11,
  LS12  = 12,
  LS13  = 13,
  LS14  = 14,
  LS15  = 15,
  LS16  = 16,
  LS18  = 18,
  LS20  = 20,
  LS22  = 22,
  LS24  = 24,
  LS26  = 26,
  LS28  = 28,
  LS30  = 30,
  LS32  = 32,
  LS36  = 36,
  LS40  = 40,
  LS44  = 44,
  LS48  = 48,
  LS52  = 52,
  LS56  = 56,
  LS60  = 60,
  LS64  = 64,
  LS72  = 72,
  LS80  = 80,
  LS88  = 88,
  LS96  = 96,
  LS104 = 104,
  LS112 = 112,
  LS120 = 120,
  LS128 = 128,
  LS144 = 144,
  LS160 = 160,
  LS176 = 176,
  LS192 = 192,
  LS208 = 208,
  LS224 = 224,
  LS240 = 240,
  LS256 = 256,
  LS288 = 288,
  LS320 = 320,
  LS352 = 352,
  LS384 = 384
};
/// Total number of lifting sizes.
constexpr unsigned NOF_LIFTING_SIZES = 51;
/// All lifting sizes are assigned an index from 0 to 7 (see TS38.212 Table 5.3.2-1).
constexpr unsigned NOF_LIFTING_INDICES = 8;

/// Array of lifting sizes, for iterations.
constexpr std::array<lifting_size_t, NOF_LIFTING_SIZES> all_lifting_sizes = {
    LS2,   LS3,   LS4,   LS5,   LS6,   LS7,   LS8,   LS9,   LS10,  LS11,  LS12,  LS13,  LS14,
    LS15,  LS16,  LS18,  LS20,  LS22,  LS24,  LS26,  LS28,  LS30,  LS32,  LS36,  LS40,  LS44,
    LS48,  LS52,  LS56,  LS60,  LS64,  LS72,  LS80,  LS88,  LS96,  LS104, LS112, LS120, LS128,
    LS144, LS160, LS176, LS192, LS208, LS224, LS240, LS256, LS288, LS320, LS352, LS384};

/// Maximum LDPC message size in bits.
constexpr unsigned MAX_MESSAGE_SIZE = all_lifting_sizes.back() * 22;

/// Maximum LDPC encoded codeblock size in bits.
constexpr unsigned MAX_CODEBLOCK_SIZE = all_lifting_sizes.back() * 66;

/// \brief Maximum LDPC rate-matched codeblock size in bits.
///
/// The minimum code rate (after rate matching) is \f$30 / 1024\f$, as per TS38.214 Table 5.1.3.1-3. Therefore, the
/// maximum rate-matched codeblock length can be computed as
/// \f[
/// \textup{MAX_MESSAGE_SIZE} \times \lceil 1024 / 30 \rceil = \textup{MAX_MESSAGE_SIZE} \times 35.
/// \f]
constexpr unsigned MAX_CODEBLOCK_RM_SIZE = MAX_MESSAGE_SIZE * 35;

/// Computes the codeblock size after the LDPC encoding.
constexpr units::bits compute_full_codeblock_size(ldpc_base_graph_type base_graph, units::bits codeblock_size)
{
  // BG1 has rate 1/3 and BG2 has rate 1/5.
  constexpr unsigned INVERSE_BG1_RATE = 3;
  constexpr unsigned INVERSE_BG2_RATE = 5;
  unsigned           inverse_rate     = (base_graph == ldpc_base_graph_type::BG1) ? INVERSE_BG1_RATE : INVERSE_BG2_RATE;

  return codeblock_size * inverse_rate;
}

/// \brief Computes the parameter \f$N_{ref}\f$ according to TS38.212 Section 5.4.2.1.
///
/// \param[in] tbs_lbrm       Transport block size for limited buffer rate match, parameter \f$TBS_{LBRM}\f$.
/// \param[in] nof_codeblocks Number of codeblocks for the transport block, parameter \f$C\f$.
/// \return The minimum between the computed \f$N_{ref}\f$ and the maximum full codeblock size.
constexpr units::bits compute_N_ref(units::bytes tbs_lbrm, unsigned nof_codeblocks)
{
  return std::min(units::bits(tbs_lbrm.to_bits().value() * 3 / (2 * nof_codeblocks)), units::bits(MAX_CODEBLOCK_SIZE));
}

} // namespace ldpc

} // namespace ocudu
