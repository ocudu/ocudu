// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/p7/builders/dl_pdsch_pdu_builder.h"

namespace ocudu {

struct dl_msg_alloc;
struct dl_paging_allocation;
struct sib_information;
struct rar_information;

namespace fapi_adaptor {

class precoding_codebook_mapper;

/// \brief Helper function that converts from a PDSCH MAC PDU to a PDSCH FAPI PDU.
///
/// \param[out] builder PDSCH FAPI builder that helps to fill the PDU.
/// \param[in] mac_pdu SIB MAC PDU.
/// \param[in] nof_csi_pdus Number of CSI-RS PDUs colliding with this PDSCH PDU.
/// \param[in] pm_mapper Precoding codebook mapper.
/// \param[in] cell_nof_prbs Number of physical resource blocks of the cell.
void convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu_builder&      builder,
                               const sib_information&           mac_pdu,
                               unsigned                         nof_csi_pdus,
                               const precoding_codebook_mapper& pm_mapper,
                               unsigned                         cell_nof_prbs);

/// \brief Helper function that converts from a RAR MAC PDU to a PDSCH FAPI PDU.
///
/// \param[out] builder PDSCH FAPI builder that helps to fill the PDU.
/// \param[in] mac_pdu RAR MAC PDU.
/// \param[in] nof_csi_pdus Number of CSI-RS PDUs colliding with this PDSCH PDU.
/// \param[in] pm_mapper Precoding codebook mapper.
/// \param[in] cell_nof_prbs Number of physical resource blocks of the cell.
void convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu_builder&      builder,
                               const rar_information&           mac_pdu,
                               unsigned                         nof_csi_pdus,
                               const precoding_codebook_mapper& pm_mapper,
                               unsigned                         cell_nof_prbs);

/// \brief Helper function that converts from a UE MAC PDU to a PDSCH FAPI PDU.
///
/// \param[out] builder PDSCH FAPI builder that helps to fill the PDU.
/// \param[in] mac_pdu Grant MAC PDU.
/// \param[in] nof_csi_pdus Number of CSI-RS PDUs colliding with this PDSCH PDU.
/// \param[in] pm_mapper Precoding codebook mapper.
/// \param[in] cell_nof_prbs Number of physical resource blocks of the cell.
void convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu_builder&      builder,
                               const dl_msg_alloc&              mac_pdu,
                               unsigned                         nof_csi_pdus,
                               const precoding_codebook_mapper& pm_mapper,
                               unsigned                         cell_nof_prbs);

/// \brief Helper function that converts from a Paging MAC PDU to a PDSCH FAPI PDU.
///
/// \param[out] builder PDSCH FAPI builder that helps to fill the PDU.
/// \param[in] mac_pdu Paging MAC PDU.
/// \param[in] nof_csi_pdus Number of CSI-RS PDUs colliding with this PDSCH PDU.
/// \param[in] pm_mapper Precoding codebook mapper.
/// \param[in] cell_nof_prbs Number of physical resource blocks of the cell.
void convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu_builder&      builder,
                               const dl_paging_allocation&      mac_pdu,
                               unsigned                         nof_csi_pdus,
                               const precoding_codebook_mapper& pm_mapper,
                               unsigned                         cell_nof_prbs);

} // namespace fapi_adaptor
} // namespace ocudu
