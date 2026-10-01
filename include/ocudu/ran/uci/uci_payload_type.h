// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/ran/uci/uci_constants.h"

namespace ocudu {

/// Generic UCI payload type.
using uci_payload_type = bounded_bitset<uci_constants::MAX_NOF_PAYLOAD_BITS>;

} // namespace ocudu
