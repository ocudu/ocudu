// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "e2sm_ccc_impl.h"
#include "ocudu/asn1/asn1_dispatch.h"
#include "ocudu/asn1/e2sm_ccc/e2sm_ccc.h"
#include "ocudu/e2/e2sm/e2sm.h"

using namespace asn1::e2ap;
using namespace asn1::e2sm_ccc;
using namespace ocudu;

e2sm_ccc_impl::e2sm_ccc_impl(ocudulog::basic_logger& logger_, e2sm_handler& e2sm_packer_) :
  logger(logger_), e2sm_packer(e2sm_packer_)
{
}

bool e2sm_ccc_impl::action_supported(const ric_action_to_be_setup_item_s& ric_action)
{
  if (ric_action.ric_action_type != ric_action_type_e::report) {
    logger.debug("Only REPORT actions are supported, action {} is not", ric_action.ric_action_id);
    return false;
  }
  e2sm_action_definition action_def =
      e2sm_packer.handle_packed_e2sm_action_definition(ric_action.ric_action_definition);
  if (action_def.service_model != e2sm_service_model_t::CCC) {
    logger.debug("Failed to decode the action definition of action {}", ric_action.ric_action_id);
    return false;
  }
  const auto& definition = std::get<ric_action_definition_s>(action_def.action_definition);
  if (definition.action_definition_format.type() == action_definition_format_c::types::nulltype) {
    logger.debug("Action {} has no action definition format", ric_action.ric_action_id);
    return false;
  }
  return true;
}

e2sm_handler& e2sm_ccc_impl::get_e2sm_packer()
{
  return e2sm_packer;
}

std::unique_ptr<e2sm_report_service> e2sm_ccc_impl::get_e2sm_report_service(const ocudu::byte_buffer& action_definition)
{
  logger.debug("Report service not supported.");
  return nullptr;
}

bool e2sm_ccc_impl::add_e2sm_control_service(std::unique_ptr<e2sm_control_service> control_service)
{
  if (control_service == nullptr) {
    return false;
  }
  // Only 2 control service styles are defined.
  if (control_service->get_style_type() == 0 or control_service->get_style_type() > 2) {
    return false;
  }
  control_services.emplace(control_service->get_style_type(), std::move(control_service));
  return true;
}

e2sm_control_service* e2sm_ccc_impl::get_e2sm_control_service(const e2sm_ric_control_request& request)
{
  // A request that failed to unpack carries the header of another service model, and holds no style to select by.
  const auto* ctrl_hdr = std::get_if<ric_ctrl_hdr_s>(&request.request_ctrl_hdr);
  if (ctrl_hdr == nullptr) {
    return nullptr;
  }
  int64_t ric_style_type = ctrl_hdr->ctrl_hdr_format.ctrl_hdr_format1().ric_style_type;

  if (control_services.find(ric_style_type) != control_services.end()) {
    return control_services.at(ric_style_type).get();
  }
  return nullptr;
}
