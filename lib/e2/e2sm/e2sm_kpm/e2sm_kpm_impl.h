// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "e2sm_kpm_utils.h"
#include "ocudu/asn1/asn1_utils.h"
#include "ocudu/asn1/e2sm/e2sm_kpm_ies.h"
#include "ocudu/e2/e2.h"
#include "ocudu/e2/e2sm/e2sm.h"
#include "ocudu/e2/e2sm/e2sm_kpm.h"
#include <map>

namespace ocudu {

class e2sm_kpm_impl : public e2sm_interface
{
public:
  e2sm_kpm_impl(ocudulog::basic_logger& logger_, e2sm_handler& e2sm_packer_, e2sm_kpm_meas_provider& du_meas_provider_);

  e2sm_handler& get_e2sm_packer() override;

  bool action_supported(const asn1::e2ap::ric_action_to_be_setup_item_s& ric_action) override;

  std::unique_ptr<e2sm_report_service> get_e2sm_report_service(const ocudu::byte_buffer& action_definition) override;
  e2sm_control_service*                get_e2sm_control_service(const e2sm_ric_control_request& request) override;

  bool add_e2sm_control_service(std::unique_ptr<e2sm_control_service> control_service) override;

private:
  /// Helper functions to check whether subscription actions are supported.
  bool process_action_def_meas_info_list(const asn1::e2sm::meas_info_list_l& meas_info_list,
                                         const e2sm_kpm_metric_level_enum&   level,
                                         const bool&                         cell_scope);

  bool process_action_definition_format1(const asn1::e2sm::e2sm_kpm_action_definition_format1_s& action_definition,
                                         const e2sm_kpm_metric_level_enum                        level);

  bool process_action_definition_format1(const asn1::e2sm::e2sm_kpm_action_definition_s& action_def_generic);
  bool process_action_definition_format2(const asn1::e2sm::e2sm_kpm_action_definition_s& action_def_generic);
  bool process_action_definition_format3(const asn1::e2sm::e2sm_kpm_action_definition_s& action_def_generic);
  bool process_action_definition_format4(const asn1::e2sm::e2sm_kpm_action_definition_s& action_def_generic);
  bool process_action_definition_format5(const asn1::e2sm::e2sm_kpm_action_definition_s& action_def_generic);

  ocudulog::basic_logger& logger;
  e2sm_handler&           e2sm_packer;
  e2sm_kpm_meas_provider& du_meas_provider;
};

} // namespace ocudu
