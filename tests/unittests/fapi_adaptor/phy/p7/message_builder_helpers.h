// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/p7/messages/dl_tti_request.h"
#include "ocudu/fapi/p7/messages/ul_dci_request.h"
#include "ocudu/fapi/p7/messages/ul_tti_request.h"

namespace unittest {

/// Builds and returns a valid DL CSI-RS pdu. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.2.3.
ocudu::fapi::dl_csi_rs_pdu build_valid_dl_csi_pdu();

/// Builds and returns a valid DL PDCCH PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.2.1.
ocudu::fapi::dl_pdcch_pdu build_valid_dl_pdcch_pdu();

/// Builds and returns a valid DL PDSCH PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.2.2.
ocudu::fapi::dl_pdsch_pdu build_valid_dl_pdsch_pdu();

/// Builds and returns a valid DL PRS pdu. Every parameter is within the range defined in SCF-222 v8.0
/// Section 3.4.2.4a.
ocudu::fapi::dl_prs_pdu build_valid_dl_prs_pdu();

/// Builds and returns a valid DL SSB pdu. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.2.4.
ocudu::fapi::dl_ssb_pdu build_valid_dl_ssb_pdu();

/// Builds and returns a valid DL TTI request message. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.2.
ocudu::fapi::dl_tti_request build_valid_dl_tti_request();

/// Builds and returns a valid transmission precoding and beamforming PDU. Every parameter is within the range defined
/// in SCF-222 v4.0 Section 3.4.2.5.
ocudu::fapi::tx_precoding_and_beamforming_pdu build_valid_tx_precoding_and_beamforming_pdu();

/// Builds and returns a valid UL PRACH PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.3.1.
ocudu::fapi::ul_prach_pdu build_valid_ul_prach_pdu();

/// Builds and returns a valid UL PUCCH format 0 PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.3.3.
ocudu::fapi::ul_pucch_pdu build_valid_ul_pucch_f0_pdu();

/// Builds and returns a valid UL PUCCH format 1 PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.3.3.
ocudu::fapi::ul_pucch_pdu build_valid_ul_pucch_f1_pdu();

/// Builds and returns a valid UL PUCCH format 2 PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.3.3.
ocudu::fapi::ul_pucch_pdu build_valid_ul_pucch_f2_pdu();

/// Builds and returns a valid UL PUCCH format 3 PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.3.3.
ocudu::fapi::ul_pucch_pdu build_valid_ul_pucch_f3_pdu();

/// Builds and returns a valid UL PUCCH format 4 PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.3.3.
ocudu::fapi::ul_pucch_pdu build_valid_ul_pucch_f4_pdu();

/// Builds and returns a valid UL PUSCH PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.3.2.
ocudu::fapi::ul_pusch_pdu build_valid_ul_pusch_pdu();

} // namespace unittest
