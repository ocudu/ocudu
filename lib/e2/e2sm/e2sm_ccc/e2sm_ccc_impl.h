// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e2/e2.h"
#include "ocudu/e2/e2sm/e2sm.h"
#include <map>

namespace ocudu {
class e2sm_ccc_impl : public e2sm_interface
{
public:
  e2sm_ccc_impl(ocudulog::basic_logger& logger_, e2sm_handler& e2sm_packer_);

  e2sm_handler& get_e2sm_packer() override;

  bool action_supported(const asn1::e2ap::ric_action_to_be_setup_item_s& ric_action) override;

  std::unique_ptr<e2sm_report_service> get_e2sm_report_service(const ocudu::byte_buffer& action_definition) override;
  e2sm_control_service*                get_e2sm_control_service(const e2sm_ric_control_request& request) override;

  bool add_e2sm_control_service(std::unique_ptr<e2sm_control_service> control_service) override;

private:
  ocudulog::basic_logger&                                   logger;
  e2sm_handler&                                             e2sm_packer;
  std::map<uint32_t, std::unique_ptr<e2sm_control_service>> control_services;
};

} // namespace ocudu
