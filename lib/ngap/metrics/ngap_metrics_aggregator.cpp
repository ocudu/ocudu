// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ngap_metrics_aggregator.h"

using namespace ocudu;
using namespace ocucp;

void ngap_metrics_aggregator::aggregate_requested_pdu_session(s_nssai_t s_nssai)
{
  if (aggregated_ngap_metrics.pdu_session_metrics.find(s_nssai) == aggregated_ngap_metrics.pdu_session_metrics.end()) {
    aggregated_ngap_metrics.pdu_session_metrics.emplace(s_nssai, pdu_session_metrics_t{1, 0, {}});
  } else {
    ++aggregated_ngap_metrics.pdu_session_metrics[s_nssai].nof_pdu_sessions_requested_to_setup;
  }
}

void ngap_metrics_aggregator::aggregate_successful_pdu_session_setup(s_nssai_t s_nssai)
{
  if (aggregated_ngap_metrics.pdu_session_metrics.find(s_nssai) == aggregated_ngap_metrics.pdu_session_metrics.end()) {
    aggregated_ngap_metrics.pdu_session_metrics.emplace(s_nssai, pdu_session_metrics_t{0, 1, {}});
  } else {
    ++aggregated_ngap_metrics.pdu_session_metrics[s_nssai].nof_pdu_sessions_successfully_setup;
  }
}

void ngap_metrics_aggregator::aggregate_failed_pdu_session_setup(s_nssai_t s_nssai, ngap_cause_t cause)
{
  if (aggregated_ngap_metrics.pdu_session_metrics.find(s_nssai) == aggregated_ngap_metrics.pdu_session_metrics.end()) {
    aggregated_ngap_metrics.pdu_session_metrics.emplace(s_nssai, pdu_session_metrics_t{0, 0, {}});
  }

  aggregated_ngap_metrics.pdu_session_metrics[s_nssai].nof_pdu_sessions_failed_to_setup.increase(cause);
}

void ngap_metrics_aggregator::aggregate_cn_initiated_paging_request()
{
  ++aggregated_ngap_metrics.nof_cn_initiated_paging_requests;
}

void ngap_metrics_aggregator::aggregate_ue_associated_logical_ng_connection_establishment_attempt()
{
  ++aggregated_ngap_metrics.nof_ue_associated_logical_ng_connection_establishment_attempts;
}

void ngap_metrics_aggregator::aggregate_ue_associated_logical_ng_connection_establishment_success()
{
  ++aggregated_ngap_metrics.nof_ue_associated_logical_ng_connection_establishment_successes;
}

ngap_metrics ngap_metrics_aggregator::request_metrics_report() const
{
  return aggregated_ngap_metrics;
}
