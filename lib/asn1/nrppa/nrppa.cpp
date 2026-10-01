// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/nrppa/nrppa.h"
#include "ocudu/asn1/nrppa/nrppa_pdu_contents.h"
using namespace asn1;
using namespace asn1::nrppa;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// NRPPA-ELEMENTARY-PROCEDURES ::= OBJECT SET OF NRPPA-ELEMENTARY-PROCEDURE
uint16_t nr_ppa_elem_procs_o::idx_to_proc_code(uint32_t idx)
{
  static const uint16_t names[] = {2, 6, 9, 11, 16, 17, 19, 20, 3, 4, 5, 0, 1, 7, 8, 10, 12, 13, 14, 15, 18, 21, 22};
  return map_enum_number(names, 23, idx, "proc_code");
}
bool nr_ppa_elem_procs_o::is_proc_code_valid(const uint16_t& proc_code)
{
  static const uint16_t names[] = {2, 6, 9, 11, 16, 17, 19, 20, 3, 4, 5, 0, 1, 7, 8, 10, 12, 13, 14, 15, 18, 21, 22};
  for (const auto& o : names) {
    if (o == proc_code) {
      return true;
    }
  }
  return false;
}
nr_ppa_elem_procs_o::init_msg_c nr_ppa_elem_procs_o::get_init_msg(const uint16_t& proc_code)
{
  init_msg_c ret{};
  switch (proc_code) {
    case 2:
      ret.set(init_msg_c::types::e_c_id_meas_initiation_request);
      break;
    case 6:
      ret.set(init_msg_c::types::otdoa_info_request);
      break;
    case 9:
      ret.set(init_msg_c::types::positioning_info_request);
      break;
    case 11:
      ret.set(init_msg_c::types::meas_request);
      break;
    case 16:
      ret.set(init_msg_c::types::trp_info_request);
      break;
    case 17:
      ret.set(init_msg_c::types::positioning_activation_request);
      break;
    case 19:
      ret.set(init_msg_c::types::prs_cfg_request);
      break;
    case 20:
      ret.set(init_msg_c::types::meas_precfg_required);
      break;
    case 3:
      ret.set(init_msg_c::types::e_c_id_meas_fail_ind);
      break;
    case 4:
      ret.set(init_msg_c::types::e_c_id_meas_report);
      break;
    case 5:
      ret.set(init_msg_c::types::e_c_id_meas_termination_cmd);
      break;
    case 0:
      ret.set(init_msg_c::types::error_ind);
      break;
    case 1:
      ret.set(init_msg_c::types::private_msg);
      break;
    case 7:
      ret.set(init_msg_c::types::assist_info_ctrl);
      break;
    case 8:
      ret.set(init_msg_c::types::assist_info_feedback);
      break;
    case 10:
      ret.set(init_msg_c::types::positioning_info_upd);
      break;
    case 12:
      ret.set(init_msg_c::types::meas_report);
      break;
    case 13:
      ret.set(init_msg_c::types::meas_upd);
      break;
    case 14:
      ret.set(init_msg_c::types::meas_abort);
      break;
    case 15:
      ret.set(init_msg_c::types::meas_fail_ind);
      break;
    case 18:
      ret.set(init_msg_c::types::positioning_deactivation);
      break;
    case 21:
      ret.set(init_msg_c::types::meas_activation);
      break;
    case 22:
      ret.set(init_msg_c::types::srs_info_reserv_notif);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
nr_ppa_elem_procs_o::successful_outcome_c nr_ppa_elem_procs_o::get_successful_outcome(const uint16_t& proc_code)
{
  successful_outcome_c ret{};
  switch (proc_code) {
    case 2:
      ret.set(successful_outcome_c::types::e_c_id_meas_initiation_resp);
      break;
    case 6:
      ret.set(successful_outcome_c::types::otdoa_info_resp);
      break;
    case 9:
      ret.set(successful_outcome_c::types::positioning_info_resp);
      break;
    case 11:
      ret.set(successful_outcome_c::types::meas_resp);
      break;
    case 16:
      ret.set(successful_outcome_c::types::trp_info_resp);
      break;
    case 17:
      ret.set(successful_outcome_c::types::positioning_activation_resp);
      break;
    case 19:
      ret.set(successful_outcome_c::types::prs_cfg_resp);
      break;
    case 20:
      ret.set(successful_outcome_c::types::meas_precfg_confirm);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
nr_ppa_elem_procs_o::unsuccessful_outcome_c nr_ppa_elem_procs_o::get_unsuccessful_outcome(const uint16_t& proc_code)
{
  unsuccessful_outcome_c ret{};
  switch (proc_code) {
    case 2:
      ret.set(unsuccessful_outcome_c::types::e_c_id_meas_initiation_fail);
      break;
    case 6:
      ret.set(unsuccessful_outcome_c::types::otdoa_info_fail);
      break;
    case 9:
      ret.set(unsuccessful_outcome_c::types::positioning_info_fail);
      break;
    case 11:
      ret.set(unsuccessful_outcome_c::types::meas_fail);
      break;
    case 16:
      ret.set(unsuccessful_outcome_c::types::trp_info_fail);
      break;
    case 17:
      ret.set(unsuccessful_outcome_c::types::positioning_activation_fail);
      break;
    case 19:
      ret.set(unsuccessful_outcome_c::types::prs_cfg_fail);
      break;
    case 20:
      ret.set(unsuccessful_outcome_c::types::meas_precfg_refuse);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
crit_e nr_ppa_elem_procs_o::get_crit(const uint16_t& proc_code)
{
  switch (proc_code) {
    case 2:
      return crit_e::reject;
    case 6:
      return crit_e::reject;
    case 9:
      return crit_e::reject;
    case 11:
      return crit_e::reject;
    case 16:
      return crit_e::reject;
    case 17:
      return crit_e::reject;
    case 19:
      return crit_e::reject;
    case 20:
      return crit_e::reject;
    case 3:
      return crit_e::ignore;
    case 4:
      return crit_e::ignore;
    case 5:
      return crit_e::reject;
    case 0:
      return crit_e::ignore;
    case 1:
      return crit_e::ignore;
    case 7:
      return crit_e::reject;
    case 8:
      return crit_e::reject;
    case 10:
      return crit_e::ignore;
    case 12:
      return crit_e::ignore;
    case 13:
      return crit_e::ignore;
    case 14:
      return crit_e::ignore;
    case 15:
      return crit_e::ignore;
    case 18:
      return crit_e::ignore;
    case 21:
      return crit_e::ignore;
    case 22:
      return crit_e::reject;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return {};
}

// InitiatingMessage ::= OPEN TYPE
void nr_ppa_elem_procs_o::init_msg_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::e_c_id_meas_initiation_request:
      c = e_c_id_meas_initiation_request_s{};
      break;
    case types::otdoa_info_request:
      c = otdoa_info_request_s{};
      break;
    case types::positioning_info_request:
      c = positioning_info_request_s{};
      break;
    case types::meas_request:
      c = meas_request_s{};
      break;
    case types::trp_info_request:
      c = trp_info_request_s{};
      break;
    case types::positioning_activation_request:
      c = positioning_activation_request_s{};
      break;
    case types::prs_cfg_request:
      c = prs_cfg_request_s{};
      break;
    case types::meas_precfg_required:
      c = meas_precfg_required_s{};
      break;
    case types::e_c_id_meas_fail_ind:
      c = e_c_id_meas_fail_ind_s{};
      break;
    case types::e_c_id_meas_report:
      c = e_c_id_meas_report_s{};
      break;
    case types::e_c_id_meas_termination_cmd:
      c = e_c_id_meas_termination_cmd_s{};
      break;
    case types::error_ind:
      c = error_ind_s{};
      break;
    case types::private_msg:
      c = private_msg_s{};
      break;
    case types::assist_info_ctrl:
      c = assist_info_ctrl_s{};
      break;
    case types::assist_info_feedback:
      c = assist_info_feedback_s{};
      break;
    case types::positioning_info_upd:
      c = positioning_info_upd_s{};
      break;
    case types::meas_report:
      c = meas_report_s{};
      break;
    case types::meas_upd:
      c = meas_upd_s{};
      break;
    case types::meas_abort:
      c = meas_abort_s{};
      break;
    case types::meas_fail_ind:
      c = meas_fail_ind_s{};
      break;
    case types::positioning_deactivation:
      c = positioning_deactivation_s{};
      break;
    case types::meas_activation:
      c = meas_activation_s{};
      break;
    case types::srs_info_reserv_notif:
      c = srs_info_reserv_notif_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "nr_ppa_elem_procs_o::init_msg_c");
  }
}
e_c_id_meas_initiation_request_s& nr_ppa_elem_procs_o::init_msg_c::e_c_id_meas_initiation_request()
{
  assert_choice_type(types::e_c_id_meas_initiation_request, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_initiation_request_s>();
}
otdoa_info_request_s& nr_ppa_elem_procs_o::init_msg_c::otdoa_info_request()
{
  assert_choice_type(types::otdoa_info_request, type_, "InitiatingMessage");
  return c.get<otdoa_info_request_s>();
}
positioning_info_request_s& nr_ppa_elem_procs_o::init_msg_c::positioning_info_request()
{
  assert_choice_type(types::positioning_info_request, type_, "InitiatingMessage");
  return c.get<positioning_info_request_s>();
}
meas_request_s& nr_ppa_elem_procs_o::init_msg_c::meas_request()
{
  assert_choice_type(types::meas_request, type_, "InitiatingMessage");
  return c.get<meas_request_s>();
}
trp_info_request_s& nr_ppa_elem_procs_o::init_msg_c::trp_info_request()
{
  assert_choice_type(types::trp_info_request, type_, "InitiatingMessage");
  return c.get<trp_info_request_s>();
}
positioning_activation_request_s& nr_ppa_elem_procs_o::init_msg_c::positioning_activation_request()
{
  assert_choice_type(types::positioning_activation_request, type_, "InitiatingMessage");
  return c.get<positioning_activation_request_s>();
}
prs_cfg_request_s& nr_ppa_elem_procs_o::init_msg_c::prs_cfg_request()
{
  assert_choice_type(types::prs_cfg_request, type_, "InitiatingMessage");
  return c.get<prs_cfg_request_s>();
}
meas_precfg_required_s& nr_ppa_elem_procs_o::init_msg_c::meas_precfg_required()
{
  assert_choice_type(types::meas_precfg_required, type_, "InitiatingMessage");
  return c.get<meas_precfg_required_s>();
}
e_c_id_meas_fail_ind_s& nr_ppa_elem_procs_o::init_msg_c::e_c_id_meas_fail_ind()
{
  assert_choice_type(types::e_c_id_meas_fail_ind, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_fail_ind_s>();
}
e_c_id_meas_report_s& nr_ppa_elem_procs_o::init_msg_c::e_c_id_meas_report()
{
  assert_choice_type(types::e_c_id_meas_report, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_report_s>();
}
e_c_id_meas_termination_cmd_s& nr_ppa_elem_procs_o::init_msg_c::e_c_id_meas_termination_cmd()
{
  assert_choice_type(types::e_c_id_meas_termination_cmd, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_termination_cmd_s>();
}
error_ind_s& nr_ppa_elem_procs_o::init_msg_c::error_ind()
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
private_msg_s& nr_ppa_elem_procs_o::init_msg_c::private_msg()
{
  assert_choice_type(types::private_msg, type_, "InitiatingMessage");
  return c.get<private_msg_s>();
}
assist_info_ctrl_s& nr_ppa_elem_procs_o::init_msg_c::assist_info_ctrl()
{
  assert_choice_type(types::assist_info_ctrl, type_, "InitiatingMessage");
  return c.get<assist_info_ctrl_s>();
}
assist_info_feedback_s& nr_ppa_elem_procs_o::init_msg_c::assist_info_feedback()
{
  assert_choice_type(types::assist_info_feedback, type_, "InitiatingMessage");
  return c.get<assist_info_feedback_s>();
}
positioning_info_upd_s& nr_ppa_elem_procs_o::init_msg_c::positioning_info_upd()
{
  assert_choice_type(types::positioning_info_upd, type_, "InitiatingMessage");
  return c.get<positioning_info_upd_s>();
}
meas_report_s& nr_ppa_elem_procs_o::init_msg_c::meas_report()
{
  assert_choice_type(types::meas_report, type_, "InitiatingMessage");
  return c.get<meas_report_s>();
}
meas_upd_s& nr_ppa_elem_procs_o::init_msg_c::meas_upd()
{
  assert_choice_type(types::meas_upd, type_, "InitiatingMessage");
  return c.get<meas_upd_s>();
}
meas_abort_s& nr_ppa_elem_procs_o::init_msg_c::meas_abort()
{
  assert_choice_type(types::meas_abort, type_, "InitiatingMessage");
  return c.get<meas_abort_s>();
}
meas_fail_ind_s& nr_ppa_elem_procs_o::init_msg_c::meas_fail_ind()
{
  assert_choice_type(types::meas_fail_ind, type_, "InitiatingMessage");
  return c.get<meas_fail_ind_s>();
}
positioning_deactivation_s& nr_ppa_elem_procs_o::init_msg_c::positioning_deactivation()
{
  assert_choice_type(types::positioning_deactivation, type_, "InitiatingMessage");
  return c.get<positioning_deactivation_s>();
}
meas_activation_s& nr_ppa_elem_procs_o::init_msg_c::meas_activation()
{
  assert_choice_type(types::meas_activation, type_, "InitiatingMessage");
  return c.get<meas_activation_s>();
}
srs_info_reserv_notif_s& nr_ppa_elem_procs_o::init_msg_c::srs_info_reserv_notif()
{
  assert_choice_type(types::srs_info_reserv_notif, type_, "InitiatingMessage");
  return c.get<srs_info_reserv_notif_s>();
}
const e_c_id_meas_initiation_request_s& nr_ppa_elem_procs_o::init_msg_c::e_c_id_meas_initiation_request() const
{
  assert_choice_type(types::e_c_id_meas_initiation_request, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_initiation_request_s>();
}
const otdoa_info_request_s& nr_ppa_elem_procs_o::init_msg_c::otdoa_info_request() const
{
  assert_choice_type(types::otdoa_info_request, type_, "InitiatingMessage");
  return c.get<otdoa_info_request_s>();
}
const positioning_info_request_s& nr_ppa_elem_procs_o::init_msg_c::positioning_info_request() const
{
  assert_choice_type(types::positioning_info_request, type_, "InitiatingMessage");
  return c.get<positioning_info_request_s>();
}
const meas_request_s& nr_ppa_elem_procs_o::init_msg_c::meas_request() const
{
  assert_choice_type(types::meas_request, type_, "InitiatingMessage");
  return c.get<meas_request_s>();
}
const trp_info_request_s& nr_ppa_elem_procs_o::init_msg_c::trp_info_request() const
{
  assert_choice_type(types::trp_info_request, type_, "InitiatingMessage");
  return c.get<trp_info_request_s>();
}
const positioning_activation_request_s& nr_ppa_elem_procs_o::init_msg_c::positioning_activation_request() const
{
  assert_choice_type(types::positioning_activation_request, type_, "InitiatingMessage");
  return c.get<positioning_activation_request_s>();
}
const prs_cfg_request_s& nr_ppa_elem_procs_o::init_msg_c::prs_cfg_request() const
{
  assert_choice_type(types::prs_cfg_request, type_, "InitiatingMessage");
  return c.get<prs_cfg_request_s>();
}
const meas_precfg_required_s& nr_ppa_elem_procs_o::init_msg_c::meas_precfg_required() const
{
  assert_choice_type(types::meas_precfg_required, type_, "InitiatingMessage");
  return c.get<meas_precfg_required_s>();
}
const e_c_id_meas_fail_ind_s& nr_ppa_elem_procs_o::init_msg_c::e_c_id_meas_fail_ind() const
{
  assert_choice_type(types::e_c_id_meas_fail_ind, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_fail_ind_s>();
}
const e_c_id_meas_report_s& nr_ppa_elem_procs_o::init_msg_c::e_c_id_meas_report() const
{
  assert_choice_type(types::e_c_id_meas_report, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_report_s>();
}
const e_c_id_meas_termination_cmd_s& nr_ppa_elem_procs_o::init_msg_c::e_c_id_meas_termination_cmd() const
{
  assert_choice_type(types::e_c_id_meas_termination_cmd, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_termination_cmd_s>();
}
const error_ind_s& nr_ppa_elem_procs_o::init_msg_c::error_ind() const
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
const private_msg_s& nr_ppa_elem_procs_o::init_msg_c::private_msg() const
{
  assert_choice_type(types::private_msg, type_, "InitiatingMessage");
  return c.get<private_msg_s>();
}
const assist_info_ctrl_s& nr_ppa_elem_procs_o::init_msg_c::assist_info_ctrl() const
{
  assert_choice_type(types::assist_info_ctrl, type_, "InitiatingMessage");
  return c.get<assist_info_ctrl_s>();
}
const assist_info_feedback_s& nr_ppa_elem_procs_o::init_msg_c::assist_info_feedback() const
{
  assert_choice_type(types::assist_info_feedback, type_, "InitiatingMessage");
  return c.get<assist_info_feedback_s>();
}
const positioning_info_upd_s& nr_ppa_elem_procs_o::init_msg_c::positioning_info_upd() const
{
  assert_choice_type(types::positioning_info_upd, type_, "InitiatingMessage");
  return c.get<positioning_info_upd_s>();
}
const meas_report_s& nr_ppa_elem_procs_o::init_msg_c::meas_report() const
{
  assert_choice_type(types::meas_report, type_, "InitiatingMessage");
  return c.get<meas_report_s>();
}
const meas_upd_s& nr_ppa_elem_procs_o::init_msg_c::meas_upd() const
{
  assert_choice_type(types::meas_upd, type_, "InitiatingMessage");
  return c.get<meas_upd_s>();
}
const meas_abort_s& nr_ppa_elem_procs_o::init_msg_c::meas_abort() const
{
  assert_choice_type(types::meas_abort, type_, "InitiatingMessage");
  return c.get<meas_abort_s>();
}
const meas_fail_ind_s& nr_ppa_elem_procs_o::init_msg_c::meas_fail_ind() const
{
  assert_choice_type(types::meas_fail_ind, type_, "InitiatingMessage");
  return c.get<meas_fail_ind_s>();
}
const positioning_deactivation_s& nr_ppa_elem_procs_o::init_msg_c::positioning_deactivation() const
{
  assert_choice_type(types::positioning_deactivation, type_, "InitiatingMessage");
  return c.get<positioning_deactivation_s>();
}
const meas_activation_s& nr_ppa_elem_procs_o::init_msg_c::meas_activation() const
{
  assert_choice_type(types::meas_activation, type_, "InitiatingMessage");
  return c.get<meas_activation_s>();
}
const srs_info_reserv_notif_s& nr_ppa_elem_procs_o::init_msg_c::srs_info_reserv_notif() const
{
  assert_choice_type(types::srs_info_reserv_notif, type_, "InitiatingMessage");
  return c.get<srs_info_reserv_notif_s>();
}
void nr_ppa_elem_procs_o::init_msg_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::e_c_id_meas_initiation_request:
      j.write_fieldname("E-CIDMeasurementInitiationRequest");
      c.get<e_c_id_meas_initiation_request_s>().to_json(j);
      break;
    case types::otdoa_info_request:
      j.write_fieldname("OTDOAInformationRequest");
      c.get<otdoa_info_request_s>().to_json(j);
      break;
    case types::positioning_info_request:
      j.write_fieldname("PositioningInformationRequest");
      c.get<positioning_info_request_s>().to_json(j);
      break;
    case types::meas_request:
      j.write_fieldname("MeasurementRequest");
      c.get<meas_request_s>().to_json(j);
      break;
    case types::trp_info_request:
      j.write_fieldname("TRPInformationRequest");
      c.get<trp_info_request_s>().to_json(j);
      break;
    case types::positioning_activation_request:
      j.write_fieldname("PositioningActivationRequest");
      c.get<positioning_activation_request_s>().to_json(j);
      break;
    case types::prs_cfg_request:
      j.write_fieldname("PRSConfigurationRequest");
      c.get<prs_cfg_request_s>().to_json(j);
      break;
    case types::meas_precfg_required:
      j.write_fieldname("MeasurementPreconfigurationRequired");
      c.get<meas_precfg_required_s>().to_json(j);
      break;
    case types::e_c_id_meas_fail_ind:
      j.write_fieldname("E-CIDMeasurementFailureIndication");
      c.get<e_c_id_meas_fail_ind_s>().to_json(j);
      break;
    case types::e_c_id_meas_report:
      j.write_fieldname("E-CIDMeasurementReport");
      c.get<e_c_id_meas_report_s>().to_json(j);
      break;
    case types::e_c_id_meas_termination_cmd:
      j.write_fieldname("E-CIDMeasurementTerminationCommand");
      c.get<e_c_id_meas_termination_cmd_s>().to_json(j);
      break;
    case types::error_ind:
      j.write_fieldname("ErrorIndication");
      c.get<error_ind_s>().to_json(j);
      break;
    case types::private_msg:
      j.write_fieldname("PrivateMessage");
      c.get<private_msg_s>().to_json(j);
      break;
    case types::assist_info_ctrl:
      j.write_fieldname("AssistanceInformationControl");
      c.get<assist_info_ctrl_s>().to_json(j);
      break;
    case types::assist_info_feedback:
      j.write_fieldname("AssistanceInformationFeedback");
      c.get<assist_info_feedback_s>().to_json(j);
      break;
    case types::positioning_info_upd:
      j.write_fieldname("PositioningInformationUpdate");
      c.get<positioning_info_upd_s>().to_json(j);
      break;
    case types::meas_report:
      j.write_fieldname("MeasurementReport");
      c.get<meas_report_s>().to_json(j);
      break;
    case types::meas_upd:
      j.write_fieldname("MeasurementUpdate");
      c.get<meas_upd_s>().to_json(j);
      break;
    case types::meas_abort:
      j.write_fieldname("MeasurementAbort");
      c.get<meas_abort_s>().to_json(j);
      break;
    case types::meas_fail_ind:
      j.write_fieldname("MeasurementFailureIndication");
      c.get<meas_fail_ind_s>().to_json(j);
      break;
    case types::positioning_deactivation:
      j.write_fieldname("PositioningDeactivation");
      c.get<positioning_deactivation_s>().to_json(j);
      break;
    case types::meas_activation:
      j.write_fieldname("MeasurementActivation");
      c.get<meas_activation_s>().to_json(j);
      break;
    case types::srs_info_reserv_notif:
      j.write_fieldname("SRSInformationReservationNotification");
      c.get<srs_info_reserv_notif_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "nr_ppa_elem_procs_o::init_msg_c");
  }
  j.end_obj();
}
OCUDUASN_CODE nr_ppa_elem_procs_o::init_msg_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::e_c_id_meas_initiation_request:
      HANDLE_CODE(c.get<e_c_id_meas_initiation_request_s>().pack(bref));
      break;
    case types::otdoa_info_request:
      HANDLE_CODE(c.get<otdoa_info_request_s>().pack(bref));
      break;
    case types::positioning_info_request:
      HANDLE_CODE(c.get<positioning_info_request_s>().pack(bref));
      break;
    case types::meas_request:
      HANDLE_CODE(c.get<meas_request_s>().pack(bref));
      break;
    case types::trp_info_request:
      HANDLE_CODE(c.get<trp_info_request_s>().pack(bref));
      break;
    case types::positioning_activation_request:
      HANDLE_CODE(c.get<positioning_activation_request_s>().pack(bref));
      break;
    case types::prs_cfg_request:
      HANDLE_CODE(c.get<prs_cfg_request_s>().pack(bref));
      break;
    case types::meas_precfg_required:
      HANDLE_CODE(c.get<meas_precfg_required_s>().pack(bref));
      break;
    case types::e_c_id_meas_fail_ind:
      HANDLE_CODE(c.get<e_c_id_meas_fail_ind_s>().pack(bref));
      break;
    case types::e_c_id_meas_report:
      HANDLE_CODE(c.get<e_c_id_meas_report_s>().pack(bref));
      break;
    case types::e_c_id_meas_termination_cmd:
      HANDLE_CODE(c.get<e_c_id_meas_termination_cmd_s>().pack(bref));
      break;
    case types::error_ind:
      HANDLE_CODE(c.get<error_ind_s>().pack(bref));
      break;
    case types::private_msg:
      HANDLE_CODE(c.get<private_msg_s>().pack(bref));
      break;
    case types::assist_info_ctrl:
      HANDLE_CODE(c.get<assist_info_ctrl_s>().pack(bref));
      break;
    case types::assist_info_feedback:
      HANDLE_CODE(c.get<assist_info_feedback_s>().pack(bref));
      break;
    case types::positioning_info_upd:
      HANDLE_CODE(c.get<positioning_info_upd_s>().pack(bref));
      break;
    case types::meas_report:
      HANDLE_CODE(c.get<meas_report_s>().pack(bref));
      break;
    case types::meas_upd:
      HANDLE_CODE(c.get<meas_upd_s>().pack(bref));
      break;
    case types::meas_abort:
      HANDLE_CODE(c.get<meas_abort_s>().pack(bref));
      break;
    case types::meas_fail_ind:
      HANDLE_CODE(c.get<meas_fail_ind_s>().pack(bref));
      break;
    case types::positioning_deactivation:
      HANDLE_CODE(c.get<positioning_deactivation_s>().pack(bref));
      break;
    case types::meas_activation:
      HANDLE_CODE(c.get<meas_activation_s>().pack(bref));
      break;
    case types::srs_info_reserv_notif:
      HANDLE_CODE(c.get<srs_info_reserv_notif_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "nr_ppa_elem_procs_o::init_msg_c");
      return OCUDUASN_ERROR_ENCODE_FAIL;
  }
  return OCUDUASN_SUCCESS;
}
OCUDUASN_CODE nr_ppa_elem_procs_o::init_msg_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::e_c_id_meas_initiation_request:
      HANDLE_CODE(c.get<e_c_id_meas_initiation_request_s>().unpack(bref));
      break;
    case types::otdoa_info_request:
      HANDLE_CODE(c.get<otdoa_info_request_s>().unpack(bref));
      break;
    case types::positioning_info_request:
      HANDLE_CODE(c.get<positioning_info_request_s>().unpack(bref));
      break;
    case types::meas_request:
      HANDLE_CODE(c.get<meas_request_s>().unpack(bref));
      break;
    case types::trp_info_request:
      HANDLE_CODE(c.get<trp_info_request_s>().unpack(bref));
      break;
    case types::positioning_activation_request:
      HANDLE_CODE(c.get<positioning_activation_request_s>().unpack(bref));
      break;
    case types::prs_cfg_request:
      HANDLE_CODE(c.get<prs_cfg_request_s>().unpack(bref));
      break;
    case types::meas_precfg_required:
      HANDLE_CODE(c.get<meas_precfg_required_s>().unpack(bref));
      break;
    case types::e_c_id_meas_fail_ind:
      HANDLE_CODE(c.get<e_c_id_meas_fail_ind_s>().unpack(bref));
      break;
    case types::e_c_id_meas_report:
      HANDLE_CODE(c.get<e_c_id_meas_report_s>().unpack(bref));
      break;
    case types::e_c_id_meas_termination_cmd:
      HANDLE_CODE(c.get<e_c_id_meas_termination_cmd_s>().unpack(bref));
      break;
    case types::error_ind:
      HANDLE_CODE(c.get<error_ind_s>().unpack(bref));
      break;
    case types::private_msg:
      HANDLE_CODE(c.get<private_msg_s>().unpack(bref));
      break;
    case types::assist_info_ctrl:
      HANDLE_CODE(c.get<assist_info_ctrl_s>().unpack(bref));
      break;
    case types::assist_info_feedback:
      HANDLE_CODE(c.get<assist_info_feedback_s>().unpack(bref));
      break;
    case types::positioning_info_upd:
      HANDLE_CODE(c.get<positioning_info_upd_s>().unpack(bref));
      break;
    case types::meas_report:
      HANDLE_CODE(c.get<meas_report_s>().unpack(bref));
      break;
    case types::meas_upd:
      HANDLE_CODE(c.get<meas_upd_s>().unpack(bref));
      break;
    case types::meas_abort:
      HANDLE_CODE(c.get<meas_abort_s>().unpack(bref));
      break;
    case types::meas_fail_ind:
      HANDLE_CODE(c.get<meas_fail_ind_s>().unpack(bref));
      break;
    case types::positioning_deactivation:
      HANDLE_CODE(c.get<positioning_deactivation_s>().unpack(bref));
      break;
    case types::meas_activation:
      HANDLE_CODE(c.get<meas_activation_s>().unpack(bref));
      break;
    case types::srs_info_reserv_notif:
      HANDLE_CODE(c.get<srs_info_reserv_notif_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "nr_ppa_elem_procs_o::init_msg_c");
      return OCUDUASN_ERROR_DECODE_FAIL;
  }
  return OCUDUASN_SUCCESS;
}

const char* nr_ppa_elem_procs_o::init_msg_c::types_opts::to_string() const
{
  static const char* names[] = {"E-CIDMeasurementInitiationRequest",
                                "OTDOAInformationRequest",
                                "PositioningInformationRequest",
                                "MeasurementRequest",
                                "TRPInformationRequest",
                                "PositioningActivationRequest",
                                "PRSConfigurationRequest",
                                "MeasurementPreconfigurationRequired",
                                "E-CIDMeasurementFailureIndication",
                                "E-CIDMeasurementReport",
                                "E-CIDMeasurementTerminationCommand",
                                "ErrorIndication",
                                "PrivateMessage",
                                "AssistanceInformationControl",
                                "AssistanceInformationFeedback",
                                "PositioningInformationUpdate",
                                "MeasurementReport",
                                "MeasurementUpdate",
                                "MeasurementAbort",
                                "MeasurementFailureIndication",
                                "PositioningDeactivation",
                                "MeasurementActivation",
                                "SRSInformationReservationNotification"};
  return convert_enum_idx(names, 23, value, "nr_ppa_elem_procs_o::init_msg_c::types");
}

// SuccessfulOutcome ::= OPEN TYPE
void nr_ppa_elem_procs_o::successful_outcome_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::e_c_id_meas_initiation_resp:
      c = e_c_id_meas_initiation_resp_s{};
      break;
    case types::otdoa_info_resp:
      c = otdoa_info_resp_s{};
      break;
    case types::positioning_info_resp:
      c = positioning_info_resp_s{};
      break;
    case types::meas_resp:
      c = meas_resp_s{};
      break;
    case types::trp_info_resp:
      c = trp_info_resp_s{};
      break;
    case types::positioning_activation_resp:
      c = positioning_activation_resp_s{};
      break;
    case types::prs_cfg_resp:
      c = prs_cfg_resp_s{};
      break;
    case types::meas_precfg_confirm:
      c = meas_precfg_confirm_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "nr_ppa_elem_procs_o::successful_outcome_c");
  }
}
e_c_id_meas_initiation_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::e_c_id_meas_initiation_resp()
{
  assert_choice_type(types::e_c_id_meas_initiation_resp, type_, "SuccessfulOutcome");
  return c.get<e_c_id_meas_initiation_resp_s>();
}
otdoa_info_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::otdoa_info_resp()
{
  assert_choice_type(types::otdoa_info_resp, type_, "SuccessfulOutcome");
  return c.get<otdoa_info_resp_s>();
}
positioning_info_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::positioning_info_resp()
{
  assert_choice_type(types::positioning_info_resp, type_, "SuccessfulOutcome");
  return c.get<positioning_info_resp_s>();
}
meas_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::meas_resp()
{
  assert_choice_type(types::meas_resp, type_, "SuccessfulOutcome");
  return c.get<meas_resp_s>();
}
trp_info_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::trp_info_resp()
{
  assert_choice_type(types::trp_info_resp, type_, "SuccessfulOutcome");
  return c.get<trp_info_resp_s>();
}
positioning_activation_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::positioning_activation_resp()
{
  assert_choice_type(types::positioning_activation_resp, type_, "SuccessfulOutcome");
  return c.get<positioning_activation_resp_s>();
}
prs_cfg_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::prs_cfg_resp()
{
  assert_choice_type(types::prs_cfg_resp, type_, "SuccessfulOutcome");
  return c.get<prs_cfg_resp_s>();
}
meas_precfg_confirm_s& nr_ppa_elem_procs_o::successful_outcome_c::meas_precfg_confirm()
{
  assert_choice_type(types::meas_precfg_confirm, type_, "SuccessfulOutcome");
  return c.get<meas_precfg_confirm_s>();
}
const e_c_id_meas_initiation_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::e_c_id_meas_initiation_resp() const
{
  assert_choice_type(types::e_c_id_meas_initiation_resp, type_, "SuccessfulOutcome");
  return c.get<e_c_id_meas_initiation_resp_s>();
}
const otdoa_info_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::otdoa_info_resp() const
{
  assert_choice_type(types::otdoa_info_resp, type_, "SuccessfulOutcome");
  return c.get<otdoa_info_resp_s>();
}
const positioning_info_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::positioning_info_resp() const
{
  assert_choice_type(types::positioning_info_resp, type_, "SuccessfulOutcome");
  return c.get<positioning_info_resp_s>();
}
const meas_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::meas_resp() const
{
  assert_choice_type(types::meas_resp, type_, "SuccessfulOutcome");
  return c.get<meas_resp_s>();
}
const trp_info_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::trp_info_resp() const
{
  assert_choice_type(types::trp_info_resp, type_, "SuccessfulOutcome");
  return c.get<trp_info_resp_s>();
}
const positioning_activation_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::positioning_activation_resp() const
{
  assert_choice_type(types::positioning_activation_resp, type_, "SuccessfulOutcome");
  return c.get<positioning_activation_resp_s>();
}
const prs_cfg_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::prs_cfg_resp() const
{
  assert_choice_type(types::prs_cfg_resp, type_, "SuccessfulOutcome");
  return c.get<prs_cfg_resp_s>();
}
const meas_precfg_confirm_s& nr_ppa_elem_procs_o::successful_outcome_c::meas_precfg_confirm() const
{
  assert_choice_type(types::meas_precfg_confirm, type_, "SuccessfulOutcome");
  return c.get<meas_precfg_confirm_s>();
}
void nr_ppa_elem_procs_o::successful_outcome_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::e_c_id_meas_initiation_resp:
      j.write_fieldname("E-CIDMeasurementInitiationResponse");
      c.get<e_c_id_meas_initiation_resp_s>().to_json(j);
      break;
    case types::otdoa_info_resp:
      j.write_fieldname("OTDOAInformationResponse");
      c.get<otdoa_info_resp_s>().to_json(j);
      break;
    case types::positioning_info_resp:
      j.write_fieldname("PositioningInformationResponse");
      c.get<positioning_info_resp_s>().to_json(j);
      break;
    case types::meas_resp:
      j.write_fieldname("MeasurementResponse");
      c.get<meas_resp_s>().to_json(j);
      break;
    case types::trp_info_resp:
      j.write_fieldname("TRPInformationResponse");
      c.get<trp_info_resp_s>().to_json(j);
      break;
    case types::positioning_activation_resp:
      j.write_fieldname("PositioningActivationResponse");
      c.get<positioning_activation_resp_s>().to_json(j);
      break;
    case types::prs_cfg_resp:
      j.write_fieldname("PRSConfigurationResponse");
      c.get<prs_cfg_resp_s>().to_json(j);
      break;
    case types::meas_precfg_confirm:
      j.write_fieldname("MeasurementPreconfigurationConfirm");
      c.get<meas_precfg_confirm_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "nr_ppa_elem_procs_o::successful_outcome_c");
  }
  j.end_obj();
}
OCUDUASN_CODE nr_ppa_elem_procs_o::successful_outcome_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::e_c_id_meas_initiation_resp:
      HANDLE_CODE(c.get<e_c_id_meas_initiation_resp_s>().pack(bref));
      break;
    case types::otdoa_info_resp:
      HANDLE_CODE(c.get<otdoa_info_resp_s>().pack(bref));
      break;
    case types::positioning_info_resp:
      HANDLE_CODE(c.get<positioning_info_resp_s>().pack(bref));
      break;
    case types::meas_resp:
      HANDLE_CODE(c.get<meas_resp_s>().pack(bref));
      break;
    case types::trp_info_resp:
      HANDLE_CODE(c.get<trp_info_resp_s>().pack(bref));
      break;
    case types::positioning_activation_resp:
      HANDLE_CODE(c.get<positioning_activation_resp_s>().pack(bref));
      break;
    case types::prs_cfg_resp:
      HANDLE_CODE(c.get<prs_cfg_resp_s>().pack(bref));
      break;
    case types::meas_precfg_confirm:
      HANDLE_CODE(c.get<meas_precfg_confirm_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "nr_ppa_elem_procs_o::successful_outcome_c");
      return OCUDUASN_ERROR_ENCODE_FAIL;
  }
  return OCUDUASN_SUCCESS;
}
OCUDUASN_CODE nr_ppa_elem_procs_o::successful_outcome_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::e_c_id_meas_initiation_resp:
      HANDLE_CODE(c.get<e_c_id_meas_initiation_resp_s>().unpack(bref));
      break;
    case types::otdoa_info_resp:
      HANDLE_CODE(c.get<otdoa_info_resp_s>().unpack(bref));
      break;
    case types::positioning_info_resp:
      HANDLE_CODE(c.get<positioning_info_resp_s>().unpack(bref));
      break;
    case types::meas_resp:
      HANDLE_CODE(c.get<meas_resp_s>().unpack(bref));
      break;
    case types::trp_info_resp:
      HANDLE_CODE(c.get<trp_info_resp_s>().unpack(bref));
      break;
    case types::positioning_activation_resp:
      HANDLE_CODE(c.get<positioning_activation_resp_s>().unpack(bref));
      break;
    case types::prs_cfg_resp:
      HANDLE_CODE(c.get<prs_cfg_resp_s>().unpack(bref));
      break;
    case types::meas_precfg_confirm:
      HANDLE_CODE(c.get<meas_precfg_confirm_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "nr_ppa_elem_procs_o::successful_outcome_c");
      return OCUDUASN_ERROR_DECODE_FAIL;
  }
  return OCUDUASN_SUCCESS;
}

const char* nr_ppa_elem_procs_o::successful_outcome_c::types_opts::to_string() const
{
  static const char* names[] = {"E-CIDMeasurementInitiationResponse",
                                "OTDOAInformationResponse",
                                "PositioningInformationResponse",
                                "MeasurementResponse",
                                "TRPInformationResponse",
                                "PositioningActivationResponse",
                                "PRSConfigurationResponse",
                                "MeasurementPreconfigurationConfirm"};
  return convert_enum_idx(names, 8, value, "nr_ppa_elem_procs_o::successful_outcome_c::types");
}

// UnsuccessfulOutcome ::= OPEN TYPE
void nr_ppa_elem_procs_o::unsuccessful_outcome_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::e_c_id_meas_initiation_fail:
      c = e_c_id_meas_initiation_fail_s{};
      break;
    case types::otdoa_info_fail:
      c = otdoa_info_fail_s{};
      break;
    case types::positioning_info_fail:
      c = positioning_info_fail_s{};
      break;
    case types::meas_fail:
      c = meas_fail_s{};
      break;
    case types::trp_info_fail:
      c = trp_info_fail_s{};
      break;
    case types::positioning_activation_fail:
      c = positioning_activation_fail_s{};
      break;
    case types::prs_cfg_fail:
      c = prs_cfg_fail_s{};
      break;
    case types::meas_precfg_refuse:
      c = meas_precfg_refuse_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "nr_ppa_elem_procs_o::unsuccessful_outcome_c");
  }
}
e_c_id_meas_initiation_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::e_c_id_meas_initiation_fail()
{
  assert_choice_type(types::e_c_id_meas_initiation_fail, type_, "UnsuccessfulOutcome");
  return c.get<e_c_id_meas_initiation_fail_s>();
}
otdoa_info_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::otdoa_info_fail()
{
  assert_choice_type(types::otdoa_info_fail, type_, "UnsuccessfulOutcome");
  return c.get<otdoa_info_fail_s>();
}
positioning_info_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::positioning_info_fail()
{
  assert_choice_type(types::positioning_info_fail, type_, "UnsuccessfulOutcome");
  return c.get<positioning_info_fail_s>();
}
meas_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::meas_fail()
{
  assert_choice_type(types::meas_fail, type_, "UnsuccessfulOutcome");
  return c.get<meas_fail_s>();
}
trp_info_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::trp_info_fail()
{
  assert_choice_type(types::trp_info_fail, type_, "UnsuccessfulOutcome");
  return c.get<trp_info_fail_s>();
}
positioning_activation_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::positioning_activation_fail()
{
  assert_choice_type(types::positioning_activation_fail, type_, "UnsuccessfulOutcome");
  return c.get<positioning_activation_fail_s>();
}
prs_cfg_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::prs_cfg_fail()
{
  assert_choice_type(types::prs_cfg_fail, type_, "UnsuccessfulOutcome");
  return c.get<prs_cfg_fail_s>();
}
meas_precfg_refuse_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::meas_precfg_refuse()
{
  assert_choice_type(types::meas_precfg_refuse, type_, "UnsuccessfulOutcome");
  return c.get<meas_precfg_refuse_s>();
}
const e_c_id_meas_initiation_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::e_c_id_meas_initiation_fail() const
{
  assert_choice_type(types::e_c_id_meas_initiation_fail, type_, "UnsuccessfulOutcome");
  return c.get<e_c_id_meas_initiation_fail_s>();
}
const otdoa_info_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::otdoa_info_fail() const
{
  assert_choice_type(types::otdoa_info_fail, type_, "UnsuccessfulOutcome");
  return c.get<otdoa_info_fail_s>();
}
const positioning_info_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::positioning_info_fail() const
{
  assert_choice_type(types::positioning_info_fail, type_, "UnsuccessfulOutcome");
  return c.get<positioning_info_fail_s>();
}
const meas_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::meas_fail() const
{
  assert_choice_type(types::meas_fail, type_, "UnsuccessfulOutcome");
  return c.get<meas_fail_s>();
}
const trp_info_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::trp_info_fail() const
{
  assert_choice_type(types::trp_info_fail, type_, "UnsuccessfulOutcome");
  return c.get<trp_info_fail_s>();
}
const positioning_activation_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::positioning_activation_fail() const
{
  assert_choice_type(types::positioning_activation_fail, type_, "UnsuccessfulOutcome");
  return c.get<positioning_activation_fail_s>();
}
const prs_cfg_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::prs_cfg_fail() const
{
  assert_choice_type(types::prs_cfg_fail, type_, "UnsuccessfulOutcome");
  return c.get<prs_cfg_fail_s>();
}
const meas_precfg_refuse_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::meas_precfg_refuse() const
{
  assert_choice_type(types::meas_precfg_refuse, type_, "UnsuccessfulOutcome");
  return c.get<meas_precfg_refuse_s>();
}
void nr_ppa_elem_procs_o::unsuccessful_outcome_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::e_c_id_meas_initiation_fail:
      j.write_fieldname("E-CIDMeasurementInitiationFailure");
      c.get<e_c_id_meas_initiation_fail_s>().to_json(j);
      break;
    case types::otdoa_info_fail:
      j.write_fieldname("OTDOAInformationFailure");
      c.get<otdoa_info_fail_s>().to_json(j);
      break;
    case types::positioning_info_fail:
      j.write_fieldname("PositioningInformationFailure");
      c.get<positioning_info_fail_s>().to_json(j);
      break;
    case types::meas_fail:
      j.write_fieldname("MeasurementFailure");
      c.get<meas_fail_s>().to_json(j);
      break;
    case types::trp_info_fail:
      j.write_fieldname("TRPInformationFailure");
      c.get<trp_info_fail_s>().to_json(j);
      break;
    case types::positioning_activation_fail:
      j.write_fieldname("PositioningActivationFailure");
      c.get<positioning_activation_fail_s>().to_json(j);
      break;
    case types::prs_cfg_fail:
      j.write_fieldname("PRSConfigurationFailure");
      c.get<prs_cfg_fail_s>().to_json(j);
      break;
    case types::meas_precfg_refuse:
      j.write_fieldname("MeasurementPreconfigurationRefuse");
      c.get<meas_precfg_refuse_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "nr_ppa_elem_procs_o::unsuccessful_outcome_c");
  }
  j.end_obj();
}
OCUDUASN_CODE nr_ppa_elem_procs_o::unsuccessful_outcome_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::e_c_id_meas_initiation_fail:
      HANDLE_CODE(c.get<e_c_id_meas_initiation_fail_s>().pack(bref));
      break;
    case types::otdoa_info_fail:
      HANDLE_CODE(c.get<otdoa_info_fail_s>().pack(bref));
      break;
    case types::positioning_info_fail:
      HANDLE_CODE(c.get<positioning_info_fail_s>().pack(bref));
      break;
    case types::meas_fail:
      HANDLE_CODE(c.get<meas_fail_s>().pack(bref));
      break;
    case types::trp_info_fail:
      HANDLE_CODE(c.get<trp_info_fail_s>().pack(bref));
      break;
    case types::positioning_activation_fail:
      HANDLE_CODE(c.get<positioning_activation_fail_s>().pack(bref));
      break;
    case types::prs_cfg_fail:
      HANDLE_CODE(c.get<prs_cfg_fail_s>().pack(bref));
      break;
    case types::meas_precfg_refuse:
      HANDLE_CODE(c.get<meas_precfg_refuse_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "nr_ppa_elem_procs_o::unsuccessful_outcome_c");
      return OCUDUASN_ERROR_ENCODE_FAIL;
  }
  return OCUDUASN_SUCCESS;
}
OCUDUASN_CODE nr_ppa_elem_procs_o::unsuccessful_outcome_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::e_c_id_meas_initiation_fail:
      HANDLE_CODE(c.get<e_c_id_meas_initiation_fail_s>().unpack(bref));
      break;
    case types::otdoa_info_fail:
      HANDLE_CODE(c.get<otdoa_info_fail_s>().unpack(bref));
      break;
    case types::positioning_info_fail:
      HANDLE_CODE(c.get<positioning_info_fail_s>().unpack(bref));
      break;
    case types::meas_fail:
      HANDLE_CODE(c.get<meas_fail_s>().unpack(bref));
      break;
    case types::trp_info_fail:
      HANDLE_CODE(c.get<trp_info_fail_s>().unpack(bref));
      break;
    case types::positioning_activation_fail:
      HANDLE_CODE(c.get<positioning_activation_fail_s>().unpack(bref));
      break;
    case types::prs_cfg_fail:
      HANDLE_CODE(c.get<prs_cfg_fail_s>().unpack(bref));
      break;
    case types::meas_precfg_refuse:
      HANDLE_CODE(c.get<meas_precfg_refuse_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "nr_ppa_elem_procs_o::unsuccessful_outcome_c");
      return OCUDUASN_ERROR_DECODE_FAIL;
  }
  return OCUDUASN_SUCCESS;
}

const char* nr_ppa_elem_procs_o::unsuccessful_outcome_c::types_opts::to_string() const
{
  static const char* names[] = {"E-CIDMeasurementInitiationFailure",
                                "OTDOAInformationFailure",
                                "PositioningInformationFailure",
                                "MeasurementFailure",
                                "TRPInformationFailure",
                                "PositioningActivationFailure",
                                "PRSConfigurationFailure",
                                "MeasurementPreconfigurationRefuse"};
  return convert_enum_idx(names, 8, value, "nr_ppa_elem_procs_o::unsuccessful_outcome_c::types");
}

// InitiatingMessage ::= SEQUENCE{{NRPPA-ELEMENTARY-PROCEDURE}}
OCUDUASN_CODE init_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != nr_ppa_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(pack_integer(bref, nrppatransaction_id, (uint16_t)0u, (uint16_t)32767u, false, true));
  HANDLE_CODE(value.pack(bref));

  return OCUDUASN_SUCCESS;
}
OCUDUASN_CODE init_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  HANDLE_CODE(unpack_integer(nrppatransaction_id, bref, (uint16_t)0u, (uint16_t)32767u, false, true));
  value = nr_ppa_elem_procs_o::get_init_msg(proc_code);
  HANDLE_CODE(value.unpack(bref));

  return OCUDUASN_SUCCESS;
}
void init_msg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("procedureCode", proc_code);
  j.write_str("criticality", crit.to_string());
  j.write_int("nrppatransactionID", nrppatransaction_id);
  j.write_fieldname("value");
  value.to_json(j);
  j.end_obj();
}
bool init_msg_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not nr_ppa_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = nr_ppa_elem_procs_o::get_crit(proc_code);
  value     = nr_ppa_elem_procs_o::get_init_msg(proc_code);
  return value.type().value != nr_ppa_elem_procs_o::init_msg_c::types_opts::nulltype;
}

// SuccessfulOutcome ::= SEQUENCE{{NRPPA-ELEMENTARY-PROCEDURE}}
OCUDUASN_CODE successful_outcome_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != nr_ppa_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(pack_integer(bref, nrppatransaction_id, (uint16_t)0u, (uint16_t)32767u, false, true));
  HANDLE_CODE(value.pack(bref));

  return OCUDUASN_SUCCESS;
}
OCUDUASN_CODE successful_outcome_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  HANDLE_CODE(unpack_integer(nrppatransaction_id, bref, (uint16_t)0u, (uint16_t)32767u, false, true));
  value = nr_ppa_elem_procs_o::get_successful_outcome(proc_code);
  HANDLE_CODE(value.unpack(bref));

  return OCUDUASN_SUCCESS;
}
void successful_outcome_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("procedureCode", proc_code);
  j.write_str("criticality", crit.to_string());
  j.write_int("nrppatransactionID", nrppatransaction_id);
  j.write_fieldname("value");
  value.to_json(j);
  j.end_obj();
}
bool successful_outcome_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not nr_ppa_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = nr_ppa_elem_procs_o::get_crit(proc_code);
  value     = nr_ppa_elem_procs_o::get_successful_outcome(proc_code);
  return value.type().value != nr_ppa_elem_procs_o::successful_outcome_c::types_opts::nulltype;
}

// UnsuccessfulOutcome ::= SEQUENCE{{NRPPA-ELEMENTARY-PROCEDURE}}
OCUDUASN_CODE unsuccessful_outcome_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != nr_ppa_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(pack_integer(bref, nrppatransaction_id, (uint16_t)0u, (uint16_t)32767u, false, true));
  HANDLE_CODE(value.pack(bref));

  return OCUDUASN_SUCCESS;
}
OCUDUASN_CODE unsuccessful_outcome_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  HANDLE_CODE(unpack_integer(nrppatransaction_id, bref, (uint16_t)0u, (uint16_t)32767u, false, true));
  value = nr_ppa_elem_procs_o::get_unsuccessful_outcome(proc_code);
  HANDLE_CODE(value.unpack(bref));

  return OCUDUASN_SUCCESS;
}
void unsuccessful_outcome_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("procedureCode", proc_code);
  j.write_str("criticality", crit.to_string());
  j.write_int("nrppatransactionID", nrppatransaction_id);
  j.write_fieldname("value");
  value.to_json(j);
  j.end_obj();
}
bool unsuccessful_outcome_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not nr_ppa_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = nr_ppa_elem_procs_o::get_crit(proc_code);
  value     = nr_ppa_elem_procs_o::get_unsuccessful_outcome(proc_code);
  return value.type().value != nr_ppa_elem_procs_o::unsuccessful_outcome_c::types_opts::nulltype;
}

// NRPPA-PDU ::= CHOICE
void nr_ppa_pdu_c::destroy_()
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
void nr_ppa_pdu_c::set(types::options e)
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
      log_invalid_choice_id(type_, "nr_ppa_pdu_c");
  }
}
nr_ppa_pdu_c::nr_ppa_pdu_c(const nr_ppa_pdu_c& other)
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
      log_invalid_choice_id(type_, "nr_ppa_pdu_c");
  }
}
nr_ppa_pdu_c& nr_ppa_pdu_c::operator=(const nr_ppa_pdu_c& other)
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
      log_invalid_choice_id(type_, "nr_ppa_pdu_c");
  }

  return *this;
}
init_msg_s& nr_ppa_pdu_c::set_init_msg()
{
  set(types::init_msg);
  return c.get<init_msg_s>();
}
successful_outcome_s& nr_ppa_pdu_c::set_successful_outcome()
{
  set(types::successful_outcome);
  return c.get<successful_outcome_s>();
}
unsuccessful_outcome_s& nr_ppa_pdu_c::set_unsuccessful_outcome()
{
  set(types::unsuccessful_outcome);
  return c.get<unsuccessful_outcome_s>();
}
void nr_ppa_pdu_c::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "nr_ppa_pdu_c");
  }
  j.end_obj();
}
OCUDUASN_CODE nr_ppa_pdu_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
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
      log_invalid_choice_id(type_, "nr_ppa_pdu_c");
      return OCUDUASN_ERROR_ENCODE_FAIL;
  }
  return OCUDUASN_SUCCESS;
}
OCUDUASN_CODE nr_ppa_pdu_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
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
      log_invalid_choice_id(type_, "nr_ppa_pdu_c");
      return OCUDUASN_ERROR_DECODE_FAIL;
  }
  return OCUDUASN_SUCCESS;
}

const char* nr_ppa_pdu_c::types_opts::to_string() const
{
  static const char* names[] = {"initiatingMessage", "successfulOutcome", "unsuccessfulOutcome"};
  return convert_enum_idx(names, 3, value, "nr_ppa_pdu_c::types");
}
