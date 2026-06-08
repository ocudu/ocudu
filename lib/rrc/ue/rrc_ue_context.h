// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "rrc_ue_logger.h"
#include "ocudu/asn1/rrc_nr/ul_dcch_msg_ies.h"
#include "ocudu/ran/five_g_s_tmsi.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/rrc/rrc_cell_context.h"
#include "ocudu/rrc/rrc_ue.h"
#include "ocudu/rrc/rrc_ue_capabilities.h"
#include "ocudu/rrc/rrc_ue_config.h"

namespace ocudu::ocucp {

/// Holds the RRC UE context used by the UE object and all its procedures.
class rrc_ue_context_t
{
public:
  rrc_ue_context_t(const cu_cp_ue_index_t                 ue_index_,
                   rnti_t                                 c_rnti_,
                   const rrc_cell_context&                cell_,
                   const rrc_ue_cfg_t&                    cfg_,
                   std::optional<rrc_ue_transfer_context> rrc_context_,
                   rrc_ue_logger&                         logger_);

  const cu_cp_ue_index_t         ue_index; // UE index assigned by the DU processor
  rnti_t                         c_rnti;   // current C-RNTI
  rrc_cell_context               cell;     // current cell
  const rrc_ue_cfg_t             cfg;
  plmn_identity                  plmn_id = plmn_identity::test_value(); // PLMN identity of the UE
  rrc_state                      state   = rrc_state::idle;
  std::optional<rrc_meas_cfg>    meas_cfg;
  std::optional<uint8_t>         serving_cell_mo;
  std::optional<five_g_s_tmsi_t> five_g_s_tmsi;
  std::variant<uint64_t, asn1::fixed_bitstring<39>>
                        setup_ue_id; ///< this is either a random value or the 5G-S-TMSI-PART1
  establishment_cause_t connection_cause;
  rrc_ue_pdcp_notifier* pdcp_notifier = nullptr;
  std::optional<asn1::rrc_nr::ue_cap_rat_container_list_l> capabilities_list;
  rrc_ue_capabilities_t                                    capabilities;
  std::optional<rrc_ue_transfer_context> transfer_context; // Context of old UE when created through mobility.
  byte_buffer                            cell_group_config;
  bool                                   reestablishment_ongoing = false;
  // NAS messages received while UE is in RRC Inactive, to be sent after successful resume.
  std::vector<byte_buffer> pending_dl_nas_transport_messages;
  rrc_ue_logger&           logger;
};

} // namespace ocudu::ocucp
