// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/scheduler/result/pusch_info.h"
#include "ocudu/scheduler/scheduler_configurator.h"
#include "ocudu/scheduler/scheduler_feedback_handler.h"
#include "ocudu/scheduler/scheduler_rach_handler.h"

namespace ocudu {

struct pucch_info;
class cell_configuration;

namespace test_helper {

/// Create a detected PRACH preamble.
rach_indication_message::preamble create_preamble(unsigned preamble_id, rnti_t tc_rnti);

/// \brief Computes the RA-RNTI associated with a RACH occasion, as per TS 38.321, 5.1.3.
///
/// MsgA PUSCH (2-step RACH) is scheduled and decoded using this RA-RNTI, not the preamble's TC-RNTI, as per
/// TS 38.211, 6.3.1.1.
rnti_t compute_ra_rnti(const cell_configuration& cell_cfg,
                       slot_point                prach_slot_rx,
                       unsigned                  start_symbol,
                       unsigned                  frequency_index);

/// Create a RACH indication with one occation and multiple PRACH preambles.
rach_indication_message create_rach_indication(slot_point                                            slot_rx,
                                               const std::vector<rach_indication_message::preamble>& preambles);

/// Create dummy UCI indication based on a PUCCH PDU.
uci_indication create_uci_indication(slot_point uci_sl, du_ue_index_t ue_idx, const pucch_info& pucch_pdu);

/// Create dummy UCI PDU based on a PUCCH PDU.
/// \param ue_idx UE index to set in the UCI PDU.
/// \param pucch_pdu PUCCH PDU based on which the UCI PDU will be created.
/// \param ack_set If set, it overrides the ACK/NACK bits in the PUCCH PDU. If not set, we assume ACK for all bits,
/// except for SR-only PUCCHs, which are set to DTX.
/// \return Created UCI PDU.
uci_indication::uci_pdu create_uci_indication_pdu(du_ue_index_t                             ue_idx,
                                                  const pucch_info&                         pucch_pdu,
                                                  std::optional<mac_harq_ack_report_status> ack_set = std::nullopt);

/// Create dummy UCI PDU based on a PUSCH UCI PDU.
uci_indication::uci_pdu create_uci_indication_pdu(rnti_t rnti, du_ue_index_t ue_idx, const uci_info& uci);

/// Create dummy CRC PDU indication based on a PUSCH PDU.
ul_crc_pdu_indication create_crc_pdu_indication(const ul_sched_info& ul_grant);

/// Create a CRC indication for multiple UL grants.
ul_crc_indication create_crc_indication(slot_point sl_rx, span<const ul_sched_info> puschs, bool ack);

} // namespace test_helper

} // namespace ocudu
