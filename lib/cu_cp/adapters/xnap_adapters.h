// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../cu_cp_impl_interface.h"
#include "ocudu/xnap/xnap.h"

namespace ocudu::ocucp {

/// Adapter between NGAP and CU-CP
class xnap_cu_cp_adapter : public xnap_cu_cp_notifier
{
public:
  void connect_cu_cp(cu_cp_xnap_handler& cu_cp_handler_, xnc_peer_index_t xnc_index_)
  {
    cu_cp_handler = &cu_cp_handler_;
    xnc_index     = xnc_index_;
  }

  async_task<bool> on_new_rrc_handover_command(cu_cp_ue_index_t ue_index, byte_buffer command) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP XNAP handler must not be nullptr");
    return cu_cp_handler->handle_new_rrc_handover_command(ue_index, std::move(command), xnc_index);
  }

  cu_cp_ue_index_t request_new_ue_index_allocation(const nr_cell_global_id_t& cgi, const plmn_identity& plmn) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP XNAP handler must not be nullptr");
    return cu_cp_handler->handle_ue_index_allocation_request(cgi, plmn);
  }

  bool on_handover_request_received(cu_cp_ue_index_t                  ue_index,
                                    const plmn_identity&              selected_plmn,
                                    const security::security_context& sec_ctxt) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP XNAP handler must not be nullptr");
    return cu_cp_handler->handle_handover_request(ue_index, selected_plmn, sec_ctxt);
  }

  bool schedule_async_task(cu_cp_ue_index_t ue_index, async_task<void> task) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP XNAP handler must not be nullptr");
    return cu_cp_handler->schedule_ue_task(ue_index, std::move(task));
  }

  async_task<cu_cp_handover_resource_allocation_response>
  on_xnap_handover_request(const xnap_handover_request& request) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP XNAP handler must not be nullptr");
    return cu_cp_handler->handle_xnap_handover_request(request);
  }

  void on_xn_handover_execution(cu_cp_ue_index_t                              ue_index,
                                const xnap_handover_target_execution_context& xnap_ho_target_execution_ctxt) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP XNAP handler must not be nullptr");
    cu_cp_handler->handle_inter_cu_target_handover_execution(ue_index, xnap_ho_target_execution_ctxt);
  }

  void on_handover_cancel_received(cu_cp_ue_index_t ue_index) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP XNAP handler must not be nullptr");
    cu_cp_handler->handle_handover_cancel_received(ue_index);
  }

  void on_handover_success_received(cu_cp_ue_index_t source_ue_index, peer_xnap_ue_id_t winner_peer_xnap_ue_id) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP XNAP handler must not be nullptr");
    cu_cp_handler->handle_xnap_handover_success_received(source_ue_index, winner_peer_xnap_ue_id);
  }

  void on_ue_context_release_received(cu_cp_ue_index_t ue_index) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP XNAP handler must not be nullptr");
    cu_cp_handler->handle_xnap_ue_context_release_received(ue_index);
  }

private:
  cu_cp_xnap_handler* cu_cp_handler = nullptr;
  xnc_peer_index_t    xnc_index     = xnc_peer_index_t::invalid;
};

} // namespace ocudu::ocucp
