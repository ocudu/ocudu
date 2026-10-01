// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/csi_report/csi_report_data.h"
#include "ocudu/ran/csi_report/csi_report_packed.h"
#include "ocudu/ran/csi_report/csi_report_size.h"

namespace ocudu {
struct csi_report_configuration;

/// \brief Calculates the Channel State Information (CSI) payload sizes when the CSI report is transmitted in PUSCH.
///
/// Field widths are defined in TS38.212 Section 6.3.2.1.2.
csi_report_size get_csi_report_pusch_size(const csi_report_configuration& config);

/// \brief Checks that the CSI payload carried over PUSCH is valid given a CSI report configuration.
///
/// This validator can be used before calling \ref csi_report_unpack_pusch to avoid triggering any assertions.
///
/// \param[in] csi1_packed Packed CSI Part 1 report.
/// \param[in] csi2_packed Packed CSI Part 2 report.
/// \param[in] config CSI report configuration.
/// \return \c true if the CSI payload is valid, \c false otherwise.
bool validate_pusch_csi_payload(const csi_report_packed&        csi1_packed,
                                const csi_report_packed&        csi2_packed,
                                const csi_report_configuration& config);

/// \brief Unpacks a Channel State Information (CSI) report multiplexed in PUSCH containing CSI Part 1 and Part 2.
///
/// The CSI report is defined in TS38.212 Section 6.3.2.1.2.
///
/// \param[in] csi1_packed Packed CSI Part 1 report.
/// \param[in] csi2_packed Packed CSI Part 2 report.
/// \param[in] config CSI report configuration.
/// \return The CSI report data.
csi_report_data csi_report_unpack_pusch(const csi_report_packed&        csi1_packed,
                                        const csi_report_packed&        csi2_packed,
                                        const csi_report_configuration& config);

/// \brief Unpacks Channel State Information (CSI) report multiplexed in PUSCH containing CSI Part 1 only.
///
/// The CSI report is defined in TS38.212 Section 6.3.2.1.2.
///
/// \param[in] csi1_packed Packed CSI Part 1 report.
/// \param[in] config CSI report configuration.
/// \return The CSI report data.
csi_report_data csi_report_unpack_pusch(const csi_report_packed& csi1_packed, const csi_report_configuration& config);
} // namespace ocudu
