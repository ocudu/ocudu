// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1ap/cu_cp/du_setup_notifier.h"
#include "ocudu/f1ap/cu_cp/f1ap_cu.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/ran/cu_cp_types.h"

namespace ocudu::ocucp {

/// \brief Generate a valid dummy F1 Setup Request.
void generate_valid_f1_setup_request(du_setup_request& setup_request,
                                     gnb_du_id_t       gnb_du_id = int_to_gnb_du_id(0x11),
                                     nr_cell_identity  nci = nr_cell_identity::create(gnb_id_t{411, 22}, 0).value(),
                                     pci_t             pci = 0,
                                     unsigned          tac = 7);

/// \brief Generate a dummy F1 Setup Request base to extend.
void generate_f1_setup_request_base(du_setup_request& setup_request);

/// \brief Generate a dummy F1 Setup Request with unsupported number of cells.
f1ap_message create_f1_setup_request_with_too_many_cells(const f1ap_message& base = {});
void         generate_f1_setup_request_with_too_many_cells(du_setup_request& setup_request);

/// \brief Generate a dummy UE Creation Message.
/// \param[in] ue_index The UE index to use.
/// \param[in] c_rnti The C-RNTI to use.
/// \param[in] nrcell_id The NR Cell Id to use.
/// \return The dummy UE Creation Message.
ue_rrc_context_creation_request generate_ue_rrc_context_creation_request(
    cu_cp_ue_index_t ue_index,
    rnti_t           c_rnti,
    nr_cell_identity nrcell_id = nr_cell_identity::create(gnb_id_t{411, 22}, 0).value());

} // namespace ocudu::ocucp
