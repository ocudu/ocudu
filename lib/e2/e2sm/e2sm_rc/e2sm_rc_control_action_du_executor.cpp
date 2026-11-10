// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "e2sm_rc_control_action_du_executor.h"
#include "ocudu/f1ap/du/f1ap_du.h"
#include "ocudu/support/format/fmt_to_c_str.h"
#include <future>

using namespace asn1::e2ap;
using namespace asn1::e2sm;
using namespace ocudu;

e2sm_rc_control_action_du_executor_base::e2sm_rc_control_action_du_executor_base(
    odu::du_configurator&       du_configurator_,
    odu::f1ap_ue_id_translator& f1ap_ue_id_translator_,
    uint32_t                    action_id_) :
  logger(ocudulog::fetch_basic_logger("E2SM-RC")),
  action_id(action_id_),
  du_param_configurator(du_configurator_),
  f1ap_ue_id_provider(f1ap_ue_id_translator_)
{
}

uint32_t e2sm_rc_control_action_du_executor_base::get_action_id()
{
  return action_id;
}

std::string e2sm_rc_control_action_du_executor_base::get_action_name()
{
  return action_name;
}

ran_function_definition_ctrl_action_item_s e2sm_rc_control_action_du_executor_base::get_control_action_definition()
{
  ran_function_definition_ctrl_action_item_s action_item;
  action_item.ric_ctrl_action_id = action_id;
  action_item.ric_ctrl_action_name.resize(action_name.size());
  action_item.ric_ctrl_action_name.from_string(action_name);

  for (auto& ran_p : action_params) {
    ctrl_action_ran_param_item_s ctrl_action_ran_param_item;
    ctrl_action_ran_param_item.ran_param_id = ran_p.first;
    ctrl_action_ran_param_item.ran_param_name.resize(ran_p.second.size());
    ctrl_action_ran_param_item.ran_param_name.from_string(ran_p.second);
    action_item.ran_ctrl_action_params_list.push_back(ctrl_action_ran_param_item);
  }

  return action_item;
}

async_task<e2sm_ric_control_response>
e2sm_rc_control_action_du_executor_base::return_ctrl_failure(const e2sm_ric_control_request& req)
{
  cause_c cause;
  cause.set_misc().value = cause_misc_e::options::unspecified;
  return return_ctrl_failure(req, cause);
}

async_task<e2sm_ric_control_response>
e2sm_rc_control_action_du_executor_base::return_ctrl_failure(const e2sm_ric_control_request& req, const cause_c& cause)
{
  return launch_async([req, cause](coro_context<async_task<e2sm_ric_control_response>>& ctx) {
    CORO_BEGIN(ctx);
    e2sm_ric_control_response e2sm_response;
    e2sm_response.success = false;
    e2sm_response.cause   = cause;
    CORO_RETURN(e2sm_response);
  });
}

e2sm_rc_control_action_2_6_du_executor::e2sm_rc_control_action_2_6_du_executor(
    odu::du_configurator&       du_configurator_,
    odu::f1ap_ue_id_translator& f1ap_ue_id_translator_) :
  e2sm_rc_control_action_du_executor_base(du_configurator_, f1ap_ue_id_translator_, 6)
{
  // Control Action description:
  // To control the radio resource management policy for slice-specific PRB quota allocation
  // Note: Parameter id 4 missing/skipped in TS.
  action_name = "Slice-level PRB quota";
  action_params.insert({1, "RRM Policy Ratio List"});
  action_params.insert({2, "RRM Policy Ratio Group"});
  action_params.insert({3, "RRM Policy"});
  action_params.insert({5, "RRM Policy Member List"});
  action_params.insert({6, "RRM Policy Member"});
  action_params.insert({7, "PLMN Identity"});
  action_params.insert({8, "S-NSSAI"});
  action_params.insert({9, "SST"});
  action_params.insert({10, "SD"});
  action_params.insert({11, "Min PRB Policy Ratio"});
  action_params.insert({12, "Max PRB Policy Ratio"});
  action_params.insert({13, "Dedicated PRB Policy Ratio"});
}

bool e2sm_rc_control_action_2_6_du_executor::parse_action_ran_parameter_value(
    const ran_param_value_type_c&     ran_param,
    uint64_t                          ran_param_id,
    uint64_t                          ue_id,
    odu::du_mac_sched_control_config& ctrl_cfg)
{
  // Parameters 7, 9 and 10 identify a RRM Policy Member, and parameters 11, 12 and 13 set the PRB quota of a RRM
  // Policy Ratio Group. The remaining ones only group the parameters below them.
  const bool is_policy_member_param = ran_param_id == 7 or ran_param_id == 9 or ran_param_id == 10;
  const bool is_policy_ratio_param  = ran_param_id == 11 or ran_param_id == 12 or ran_param_id == 13;

  if (is_policy_member_param or is_policy_ratio_param) {
    // Reading a RAN parameter as a type it does not hold reinterprets its storage, so the value type has to be
    // checked before the value is read.
    if (ran_param.type() != ran_param_value_type_c::types_opts::ran_p_choice_elem_false) {
      logger.info("E2SM-RC Slice-level PRB quota Control Request: RAN parameter (param_id={}) is not an element.",
                  ran_param_id);
      return false;
    }
    const auto& elem = ran_param.ran_p_choice_elem_false();
    if (not elem.ran_param_value_present) {
      logger.info("E2SM-RC Slice-level PRB quota Control Request: RAN parameter (param_id={}) carries no value.",
                  ran_param_id);
      return false;
    }
    const auto expected_value_type =
        is_policy_member_param ? ran_param_value_c::types_opts::value_oct_s : ran_param_value_c::types_opts::value_int;
    if (elem.ran_param_value.type() != expected_value_type) {
      logger.info(
          "E2SM-RC Slice-level PRB quota Control Request: RAN parameter (param_id={}) carries an unexpected value "
          "type.",
          ran_param_id);
      return false;
    }
  }

  // A RRM Policy Member and the PRB quota belong to the RRM Policy Ratio Group that encloses them, and the PLMN, the
  // SST and the SD to the RRM Policy Member. Reject a parameter that its enclosing structure does not precede.
  if ((ran_param_id == 6 or is_policy_member_param or is_policy_ratio_param) and
      ctrl_cfg.rrm_policy_ratio_list.empty()) {
    logger.info("E2SM-RC Slice-level PRB quota Control Request: RAN parameter (param_id={}) is not preceded by a RRM "
                "Policy Ratio Group.",
                ran_param_id);
    return false;
  }
  if (is_policy_member_param and ctrl_cfg.rrm_policy_ratio_list.back().policy_members_list.empty()) {
    logger.info("E2SM-RC Slice-level PRB quota Control Request: RAN parameter (param_id={}) is not preceded by a RRM "
                "Policy Member.",
                ran_param_id);
    return false;
  }

  if (ran_param_id == 1 or ran_param_id == 3 or ran_param_id == 5 or ran_param_id == 8) {
    // No need to parse.
  } else if (ran_param_id == 2) {
    // RRM Policy Ratio Group.
    ctrl_cfg.rrm_policy_ratio_list.emplace_back();
  } else if (ran_param_id == 6) {
    // RRM Policy Member List.
    ctrl_cfg.rrm_policy_ratio_list.back().policy_members_list.emplace_back();
  } else if (ran_param_id == 7) {
    // PLMN Identity.
    const auto& plmn_octets = ran_param.ran_p_choice_elem_false().ran_param_value.value_oct_s();
    // A PLMN identity is an OCTET STRING (SIZE(3)), as per TS 38.473, Section 9.3.1.14.
    if (plmn_octets.size() != 3) {
      logger.info("E2SM-RC Slice-level PRB quota Control Request: PLMN (param_id={}) encoded not correctly.",
                  ran_param_id);
      return false;
    }
    std::array<uint8_t, 3> plmn_bytes;
    std::copy(plmn_octets.begin(), plmn_octets.end(), plmn_bytes.begin());
    expected<plmn_identity> plmn_id = plmn_identity::from_bytes(plmn_bytes);
    if (not plmn_id.has_value()) {
      logger.info("E2SM-RC Slice-level PRB quota Control Request: PLMN (param_id={}) is not a valid PLMN identity.",
                  ran_param_id);
      return false;
    }
    ctrl_cfg.rrm_policy_ratio_list.back().policy_members_list.back().plmn_id = plmn_id.value();
  } else if (ran_param_id == 9) {
    // SST.
    const auto& sst_octets = ran_param.ran_p_choice_elem_false().ran_param_value.value_oct_s();
    // An SST is an OCTET STRING (SIZE(1)), as per TS 38.473, Section 9.3.1.38.
    if (sst_octets.size() != 1) {
      logger.info("E2SM-RC Slice-level PRB quota Control Request: SST (param_id={}) encoded not correctly.",
                  ran_param_id);
      return false;
    }
    ctrl_cfg.rrm_policy_ratio_list.back().policy_members_list.back().s_nssai.sst =
        slice_service_type{static_cast<uint8_t>(sst_octets.to_number())};
  } else if (ran_param_id == 10) {
    // SD.
    const auto& sd_octets = ran_param.ran_p_choice_elem_false().ran_param_value.value_oct_s();
    // An SD is an OCTET STRING (SIZE(3)), as per TS 38.473, Section 9.3.1.38.
    if (sd_octets.size() != 3) {
      logger.info("E2SM-RC Slice-level PRB quota Control Request: SD (param_id={}) encoded not correctly.",
                  ran_param_id);
      return false;
    }
    expected<slice_differentiator> sd = slice_differentiator::create(static_cast<uint32_t>(sd_octets.to_number()));
    if (not sd.has_value()) {
      logger.info("E2SM-RC Slice-level PRB quota Control Request: SD (param_id={}) exceeds 24 bits.", ran_param_id);
      return false;
    }
    ctrl_cfg.rrm_policy_ratio_list.back().policy_members_list.back().s_nssai.sd = sd.value();
  } else if (ran_param_id == 11) {
    // Min PRB Policy Ratio.
    const int64_t min_ratio = ran_param.ran_p_choice_elem_false().ran_param_value.value_int();
    if (min_ratio < 0 or min_ratio > 100) {
      logger.info("E2SM-RC Slice-level PRB quota Control Request: Min PRB Policy Ratio (param_id={}) is out of range.",
                  ran_param_id);
      return false;
    }
    ctrl_cfg.rrm_policy_ratio_list.back().minimum_ratio = static_cast<unsigned>(min_ratio);
  } else if (ran_param_id == 12) {
    // Max PRB Policy Ratio.
    const int64_t max_ratio = ran_param.ran_p_choice_elem_false().ran_param_value.value_int();
    if (max_ratio < 0 or max_ratio > 100) {
      logger.info("E2SM-RC Slice-level PRB quota Control Request: Max PRB Policy Ratio (param_id={}) is out of range.",
                  ran_param_id);
      return false;
    }
    ctrl_cfg.rrm_policy_ratio_list.back().maximum_ratio = static_cast<unsigned>(max_ratio);
  } else if (ran_param_id == 13) {
    // Dedicated PRB Policy Ratio.
    const int64_t ded_ratio = ran_param.ran_p_choice_elem_false().ran_param_value.value_int();
    if (ded_ratio < 0 or ded_ratio > 100) {
      logger.info(
          "E2SM-RC Slice-level PRB quota Control Request: Dedicated PRB Policy Ratio (param_id={}) is out of range.",
          ran_param_id);
      return false;
    }
    ctrl_cfg.rrm_policy_ratio_list.back().dedicated_ratio = static_cast<unsigned>(ded_ratio);
  } else {
    logger.error("Unknown RAN parameter ID {}", ran_param_id);
  }
  return true;
}

bool e2sm_rc_control_action_2_6_du_executor::ric_control_action_supported(const e2sm_ric_control_request& req)
{
  const e2sm_rc_ctrl_msg_format1_s& ctrl_msg =
      std::get<e2sm_rc_ctrl_msg_s>(req.request_ctrl_msg).ric_ctrl_msg_formats.ctrl_msg_format1();

  for (auto& ran_p : ctrl_msg.ran_p_list) {
    if (action_params.find(ran_p.ran_param_id) == action_params.end()) {
      return false;
    }
  };
  return true;
}

async_task<e2sm_ric_control_response>
e2sm_rc_control_action_2_6_du_executor::execute_ric_control_action(const e2sm_ric_control_request& req)
{
  bool                             malformed   = false;
  odu::du_mac_sched_control_config ctrl_config = convert_to_du_config_request(req, malformed);
  // If the control message does not respect the RAN parameter definitions, report it as invalid.
  if (malformed) {
    cause_c cause;
    cause.set_ric_request().value = cause_ric_request_e::options::ctrl_msg_invalid;
    return return_ctrl_failure(req, cause);
  }
  // If empty request, return failure.
  if (ctrl_config.rrm_policy_ratio_list.empty()) {
    return return_ctrl_failure(req);
  }
  // If any policy is missing members, return failure.
  for (const auto& policy : ctrl_config.rrm_policy_ratio_list) {
    if (policy.policy_members_list.empty()) {
      return return_ctrl_failure(req);
    }
  }
  // If PRB quota not provided, return failure.
  for (const auto& policy : ctrl_config.rrm_policy_ratio_list) {
    if (!policy.minimum_ratio.has_value()) {
      return return_ctrl_failure(req);
    }
    if (!policy.maximum_ratio.has_value()) {
      return return_ctrl_failure(req);
    }
    if (!policy.dedicated_ratio.has_value()) {
      return return_ctrl_failure(req);
    }
  }

  // Log received control request.
  fmt::memory_buffer log_buffer;
  fmt::format_to(std::back_inserter(log_buffer),
                 "Slice-level PRB quota Control Request for UE gNB-DU-UE-F1AP-ID={}:\n",
                 to_underlying(ctrl_config.ue_id));
  fmt::format_to(std::back_inserter(log_buffer), "RRM Policy Ratio Group:\n");
  for (const auto& rrm_policy_ratio : ctrl_config.rrm_policy_ratio_list) {
    fmt::format_to(std::back_inserter(log_buffer), " RRM Policy:\n");
    fmt::format_to(std::back_inserter(log_buffer), "  Min PRB Policy Ratio: {}\n", *rrm_policy_ratio.minimum_ratio);
    fmt::format_to(std::back_inserter(log_buffer), "  Max PRB Policy Ratio: {}\n", *rrm_policy_ratio.maximum_ratio);
    fmt::format_to(
        std::back_inserter(log_buffer), "  Dedicated PRB Policy Ratio: {}\n", *rrm_policy_ratio.dedicated_ratio);
    fmt::format_to(std::back_inserter(log_buffer), "  RRM Policy Member List:\n");
    for (const auto& policy_member : rrm_policy_ratio.policy_members_list) {
      fmt::format_to(std::back_inserter(log_buffer),
                     "  - PLMN:{}, SST:{}, SD:{}\n",
                     policy_member.plmn_id.to_string(),
                     policy_member.s_nssai.sst.value(),
                     policy_member.s_nssai.sd.value());
    }
  }
  logger.info("{}", to_c_str(log_buffer));
  log_buffer.clear();

  return launch_async(
      [this, ctrl_config = std::move(ctrl_config)](coro_context<async_task<e2sm_ric_control_response>>& ctx) {
        CORO_BEGIN(ctx);
        odu::du_mac_sched_control_config_response ctrl_response;
        CORO_AWAIT_VALUE(ctrl_response, du_param_configurator.configure_ue_mac_scheduler(ctrl_config));
        e2sm_ric_control_response e2_resp = convert_to_e2sm_response(ctrl_config, ctrl_response);
        CORO_RETURN(e2_resp);
      });
}

odu::du_mac_sched_control_config
e2sm_rc_control_action_2_6_du_executor::convert_to_du_config_request(const e2sm_ric_control_request& e2sm_req_,
                                                                     bool&                           malformed)
{
  odu::du_mac_sched_control_config  ctrl_config = {};
  const e2sm_rc_ctrl_hdr_format1_s& ctrl_hdr =
      std::get<e2sm_rc_ctrl_hdr_s>(e2sm_req_.request_ctrl_hdr).ric_ctrl_hdr_formats.ctrl_hdr_format1();
  const e2sm_rc_ctrl_msg_format1_s& ctrl_msg =
      std::get<e2sm_rc_ctrl_msg_s>(e2sm_req_.request_ctrl_msg).ric_ctrl_msg_formats.ctrl_msg_format1();

  if (ctrl_hdr.ue_id.type() != ue_id_c::types_opts::gnb_du_ue_id) {
    return {};
  }
  uint64_t gnb_cu_ue_f1ap_id = ctrl_hdr.ue_id.gnb_du_ue_id().gnb_cu_ue_f1ap_id;
  ctrl_config.ue_id          = f1ap_ue_id_provider.get_gnb_du_ue_f1ap_id(int_to_gnb_cu_ue_f1ap_id(gnb_cu_ue_f1ap_id));

  if (ctrl_config.ue_id == gnb_du_ue_f1ap_id_t::invalid) {
    logger.info("Slice-level PRB quota Control Request received but UE not found, gNB-CU-UE-F1AP-ID={}",
                gnb_cu_ue_f1ap_id);
    return {};
  }

  // Use a lambda to define the template function in parse_ran_parameter_value
  auto parse_action_ran_parameter_value_lambda = [this, &malformed](const ran_param_value_type_c&     ran_param,
                                                                    uint64_t                          ran_param_id,
                                                                    uint64_t                          ue_id,
                                                                    odu::du_mac_sched_control_config& ctrl_cfg) {
    if (not this->parse_action_ran_parameter_value(ran_param, ran_param_id, ue_id, ctrl_cfg)) {
      malformed = true;
    }
  };
  for (auto& ran_p : ctrl_msg.ran_p_list) {
    if (action_params.find(ran_p.ran_param_id) != action_params.end()) {
      parse_ran_parameter_value(ran_p.ran_param_value_type,
                                ran_p.ran_param_id,
                                gnb_cu_ue_f1ap_id,
                                ctrl_config,
                                parse_action_ran_parameter_value_lambda);
    }
  }
  // Reject the whole request, so that a malformed policy member cannot be applied in part.
  if (malformed) {
    return {};
  }
  if (ctrl_config.rrm_policy_ratio_list.empty()) {
    return {};
  }
  return ctrl_config;
}

e2sm_ric_control_response e2sm_rc_control_action_2_6_du_executor::convert_to_e2sm_response(
    const odu::du_mac_sched_control_config&          du_config_req_,
    const odu::du_mac_sched_control_config_response& du_response_)
{
  e2sm_ric_control_response e2sm_response;
  e2sm_response.success =
      du_response_.harq_processes_result and du_response_.max_prb_alloc_result and du_response_.min_prb_alloc_result;

  // Always fill outcome here, it will be decided later whether it should be included in the e2 response.
  e2sm_response.ric_ctrl_outcome_present = false;
  if (!e2sm_response.success) {
    e2sm_response.cause.set_misc().value = cause_misc_e::options::unspecified;
  }

  return e2sm_response;
}
