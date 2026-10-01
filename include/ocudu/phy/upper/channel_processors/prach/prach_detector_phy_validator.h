// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Declaration of the validator of PRACH configurations at the PHY layer.

#pragma once

#include "ocudu/adt/expected.h"
#include "ocudu/ran/prach/prach_format_type.h"
#include "ocudu/ran/prach/prach_subcarrier_spacing.h"

namespace ocudu {

/// \brief Ensures the PRACH configuration is supported by the detector.
///
/// \param[in] format                   PRACH format.
/// \param[in] scs                      PRACH subcarrier spacing.
/// \param[in] zero_correlation_zone    PRACH zero correlation zone.
/// \param[in] nof_rx_ports             Number of receive antenna ports.
/// \return A success if the PRACH configuration is support by the detector, an error message otherwise.
error_type<std::string> validate_prach_detector_phy(prach_format_type        format,
                                                    prach_subcarrier_spacing scs,
                                                    unsigned                 zero_correlation_zone,
                                                    unsigned                 nof_rx_ports);

} // namespace ocudu
