// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/csi_report/csi_report_configuration.h"

namespace ocudu {
struct csi_meas_config;

/// \brief Returns the Channel State Information (CSI) report configuration parameters based on CSI-MeasConfig.
/// \param csi_meas CSI-RS measurement configuration.
/// \return Returns CSI report configuration parameters.
csi_report_configuration create_csi_report_configuration(const csi_meas_config& csi_meas);

/// CSI report configuration validator.
bool is_valid(const csi_report_configuration& config);

/// \brief Tells whether the CSI report is configured for PUSCH.
///
/// \remark There is no explicit statement defining this in the TS; however, we can infer from \c reportConfigType in \c
/// CSI-ReportConfig, TS 38.331, that the CSI report is configured for PUSCH if it is either \c semiPersistentOnPUSCH or
/// \c aperiodic, while it's configured for PUCCH if it is \c periodic or \c semiPersistentOnPUCCH.
bool is_pusch_configured(const csi_meas_config& csi_meas);

} // namespace ocudu
