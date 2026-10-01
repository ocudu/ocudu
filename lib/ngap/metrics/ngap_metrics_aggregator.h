// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ngap/ngap_metrics.h"
#include "ocudu/ran/s_nssai.h"
#include <cmath>

namespace ocudu::ocucp {

class ngap_metrics_aggregator
{
public:
  void aggregate_requested_pdu_session(s_nssai_t s_nssai);

  void aggregate_successful_pdu_session_setup(s_nssai_t s_nssai);

  void aggregate_failed_pdu_session_setup(s_nssai_t s_nssai, ngap_cause_t cause);

  void aggregate_cn_initiated_paging_request();

  void aggregate_ue_associated_logical_ng_connection_establishment_attempt();

  void aggregate_ue_associated_logical_ng_connection_establishment_success();

  ngap_metrics request_metrics_report() const;

private:
  ngap_metrics aggregated_ngap_metrics;
};

} // namespace ocudu::ocucp
