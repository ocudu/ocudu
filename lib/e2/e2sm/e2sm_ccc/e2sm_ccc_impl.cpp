// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "e2sm_ccc_impl.h"
#include "ocudu/asn1/asn1_utils.h"
#include "ocudu/asn1/e2sm/e2sm_ccc.h"
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
  logger.debug("No action supported.");
  return false;
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
  const auto& ctrl_hdr       = std::get<ric_ctrl_hdr_s>(request.request_ctrl_hdr);
  int64_t     ric_style_type = ctrl_hdr.ctrl_hdr_format.ctrl_hdr_format1().ric_style_type;

  if (control_services.find(ric_style_type) != control_services.end()) {
    return control_services.at(ric_style_type).get();
  }
  return nullptr;
}
