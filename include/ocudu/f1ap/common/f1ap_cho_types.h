// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

/// \brief CHO trigger values for the inter-DU path (TS 38.473 Section 9.3.1.x).
/// Only cho_initiation and cho_replace are valid over the F1AP inter-DU interface.
enum class f1ap_cho_trigger { cho_initiation = 0, cho_replace };

} // namespace ocudu
