// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/csi_report/csi_report_configuration.h"
#include "ocudu/ran/csi_report/csi_report_data.h"
#include "ocudu/ran/csi_report/csi_report_packed.h"
#include "ocudu/ran/csi_report/csi_report_size.h"

namespace ocudu {

/// \brief Gets the Channel State Information (CSI) report size when the CSI report is transmitted in PUCCH.
///
/// Field widths are defined in TS38.212 Section 6.3.1.1.2.
///
/// \param[in] config CSI report configuration.
/// \return The report size in bits.
csi_report_size get_csi_report_pucch_size(const csi_report_configuration& config);

/// \brief Checks that the CSI payload carried over PUCCH is valid given a CSI report configuration.
///
/// This validator can be used before calling \ref csi_report_unpack_pucch to avoid triggering any assertions.
///
/// \param[in] packed Packed CSI report.
/// \param[in] config CSI report configuration.
/// \return \c true if the CSI payload is valid, \c false otherwise.
bool validate_pucch_csi_payload(const csi_report_packed& packed, const csi_report_configuration& config);

/// \brief Unpacks Channel State Information (CSI) report multiplexed in PUCCH.
///
/// The unpacking is CSI report unpacking defined in TS38.212 Section 6.3.1.1.2.
///
/// \param[in] packed Packed CSI report.
/// \param[in] config CSI report configuration.
/// \return The CSI report data.
csi_report_data csi_report_unpack_pucch(const csi_report_packed& packed, const csi_report_configuration& config);

/// \brief Unpacks CSI Part 2 wideband information multiplexed in PUCCH.
///
/// The unpacking is defined in TS38.212 Table 6.3.1.1.2-10.
///
/// \param[in,out] data CSI report data containing the decoded Part 2 fields.
/// \param[in] packed Packed CSI Part 2 report.
/// \param[in] config CSI report configuration.
/// \return A slice of the remaining data.
csi_report_packed csi_report_unpack_pucch_part2_wideband(csi_report_data&                data,
                                                         const csi_report_packed&        packed,
                                                         const csi_report_configuration& config);

/// \brief Unpacks CSI Part 2 subband CQI information multiplexed in PUCCH.
///
/// The unpacking is defined in TS38.212 Table 6.3.1.1.2-11. Subband PMI is not supported.
///
/// \param[in,out] data CSI report data containing the decoded Part 2 fields.
/// \param[in] packed Packed CSI Part 2 report.
/// \param[in] config CSI report configuration.
/// \return A slice of the remaining data.
csi_report_packed csi_report_unpack_pucch_part2_subband(csi_report_data&                data,
                                                        const csi_report_packed&        packed,
                                                        const csi_report_configuration& config);

} // namespace ocudu
