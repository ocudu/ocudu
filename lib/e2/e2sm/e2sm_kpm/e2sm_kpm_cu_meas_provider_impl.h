// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "e2sm_kpm_metric_defs.h"
#include "e2sm_kpm_utils.h"
#include "ocudu/asn1/asn1_utils.h"
#include "ocudu/asn1/e2sm/e2sm_kpm_ies.h"
#include "ocudu/cu_cp/cu_cp_metrics_notifier.h"
#include "ocudu/e2/e2_cu.h"
#include "ocudu/e2/e2sm/e2sm.h"
#include "ocudu/e2/e2sm/e2sm_kpm.h"
#include "ocudu/f1u/cu_up/f1u_bearer.h"
#include "ocudu/pdcp/pdcp_entity.h"
#include <deque>
#include <map>
#include <mutex>
#include <numeric>
#include <optional>

namespace ocudu {

class e2sm_kpm_cu_meas_provider_impl : public e2sm_kpm_meas_provider, public e2_cu_metrics_notifier
{
public:
  // constructor takes logger as argument
  e2sm_kpm_cu_meas_provider_impl();

  ~e2sm_kpm_cu_meas_provider_impl() = default;

  void report_metrics(const pdcp_metrics_container& metrics) override;

  void report_metrics(const ocuup::f1u_metrics_container& metrics) override;

  void report_metrics(const cu_cp_metrics_report& metrics) override;

  /// e2sm_kpm_meas_provider functions.
  std::vector<std::string> get_supported_metric_names(e2sm_kpm_metric_level_enum level) override;

  bool is_cell_supported(const asn1::e2sm::cgi_c& cell_global_id) override;

  bool is_ue_supported(const asn1::e2sm::ue_id_c& ueid) override;

  bool is_test_cond_supported(const asn1::e2sm::test_cond_type_c& test_cond_type) override;

  bool is_metric_supported(const asn1::e2sm::meas_type_c&   meas_type,
                           const asn1::e2sm::meas_label_s&  label,
                           const e2sm_kpm_metric_level_enum level,
                           const bool&                      cell_scope) override;

  bool get_ues_matching_test_conditions(const asn1::e2sm::matching_cond_list_l& matching_cond_list,
                                        std::vector<asn1::e2sm::ue_id_c>&       ues) override;

  bool get_ues_matching_test_conditions(const asn1::e2sm::matching_ue_cond_per_sub_list_l& matching_ue_cond_list,
                                        std::vector<asn1::e2sm::ue_id_c>&                  ues) override;

  bool get_meas_data(const asn1::e2sm::meas_type_c&               meas_type,
                     const asn1::e2sm::label_info_list_l          label_info_list,
                     const std::vector<asn1::e2sm::ue_id_c>&      ues,
                     const std::optional<asn1::e2sm::cgi_c>       cell_global_id,
                     std::vector<asn1::e2sm::meas_record_item_c>& items) override;

protected:
  typedef bool(metric_meas_getter_func_t)(const asn1::e2sm::label_info_list_l          label_info_list,
                                          const std::vector<asn1::e2sm::ue_id_c>&      ues,
                                          const std::optional<asn1::e2sm::cgi_c>       cell_global_id,
                                          std::vector<asn1::e2sm::meas_record_item_c>& items);

  typedef metric_meas_getter_func_t(e2sm_kpm_cu_meas_provider_impl::* metric_meas_getter_func_ptr);

  struct e2sm_kpm_supported_metric_t {
    uint32_t                    supported_labels;
    uint32_t                    supported_levels;
    bool                        cell_scope_supported;
    metric_meas_getter_func_ptr func;
  };

  bool check_e2sm_kpm_metrics_definitions(span<const e2sm_kpm_metric_t> metrics_defs);

  // Helper functions.
  float bytes_to_kbits(float value);

  bool handle_no_meas_data_available(const std::vector<asn1::e2sm::ue_id_c>&        ues,
                                     std::vector<asn1::e2sm::meas_record_item_c>&   items,
                                     asn1::e2sm::meas_record_item_c::types::options value_type);

  // Measurement getter functions.
  metric_meas_getter_func_t get_pdcp_reordering_delay_ul;
  metric_meas_getter_func_t get_packet_success_rate_ul_gnb_uu;
  metric_meas_getter_func_t get_rrc_conn_estab_att;
  metric_meas_getter_func_t get_rrc_conn_estab_succ;
  metric_meas_getter_func_t get_rrc_conn_estab_fail_cause_network_reject;
  metric_meas_getter_func_t get_uecntx_conn_estab_att;
  metric_meas_getter_func_t get_uecntx_conn_estab_succ;
  metric_meas_getter_func_t get_rrc_reestab_att;
  metric_meas_getter_func_t get_rrc_reestab_succ_with_ue_context;
  metric_meas_getter_func_t get_rrc_conn_mean;
  metric_meas_getter_func_t get_rrc_conn_max;

  ocudulog::basic_logger& logger;

  std::map<std::string, e2sm_kpm_supported_metric_t>           supported_metrics;
  std::optional<cu_cp_metrics_report>                          cu_cp_metrics;
  std::map<uint32_t, std::deque<pdcp_metrics_container>>       ue_aggr_pdcp_metrics;
  std::map<uint32_t, std::deque<ocuup::f1u_metrics_container>> ue_aggr_f1u_metrics;
  const uint32_t                                               max_pdcp_metrics = 10;
  const uint32_t                                               max_f1u_metrics  = 10;
};

class e2sm_kpm_cu_cp_meas_provider_impl : public e2sm_kpm_cu_meas_provider_impl
{
public:
  e2sm_kpm_cu_cp_meas_provider_impl();
};

class e2sm_kpm_cu_up_meas_provider_impl : public e2sm_kpm_cu_meas_provider_impl
{
public:
  e2sm_kpm_cu_up_meas_provider_impl();
};

} // namespace ocudu
