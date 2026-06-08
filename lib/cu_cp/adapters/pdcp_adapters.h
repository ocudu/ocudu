// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../pdcp/srb_pdcp_ue_context.h"
#include "ocudu/f1ap/cu_cp/f1ap_rrc_msg_transfer_handling.h"
#include "ocudu/ran/rb_id.h"

namespace ocudu::ocucp {

/// Adapter between F1AP UE entity and the SRB PDCP context for UL DCCH messages.
class f1ap_pdcp_ul_dcch_adapter : public f1ap_ul_dcch_notifier
{
public:
  void connect_pdcp(srb_id_t srb_id_, srb_pdcp_ue_context& pdcp_ctx_)
  {
    srb_id   = srb_id_;
    pdcp_ctx = &pdcp_ctx_;
  }

  void on_ul_dcch_pdu(byte_buffer pdu) override
  {
    ocudu_assert(pdcp_ctx != nullptr, "PDCP context must not be nullptr");
    pdcp_ctx->handle_ul_dcch_pdu(srb_id, std::move(pdu));
  }

private:
  srb_id_t             srb_id   = srb_id_t::nulltype;
  srb_pdcp_ue_context* pdcp_ctx = nullptr;
};

/// Collection of per-SRB F1AP to PDCP UL adapters for SRB1 and SRB2.
class f1ap_pdcp_ul_dcch_adapter_collection
{
public:
  void connect_pdcp(srb_pdcp_ue_context& pdcp_ctx)
  {
    srb1_adapter.connect_pdcp(srb_id_t::srb1, pdcp_ctx);
    srb2_adapter.connect_pdcp(srb_id_t::srb2, pdcp_ctx);
  }

  f1ap_ul_dcch_notifier& get_srb1_notifier() { return srb1_adapter; }
  f1ap_ul_dcch_notifier& get_srb2_notifier() { return srb2_adapter; }

private:
  f1ap_pdcp_ul_dcch_adapter srb1_adapter;
  f1ap_pdcp_ul_dcch_adapter srb2_adapter;
};

} // namespace ocudu::ocucp
