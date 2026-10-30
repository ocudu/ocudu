// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/e2ap/e2ap.h"
#include "ocudu/asn1/e2ap/e2ap_pdu_contents.h"

using namespace asn1;
using namespace asn1::e2ap;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// ProtocolIE-FieldPair{E2AP-PROTOCOL-IES-PAIR : IEsSetParam} ::= SEQUENCE{{E2AP-PROTOCOL-IES-PAIR}}
template <class ies_set_paramT_>
OCUDUASN_CODE protocol_ie_field_pair_s<ies_set_paramT_>::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, id, (uint32_t)0u, (uint32_t)65535u, false, true));
  warn_assert(first_crit != ies_set_paramT_::get_first_crit(id), __func__, __LINE__);
  HANDLE_CODE(first_crit.pack(bref));
  HANDLE_CODE(first_value.pack(bref));
  warn_assert(second_crit != ies_set_paramT_::get_second_crit(id), __func__, __LINE__);
  HANDLE_CODE(second_crit.pack(bref));
  HANDLE_CODE(second_value.pack(bref));

  return OCUDUASN_SUCCESS;
}
template <class ies_set_paramT_>
OCUDUASN_CODE protocol_ie_field_pair_s<ies_set_paramT_>::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
  HANDLE_CODE(first_crit.unpack(bref));
  first_value = ies_set_paramT_::get_first_value(id);
  HANDLE_CODE(first_value.unpack(bref));
  HANDLE_CODE(second_crit.unpack(bref));
  second_value = ies_set_paramT_::get_second_value(id);
  HANDLE_CODE(second_value.unpack(bref));

  return OCUDUASN_SUCCESS;
}
template <class ies_set_paramT_>
void protocol_ie_field_pair_s<ies_set_paramT_>::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", id);
  j.write_str("firstCriticality", first_crit.to_string());
  j.write_str("secondCriticality", second_crit.to_string());
  j.end_obj();
}
template <class ies_set_paramT_>
bool protocol_ie_field_pair_s<ies_set_paramT_>::load_info_obj(const uint32_t& id_)
{
  if (not ies_set_paramT_::is_id_valid(id_)) {
    return false;
  }
  id           = id_;
  first_crit   = ies_set_paramT_::get_first_crit(id);
  first_value  = ies_set_paramT_::get_first_value(id);
  second_crit  = ies_set_paramT_::get_second_crit(id);
  second_value = ies_set_paramT_::get_second_value(id);
  return first_value.type().value != ies_set_paramT_::first_value_c::types_opts::nulltype and
         second_value.type().value != ies_set_paramT_::second_value_c::types_opts::nulltype;
}

// E2AP-ELEMENTARY-PROCEDURES ::= OBJECT SET OF E2AP-ELEMENTARY-PROCEDURE
uint16_t e2ap_elem_procs_o::idx_to_proc_code(uint32_t idx)
{
  static const uint16_t names[] = {8,  17, 9,  14, 15, 20, 16, 7, 21, 4, 18, 1, 10,
                                   11, 3,  13, 24, 22, 23, 5,  6, 19, 2, 12, 25};
  return map_enum_number(names, 25, idx, "proc_code");
}
bool e2ap_elem_procs_o::is_proc_code_valid(const uint16_t& proc_code)
{
  static const uint16_t names[] = {8,  17, 9,  14, 15, 20, 16, 7, 21, 4, 18, 1, 10,
                                   11, 3,  13, 24, 22, 23, 5,  6, 19, 2, 12, 25};
  for (const auto& o : names) {
    if (o == proc_code) {
      return true;
    }
  }
  return false;
}
e2ap_elem_procs_o::init_msg_c e2ap_elem_procs_o::get_init_msg(const uint16_t& proc_code)
{
  init_msg_c ret{};
  switch (proc_code) {
    case 8:
      ret.set(init_msg_c::types::ric_sub_request);
      break;
    case 17:
      ret.set(init_msg_c::types::ric_sub_audit_request);
      break;
    case 9:
      ret.set(init_msg_c::types::ric_sub_delete_request);
      break;
    case 14:
      ret.set(init_msg_c::types::ric_sub_mod_request);
      break;
    case 15:
      ret.set(init_msg_c::types::ric_sub_mod_required);
      break;
    case 20:
      ret.set(init_msg_c::types::ric_sub_state_ctrl_request);
      break;
    case 16:
      ret.set(init_msg_c::types::ric_query_request);
      break;
    case 7:
      ret.set(init_msg_c::types::ric_service_upd);
      break;
    case 21:
      ret.set(init_msg_c::types::ric_assist_request);
      break;
    case 4:
      ret.set(init_msg_c::types::ric_ctrl_request);
      break;
    case 18:
      ret.set(init_msg_c::types::ric_service_load_status_request);
      break;
    case 1:
      ret.set(init_msg_c::types::e2setup_request);
      break;
    case 10:
      ret.set(init_msg_c::types::e2node_cfg_upd);
      break;
    case 11:
      ret.set(init_msg_c::types::e2conn_upd);
      break;
    case 3:
      ret.set(init_msg_c::types::reset_request);
      break;
    case 13:
      ret.set(init_msg_c::types::e2_removal_request);
      break;
    case 24:
      ret.set(init_msg_c::types::e2_res_status_request);
      break;
    case 22:
      ret.set(init_msg_c::types::ric_assist_ind);
      break;
    case 23:
      ret.set(init_msg_c::types::ric_assist_halt);
      break;
    case 5:
      ret.set(init_msg_c::types::ric_ind);
      break;
    case 6:
      ret.set(init_msg_c::types::ric_service_query);
      break;
    case 19:
      ret.set(init_msg_c::types::ric_service_load_upd);
      break;
    case 2:
      ret.set(init_msg_c::types::error_ind);
      break;
    case 12:
      ret.set(init_msg_c::types::ric_sub_delete_required);
      break;
    case 25:
      ret.set(init_msg_c::types::e2_res_status_upd);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
e2ap_elem_procs_o::successful_outcome_c e2ap_elem_procs_o::get_successful_outcome(const uint16_t& proc_code)
{
  successful_outcome_c ret{};
  switch (proc_code) {
    case 8:
      ret.set(successful_outcome_c::types::ric_sub_resp);
      break;
    case 17:
      ret.set(successful_outcome_c::types::ric_sub_audit_resp);
      break;
    case 9:
      ret.set(successful_outcome_c::types::ric_sub_delete_resp);
      break;
    case 14:
      ret.set(successful_outcome_c::types::ric_sub_mod_resp);
      break;
    case 15:
      ret.set(successful_outcome_c::types::ric_sub_mod_confirm);
      break;
    case 20:
      ret.set(successful_outcome_c::types::ric_sub_state_ctrl_resp);
      break;
    case 16:
      ret.set(successful_outcome_c::types::ric_query_resp);
      break;
    case 7:
      ret.set(successful_outcome_c::types::ric_service_upd_ack);
      break;
    case 21:
      ret.set(successful_outcome_c::types::ric_assist_resp);
      break;
    case 4:
      ret.set(successful_outcome_c::types::ric_ctrl_ack);
      break;
    case 18:
      ret.set(successful_outcome_c::types::ric_service_load_status_resp);
      break;
    case 1:
      ret.set(successful_outcome_c::types::e2setup_resp);
      break;
    case 10:
      ret.set(successful_outcome_c::types::e2node_cfg_upd_ack);
      break;
    case 11:
      ret.set(successful_outcome_c::types::e2conn_upd_ack);
      break;
    case 3:
      ret.set(successful_outcome_c::types::reset_resp);
      break;
    case 13:
      ret.set(successful_outcome_c::types::e2_removal_resp);
      break;
    case 24:
      ret.set(successful_outcome_c::types::e2_res_status_resp);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
e2ap_elem_procs_o::unsuccessful_outcome_c e2ap_elem_procs_o::get_unsuccessful_outcome(const uint16_t& proc_code)
{
  unsuccessful_outcome_c ret{};
  switch (proc_code) {
    case 8:
      ret.set(unsuccessful_outcome_c::types::ric_sub_fail);
      break;
    case 17:
      ret.set(unsuccessful_outcome_c::types::ric_sub_audit_fail);
      break;
    case 9:
      ret.set(unsuccessful_outcome_c::types::ric_sub_delete_fail);
      break;
    case 14:
      ret.set(unsuccessful_outcome_c::types::ric_sub_mod_fail);
      break;
    case 15:
      ret.set(unsuccessful_outcome_c::types::ric_sub_mod_refuse);
      break;
    case 20:
      ret.set(unsuccessful_outcome_c::types::ric_sub_state_ctrl_fail);
      break;
    case 16:
      ret.set(unsuccessful_outcome_c::types::ric_query_fail);
      break;
    case 7:
      ret.set(unsuccessful_outcome_c::types::ric_service_upd_fail);
      break;
    case 21:
      ret.set(unsuccessful_outcome_c::types::ric_assist_fail);
      break;
    case 4:
      ret.set(unsuccessful_outcome_c::types::ric_ctrl_fail);
      break;
    case 18:
      ret.set(unsuccessful_outcome_c::types::ric_service_load_status_fail);
      break;
    case 1:
      ret.set(unsuccessful_outcome_c::types::e2setup_fail);
      break;
    case 10:
      ret.set(unsuccessful_outcome_c::types::e2node_cfg_upd_fail);
      break;
    case 11:
      ret.set(unsuccessful_outcome_c::types::e2conn_upd_fail);
      break;
    case 13:
      ret.set(unsuccessful_outcome_c::types::e2_removal_fail);
      break;
    case 24:
      ret.set(unsuccessful_outcome_c::types::e2_res_status_fail);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
crit_e e2ap_elem_procs_o::get_crit(const uint16_t& proc_code)
{
  switch (proc_code) {
    case 8:
      return crit_e::reject;
    case 17:
      return crit_e::reject;
    case 9:
      return crit_e::reject;
    case 14:
      return crit_e::reject;
    case 15:
      return crit_e::reject;
    case 20:
      return crit_e::reject;
    case 16:
      return crit_e::reject;
    case 7:
      return crit_e::reject;
    case 21:
      return crit_e::reject;
    case 4:
      return crit_e::reject;
    case 18:
      return crit_e::ignore;
    case 1:
      return crit_e::reject;
    case 10:
      return crit_e::reject;
    case 11:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 13:
      return crit_e::reject;
    case 24:
      return crit_e::reject;
    case 22:
      return crit_e::reject;
    case 23:
      return crit_e::reject;
    case 5:
      return crit_e::ignore;
    case 6:
      return crit_e::ignore;
    case 19:
      return crit_e::ignore;
    case 2:
      return crit_e::ignore;
    case 12:
      return crit_e::ignore;
    case 25:
      return crit_e::ignore;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return {};
}

// InitiatingMessage ::= OPEN TYPE
void e2ap_elem_procs_o::init_msg_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_sub_request:
      c = ric_sub_request_s{};
      break;
    case types::ric_sub_audit_request:
      c = ric_sub_audit_request_s{};
      break;
    case types::ric_sub_delete_request:
      c = ric_sub_delete_request_s{};
      break;
    case types::ric_sub_mod_request:
      c = ric_sub_mod_request_s{};
      break;
    case types::ric_sub_mod_required:
      c = ric_sub_mod_required_s{};
      break;
    case types::ric_sub_state_ctrl_request:
      c = ric_sub_state_ctrl_request_s{};
      break;
    case types::ric_query_request:
      c = ric_query_request_s{};
      break;
    case types::ric_service_upd:
      c = ric_service_upd_s{};
      break;
    case types::ric_assist_request:
      c = ric_assist_request_s{};
      break;
    case types::ric_ctrl_request:
      c = ric_ctrl_request_s{};
      break;
    case types::ric_service_load_status_request:
      c = ric_service_load_status_request_s{};
      break;
    case types::e2setup_request:
      c = e2setup_request_s{};
      break;
    case types::e2node_cfg_upd:
      c = e2node_cfg_upd_s{};
      break;
    case types::e2conn_upd:
      c = e2conn_upd_s{};
      break;
    case types::reset_request:
      c = reset_request_s{};
      break;
    case types::e2_removal_request:
      c = e2_removal_request_s{};
      break;
    case types::e2_res_status_request:
      c = e2_res_status_request_s{};
      break;
    case types::ric_assist_ind:
      c = ric_assist_ind_s{};
      break;
    case types::ric_assist_halt:
      c = ric_assist_halt_s{};
      break;
    case types::ric_ind:
      c = ric_ind_s{};
      break;
    case types::ric_service_query:
      c = ric_service_query_s{};
      break;
    case types::ric_service_load_upd:
      c = ric_service_load_upd_s{};
      break;
    case types::error_ind:
      c = error_ind_s{};
      break;
    case types::ric_sub_delete_required:
      c = ric_sub_delete_required_s{};
      break;
    case types::e2_res_status_upd:
      c = e2_res_status_upd_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_o::init_msg_c");
  }
}
ric_sub_request_s& e2ap_elem_procs_o::init_msg_c::ric_sub_request()
{
  assert_choice_type(types::ric_sub_request, type_, "InitiatingMessage");
  return c.get<ric_sub_request_s>();
}
ric_sub_audit_request_s& e2ap_elem_procs_o::init_msg_c::ric_sub_audit_request()
{
  assert_choice_type(types::ric_sub_audit_request, type_, "InitiatingMessage");
  return c.get<ric_sub_audit_request_s>();
}
ric_sub_delete_request_s& e2ap_elem_procs_o::init_msg_c::ric_sub_delete_request()
{
  assert_choice_type(types::ric_sub_delete_request, type_, "InitiatingMessage");
  return c.get<ric_sub_delete_request_s>();
}
ric_sub_mod_request_s& e2ap_elem_procs_o::init_msg_c::ric_sub_mod_request()
{
  assert_choice_type(types::ric_sub_mod_request, type_, "InitiatingMessage");
  return c.get<ric_sub_mod_request_s>();
}
ric_sub_mod_required_s& e2ap_elem_procs_o::init_msg_c::ric_sub_mod_required()
{
  assert_choice_type(types::ric_sub_mod_required, type_, "InitiatingMessage");
  return c.get<ric_sub_mod_required_s>();
}
ric_sub_state_ctrl_request_s& e2ap_elem_procs_o::init_msg_c::ric_sub_state_ctrl_request()
{
  assert_choice_type(types::ric_sub_state_ctrl_request, type_, "InitiatingMessage");
  return c.get<ric_sub_state_ctrl_request_s>();
}
ric_query_request_s& e2ap_elem_procs_o::init_msg_c::ric_query_request()
{
  assert_choice_type(types::ric_query_request, type_, "InitiatingMessage");
  return c.get<ric_query_request_s>();
}
ric_service_upd_s& e2ap_elem_procs_o::init_msg_c::ric_service_upd()
{
  assert_choice_type(types::ric_service_upd, type_, "InitiatingMessage");
  return c.get<ric_service_upd_s>();
}
ric_assist_request_s& e2ap_elem_procs_o::init_msg_c::ric_assist_request()
{
  assert_choice_type(types::ric_assist_request, type_, "InitiatingMessage");
  return c.get<ric_assist_request_s>();
}
ric_ctrl_request_s& e2ap_elem_procs_o::init_msg_c::ric_ctrl_request()
{
  assert_choice_type(types::ric_ctrl_request, type_, "InitiatingMessage");
  return c.get<ric_ctrl_request_s>();
}
ric_service_load_status_request_s& e2ap_elem_procs_o::init_msg_c::ric_service_load_status_request()
{
  assert_choice_type(types::ric_service_load_status_request, type_, "InitiatingMessage");
  return c.get<ric_service_load_status_request_s>();
}
e2setup_request_s& e2ap_elem_procs_o::init_msg_c::e2setup_request()
{
  assert_choice_type(types::e2setup_request, type_, "InitiatingMessage");
  return c.get<e2setup_request_s>();
}
e2node_cfg_upd_s& e2ap_elem_procs_o::init_msg_c::e2node_cfg_upd()
{
  assert_choice_type(types::e2node_cfg_upd, type_, "InitiatingMessage");
  return c.get<e2node_cfg_upd_s>();
}
e2conn_upd_s& e2ap_elem_procs_o::init_msg_c::e2conn_upd()
{
  assert_choice_type(types::e2conn_upd, type_, "InitiatingMessage");
  return c.get<e2conn_upd_s>();
}
reset_request_s& e2ap_elem_procs_o::init_msg_c::reset_request()
{
  assert_choice_type(types::reset_request, type_, "InitiatingMessage");
  return c.get<reset_request_s>();
}
e2_removal_request_s& e2ap_elem_procs_o::init_msg_c::e2_removal_request()
{
  assert_choice_type(types::e2_removal_request, type_, "InitiatingMessage");
  return c.get<e2_removal_request_s>();
}
e2_res_status_request_s& e2ap_elem_procs_o::init_msg_c::e2_res_status_request()
{
  assert_choice_type(types::e2_res_status_request, type_, "InitiatingMessage");
  return c.get<e2_res_status_request_s>();
}
ric_assist_ind_s& e2ap_elem_procs_o::init_msg_c::ric_assist_ind()
{
  assert_choice_type(types::ric_assist_ind, type_, "InitiatingMessage");
  return c.get<ric_assist_ind_s>();
}
ric_assist_halt_s& e2ap_elem_procs_o::init_msg_c::ric_assist_halt()
{
  assert_choice_type(types::ric_assist_halt, type_, "InitiatingMessage");
  return c.get<ric_assist_halt_s>();
}
ric_ind_s& e2ap_elem_procs_o::init_msg_c::ric_ind()
{
  assert_choice_type(types::ric_ind, type_, "InitiatingMessage");
  return c.get<ric_ind_s>();
}
ric_service_query_s& e2ap_elem_procs_o::init_msg_c::ric_service_query()
{
  assert_choice_type(types::ric_service_query, type_, "InitiatingMessage");
  return c.get<ric_service_query_s>();
}
ric_service_load_upd_s& e2ap_elem_procs_o::init_msg_c::ric_service_load_upd()
{
  assert_choice_type(types::ric_service_load_upd, type_, "InitiatingMessage");
  return c.get<ric_service_load_upd_s>();
}
error_ind_s& e2ap_elem_procs_o::init_msg_c::error_ind()
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
ric_sub_delete_required_s& e2ap_elem_procs_o::init_msg_c::ric_sub_delete_required()
{
  assert_choice_type(types::ric_sub_delete_required, type_, "InitiatingMessage");
  return c.get<ric_sub_delete_required_s>();
}
e2_res_status_upd_s& e2ap_elem_procs_o::init_msg_c::e2_res_status_upd()
{
  assert_choice_type(types::e2_res_status_upd, type_, "InitiatingMessage");
  return c.get<e2_res_status_upd_s>();
}
const ric_sub_request_s& e2ap_elem_procs_o::init_msg_c::ric_sub_request() const
{
  assert_choice_type(types::ric_sub_request, type_, "InitiatingMessage");
  return c.get<ric_sub_request_s>();
}
const ric_sub_audit_request_s& e2ap_elem_procs_o::init_msg_c::ric_sub_audit_request() const
{
  assert_choice_type(types::ric_sub_audit_request, type_, "InitiatingMessage");
  return c.get<ric_sub_audit_request_s>();
}
const ric_sub_delete_request_s& e2ap_elem_procs_o::init_msg_c::ric_sub_delete_request() const
{
  assert_choice_type(types::ric_sub_delete_request, type_, "InitiatingMessage");
  return c.get<ric_sub_delete_request_s>();
}
const ric_sub_mod_request_s& e2ap_elem_procs_o::init_msg_c::ric_sub_mod_request() const
{
  assert_choice_type(types::ric_sub_mod_request, type_, "InitiatingMessage");
  return c.get<ric_sub_mod_request_s>();
}
const ric_sub_mod_required_s& e2ap_elem_procs_o::init_msg_c::ric_sub_mod_required() const
{
  assert_choice_type(types::ric_sub_mod_required, type_, "InitiatingMessage");
  return c.get<ric_sub_mod_required_s>();
}
const ric_sub_state_ctrl_request_s& e2ap_elem_procs_o::init_msg_c::ric_sub_state_ctrl_request() const
{
  assert_choice_type(types::ric_sub_state_ctrl_request, type_, "InitiatingMessage");
  return c.get<ric_sub_state_ctrl_request_s>();
}
const ric_query_request_s& e2ap_elem_procs_o::init_msg_c::ric_query_request() const
{
  assert_choice_type(types::ric_query_request, type_, "InitiatingMessage");
  return c.get<ric_query_request_s>();
}
const ric_service_upd_s& e2ap_elem_procs_o::init_msg_c::ric_service_upd() const
{
  assert_choice_type(types::ric_service_upd, type_, "InitiatingMessage");
  return c.get<ric_service_upd_s>();
}
const ric_assist_request_s& e2ap_elem_procs_o::init_msg_c::ric_assist_request() const
{
  assert_choice_type(types::ric_assist_request, type_, "InitiatingMessage");
  return c.get<ric_assist_request_s>();
}
const ric_ctrl_request_s& e2ap_elem_procs_o::init_msg_c::ric_ctrl_request() const
{
  assert_choice_type(types::ric_ctrl_request, type_, "InitiatingMessage");
  return c.get<ric_ctrl_request_s>();
}
const ric_service_load_status_request_s& e2ap_elem_procs_o::init_msg_c::ric_service_load_status_request() const
{
  assert_choice_type(types::ric_service_load_status_request, type_, "InitiatingMessage");
  return c.get<ric_service_load_status_request_s>();
}
const e2setup_request_s& e2ap_elem_procs_o::init_msg_c::e2setup_request() const
{
  assert_choice_type(types::e2setup_request, type_, "InitiatingMessage");
  return c.get<e2setup_request_s>();
}
const e2node_cfg_upd_s& e2ap_elem_procs_o::init_msg_c::e2node_cfg_upd() const
{
  assert_choice_type(types::e2node_cfg_upd, type_, "InitiatingMessage");
  return c.get<e2node_cfg_upd_s>();
}
const e2conn_upd_s& e2ap_elem_procs_o::init_msg_c::e2conn_upd() const
{
  assert_choice_type(types::e2conn_upd, type_, "InitiatingMessage");
  return c.get<e2conn_upd_s>();
}
const reset_request_s& e2ap_elem_procs_o::init_msg_c::reset_request() const
{
  assert_choice_type(types::reset_request, type_, "InitiatingMessage");
  return c.get<reset_request_s>();
}
const e2_removal_request_s& e2ap_elem_procs_o::init_msg_c::e2_removal_request() const
{
  assert_choice_type(types::e2_removal_request, type_, "InitiatingMessage");
  return c.get<e2_removal_request_s>();
}
const e2_res_status_request_s& e2ap_elem_procs_o::init_msg_c::e2_res_status_request() const
{
  assert_choice_type(types::e2_res_status_request, type_, "InitiatingMessage");
  return c.get<e2_res_status_request_s>();
}
const ric_assist_ind_s& e2ap_elem_procs_o::init_msg_c::ric_assist_ind() const
{
  assert_choice_type(types::ric_assist_ind, type_, "InitiatingMessage");
  return c.get<ric_assist_ind_s>();
}
const ric_assist_halt_s& e2ap_elem_procs_o::init_msg_c::ric_assist_halt() const
{
  assert_choice_type(types::ric_assist_halt, type_, "InitiatingMessage");
  return c.get<ric_assist_halt_s>();
}
const ric_ind_s& e2ap_elem_procs_o::init_msg_c::ric_ind() const
{
  assert_choice_type(types::ric_ind, type_, "InitiatingMessage");
  return c.get<ric_ind_s>();
}
const ric_service_query_s& e2ap_elem_procs_o::init_msg_c::ric_service_query() const
{
  assert_choice_type(types::ric_service_query, type_, "InitiatingMessage");
  return c.get<ric_service_query_s>();
}
const ric_service_load_upd_s& e2ap_elem_procs_o::init_msg_c::ric_service_load_upd() const
{
  assert_choice_type(types::ric_service_load_upd, type_, "InitiatingMessage");
  return c.get<ric_service_load_upd_s>();
}
const error_ind_s& e2ap_elem_procs_o::init_msg_c::error_ind() const
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
const ric_sub_delete_required_s& e2ap_elem_procs_o::init_msg_c::ric_sub_delete_required() const
{
  assert_choice_type(types::ric_sub_delete_required, type_, "InitiatingMessage");
  return c.get<ric_sub_delete_required_s>();
}
const e2_res_status_upd_s& e2ap_elem_procs_o::init_msg_c::e2_res_status_upd() const
{
  assert_choice_type(types::e2_res_status_upd, type_, "InitiatingMessage");
  return c.get<e2_res_status_upd_s>();
}
void e2ap_elem_procs_o::init_msg_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_sub_request:
      j.write_fieldname("RICsubscriptionRequest");
      c.get<ric_sub_request_s>().to_json(j);
      break;
    case types::ric_sub_audit_request:
      j.write_fieldname("RICsubscriptionAuditRequest");
      c.get<ric_sub_audit_request_s>().to_json(j);
      break;
    case types::ric_sub_delete_request:
      j.write_fieldname("RICsubscriptionDeleteRequest");
      c.get<ric_sub_delete_request_s>().to_json(j);
      break;
    case types::ric_sub_mod_request:
      j.write_fieldname("RICsubscriptionModificationRequest");
      c.get<ric_sub_mod_request_s>().to_json(j);
      break;
    case types::ric_sub_mod_required:
      j.write_fieldname("RICsubscriptionModificationRequired");
      c.get<ric_sub_mod_required_s>().to_json(j);
      break;
    case types::ric_sub_state_ctrl_request:
      j.write_fieldname("RICsubscriptionStateControlRequest");
      c.get<ric_sub_state_ctrl_request_s>().to_json(j);
      break;
    case types::ric_query_request:
      j.write_fieldname("RICqueryRequest");
      c.get<ric_query_request_s>().to_json(j);
      break;
    case types::ric_service_upd:
      j.write_fieldname("RICserviceUpdate");
      c.get<ric_service_upd_s>().to_json(j);
      break;
    case types::ric_assist_request:
      j.write_fieldname("RICassistanceRequest");
      c.get<ric_assist_request_s>().to_json(j);
      break;
    case types::ric_ctrl_request:
      j.write_fieldname("RICcontrolRequest");
      c.get<ric_ctrl_request_s>().to_json(j);
      break;
    case types::ric_service_load_status_request:
      j.write_fieldname("RICserviceLoadStatusRequest");
      c.get<ric_service_load_status_request_s>().to_json(j);
      break;
    case types::e2setup_request:
      j.write_fieldname("E2setupRequest");
      c.get<e2setup_request_s>().to_json(j);
      break;
    case types::e2node_cfg_upd:
      j.write_fieldname("E2nodeConfigurationUpdate");
      c.get<e2node_cfg_upd_s>().to_json(j);
      break;
    case types::e2conn_upd:
      j.write_fieldname("E2connectionUpdate");
      c.get<e2conn_upd_s>().to_json(j);
      break;
    case types::reset_request:
      j.write_fieldname("ResetRequest");
      c.get<reset_request_s>().to_json(j);
      break;
    case types::e2_removal_request:
      j.write_fieldname("E2RemovalRequest");
      c.get<e2_removal_request_s>().to_json(j);
      break;
    case types::e2_res_status_request:
      j.write_fieldname("E2ResourceStatusRequest");
      c.get<e2_res_status_request_s>().to_json(j);
      break;
    case types::ric_assist_ind:
      j.write_fieldname("RICassistanceIndication");
      c.get<ric_assist_ind_s>().to_json(j);
      break;
    case types::ric_assist_halt:
      j.write_fieldname("RICassistanceHalt");
      c.get<ric_assist_halt_s>().to_json(j);
      break;
    case types::ric_ind:
      j.write_fieldname("RICindication");
      c.get<ric_ind_s>().to_json(j);
      break;
    case types::ric_service_query:
      j.write_fieldname("RICserviceQuery");
      c.get<ric_service_query_s>().to_json(j);
      break;
    case types::ric_service_load_upd:
      j.write_fieldname("RICserviceLoadUpdate");
      c.get<ric_service_load_upd_s>().to_json(j);
      break;
    case types::error_ind:
      j.write_fieldname("ErrorIndication");
      c.get<error_ind_s>().to_json(j);
      break;
    case types::ric_sub_delete_required:
      j.write_fieldname("RICsubscriptionDeleteRequired");
      c.get<ric_sub_delete_required_s>().to_json(j);
      break;
    case types::e2_res_status_upd:
      j.write_fieldname("E2ResourceStatusUpdate");
      c.get<e2_res_status_upd_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_o::init_msg_c");
  }
  j.end_obj();
}
OCUDUASN_CODE e2ap_elem_procs_o::init_msg_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_sub_request:
      HANDLE_CODE(c.get<ric_sub_request_s>().pack(bref));
      break;
    case types::ric_sub_audit_request:
      HANDLE_CODE(c.get<ric_sub_audit_request_s>().pack(bref));
      break;
    case types::ric_sub_delete_request:
      HANDLE_CODE(c.get<ric_sub_delete_request_s>().pack(bref));
      break;
    case types::ric_sub_mod_request:
      HANDLE_CODE(c.get<ric_sub_mod_request_s>().pack(bref));
      break;
    case types::ric_sub_mod_required:
      HANDLE_CODE(c.get<ric_sub_mod_required_s>().pack(bref));
      break;
    case types::ric_sub_state_ctrl_request:
      HANDLE_CODE(c.get<ric_sub_state_ctrl_request_s>().pack(bref));
      break;
    case types::ric_query_request:
      HANDLE_CODE(c.get<ric_query_request_s>().pack(bref));
      break;
    case types::ric_service_upd:
      HANDLE_CODE(c.get<ric_service_upd_s>().pack(bref));
      break;
    case types::ric_assist_request:
      HANDLE_CODE(c.get<ric_assist_request_s>().pack(bref));
      break;
    case types::ric_ctrl_request:
      HANDLE_CODE(c.get<ric_ctrl_request_s>().pack(bref));
      break;
    case types::ric_service_load_status_request:
      HANDLE_CODE(c.get<ric_service_load_status_request_s>().pack(bref));
      break;
    case types::e2setup_request:
      HANDLE_CODE(c.get<e2setup_request_s>().pack(bref));
      break;
    case types::e2node_cfg_upd:
      HANDLE_CODE(c.get<e2node_cfg_upd_s>().pack(bref));
      break;
    case types::e2conn_upd:
      HANDLE_CODE(c.get<e2conn_upd_s>().pack(bref));
      break;
    case types::reset_request:
      HANDLE_CODE(c.get<reset_request_s>().pack(bref));
      break;
    case types::e2_removal_request:
      HANDLE_CODE(c.get<e2_removal_request_s>().pack(bref));
      break;
    case types::e2_res_status_request:
      HANDLE_CODE(c.get<e2_res_status_request_s>().pack(bref));
      break;
    case types::ric_assist_ind:
      HANDLE_CODE(c.get<ric_assist_ind_s>().pack(bref));
      break;
    case types::ric_assist_halt:
      HANDLE_CODE(c.get<ric_assist_halt_s>().pack(bref));
      break;
    case types::ric_ind:
      HANDLE_CODE(c.get<ric_ind_s>().pack(bref));
      break;
    case types::ric_service_query:
      HANDLE_CODE(c.get<ric_service_query_s>().pack(bref));
      break;
    case types::ric_service_load_upd:
      HANDLE_CODE(c.get<ric_service_load_upd_s>().pack(bref));
      break;
    case types::error_ind:
      HANDLE_CODE(c.get<error_ind_s>().pack(bref));
      break;
    case types::ric_sub_delete_required:
      HANDLE_CODE(c.get<ric_sub_delete_required_s>().pack(bref));
      break;
    case types::e2_res_status_upd:
      HANDLE_CODE(c.get<e2_res_status_upd_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_o::init_msg_c");
      return OCUDUASN_ERROR_ENCODE_FAIL;
  }
  return OCUDUASN_SUCCESS;
}
OCUDUASN_CODE e2ap_elem_procs_o::init_msg_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_sub_request:
      HANDLE_CODE(c.get<ric_sub_request_s>().unpack(bref));
      break;
    case types::ric_sub_audit_request:
      HANDLE_CODE(c.get<ric_sub_audit_request_s>().unpack(bref));
      break;
    case types::ric_sub_delete_request:
      HANDLE_CODE(c.get<ric_sub_delete_request_s>().unpack(bref));
      break;
    case types::ric_sub_mod_request:
      HANDLE_CODE(c.get<ric_sub_mod_request_s>().unpack(bref));
      break;
    case types::ric_sub_mod_required:
      HANDLE_CODE(c.get<ric_sub_mod_required_s>().unpack(bref));
      break;
    case types::ric_sub_state_ctrl_request:
      HANDLE_CODE(c.get<ric_sub_state_ctrl_request_s>().unpack(bref));
      break;
    case types::ric_query_request:
      HANDLE_CODE(c.get<ric_query_request_s>().unpack(bref));
      break;
    case types::ric_service_upd:
      HANDLE_CODE(c.get<ric_service_upd_s>().unpack(bref));
      break;
    case types::ric_assist_request:
      HANDLE_CODE(c.get<ric_assist_request_s>().unpack(bref));
      break;
    case types::ric_ctrl_request:
      HANDLE_CODE(c.get<ric_ctrl_request_s>().unpack(bref));
      break;
    case types::ric_service_load_status_request:
      HANDLE_CODE(c.get<ric_service_load_status_request_s>().unpack(bref));
      break;
    case types::e2setup_request:
      HANDLE_CODE(c.get<e2setup_request_s>().unpack(bref));
      break;
    case types::e2node_cfg_upd:
      HANDLE_CODE(c.get<e2node_cfg_upd_s>().unpack(bref));
      break;
    case types::e2conn_upd:
      HANDLE_CODE(c.get<e2conn_upd_s>().unpack(bref));
      break;
    case types::reset_request:
      HANDLE_CODE(c.get<reset_request_s>().unpack(bref));
      break;
    case types::e2_removal_request:
      HANDLE_CODE(c.get<e2_removal_request_s>().unpack(bref));
      break;
    case types::e2_res_status_request:
      HANDLE_CODE(c.get<e2_res_status_request_s>().unpack(bref));
      break;
    case types::ric_assist_ind:
      HANDLE_CODE(c.get<ric_assist_ind_s>().unpack(bref));
      break;
    case types::ric_assist_halt:
      HANDLE_CODE(c.get<ric_assist_halt_s>().unpack(bref));
      break;
    case types::ric_ind:
      HANDLE_CODE(c.get<ric_ind_s>().unpack(bref));
      break;
    case types::ric_service_query:
      HANDLE_CODE(c.get<ric_service_query_s>().unpack(bref));
      break;
    case types::ric_service_load_upd:
      HANDLE_CODE(c.get<ric_service_load_upd_s>().unpack(bref));
      break;
    case types::error_ind:
      HANDLE_CODE(c.get<error_ind_s>().unpack(bref));
      break;
    case types::ric_sub_delete_required:
      HANDLE_CODE(c.get<ric_sub_delete_required_s>().unpack(bref));
      break;
    case types::e2_res_status_upd:
      HANDLE_CODE(c.get<e2_res_status_upd_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_o::init_msg_c");
      return OCUDUASN_ERROR_DECODE_FAIL;
  }
  return OCUDUASN_SUCCESS;
}

const char* e2ap_elem_procs_o::init_msg_c::types_opts::to_string() const
{
  static const char* names[] = {"RICsubscriptionRequest",
                                "RICsubscriptionAuditRequest",
                                "RICsubscriptionDeleteRequest",
                                "RICsubscriptionModificationRequest",
                                "RICsubscriptionModificationRequired",
                                "RICsubscriptionStateControlRequest",
                                "RICqueryRequest",
                                "RICserviceUpdate",
                                "RICassistanceRequest",
                                "RICcontrolRequest",
                                "RICserviceLoadStatusRequest",
                                "E2setupRequest",
                                "E2nodeConfigurationUpdate",
                                "E2connectionUpdate",
                                "ResetRequest",
                                "E2RemovalRequest",
                                "E2ResourceStatusRequest",
                                "RICassistanceIndication",
                                "RICassistanceHalt",
                                "RICindication",
                                "RICserviceQuery",
                                "RICserviceLoadUpdate",
                                "ErrorIndication",
                                "RICsubscriptionDeleteRequired",
                                "E2ResourceStatusUpdate"};
  return convert_enum_idx(names, 25, value, "e2ap_elem_procs_o::init_msg_c::types");
}

// SuccessfulOutcome ::= OPEN TYPE
void e2ap_elem_procs_o::successful_outcome_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_sub_resp:
      c = ric_sub_resp_s{};
      break;
    case types::ric_sub_audit_resp:
      c = ric_sub_audit_resp_s{};
      break;
    case types::ric_sub_delete_resp:
      c = ric_sub_delete_resp_s{};
      break;
    case types::ric_sub_mod_resp:
      c = ric_sub_mod_resp_s{};
      break;
    case types::ric_sub_mod_confirm:
      c = ric_sub_mod_confirm_s{};
      break;
    case types::ric_sub_state_ctrl_resp:
      c = ric_sub_state_ctrl_resp_s{};
      break;
    case types::ric_query_resp:
      c = ric_query_resp_s{};
      break;
    case types::ric_service_upd_ack:
      c = ric_service_upd_ack_s{};
      break;
    case types::ric_assist_resp:
      c = ric_assist_resp_s{};
      break;
    case types::ric_ctrl_ack:
      c = ric_ctrl_ack_s{};
      break;
    case types::ric_service_load_status_resp:
      c = ric_service_load_status_resp_s{};
      break;
    case types::e2setup_resp:
      c = e2setup_resp_s{};
      break;
    case types::e2node_cfg_upd_ack:
      c = e2node_cfg_upd_ack_s{};
      break;
    case types::e2conn_upd_ack:
      c = e2conn_upd_ack_s{};
      break;
    case types::reset_resp:
      c = reset_resp_s{};
      break;
    case types::e2_removal_resp:
      c = e2_removal_resp_s{};
      break;
    case types::e2_res_status_resp:
      c = e2_res_status_resp_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_o::successful_outcome_c");
  }
}
ric_sub_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_resp()
{
  assert_choice_type(types::ric_sub_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_resp_s>();
}
ric_sub_audit_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_audit_resp()
{
  assert_choice_type(types::ric_sub_audit_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_audit_resp_s>();
}
ric_sub_delete_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_delete_resp()
{
  assert_choice_type(types::ric_sub_delete_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_delete_resp_s>();
}
ric_sub_mod_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_mod_resp()
{
  assert_choice_type(types::ric_sub_mod_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_mod_resp_s>();
}
ric_sub_mod_confirm_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_mod_confirm()
{
  assert_choice_type(types::ric_sub_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<ric_sub_mod_confirm_s>();
}
ric_sub_state_ctrl_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_state_ctrl_resp()
{
  assert_choice_type(types::ric_sub_state_ctrl_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_state_ctrl_resp_s>();
}
ric_query_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_query_resp()
{
  assert_choice_type(types::ric_query_resp, type_, "SuccessfulOutcome");
  return c.get<ric_query_resp_s>();
}
ric_service_upd_ack_s& e2ap_elem_procs_o::successful_outcome_c::ric_service_upd_ack()
{
  assert_choice_type(types::ric_service_upd_ack, type_, "SuccessfulOutcome");
  return c.get<ric_service_upd_ack_s>();
}
ric_assist_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_assist_resp()
{
  assert_choice_type(types::ric_assist_resp, type_, "SuccessfulOutcome");
  return c.get<ric_assist_resp_s>();
}
ric_ctrl_ack_s& e2ap_elem_procs_o::successful_outcome_c::ric_ctrl_ack()
{
  assert_choice_type(types::ric_ctrl_ack, type_, "SuccessfulOutcome");
  return c.get<ric_ctrl_ack_s>();
}
ric_service_load_status_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_service_load_status_resp()
{
  assert_choice_type(types::ric_service_load_status_resp, type_, "SuccessfulOutcome");
  return c.get<ric_service_load_status_resp_s>();
}
e2setup_resp_s& e2ap_elem_procs_o::successful_outcome_c::e2setup_resp()
{
  assert_choice_type(types::e2setup_resp, type_, "SuccessfulOutcome");
  return c.get<e2setup_resp_s>();
}
e2node_cfg_upd_ack_s& e2ap_elem_procs_o::successful_outcome_c::e2node_cfg_upd_ack()
{
  assert_choice_type(types::e2node_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<e2node_cfg_upd_ack_s>();
}
e2conn_upd_ack_s& e2ap_elem_procs_o::successful_outcome_c::e2conn_upd_ack()
{
  assert_choice_type(types::e2conn_upd_ack, type_, "SuccessfulOutcome");
  return c.get<e2conn_upd_ack_s>();
}
reset_resp_s& e2ap_elem_procs_o::successful_outcome_c::reset_resp()
{
  assert_choice_type(types::reset_resp, type_, "SuccessfulOutcome");
  return c.get<reset_resp_s>();
}
e2_removal_resp_s& e2ap_elem_procs_o::successful_outcome_c::e2_removal_resp()
{
  assert_choice_type(types::e2_removal_resp, type_, "SuccessfulOutcome");
  return c.get<e2_removal_resp_s>();
}
e2_res_status_resp_s& e2ap_elem_procs_o::successful_outcome_c::e2_res_status_resp()
{
  assert_choice_type(types::e2_res_status_resp, type_, "SuccessfulOutcome");
  return c.get<e2_res_status_resp_s>();
}
const ric_sub_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_resp() const
{
  assert_choice_type(types::ric_sub_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_resp_s>();
}
const ric_sub_audit_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_audit_resp() const
{
  assert_choice_type(types::ric_sub_audit_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_audit_resp_s>();
}
const ric_sub_delete_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_delete_resp() const
{
  assert_choice_type(types::ric_sub_delete_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_delete_resp_s>();
}
const ric_sub_mod_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_mod_resp() const
{
  assert_choice_type(types::ric_sub_mod_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_mod_resp_s>();
}
const ric_sub_mod_confirm_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_mod_confirm() const
{
  assert_choice_type(types::ric_sub_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<ric_sub_mod_confirm_s>();
}
const ric_sub_state_ctrl_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_state_ctrl_resp() const
{
  assert_choice_type(types::ric_sub_state_ctrl_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_state_ctrl_resp_s>();
}
const ric_query_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_query_resp() const
{
  assert_choice_type(types::ric_query_resp, type_, "SuccessfulOutcome");
  return c.get<ric_query_resp_s>();
}
const ric_service_upd_ack_s& e2ap_elem_procs_o::successful_outcome_c::ric_service_upd_ack() const
{
  assert_choice_type(types::ric_service_upd_ack, type_, "SuccessfulOutcome");
  return c.get<ric_service_upd_ack_s>();
}
const ric_assist_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_assist_resp() const
{
  assert_choice_type(types::ric_assist_resp, type_, "SuccessfulOutcome");
  return c.get<ric_assist_resp_s>();
}
const ric_ctrl_ack_s& e2ap_elem_procs_o::successful_outcome_c::ric_ctrl_ack() const
{
  assert_choice_type(types::ric_ctrl_ack, type_, "SuccessfulOutcome");
  return c.get<ric_ctrl_ack_s>();
}
const ric_service_load_status_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_service_load_status_resp() const
{
  assert_choice_type(types::ric_service_load_status_resp, type_, "SuccessfulOutcome");
  return c.get<ric_service_load_status_resp_s>();
}
const e2setup_resp_s& e2ap_elem_procs_o::successful_outcome_c::e2setup_resp() const
{
  assert_choice_type(types::e2setup_resp, type_, "SuccessfulOutcome");
  return c.get<e2setup_resp_s>();
}
const e2node_cfg_upd_ack_s& e2ap_elem_procs_o::successful_outcome_c::e2node_cfg_upd_ack() const
{
  assert_choice_type(types::e2node_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<e2node_cfg_upd_ack_s>();
}
const e2conn_upd_ack_s& e2ap_elem_procs_o::successful_outcome_c::e2conn_upd_ack() const
{
  assert_choice_type(types::e2conn_upd_ack, type_, "SuccessfulOutcome");
  return c.get<e2conn_upd_ack_s>();
}
const reset_resp_s& e2ap_elem_procs_o::successful_outcome_c::reset_resp() const
{
  assert_choice_type(types::reset_resp, type_, "SuccessfulOutcome");
  return c.get<reset_resp_s>();
}
const e2_removal_resp_s& e2ap_elem_procs_o::successful_outcome_c::e2_removal_resp() const
{
  assert_choice_type(types::e2_removal_resp, type_, "SuccessfulOutcome");
  return c.get<e2_removal_resp_s>();
}
const e2_res_status_resp_s& e2ap_elem_procs_o::successful_outcome_c::e2_res_status_resp() const
{
  assert_choice_type(types::e2_res_status_resp, type_, "SuccessfulOutcome");
  return c.get<e2_res_status_resp_s>();
}
void e2ap_elem_procs_o::successful_outcome_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_sub_resp:
      j.write_fieldname("RICsubscriptionResponse");
      c.get<ric_sub_resp_s>().to_json(j);
      break;
    case types::ric_sub_audit_resp:
      j.write_fieldname("RICsubscriptionAuditResponse");
      c.get<ric_sub_audit_resp_s>().to_json(j);
      break;
    case types::ric_sub_delete_resp:
      j.write_fieldname("RICsubscriptionDeleteResponse");
      c.get<ric_sub_delete_resp_s>().to_json(j);
      break;
    case types::ric_sub_mod_resp:
      j.write_fieldname("RICsubscriptionModificationResponse");
      c.get<ric_sub_mod_resp_s>().to_json(j);
      break;
    case types::ric_sub_mod_confirm:
      j.write_fieldname("RICsubscriptionModificationConfirm");
      c.get<ric_sub_mod_confirm_s>().to_json(j);
      break;
    case types::ric_sub_state_ctrl_resp:
      j.write_fieldname("RICsubscriptionStateControlResponse");
      c.get<ric_sub_state_ctrl_resp_s>().to_json(j);
      break;
    case types::ric_query_resp:
      j.write_fieldname("RICqueryResponse");
      c.get<ric_query_resp_s>().to_json(j);
      break;
    case types::ric_service_upd_ack:
      j.write_fieldname("RICserviceUpdateAcknowledge");
      c.get<ric_service_upd_ack_s>().to_json(j);
      break;
    case types::ric_assist_resp:
      j.write_fieldname("RICassistanceResponse");
      c.get<ric_assist_resp_s>().to_json(j);
      break;
    case types::ric_ctrl_ack:
      j.write_fieldname("RICcontrolAcknowledge");
      c.get<ric_ctrl_ack_s>().to_json(j);
      break;
    case types::ric_service_load_status_resp:
      j.write_fieldname("RICserviceLoadStatusResponse");
      c.get<ric_service_load_status_resp_s>().to_json(j);
      break;
    case types::e2setup_resp:
      j.write_fieldname("E2setupResponse");
      c.get<e2setup_resp_s>().to_json(j);
      break;
    case types::e2node_cfg_upd_ack:
      j.write_fieldname("E2nodeConfigurationUpdateAcknowledge");
      c.get<e2node_cfg_upd_ack_s>().to_json(j);
      break;
    case types::e2conn_upd_ack:
      j.write_fieldname("E2connectionUpdateAcknowledge");
      c.get<e2conn_upd_ack_s>().to_json(j);
      break;
    case types::reset_resp:
      j.write_fieldname("ResetResponse");
      c.get<reset_resp_s>().to_json(j);
      break;
    case types::e2_removal_resp:
      j.write_fieldname("E2RemovalResponse");
      c.get<e2_removal_resp_s>().to_json(j);
      break;
    case types::e2_res_status_resp:
      j.write_fieldname("E2ResourceStatusResponse");
      c.get<e2_res_status_resp_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_o::successful_outcome_c");
  }
  j.end_obj();
}
OCUDUASN_CODE e2ap_elem_procs_o::successful_outcome_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_sub_resp:
      HANDLE_CODE(c.get<ric_sub_resp_s>().pack(bref));
      break;
    case types::ric_sub_audit_resp:
      HANDLE_CODE(c.get<ric_sub_audit_resp_s>().pack(bref));
      break;
    case types::ric_sub_delete_resp:
      HANDLE_CODE(c.get<ric_sub_delete_resp_s>().pack(bref));
      break;
    case types::ric_sub_mod_resp:
      HANDLE_CODE(c.get<ric_sub_mod_resp_s>().pack(bref));
      break;
    case types::ric_sub_mod_confirm:
      HANDLE_CODE(c.get<ric_sub_mod_confirm_s>().pack(bref));
      break;
    case types::ric_sub_state_ctrl_resp:
      HANDLE_CODE(c.get<ric_sub_state_ctrl_resp_s>().pack(bref));
      break;
    case types::ric_query_resp:
      HANDLE_CODE(c.get<ric_query_resp_s>().pack(bref));
      break;
    case types::ric_service_upd_ack:
      HANDLE_CODE(c.get<ric_service_upd_ack_s>().pack(bref));
      break;
    case types::ric_assist_resp:
      HANDLE_CODE(c.get<ric_assist_resp_s>().pack(bref));
      break;
    case types::ric_ctrl_ack:
      HANDLE_CODE(c.get<ric_ctrl_ack_s>().pack(bref));
      break;
    case types::ric_service_load_status_resp:
      HANDLE_CODE(c.get<ric_service_load_status_resp_s>().pack(bref));
      break;
    case types::e2setup_resp:
      HANDLE_CODE(c.get<e2setup_resp_s>().pack(bref));
      break;
    case types::e2node_cfg_upd_ack:
      HANDLE_CODE(c.get<e2node_cfg_upd_ack_s>().pack(bref));
      break;
    case types::e2conn_upd_ack:
      HANDLE_CODE(c.get<e2conn_upd_ack_s>().pack(bref));
      break;
    case types::reset_resp:
      HANDLE_CODE(c.get<reset_resp_s>().pack(bref));
      break;
    case types::e2_removal_resp:
      HANDLE_CODE(c.get<e2_removal_resp_s>().pack(bref));
      break;
    case types::e2_res_status_resp:
      HANDLE_CODE(c.get<e2_res_status_resp_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_o::successful_outcome_c");
      return OCUDUASN_ERROR_ENCODE_FAIL;
  }
  return OCUDUASN_SUCCESS;
}
OCUDUASN_CODE e2ap_elem_procs_o::successful_outcome_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_sub_resp:
      HANDLE_CODE(c.get<ric_sub_resp_s>().unpack(bref));
      break;
    case types::ric_sub_audit_resp:
      HANDLE_CODE(c.get<ric_sub_audit_resp_s>().unpack(bref));
      break;
    case types::ric_sub_delete_resp:
      HANDLE_CODE(c.get<ric_sub_delete_resp_s>().unpack(bref));
      break;
    case types::ric_sub_mod_resp:
      HANDLE_CODE(c.get<ric_sub_mod_resp_s>().unpack(bref));
      break;
    case types::ric_sub_mod_confirm:
      HANDLE_CODE(c.get<ric_sub_mod_confirm_s>().unpack(bref));
      break;
    case types::ric_sub_state_ctrl_resp:
      HANDLE_CODE(c.get<ric_sub_state_ctrl_resp_s>().unpack(bref));
      break;
    case types::ric_query_resp:
      HANDLE_CODE(c.get<ric_query_resp_s>().unpack(bref));
      break;
    case types::ric_service_upd_ack:
      HANDLE_CODE(c.get<ric_service_upd_ack_s>().unpack(bref));
      break;
    case types::ric_assist_resp:
      HANDLE_CODE(c.get<ric_assist_resp_s>().unpack(bref));
      break;
    case types::ric_ctrl_ack:
      HANDLE_CODE(c.get<ric_ctrl_ack_s>().unpack(bref));
      break;
    case types::ric_service_load_status_resp:
      HANDLE_CODE(c.get<ric_service_load_status_resp_s>().unpack(bref));
      break;
    case types::e2setup_resp:
      HANDLE_CODE(c.get<e2setup_resp_s>().unpack(bref));
      break;
    case types::e2node_cfg_upd_ack:
      HANDLE_CODE(c.get<e2node_cfg_upd_ack_s>().unpack(bref));
      break;
    case types::e2conn_upd_ack:
      HANDLE_CODE(c.get<e2conn_upd_ack_s>().unpack(bref));
      break;
    case types::reset_resp:
      HANDLE_CODE(c.get<reset_resp_s>().unpack(bref));
      break;
    case types::e2_removal_resp:
      HANDLE_CODE(c.get<e2_removal_resp_s>().unpack(bref));
      break;
    case types::e2_res_status_resp:
      HANDLE_CODE(c.get<e2_res_status_resp_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_o::successful_outcome_c");
      return OCUDUASN_ERROR_DECODE_FAIL;
  }
  return OCUDUASN_SUCCESS;
}

const char* e2ap_elem_procs_o::successful_outcome_c::types_opts::to_string() const
{
  static const char* names[] = {"RICsubscriptionResponse",
                                "RICsubscriptionAuditResponse",
                                "RICsubscriptionDeleteResponse",
                                "RICsubscriptionModificationResponse",
                                "RICsubscriptionModificationConfirm",
                                "RICsubscriptionStateControlResponse",
                                "RICqueryResponse",
                                "RICserviceUpdateAcknowledge",
                                "RICassistanceResponse",
                                "RICcontrolAcknowledge",
                                "RICserviceLoadStatusResponse",
                                "E2setupResponse",
                                "E2nodeConfigurationUpdateAcknowledge",
                                "E2connectionUpdateAcknowledge",
                                "ResetResponse",
                                "E2RemovalResponse",
                                "E2ResourceStatusResponse"};
  return convert_enum_idx(names, 17, value, "e2ap_elem_procs_o::successful_outcome_c::types");
}

// UnsuccessfulOutcome ::= OPEN TYPE
void e2ap_elem_procs_o::unsuccessful_outcome_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_sub_fail:
      c = ric_sub_fail_s{};
      break;
    case types::ric_sub_audit_fail:
      c = ric_sub_audit_fail_s{};
      break;
    case types::ric_sub_delete_fail:
      c = ric_sub_delete_fail_s{};
      break;
    case types::ric_sub_mod_fail:
      c = ric_sub_mod_fail_s{};
      break;
    case types::ric_sub_mod_refuse:
      c = ric_sub_mod_refuse_s{};
      break;
    case types::ric_sub_state_ctrl_fail:
      c = ric_sub_state_ctrl_fail_s{};
      break;
    case types::ric_query_fail:
      c = ric_query_fail_s{};
      break;
    case types::ric_service_upd_fail:
      c = ric_service_upd_fail_s{};
      break;
    case types::ric_assist_fail:
      c = ric_assist_fail_s{};
      break;
    case types::ric_ctrl_fail:
      c = ric_ctrl_fail_s{};
      break;
    case types::ric_service_load_status_fail:
      c = ric_service_load_status_fail_s{};
      break;
    case types::e2setup_fail:
      c = e2setup_fail_s{};
      break;
    case types::e2node_cfg_upd_fail:
      c = e2node_cfg_upd_fail_s{};
      break;
    case types::e2conn_upd_fail:
      c = e2conn_upd_fail_s{};
      break;
    case types::e2_removal_fail:
      c = e2_removal_fail_s{};
      break;
    case types::e2_res_status_fail:
      c = e2_res_status_fail_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_o::unsuccessful_outcome_c");
  }
}
ric_sub_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_fail()
{
  assert_choice_type(types::ric_sub_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_fail_s>();
}
ric_sub_audit_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_audit_fail()
{
  assert_choice_type(types::ric_sub_audit_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_audit_fail_s>();
}
ric_sub_delete_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_delete_fail()
{
  assert_choice_type(types::ric_sub_delete_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_delete_fail_s>();
}
ric_sub_mod_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_mod_fail()
{
  assert_choice_type(types::ric_sub_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_mod_fail_s>();
}
ric_sub_mod_refuse_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_mod_refuse()
{
  assert_choice_type(types::ric_sub_mod_refuse, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_mod_refuse_s>();
}
ric_sub_state_ctrl_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_state_ctrl_fail()
{
  assert_choice_type(types::ric_sub_state_ctrl_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_state_ctrl_fail_s>();
}
ric_query_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_query_fail()
{
  assert_choice_type(types::ric_query_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_query_fail_s>();
}
ric_service_upd_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_service_upd_fail()
{
  assert_choice_type(types::ric_service_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_service_upd_fail_s>();
}
ric_assist_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_assist_fail()
{
  assert_choice_type(types::ric_assist_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_assist_fail_s>();
}
ric_ctrl_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_ctrl_fail()
{
  assert_choice_type(types::ric_ctrl_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_ctrl_fail_s>();
}
ric_service_load_status_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_service_load_status_fail()
{
  assert_choice_type(types::ric_service_load_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_service_load_status_fail_s>();
}
e2setup_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2setup_fail()
{
  assert_choice_type(types::e2setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2setup_fail_s>();
}
e2node_cfg_upd_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2node_cfg_upd_fail()
{
  assert_choice_type(types::e2node_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2node_cfg_upd_fail_s>();
}
e2conn_upd_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2conn_upd_fail()
{
  assert_choice_type(types::e2conn_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2conn_upd_fail_s>();
}
e2_removal_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2_removal_fail()
{
  assert_choice_type(types::e2_removal_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2_removal_fail_s>();
}
e2_res_status_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2_res_status_fail()
{
  assert_choice_type(types::e2_res_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2_res_status_fail_s>();
}
const ric_sub_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_fail() const
{
  assert_choice_type(types::ric_sub_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_fail_s>();
}
const ric_sub_audit_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_audit_fail() const
{
  assert_choice_type(types::ric_sub_audit_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_audit_fail_s>();
}
const ric_sub_delete_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_delete_fail() const
{
  assert_choice_type(types::ric_sub_delete_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_delete_fail_s>();
}
const ric_sub_mod_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_mod_fail() const
{
  assert_choice_type(types::ric_sub_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_mod_fail_s>();
}
const ric_sub_mod_refuse_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_mod_refuse() const
{
  assert_choice_type(types::ric_sub_mod_refuse, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_mod_refuse_s>();
}
const ric_sub_state_ctrl_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_state_ctrl_fail() const
{
  assert_choice_type(types::ric_sub_state_ctrl_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_state_ctrl_fail_s>();
}
const ric_query_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_query_fail() const
{
  assert_choice_type(types::ric_query_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_query_fail_s>();
}
const ric_service_upd_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_service_upd_fail() const
{
  assert_choice_type(types::ric_service_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_service_upd_fail_s>();
}
const ric_assist_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_assist_fail() const
{
  assert_choice_type(types::ric_assist_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_assist_fail_s>();
}
const ric_ctrl_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_ctrl_fail() const
{
  assert_choice_type(types::ric_ctrl_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_ctrl_fail_s>();
}
const ric_service_load_status_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_service_load_status_fail() const
{
  assert_choice_type(types::ric_service_load_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_service_load_status_fail_s>();
}
const e2setup_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2setup_fail() const
{
  assert_choice_type(types::e2setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2setup_fail_s>();
}
const e2node_cfg_upd_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2node_cfg_upd_fail() const
{
  assert_choice_type(types::e2node_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2node_cfg_upd_fail_s>();
}
const e2conn_upd_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2conn_upd_fail() const
{
  assert_choice_type(types::e2conn_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2conn_upd_fail_s>();
}
const e2_removal_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2_removal_fail() const
{
  assert_choice_type(types::e2_removal_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2_removal_fail_s>();
}
const e2_res_status_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2_res_status_fail() const
{
  assert_choice_type(types::e2_res_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2_res_status_fail_s>();
}
void e2ap_elem_procs_o::unsuccessful_outcome_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_sub_fail:
      j.write_fieldname("RICsubscriptionFailure");
      c.get<ric_sub_fail_s>().to_json(j);
      break;
    case types::ric_sub_audit_fail:
      j.write_fieldname("RICsubscriptionAuditFailure");
      c.get<ric_sub_audit_fail_s>().to_json(j);
      break;
    case types::ric_sub_delete_fail:
      j.write_fieldname("RICsubscriptionDeleteFailure");
      c.get<ric_sub_delete_fail_s>().to_json(j);
      break;
    case types::ric_sub_mod_fail:
      j.write_fieldname("RICsubscriptionModificationFailure");
      c.get<ric_sub_mod_fail_s>().to_json(j);
      break;
    case types::ric_sub_mod_refuse:
      j.write_fieldname("RICsubscriptionModificationRefuse");
      c.get<ric_sub_mod_refuse_s>().to_json(j);
      break;
    case types::ric_sub_state_ctrl_fail:
      j.write_fieldname("RICsubscriptionStateControlFailure");
      c.get<ric_sub_state_ctrl_fail_s>().to_json(j);
      break;
    case types::ric_query_fail:
      j.write_fieldname("RICqueryFailure");
      c.get<ric_query_fail_s>().to_json(j);
      break;
    case types::ric_service_upd_fail:
      j.write_fieldname("RICserviceUpdateFailure");
      c.get<ric_service_upd_fail_s>().to_json(j);
      break;
    case types::ric_assist_fail:
      j.write_fieldname("RICassistanceFailure");
      c.get<ric_assist_fail_s>().to_json(j);
      break;
    case types::ric_ctrl_fail:
      j.write_fieldname("RICcontrolFailure");
      c.get<ric_ctrl_fail_s>().to_json(j);
      break;
    case types::ric_service_load_status_fail:
      j.write_fieldname("RICserviceLoadStatusFailure");
      c.get<ric_service_load_status_fail_s>().to_json(j);
      break;
    case types::e2setup_fail:
      j.write_fieldname("E2setupFailure");
      c.get<e2setup_fail_s>().to_json(j);
      break;
    case types::e2node_cfg_upd_fail:
      j.write_fieldname("E2nodeConfigurationUpdateFailure");
      c.get<e2node_cfg_upd_fail_s>().to_json(j);
      break;
    case types::e2conn_upd_fail:
      j.write_fieldname("E2connectionUpdateFailure");
      c.get<e2conn_upd_fail_s>().to_json(j);
      break;
    case types::e2_removal_fail:
      j.write_fieldname("E2RemovalFailure");
      c.get<e2_removal_fail_s>().to_json(j);
      break;
    case types::e2_res_status_fail:
      j.write_fieldname("E2ResourceStatusFailure");
      c.get<e2_res_status_fail_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_o::unsuccessful_outcome_c");
  }
  j.end_obj();
}
OCUDUASN_CODE e2ap_elem_procs_o::unsuccessful_outcome_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_sub_fail:
      HANDLE_CODE(c.get<ric_sub_fail_s>().pack(bref));
      break;
    case types::ric_sub_audit_fail:
      HANDLE_CODE(c.get<ric_sub_audit_fail_s>().pack(bref));
      break;
    case types::ric_sub_delete_fail:
      HANDLE_CODE(c.get<ric_sub_delete_fail_s>().pack(bref));
      break;
    case types::ric_sub_mod_fail:
      HANDLE_CODE(c.get<ric_sub_mod_fail_s>().pack(bref));
      break;
    case types::ric_sub_mod_refuse:
      HANDLE_CODE(c.get<ric_sub_mod_refuse_s>().pack(bref));
      break;
    case types::ric_sub_state_ctrl_fail:
      HANDLE_CODE(c.get<ric_sub_state_ctrl_fail_s>().pack(bref));
      break;
    case types::ric_query_fail:
      HANDLE_CODE(c.get<ric_query_fail_s>().pack(bref));
      break;
    case types::ric_service_upd_fail:
      HANDLE_CODE(c.get<ric_service_upd_fail_s>().pack(bref));
      break;
    case types::ric_assist_fail:
      HANDLE_CODE(c.get<ric_assist_fail_s>().pack(bref));
      break;
    case types::ric_ctrl_fail:
      HANDLE_CODE(c.get<ric_ctrl_fail_s>().pack(bref));
      break;
    case types::ric_service_load_status_fail:
      HANDLE_CODE(c.get<ric_service_load_status_fail_s>().pack(bref));
      break;
    case types::e2setup_fail:
      HANDLE_CODE(c.get<e2setup_fail_s>().pack(bref));
      break;
    case types::e2node_cfg_upd_fail:
      HANDLE_CODE(c.get<e2node_cfg_upd_fail_s>().pack(bref));
      break;
    case types::e2conn_upd_fail:
      HANDLE_CODE(c.get<e2conn_upd_fail_s>().pack(bref));
      break;
    case types::e2_removal_fail:
      HANDLE_CODE(c.get<e2_removal_fail_s>().pack(bref));
      break;
    case types::e2_res_status_fail:
      HANDLE_CODE(c.get<e2_res_status_fail_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_o::unsuccessful_outcome_c");
      return OCUDUASN_ERROR_ENCODE_FAIL;
  }
  return OCUDUASN_SUCCESS;
}
OCUDUASN_CODE e2ap_elem_procs_o::unsuccessful_outcome_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_sub_fail:
      HANDLE_CODE(c.get<ric_sub_fail_s>().unpack(bref));
      break;
    case types::ric_sub_audit_fail:
      HANDLE_CODE(c.get<ric_sub_audit_fail_s>().unpack(bref));
      break;
    case types::ric_sub_delete_fail:
      HANDLE_CODE(c.get<ric_sub_delete_fail_s>().unpack(bref));
      break;
    case types::ric_sub_mod_fail:
      HANDLE_CODE(c.get<ric_sub_mod_fail_s>().unpack(bref));
      break;
    case types::ric_sub_mod_refuse:
      HANDLE_CODE(c.get<ric_sub_mod_refuse_s>().unpack(bref));
      break;
    case types::ric_sub_state_ctrl_fail:
      HANDLE_CODE(c.get<ric_sub_state_ctrl_fail_s>().unpack(bref));
      break;
    case types::ric_query_fail:
      HANDLE_CODE(c.get<ric_query_fail_s>().unpack(bref));
      break;
    case types::ric_service_upd_fail:
      HANDLE_CODE(c.get<ric_service_upd_fail_s>().unpack(bref));
      break;
    case types::ric_assist_fail:
      HANDLE_CODE(c.get<ric_assist_fail_s>().unpack(bref));
      break;
    case types::ric_ctrl_fail:
      HANDLE_CODE(c.get<ric_ctrl_fail_s>().unpack(bref));
      break;
    case types::ric_service_load_status_fail:
      HANDLE_CODE(c.get<ric_service_load_status_fail_s>().unpack(bref));
      break;
    case types::e2setup_fail:
      HANDLE_CODE(c.get<e2setup_fail_s>().unpack(bref));
      break;
    case types::e2node_cfg_upd_fail:
      HANDLE_CODE(c.get<e2node_cfg_upd_fail_s>().unpack(bref));
      break;
    case types::e2conn_upd_fail:
      HANDLE_CODE(c.get<e2conn_upd_fail_s>().unpack(bref));
      break;
    case types::e2_removal_fail:
      HANDLE_CODE(c.get<e2_removal_fail_s>().unpack(bref));
      break;
    case types::e2_res_status_fail:
      HANDLE_CODE(c.get<e2_res_status_fail_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_o::unsuccessful_outcome_c");
      return OCUDUASN_ERROR_DECODE_FAIL;
  }
  return OCUDUASN_SUCCESS;
}

const char* e2ap_elem_procs_o::unsuccessful_outcome_c::types_opts::to_string() const
{
  static const char* names[] = {"RICsubscriptionFailure",
                                "RICsubscriptionAuditFailure",
                                "RICsubscriptionDeleteFailure",
                                "RICsubscriptionModificationFailure",
                                "RICsubscriptionModificationRefuse",
                                "RICsubscriptionStateControlFailure",
                                "RICqueryFailure",
                                "RICserviceUpdateFailure",
                                "RICassistanceFailure",
                                "RICcontrolFailure",
                                "RICserviceLoadStatusFailure",
                                "E2setupFailure",
                                "E2nodeConfigurationUpdateFailure",
                                "E2connectionUpdateFailure",
                                "E2RemovalFailure",
                                "E2ResourceStatusFailure"};
  return convert_enum_idx(names, 16, value, "e2ap_elem_procs_o::unsuccessful_outcome_c::types");
}

// InitiatingMessage ::= SEQUENCE{{E2AP-ELEMENTARY-PROCEDURE}}
OCUDUASN_CODE init_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != e2ap_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(value.pack(bref));

  return OCUDUASN_SUCCESS;
}
OCUDUASN_CODE init_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  value = e2ap_elem_procs_o::get_init_msg(proc_code);
  HANDLE_CODE(value.unpack(bref));

  return OCUDUASN_SUCCESS;
}
void init_msg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("procedureCode", proc_code);
  j.write_str("criticality", crit.to_string());
  j.write_fieldname("value");
  value.to_json(j);
  j.end_obj();
}
bool init_msg_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not e2ap_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = e2ap_elem_procs_o::get_crit(proc_code);
  value     = e2ap_elem_procs_o::get_init_msg(proc_code);
  return value.type().value != e2ap_elem_procs_o::init_msg_c::types_opts::nulltype;
}

// SuccessfulOutcome ::= SEQUENCE{{E2AP-ELEMENTARY-PROCEDURE}}
OCUDUASN_CODE successful_outcome_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != e2ap_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(value.pack(bref));

  return OCUDUASN_SUCCESS;
}
OCUDUASN_CODE successful_outcome_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  value = e2ap_elem_procs_o::get_successful_outcome(proc_code);
  HANDLE_CODE(value.unpack(bref));

  return OCUDUASN_SUCCESS;
}
void successful_outcome_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("procedureCode", proc_code);
  j.write_str("criticality", crit.to_string());
  j.write_fieldname("value");
  value.to_json(j);
  j.end_obj();
}
bool successful_outcome_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not e2ap_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = e2ap_elem_procs_o::get_crit(proc_code);
  value     = e2ap_elem_procs_o::get_successful_outcome(proc_code);
  return value.type().value != e2ap_elem_procs_o::successful_outcome_c::types_opts::nulltype;
}

// UnsuccessfulOutcome ::= SEQUENCE{{E2AP-ELEMENTARY-PROCEDURE}}
OCUDUASN_CODE unsuccessful_outcome_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != e2ap_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(value.pack(bref));

  return OCUDUASN_SUCCESS;
}
OCUDUASN_CODE unsuccessful_outcome_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  value = e2ap_elem_procs_o::get_unsuccessful_outcome(proc_code);
  HANDLE_CODE(value.unpack(bref));

  return OCUDUASN_SUCCESS;
}
void unsuccessful_outcome_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("procedureCode", proc_code);
  j.write_str("criticality", crit.to_string());
  j.write_fieldname("value");
  value.to_json(j);
  j.end_obj();
}
bool unsuccessful_outcome_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not e2ap_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = e2ap_elem_procs_o::get_crit(proc_code);
  value     = e2ap_elem_procs_o::get_unsuccessful_outcome(proc_code);
  return value.type().value != e2ap_elem_procs_o::unsuccessful_outcome_c::types_opts::nulltype;
}

// E2AP-PDU ::= CHOICE
void e2ap_pdu_c::destroy_()
{
  switch (type_) {
    case types::init_msg:
      c.destroy<init_msg_s>();
      break;
    case types::successful_outcome:
      c.destroy<successful_outcome_s>();
      break;
    case types::unsuccessful_outcome:
      c.destroy<unsuccessful_outcome_s>();
      break;
    default:
      break;
  }
}
void e2ap_pdu_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::init_msg:
      c.init<init_msg_s>();
      break;
    case types::successful_outcome:
      c.init<successful_outcome_s>();
      break;
    case types::unsuccessful_outcome:
      c.init<unsuccessful_outcome_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_pdu_c");
  }
}
e2ap_pdu_c::e2ap_pdu_c(const e2ap_pdu_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::init_msg:
      c.init(other.c.get<init_msg_s>());
      break;
    case types::successful_outcome:
      c.init(other.c.get<successful_outcome_s>());
      break;
    case types::unsuccessful_outcome:
      c.init(other.c.get<unsuccessful_outcome_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_pdu_c");
  }
}
e2ap_pdu_c& e2ap_pdu_c::operator=(const e2ap_pdu_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::init_msg:
      c.set(other.c.get<init_msg_s>());
      break;
    case types::successful_outcome:
      c.set(other.c.get<successful_outcome_s>());
      break;
    case types::unsuccessful_outcome:
      c.set(other.c.get<unsuccessful_outcome_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_pdu_c");
  }

  return *this;
}
init_msg_s& e2ap_pdu_c::set_init_msg()
{
  set(types::init_msg);
  return c.get<init_msg_s>();
}
successful_outcome_s& e2ap_pdu_c::set_successful_outcome()
{
  set(types::successful_outcome);
  return c.get<successful_outcome_s>();
}
unsuccessful_outcome_s& e2ap_pdu_c::set_unsuccessful_outcome()
{
  set(types::unsuccessful_outcome);
  return c.get<unsuccessful_outcome_s>();
}
void e2ap_pdu_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::init_msg:
      j.write_fieldname("initiatingMessage");
      c.get<init_msg_s>().to_json(j);
      break;
    case types::successful_outcome:
      j.write_fieldname("successfulOutcome");
      c.get<successful_outcome_s>().to_json(j);
      break;
    case types::unsuccessful_outcome:
      j.write_fieldname("unsuccessfulOutcome");
      c.get<unsuccessful_outcome_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_pdu_c");
  }
  j.end_obj();
}
OCUDUASN_CODE e2ap_pdu_c::pack(bit_ref& bref) const
{
  HANDLE_CODE(type_.pack(bref));
  switch (type_) {
    case types::init_msg:
      HANDLE_CODE(c.get<init_msg_s>().pack(bref));
      break;
    case types::successful_outcome:
      HANDLE_CODE(c.get<successful_outcome_s>().pack(bref));
      break;
    case types::unsuccessful_outcome:
      HANDLE_CODE(c.get<unsuccessful_outcome_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_pdu_c");
      return OCUDUASN_ERROR_ENCODE_FAIL;
  }
  return OCUDUASN_SUCCESS;
}
OCUDUASN_CODE e2ap_pdu_c::unpack(cbit_ref& bref)
{
  types e;
  HANDLE_CODE(e.unpack(bref));
  set(e);
  switch (type_) {
    case types::init_msg:
      HANDLE_CODE(c.get<init_msg_s>().unpack(bref));
      break;
    case types::successful_outcome:
      HANDLE_CODE(c.get<successful_outcome_s>().unpack(bref));
      break;
    case types::unsuccessful_outcome:
      HANDLE_CODE(c.get<unsuccessful_outcome_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_pdu_c");
      return OCUDUASN_ERROR_DECODE_FAIL;
  }
  return OCUDUASN_SUCCESS;
}

const char* e2ap_pdu_c::types_opts::to_string() const
{
  static const char* names[] = {"initiatingMessage", "successfulOutcome", "unsuccessfulOutcome"};
  return convert_enum_idx(names, 3, value, "e2ap_pdu_c::types");
}
