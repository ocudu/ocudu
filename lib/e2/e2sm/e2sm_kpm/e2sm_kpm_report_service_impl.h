// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/asn1_utils.h"
#include "ocudu/asn1/e2sm/e2sm_kpm_ies.h"
#include "ocudu/e2/e2.h"
#include "ocudu/e2/e2sm/e2sm.h"
#include "ocudu/e2/e2sm/e2sm_kpm.h"
#include <map>

namespace ocudu {

class e2sm_kpm_report_service_base : public e2sm_report_service
{
public:
  e2sm_kpm_report_service_base() = delete;
  e2sm_kpm_report_service_base(asn1::e2sm::e2sm_kpm_action_definition_s action_def_,
                               e2sm_kpm_meas_provider&                  meas_provider_);
  virtual ~e2sm_kpm_report_service_base() = default;

  /// e2sm_report_service functions.
  virtual bool               collect_measurements() override = 0;
  virtual bool               is_ind_msg_ready() override;
  virtual ocudu::byte_buffer get_indication_message() override;
  ocudu::byte_buffer         get_indication_header() override;

protected:
  /// Helper functions.
  virtual bool initialize_ric_ind_msg_format_1(asn1::e2sm::meas_info_list_l&           action_meas_info_list,
                                               asn1::e2sm::e2sm_kpm_ind_msg_format1_s& ric_ind_msg);
  virtual void clear_collect_measurements() = 0;

  ocudulog::basic_logger&                  logger;
  asn1::e2sm::e2sm_kpm_action_definition_s action_def_generic;
  uint32_t                                 granul_period  = 0;
  std::optional<asn1::e2sm::cgi_c>         cell_global_id = {};

  e2sm_kpm_meas_provider&        meas_provider;
  asn1::e2sm::e2sm_kpm_ind_hdr_s ric_ind_header_generic;
  asn1::e2sm::e2sm_kpm_ind_msg_s ric_ind_message_generic;

  asn1::e2sm::e2sm_kpm_ind_hdr_format1_s& ric_ind_header;
  bool                                    is_ind_msg_ready_;
};

class e2sm_kpm_report_service_style1 : public e2sm_kpm_report_service_base
{
public:
  e2sm_kpm_report_service_style1(asn1::e2sm::e2sm_kpm_action_definition_s action_def_,
                                 e2sm_kpm_meas_provider&                  meas_provider_);
  virtual ~e2sm_kpm_report_service_style1() = default;

  bool collect_measurements() override;

private:
  void clear_collect_measurements() override;

  asn1::e2sm::e2sm_kpm_action_definition_format1_s& action_def;
  asn1::e2sm::e2sm_kpm_ind_msg_format1_s&           ric_ind_message;
};

class e2sm_kpm_report_service_style2 : public e2sm_kpm_report_service_base
{
public:
  e2sm_kpm_report_service_style2(asn1::e2sm::e2sm_kpm_action_definition_s action_def_,
                                 e2sm_kpm_meas_provider&                  meas_provider_);
  virtual ~e2sm_kpm_report_service_style2() = default;

  bool collect_measurements() override;

private:
  void clear_collect_measurements() override;

  asn1::e2sm::e2sm_kpm_action_definition_format2_s& action_def;
  asn1::e2sm::ue_id_c&                              ue_id;
  asn1::e2sm::e2sm_kpm_action_definition_format1_s& subscript_info;
  asn1::e2sm::e2sm_kpm_ind_msg_format1_s&           ric_ind_message;
};

class e2sm_kpm_report_service_style3 : public e2sm_kpm_report_service_base
{
public:
  e2sm_kpm_report_service_style3(asn1::e2sm::e2sm_kpm_action_definition_s action_def_,
                                 e2sm_kpm_meas_provider&                  meas_provider_);
  virtual ~e2sm_kpm_report_service_style3() = default;

  bool collect_measurements() override;

private:
  void clear_collect_measurements() override;

  asn1::e2sm::e2sm_kpm_action_definition_format3_s& action_def;
  asn1::e2sm::e2sm_kpm_ind_msg_format2_s&           ric_ind_message;
};

class e2sm_kpm_report_service_style4 : public e2sm_kpm_report_service_base
{
public:
  e2sm_kpm_report_service_style4(asn1::e2sm::e2sm_kpm_action_definition_s action_def_,
                                 e2sm_kpm_meas_provider&                  meas_provider_);
  virtual ~e2sm_kpm_report_service_style4() = default;

  bool collect_measurements() override;

private:
  void clear_collect_measurements() override;

  asn1::e2sm::e2sm_kpm_action_definition_format4_s& action_def;
  asn1::e2sm::e2sm_kpm_action_definition_format1_s& subscription_info;
  asn1::e2sm::e2sm_kpm_ind_msg_format3_s&           ric_ind_message;
  uint32_t                                          nof_collected_meas_data;
};

class e2sm_kpm_report_service_style5 : public e2sm_kpm_report_service_base
{
public:
  e2sm_kpm_report_service_style5(asn1::e2sm::e2sm_kpm_action_definition_s action_def_,
                                 e2sm_kpm_meas_provider&                  meas_provider_);
  virtual ~e2sm_kpm_report_service_style5() = default;

  bool               collect_measurements() override;
  ocudu::byte_buffer get_indication_message() override;

private:
  void clear_collect_measurements() override;

  asn1::e2sm::e2sm_kpm_action_definition_format5_s& action_def;
  asn1::e2sm::e2sm_kpm_action_definition_format1_s& subscription_info;
  asn1::e2sm::e2sm_kpm_ind_msg_format3_s&           ric_ind_message;
  std::vector<asn1::e2sm::ue_id_c>                  ue_ids;
  uint32_t                                          nof_collected_meas_data;
};

} // namespace ocudu
