// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cause/f1ap_cause.h"
#include "ocudu/ran/cause/ngap_cause.h"
#include "ocudu/ran/cause/nrppa_cause.h"

namespace ocudu {

/// \brief Converts an F1AP cause to a NGAP cause.
ngap_cause_t f1ap_to_ngap_cause(f1ap_cause_t f1ap_cause);

/// \brief Converts an F1AP cause to a NRPPa cause.
nrppa_cause_t f1ap_to_nrppa_cause(f1ap_cause_t f1ap_cause);

} // namespace ocudu
