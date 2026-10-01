// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cause/e1ap_cause.h"
#include "ocudu/ran/cause/f1ap_cause.h"
#include "ocudu/ran/cause/ngap_cause.h"

namespace ocudu {

/// \brief Converts an NGAP cause to an F1AP cause.
f1ap_cause_t ngap_to_f1ap_cause(ngap_cause_t ngap_cause);

/// \brief Converts an NGAP cause to an E1AP cause.
e1ap_cause_t ngap_to_e1ap_cause(ngap_cause_t ngap_cause);

} // namespace ocudu
