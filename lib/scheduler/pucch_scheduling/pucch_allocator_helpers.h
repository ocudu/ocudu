// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../config/ue_configuration.h"
#include "ocudu/ran/csi_report/csi_report_configuration.h"
#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/scheduler/result/pdcch_info.h"
#include "ocudu/scheduler/result/pucch_info.h"

namespace ocudu::pucch_helper {

/// Returns the PUCCH resource from Resource Set \c ResourceSetId indexed by the given PUCCH Resource Indicator.
template <unsigned ResourceSetId>
const pucch_resource& get_harq_resource(const ue_cell_configuration& ue_cfg, unsigned pri);

/// Returns the SR PUCCH resource configured for the given UE.
const pucch_resource& get_sr_resource(const ue_cell_configuration& ue_cfg);

/// Returns the CSI PUCCH resource configured for the given UE.
const pucch_resource& get_csi_resource(const ue_cell_configuration& ue_cfg);

/// Returns the common PUCCH resource indexed by the given DCI info and PRI.
const pucch_resource&
get_common_resource(const cell_configuration& cell_cfg, const dci_context_information& dci_info, unsigned d_pri);

/// \brief Fills the PUCCH PDU for common HARQ-ACK resources.
///
/// \param[out] pucch_pdu PUCCH PDU to be filled.
/// \param[in] cell_cfg Cell configuration.
/// \param[in] pucch_res PUCCH resource configuration.
/// \param[in] rnti RNTI of the UE.
void fill_common_pdu(pucch_info&               pucch_pdu,
                     const cell_configuration& cell_cfg,
                     const pucch_resource&     common_res,
                     rnti_t                    rnti);

/// \brief Fills the PUCCH PDU for dedicated resources.
///
/// \param[out] pucch_pdu PUCCH PDU to be filled.
/// \param[in] cell_cfg Cell configuration.
/// \param[in] pucch_res PUCCH resource configuration.
/// \param[in] uci_bits UCI bits to be sent in the PUCCH.
/// \param[in] csi_cfg Optional CSI report configuration if CSI bits are present in the UCI.
/// \param[in] rnti RNTI of the UE.
void fill_ded_pdu(pucch_info&                     pucch_pdu,
                  const cell_configuration&       cell_cfg,
                  const pucch_resource&           pucch_res,
                  const pucch_uci_bits&           uci_bits,
                  const csi_report_configuration* csi_cfg,
                  rnti_t                          rnti);

} // namespace ocudu::pucch_helper
