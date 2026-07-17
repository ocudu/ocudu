// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

/// \brief Geographic reference location, as carried by the LPP ReferenceLocation-r17 IE (TS 37.355 sec. 5.1).
struct reference_location {
  double latitude;  ///< degrees [-90..90]
  double longitude; ///< degrees [-180..180]
};

} // namespace ocudu
