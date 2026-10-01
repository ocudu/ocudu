// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/uci/uci_part2_size_description.h"
#include "ocudu/ran/uci/uci_payload_type.h"
#include "ocudu/support/units.h"

namespace ocudu {

/// \brief Calculates the UCI part 2 from UCI part 1.
/// \param[in] part1 UCI part 1 decoded data.
/// \param[in] descr UCI part 1 parameters correspondence to UCI part 2 size.
/// \return The size of UCI part 2 payload.
units::bits uci_part2_get_size(const uci_payload_type& part1, const uci_part2_size_description& descr);

} // namespace ocudu
