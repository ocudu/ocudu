// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../cu_cp_impl_interface.h"
#include "../paging/paging_message_handler.h"
#include "../ue_manager/cu_cp_ue_impl_interface.h"
#include "ocudu/cu_cp/ue_task_scheduler.h"
#include "ocudu/ngap/ngap.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/rrc/rrc_ue.h"

namespace ocudu::ocucp {

/// Adapter between NGAP and CU-CP
class ngap_cu_cp_adapter : public ngap_cu_cp_notifier
{
public:
  void connect_cu_cp(cu_cp_ngap_handler& cu_cp_handler_, paging_message_handler& paging_handler_)
  {
    cu_cp_handler  = &cu_cp_handler_;
    paging_handler = &paging_handler_;
  }

  void on_paging_message(cu_cp_paging_message& msg) override
  {
    ocudu_assert(paging_handler != nullptr, "CU-CP Paging handler must not be nullptr");
    paging_handler->handle_paging_message(msg);
  }

  async_task<cu_cp_handover_resource_allocation_response>
  on_ngap_handover_request(const ngap_handover_request& request) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    return cu_cp_handler->handle_ngap_handover_request(request);
  }

  ngap_cu_cp_ue_notifier* on_new_ngap_ue(cu_cp_ue_index_t ue_index) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    return cu_cp_handler->handle_new_ngap_ue(ue_index);
  }

  bool schedule_async_task(cu_cp_ue_index_t ue_index, async_task<void> task) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    return cu_cp_handler->schedule_ue_task(ue_index, std::move(task));
  }

  bool on_handover_request_received(cu_cp_ue_index_t                  ue_index,
                                    const plmn_identity&              selected_plmn,
                                    const security::security_context& sec_ctxt) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    return cu_cp_handler->handle_handover_request(ue_index, selected_plmn, sec_ctxt);
  }

  async_task<expected<ngap_init_context_setup_response, ngap_init_context_setup_failure>>
  on_new_initial_context_setup_request(ngap_init_context_setup_request& request) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    return cu_cp_handler->handle_new_initial_context_setup_request(request);
  }

  async_task<expected<ngap_ue_context_modification_response, ngap_ue_context_modification_failure>>
  on_new_ue_context_modification_request(ngap_ue_context_modification_request& request) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    return cu_cp_handler->handle_new_ue_context_modification_request(request);
  }

  async_task<ngap_pdu_session_resource_setup_response>
  on_new_pdu_session_resource_setup_request(ngap_pdu_session_resource_setup_request& request) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    return cu_cp_handler->handle_new_pdu_session_resource_setup_request(request);
  }

  async_task<ngap_pdu_session_resource_modify_response>
  on_new_pdu_session_resource_modify_request(ngap_pdu_session_resource_modify_request& request) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    return cu_cp_handler->handle_new_pdu_session_resource_modify_request(request);
  }

  async_task<ngap_pdu_session_resource_release_response>
  on_new_pdu_session_resource_release_command(ngap_pdu_session_resource_release_command& command) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    return cu_cp_handler->handle_new_pdu_session_resource_release_command(command);
  }

  async_task<cu_cp_ue_context_release_complete>
  on_new_ue_context_release_command(const cu_cp_ue_context_release_command& command) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    return cu_cp_handler->handle_ue_context_release_command(command);
  }

  void on_transmission_of_handover_required() override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    cu_cp_handler->handle_transmission_of_handover_required();
  }

  async_task<bool> on_new_rrc_handover_command(cu_cp_ue_index_t ue_index, byte_buffer command) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    return cu_cp_handler->handle_new_rrc_handover_command(ue_index, std::move(command));
  }

  void on_n2_handover_execution(cu_cp_ue_index_t ue_index) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    cu_cp_handler->handle_inter_cu_target_handover_execution(ue_index);
  }

  cu_cp_ue_index_t request_new_ue_index_allocation(nr_cell_global_id_t cgi, const plmn_identity& plmn) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    return cu_cp_handler->handle_ue_index_allocation_request(cgi, plmn);
  }

  void on_dl_ue_associated_nrppa_transport_pdu(cu_cp_ue_index_t ue_index, const byte_buffer& nrppa_pdu) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    cu_cp_handler->handle_dl_ue_associated_nrppa_transport_pdu(ue_index, nrppa_pdu);
  }

  void on_dl_non_ue_associated_nrppa_transport_pdu(cu_cp_amf_index_t amf_index, const byte_buffer& nrppa_pdu) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    cu_cp_handler->handle_dl_non_ue_associated_nrppa_transport_pdu(amf_index, nrppa_pdu);
  }

  void on_location_reporting_control_message(cu_cp_ue_index_t ue_index, const location_report_request& msg) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    cu_cp_handler->handle_location_reporting_control_message(ue_index, msg);
  }

  void on_n2_disconnection(cu_cp_amf_index_t amf_index) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    cu_cp_handler->handle_n2_disconnection(amf_index);
  }

private:
  cu_cp_ngap_handler*     cu_cp_handler  = nullptr;
  paging_message_handler* paging_handler = nullptr;
};

/// Adapter between NGAP and CU-CP UE
class ngap_cu_cp_ue_adapter : public ngap_cu_cp_ue_notifier
{
public:
  ngap_cu_cp_ue_adapter() = default;

  void connect_ue(cu_cp_ue_impl_interface& ue_) { ue = &ue_; }

  /// \brief Get the UE index of the UE.
  cu_cp_ue_index_t get_ue_index() override
  {
    ocudu_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    return ue->get_ue_index();
  }

  /// \brief Schedule an async task for the UE.
  bool schedule_async_task(async_task<void> task) override
  {
    ocudu_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    return ue->get_task_sched().schedule_async_task(std::move(task));
  }

  /// \brief Get the RRC UE notifier of the UE.
  ngap_rrc_ue_notifier& get_ngap_rrc_ue_notifier() override
  {
    ocudu_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    return ue->get_ngap_rrc_ue_notifier();
  }

  bool init_security_context(const security::security_context& sec_ctxt) override
  {
    ocudu_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    return ue->get_security_manager().init_security_context(sec_ctxt);
  }

  [[nodiscard]] bool is_security_enabled() const override
  {
    ocudu_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    return ue->get_security_manager().is_security_enabled();
  }

  void set_ue_ambr(aggregate_maximum_bit_rate_t ue_ambr) override
  {
    ocudu_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    ue->set_ue_ambr(ue_ambr);
  }

  aggregate_maximum_bit_rate_t get_ue_ambr() const override
  {
    ocudu_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    return ue->get_ue_ambr();
  }

private:
  cu_cp_ue_impl_interface* ue = nullptr;
};

/// Adapter between NGAP and RRC UE
class ngap_rrc_ue_adapter : public ngap_rrc_ue_notifier
{
public:
  ngap_rrc_ue_adapter() = default;

  void connect_rrc_ue(rrc_ngap_message_handler& rrc_ue_handler_) { rrc_ue_handler = &rrc_ue_handler_; }

  void on_new_pdu(byte_buffer nas_pdu) override
  {
    ocudu_assert(rrc_ue_handler != nullptr, "RRC UE handler must not be nullptr");
    rrc_ue_handler->handle_dl_nas_transport_message(std::move(nas_pdu));
  }

  byte_buffer on_ue_radio_access_cap_info_required() override
  {
    ocudu_assert(rrc_ue_handler != nullptr, "RRC UE handler must not be nullptr");
    return rrc_ue_handler->get_packed_ue_radio_access_cap_info();
  }

  byte_buffer on_handover_preparation_message_required() override
  {
    ocudu_assert(rrc_ue_handler != nullptr, "RRC UE handler must not be nullptr");
    return rrc_ue_handler->get_packed_handover_preparation_message();
  }

private:
  rrc_ngap_message_handler* rrc_ue_handler = nullptr;
};

} // namespace ocudu::ocucp
