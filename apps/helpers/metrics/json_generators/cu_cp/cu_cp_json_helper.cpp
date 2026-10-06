// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_json_helper.h"
#include "helpers.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/gnb_du_id.h"
#include "fmt/format.h"

using namespace ocudu;
using namespace app_helpers;
using namespace json_generators;

namespace ocudu {

struct cu_cp_rrc_metrics_json {
  std::vector<cu_cp_metrics_report::du_info> dus;
  mobility_management_metrics                mobility;
};

struct cu_cp_ngap_metrics_json {
  std::vector<ngap_info>      ngaps;
  mobility_management_metrics mobility;
};

void to_json(nlohmann::json& json, const ngap_counter_with_cause& metrics)
{
  // NGAP counter with cause.
  unsigned cause_index = 0;
  for (const auto& count : metrics) {
    json[fmt::format("{}", metrics.get_cause(cause_index))] = count;
    cause_index++;
  }
}

void to_json(nlohmann::json& json, const ngap_metrics& metrics)
{
  // NGAP metrics.
  for (const auto& [nssai, pdu_session_metrics] : metrics.pdu_session_metrics) {
    // PDU session metrics.
    json["s-nssai"] = fmt::format(
        "(sst={} sd={})", nssai.sst.value(), nssai.sd.is_set() ? fmt::format("{}", nssai.sd.value()) : "na");
    json["nof_pdu_sessions_requested_to_setup"] = pdu_session_metrics.nof_pdu_sessions_requested_to_setup;
    json["nof_pdu_sessions_successfully_setup"] = pdu_session_metrics.nof_pdu_sessions_successfully_setup;
    json["nof_pdu_sessions_failed_to_setup"]    = pdu_session_metrics.nof_pdu_sessions_failed_to_setup;
  }
}

void to_json(nlohmann::json& json, const std::vector<plmn_identity>& plmns)
{
  // PLMN identity.
  std::string plmn_str = "[ ";
  for (const auto& plmn : plmns) {
    plmn_str += fmt::format("{}, ", plmn);
  }
  plmn_str += "]";
  json = fmt::format("{}", plmn_str);
}

void to_json(nlohmann::json& json, const ngap_info& metrics)
{
  // NGAP info.
  json["amf_name"]                                = metrics.amf_name;
  json["connected"]                               = metrics.connected;
  json["supported_plmns"]                         = metrics.supported_plmns;
  json["pdu_session_management"]                  = metrics.metrics;
  nlohmann::json& paging_json                     = json["paging_measurement"];
  paging_json["nof_cn_initiated_paging_requests"] = metrics.metrics.nof_cn_initiated_paging_requests;
}

void to_json(nlohmann::json& json, const ocudu::cu_cp_ngap_metrics_json& metrics)
{
  // NGAP metrics.
  json["ngap"]                                 = metrics.ngaps;
  json["nof_handover_preparations_requested"]  = metrics.mobility.nof_handover_preparations_requested;
  json["nof_successful_handover_preparations"] = metrics.mobility.nof_successful_handover_preparations;
  json["nof_inter_gnb_handover_resource_allocations_requested"] =
      metrics.mobility.nof_inter_gnb_handover_resource_allocations_requested;
  json["nof_successful_inter_gnb_handover_resource_allocations"] =
      metrics.mobility.nof_successful_inter_gnb_handover_resource_allocations;
  json["nof_inter_gnb_handover_executions_requested"]  = metrics.mobility.nof_inter_gnb_handover_executions_requested;
  json["nof_successful_inter_gnb_handover_executions"] = metrics.mobility.nof_successful_inter_gnb_handover_executions;
  json["nof_ues_configured_with_inter_gnb_cho"]        = metrics.mobility.nof_ues_configured_with_inter_gnb_cho;
}

void to_json(nlohmann::json& json, const rrc_connection_counter_with_cause& metrics)
{
  // RRC connection counter with cause metrics.
  unsigned cause_index = 0;
  for (const auto& count : metrics) {
    json[fmt::format("{}", metrics.get_cause(cause_index))] = count;
    cause_index++;
  }
}

void to_json(nlohmann::json& json, const ocudu::cu_cp_metrics_report::du_info& metrics)
{
  // RRC-DU metrics.
  json["gnb_du_id"]                                   = metrics.id;
  nlohmann::json& rrc_connection                      = json["rrc_connection_number"];
  rrc_connection["mean_nof_rrc_connections"]          = metrics.rrc_metrics.mean_nof_rrc_connections;
  rrc_connection["max_nof_rrc_connections"]           = metrics.rrc_metrics.max_nof_rrc_connections;
  rrc_connection["mean_nof_inactive_rrc_connections"] = metrics.rrc_metrics.mean_nof_inactive_rrc_connections;
  rrc_connection["max_nof_inactive_rrc_connections"]  = metrics.rrc_metrics.max_nof_inactive_rrc_connections;

  nlohmann::json& rrc_connection_establishment = json["rrc_connection_establishment"];
  rrc_connection_establishment["attempted_rrc_connection_establishments"] =
      metrics.rrc_metrics.attempted_rrc_connection_establishments;
  rrc_connection_establishment["successful_rrc_connection_establishments"] =
      metrics.rrc_metrics.successful_rrc_connection_establishments;
  rrc_connection_establishment["mean_rrc_connection_setup_time_ms"] = nullptr;
  rrc_connection_establishment["max_rrc_connection_setup_time_ms"]  = nullptr;
  if (metrics.rrc_metrics.mean_rrc_connection_setup_time.has_value() and
      metrics.rrc_metrics.max_rrc_connection_setup_time.has_value()) {
    rrc_connection_establishment["mean_rrc_connection_setup_time_ms"] =
        metrics.rrc_metrics.mean_rrc_connection_setup_time->count();
    rrc_connection_establishment["max_rrc_connection_setup_time_ms"] =
        metrics.rrc_metrics.max_rrc_connection_setup_time->count();
  }

  nlohmann::json& rrc_connection_reestablishment = json["rrc_connection_reestablishment"];
  rrc_connection_reestablishment["attempted_rrc_connection_reestablishments"] =
      metrics.rrc_metrics.attempted_rrc_connection_reestablishments;
  rrc_connection_reestablishment["successful_rrc_connection_reestablishments_with_ue_context"] =
      metrics.rrc_metrics.successful_rrc_connection_reestablishments_with_ue_context;
  rrc_connection_reestablishment["successful_rrc_connection_reestablishments_without_ue_context"] =
      metrics.rrc_metrics.successful_rrc_connection_reestablishments_without_ue_context;

  nlohmann::json& rrc_connection_resume                      = json["rrc_connection_resume"];
  rrc_connection_resume["attempted_rrc_connection_resumes"]  = metrics.rrc_metrics.attempted_rrc_connection_resumes;
  rrc_connection_resume["successful_rrc_connection_resumes"] = metrics.rrc_metrics.successful_rrc_connection_resumes;
  rrc_connection_resume["successful_rrc_connection_resumes_with_fallback"] =
      metrics.rrc_metrics.successful_rrc_connection_resumes_with_fallback;
  rrc_connection_resume["rrc_connection_resumes_followed_by_network_release"] =
      metrics.rrc_metrics.rrc_connection_resumes_followed_by_network_release;
  rrc_connection_resume["attempted_rrc_connection_resumes_followed_by_rrc_setup"] =
      metrics.rrc_metrics.attempted_rrc_connection_resumes_followed_by_rrc_setup;
}

void to_json(nlohmann::json& json, const cu_cp_rrc_metrics_json& metrics)
{
  // RRC-DU metrics.
  json["du"] = metrics.dus;
  json["nof_intra_gnb_handover_preparations_requested"] =
      metrics.mobility.nof_intra_gnb_handover_preparations_requested;
  json["nof_successful_intra_gnb_handover_preparations"] =
      metrics.mobility.nof_successful_intra_gnb_handover_preparations;
  json["nof_handover_executions_requested"]         = metrics.mobility.nof_handover_executions_requested;
  json["nof_successful_handover_executions"]        = metrics.mobility.nof_successful_handover_executions;
  json["mean_intra_gnb_handover_execution_time_ms"] = nullptr;
  json["max_intra_gnb_handover_execution_time_ms"]  = nullptr;
  if (metrics.mobility.mean_intra_gnb_handover_execution_time.has_value() and
      metrics.mobility.max_intra_gnb_handover_execution_time.has_value()) {
    json["mean_intra_gnb_handover_execution_time_ms"] =
        metrics.mobility.mean_intra_gnb_handover_execution_time->count();
    json["max_intra_gnb_handover_execution_time_ms"] = metrics.mobility.max_intra_gnb_handover_execution_time->count();
  }
  json["nof_ues_configured_with_intra_gnb_cho"]   = metrics.mobility.nof_ues_configured_with_intra_gnb_cho;
  json["nof_successful_intra_gnb_cho_executions"] = metrics.mobility.nof_successful_intra_gnb_cho_executions;
}

} // namespace ocudu

nlohmann::json ocudu::app_helpers::json_generators::generate(const cu_cp_metrics_report& report)
{
  cu_cp_rrc_metrics_json  rrc_metrics  = {report.dus, report.mobility};
  cu_cp_ngap_metrics_json ngap_metrics = {report.ngaps, report.mobility};

  nlohmann::json json;

  json["timestamp"]          = get_time_stamp();
  nlohmann::json& cu_cp_json = json["cu-cp"];

  cu_cp_json["id"]    = "srs-cu-cp";
  cu_cp_json["ngaps"] = ngap_metrics;
  cu_cp_json["rrcs"]  = rrc_metrics;

  return json;
}

std::string ocudu::app_helpers::json_generators::generate_string(const cu_cp_metrics_report& report, int indent)
{
  return generate(report).dump(indent);
}
