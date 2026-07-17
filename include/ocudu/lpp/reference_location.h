// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ran/reference_location.h"

namespace ocudu {
namespace lpp {

/// \brief Packs a reference location into the 6-byte LPP ReferenceLocation-r17 IE (TS 37.355 sec. 5.1).
///
/// The location is encoded as a TS 23.032 Ellipsoid-Point, as used, among others, by the referenceLocation fields of
/// NTN-NeighbourCellInfo-r18 (TS 38.331) and the condEventD1-r17 / condEventD2-r18 measurement triggers.
/// Bit layout: [1-bit N/S | 23-bit latitude | 24-bit longitude].
///
/// \param[in] loc Reference location. Out-of-range coordinates are clamped to the encodable range.
/// \return A 6-byte buffer holding the packed Ellipsoid-Point.
byte_buffer pack_reference_location(const reference_location& loc);

} // namespace lpp
} // namespace ocudu
