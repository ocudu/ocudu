// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cause/e1ap_cause.h"
#include "ocudu/ran/cause/ngap_cause.h"

namespace ocudu {

/// \brief Converts an E1AP cause to an NGAP cause.
ngap_cause_t e1ap_to_ngap_cause(e1ap_cause_t e1ap_cause);

} // namespace ocudu
