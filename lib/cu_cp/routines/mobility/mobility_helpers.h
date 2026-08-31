// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../ue_manager/ue_manager_impl.h"
#include "../../up_resource_manager/up_resource_manager_impl.h"
#include "../../xnap_repository.h"
#include "ocudu/cu_cp/cu_cp_intra_cu_ho_types.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp_bearer_context_update.h"
#include "ocudu/f1ap/cu_cp/f1ap_cu_ue_context_update.h"

namespace ocudu {
namespace ocucp {

/// Decodes the source's AS-Config, embedded in the RRC HandoverPreparationInformation, and merges its DRB-to-QoS-flow
/// mapping into \c old_drb_association. This lets DRB allocation at this node prefer reusing the source's DRB ID for
/// the same QoS flow, keeping DRB numbering consistent with the source where possible (DRB IDs are otherwise allocated
/// independently by each RAN node; see TS 38.300 section 9.2.3.2.3). AS-Config carries the UE's full current radio
/// bearer configuration (TS 38.331 section 11.2.3), so it is the source of that hint whenever the peer signals PDU
/// sessions without a DRB-to-QoS-flow mapping.
void merge_old_drb_association_from_as_config(up_old_drb_association&       old_drb_association,
                                              const byte_buffer&            rrc_handover_preparation_information,
                                              const ocudulog::basic_logger& logger);

/// \brief Handle UE context setup response from target DU and prefills the Bearer context modification.
bool handle_context_setup_response(cu_cp_intra_cu_handover_response&         response_msg,
                                   e1ap_bearer_context_modification_request& bearer_context_modification_request,
                                   const f1ap_ue_context_setup_response&     target_ue_context_setup_response,
                                   up_config_update&                         next_config,
                                   const ocudulog::basic_logger&             logger,
                                   bool                                      reestablish_pdcp);

/// \brief Handler Bearer context modification response from CU-UP and prefill UE context modification for source DU.
bool handle_bearer_context_modification_response(
    cu_cp_intra_cu_handover_response&                response_msg,
    f1ap_ue_context_modification_request&            source_ue_context_mod_request,
    const e1ap_bearer_context_modification_response& bearer_context_modification_response,
    up_config_update&                                next_config,
    const ocudulog::basic_logger&                    logger);

/// \brief Cancel each non-winning CHO candidate. Intra-CU candidates have their RRC reconfiguration transaction
/// cancelled so the waiting target routine self-releases. Inter-CU candidates are cancelled via Xn when xnap_db is
/// provided. Candidates with invalid ue_index and candidates aliasing the source UE are always skipped.
/// \param[in] source_ue              Source UE holding the CHO context with candidates.
/// \param[in] ue_mng                 UE manager for intra-CU candidate lookups.
/// \param[in] xnap_db                Xn repository for inter-CU cancel; pass nullptr to skip Xn cancellation.
/// \param[in] winner_ue_index        UE index of the intra-CU winner to exclude. Pass invalid to not skip by index.
/// \param[in] winner_cgi             Target cell of the inter-CU winner to exclude. Pass nullopt to not skip by cell.
/// Identified by cell rather than by peer XNAP UE ID, which is only unique per Xn interface: two candidates at
/// different peers commonly hold the same ID, and skipping by ID would then skip both.
/// \return Number of candidates cancelled.
unsigned cancel_cho_candidates(cu_cp_ue&                          source_ue,
                               ue_manager&                        ue_mng,
                               xnap_repository*                   xnap_db         = nullptr,
                               cu_cp_ue_index_t                   winner_ue_index = cu_cp_ue_index_t::invalid,
                               std::optional<nr_cell_global_id_t> winner_cgi      = std::nullopt);

} // namespace ocucp
} // namespace ocudu
