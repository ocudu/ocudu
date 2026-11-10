// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "e2sm_rc_control_action_cu_executor.h"
#include "ocudu/f1ap/f1ap_ue_id_types.h"
#include "ocudu/ran/guami.h"
#include "ocudu/ran/nr_cgi.h"

using namespace asn1::e2ap;
using namespace asn1::e2sm;
using namespace ocudu;

e2sm_rc_control_action_cu_executor_base::e2sm_rc_control_action_cu_executor_base(cu_configurator& cu_configurator_,
                                                                                 uint32_t         action_id_) :
  logger(ocudulog::fetch_basic_logger("E2SM-RC")), action_id(action_id_), cu_param_configurator(cu_configurator_)
{
}

uint32_t e2sm_rc_control_action_cu_executor_base::get_action_id()
{
  return action_id;
}

std::string e2sm_rc_control_action_cu_executor_base::get_action_name()
{
  return action_name;
}

ran_function_definition_ctrl_action_item_s e2sm_rc_control_action_cu_executor_base::get_control_action_definition()
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
e2sm_rc_control_action_cu_executor_base::return_ctrl_failure(const e2sm_ric_control_request& req)
{
  return launch_async([req](coro_context<async_task<e2sm_ric_control_response>>& ctx) {
    CORO_BEGIN(ctx);
    e2sm_ric_control_response e2sm_response;
    e2sm_response.success                = false;
    e2sm_response.cause.set_misc().value = cause_misc_e::options::unspecified;
    CORO_RETURN(e2sm_response);
  });
}

e2sm_rc_control_action_3_1_cu_executor::e2sm_rc_control_action_3_1_cu_executor(cu_configurator& cu_configurator_) :
  e2sm_rc_control_action_cu_executor_base(cu_configurator_, 1)
{
  action_name = "Handover Control";
  action_params.insert({1, "Target Primary Cell ID"});
  action_params.insert({2, ">CHOICE Target Cell"});
  action_params.insert({3, ">>NR Cell"});
  action_params.insert({4, ">>>NR CGI"});
  action_params.insert({5, ">>E-UTRA Cell"});
  action_params.insert({6, ">>>E-UTRA CGI"});
  // TODO: add parameters 7-21 (related to PDU session, DRBs, Secondary cell) when supported.
}

bool e2sm_rc_control_action_3_1_cu_executor::ric_control_action_supported(const e2sm_ric_control_request& req)
{
  return true;
}

async_task<e2sm_ric_control_response>
e2sm_rc_control_action_3_1_cu_executor::execute_ric_control_action(const e2sm_ric_control_request& req)
{
  const e2sm_rc_ctrl_hdr_format1_s& ctrl_hdr =
      std::get<e2sm_rc_ctrl_hdr_s>(req.request_ctrl_hdr).ric_ctrl_hdr_formats.ctrl_hdr_format1();
  const e2sm_rc_ctrl_msg_format1_s& ctrl_msg =
      std::get<e2sm_rc_ctrl_msg_s>(req.request_ctrl_msg).ric_ctrl_msg_formats.ctrl_msg_format1();

  if (ctrl_hdr.ue_id.type() != ue_id_c::types_opts::gnb_ue_id) {
    return return_ctrl_failure(req);
  }
  const ue_id_gnb_s& gnb_ue_id = ctrl_hdr.ue_id.gnb_ue_id();
  if (gnb_ue_id.gnb_cu_ue_f1ap_id_list.size() == 0) {
    return return_ctrl_failure(req);
  }

  // Translate UE-IDs.
  ocucp::amf_ue_id_t      amf_ue_id  = ocucp::uint_to_amf_ue_id(gnb_ue_id.amf_ue_ngap_id);
  expected<plmn_identity> guami_plmn = plmn_identity::from_bytes(gnb_ue_id.guami.plmn_id.to_bytes());
  if (not guami_plmn.has_value()) {
    logger.error("Ignoring Handover Request. Cause: GUAMI carries an invalid PLMN identity.");
    return return_ctrl_failure(req);
  }
  guami_t guami;
  guami.plmn                                 = guami_plmn.value();
  guami.amf_set_id                           = static_cast<uint16_t>(gnb_ue_id.guami.amf_set_id.to_number());
  guami.amf_pointer                          = static_cast<uint8_t>(gnb_ue_id.guami.amf_pointer.to_number());
  guami.amf_region_id                        = static_cast<uint8_t>(gnb_ue_id.guami.amf_region_id.to_number());
  uint64_t            gnb_cu_ue_f1ap_id_uint = gnb_ue_id.gnb_cu_ue_f1ap_id_list[0].gnb_cu_ue_f1ap_id;
  gnb_cu_ue_f1ap_id_t gnb_cu_ue_f1ap_id      = int_to_gnb_cu_ue_f1ap_id(gnb_cu_ue_f1ap_id_uint);
  cu_cp_ue_index_t    ue_index               = cu_param_configurator.get_ue_index(amf_ue_id, guami, gnb_cu_ue_f1ap_id);

  if (ue_index == cu_cp_ue_index_t::invalid) {
    logger.error("Ignoring Handover Request. Cause: Couldn't find UE index for given UE-IDs.");
    return return_ctrl_failure(req);
  }

  auto source_du_index = cu_param_configurator.get_du_index(ue_index);
  if (source_du_index == cu_cp_du_index_t::invalid) {
    logger.error("Ignoring Handover Request. Cause: Couldn't find DU index for given UE-IDs.");
    return return_ctrl_failure(req);
  }

  cu_handover_control_config ho_ctrl_cfg;
  bool                       malformed = false;
  // Use a lambda to define the templete function in parse_ran_parameter_value
  auto parse_action_ran_parameter_value_lambda = [this, &malformed](const ran_param_value_type_c& ran_param,
                                                                    uint64_t                      ran_param_id,
                                                                    uint64_t                      ue_id,
                                                                    cu_handover_control_config&   ctrl_cfg) {
    if (not this->parse_action_ran_parameter_value(ran_param, ran_param_id, ue_id, ctrl_cfg)) {
      malformed = true;
    }
  };
  for (auto& ran_p : ctrl_msg.ran_p_list) {
    if (action_params.find(ran_p.ran_param_id) != action_params.end()) {
      parse_ran_parameter_value(ran_p.ran_param_value_type,
                                ran_p.ran_param_id,
                                to_underlying(ue_index),
                                ho_ctrl_cfg,
                                parse_action_ran_parameter_value_lambda);
    }
  }

  // Do not hand the UE over to a target taken from a request that does not respect the RAN parameter definitions,
  // as an earlier parameter may have left a valid target behind.
  if (malformed) {
    logger.error("Ignoring Handover Request. Cause: The control message carries a malformed RAN parameter.");
    return return_ctrl_failure(req);
  }

  ocucp::cu_cp_intra_cu_handover_request handover_req;
  handover_req.source_ue_index = ue_index;
  handover_req.target_du_index = cu_param_configurator.get_du_index(ho_ctrl_cfg.target_cell_id);
  handover_req.cgi             = ho_ctrl_cfg.target_cell_id;
  handover_req.target_pci      = cu_param_configurator.get_pci(ho_ctrl_cfg.target_cell_id);

  if (handover_req.target_du_index == cu_cp_du_index_t::invalid) {
    logger.error("Ignoring Handover Request. Cause: Couldn't find DU index for CGI=[plmn: {}, nci: {}]",
                 ho_ctrl_cfg.target_cell_id.plmn_id.to_string(),
                 ho_ctrl_cfg.target_cell_id.nci.value());
    return return_ctrl_failure(req);
  }
  if (handover_req.target_pci == INVALID_PCI) {
    logger.error("ue={}: Ignoring Handover Request. Couldn't find PCI for CGI=[plmn: {}, nci: {}]",
                 ho_ctrl_cfg.target_cell_id.plmn_id.to_string(),
                 ho_ctrl_cfg.target_cell_id.nci.value());
    return return_ctrl_failure(req);
  }

  if (source_du_index == handover_req.target_du_index) {
    logger.info("Executing E2SM-RC Handover Request Trigger intra-CU (intra-DU) handover for ue={} on du={} ",
                ue_index,
                source_du_index);
  } else {
    logger.info("Executing E2SM-RC Handover Request Trigger intra-CU (inter-DU) handover for ue={} from source_du={} "
                "to target_du={}",
                ue_index,
                source_du_index,
                handover_req.target_du_index);
  }
  return launch_async([this, source_du_index, handover_req = std::move(handover_req)](
                          coro_context<async_task<e2sm_ric_control_response>>& ctx) {
    ocucp::cu_cp_intra_cu_handover_response cu_cp_response;
    CORO_BEGIN(ctx);
    CORO_AWAIT_VALUE(cu_cp_response, cu_param_configurator.trigger_handover(source_du_index, handover_req));

    e2sm_ric_control_response e2sm_response;
    e2sm_response.success = cu_cp_response.success;
    if (!e2sm_response.success) {
      e2sm_response.cause.set_misc().value = cause_misc_e::options::unspecified;
    }
    CORO_RETURN(e2sm_response);
  });
}

bool e2sm_rc_control_action_3_1_cu_executor::parse_action_ran_parameter_value(
    const asn1::e2sm::ran_param_value_type_c& ran_p,
    uint64_t                                  ran_param_id,
    uint64_t                                  ue_id,
    cu_handover_control_config&               ctrl_cfg)
{
  if (ran_param_id != 4) {
    return true;
  }
  // Reading a RAN parameter as a type it does not hold reinterprets its storage, so the value type has to be checked
  // before the value is read.
  if (ran_p.type() != ran_param_value_type_c::types_opts::ran_p_choice_elem_false) {
    logger.error("Target NR CGI is not an element");
    return false;
  }
  const auto& elem = ran_p.ran_p_choice_elem_false();
  if (not elem.ran_param_value_present) {
    logger.error("Target NR CGI carries no value");
    return false;
  }
  if (elem.ran_param_value.type() != ran_param_value_c::types_opts::value_oct_s) {
    logger.error("Target NR CGI is not an octet string");
    return false;
  }
  const auto& value_oct = elem.ran_param_value.value_oct_s();
  if (value_oct.size() != 9) {
    logger.error("Invalid NR CGI octet string length: {}", value_oct.size());
    return false;
  }
  nr_cgi_s             nr_cgi_asn;
  std::vector<uint8_t> cgi_bytes(value_oct.size());
  std::copy(value_oct.begin(), value_oct.end(), cgi_bytes.begin());
  byte_buffer    cgi_buf = byte_buffer::create(cgi_bytes.begin(), cgi_bytes.end()).value();
  asn1::cbit_ref bref(cgi_buf);
  if (nr_cgi_asn.unpack(bref) != asn1::OCUDUASN_SUCCESS) {
    logger.error("Failed to unpack NR CGI");
    return false;
  }
  expected<plmn_identity> target_plmn = plmn_identity::from_bytes(nr_cgi_asn.plmn_id.to_bytes());
  if (not target_plmn.has_value()) {
    logger.error("Invalid PLMN identity in target NR CGI");
    return false;
  }
  ctrl_cfg.target_cell_id.nci     = nr_cell_identity::create(nr_cgi_asn.nr_cell_id.to_number()).value();
  ctrl_cfg.target_cell_id.plmn_id = target_plmn.value();
  return true;
}
