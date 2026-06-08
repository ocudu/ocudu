// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1ap/cu_cp/f1ap_rrc_msg_transfer_handling.h"
#include "ocudu/rrc/rrc_ue.h"

namespace ocudu::ocucp {

class cu_cp_controller;

/// Adapter between F1AP UE entity and RRC UE entity that forwards CCCH messages.
class f1ap_rrc_ul_ccch_adapter : public f1ap_ul_ccch_notifier
{
public:
  void connect_rrc_ue(rrc_ul_pdu_handler& rrc_pdu_handler_) { rrc_pdu_handler = &rrc_pdu_handler_; }

  void on_ul_ccch_pdu(byte_buffer pdu, rnti_t c_rnti) override
  {
    ocudu_assert(rrc_pdu_handler != nullptr, "RRC UL handler must not be nullptr");
    rrc_pdu_handler->handle_ul_ccch_pdu(std::move(pdu), c_rnti);
  }

private:
  rrc_ul_pdu_handler* rrc_pdu_handler = nullptr;
};

} // namespace ocudu::ocucp
