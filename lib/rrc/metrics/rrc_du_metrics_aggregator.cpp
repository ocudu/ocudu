// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rrc_du_metrics_aggregator.h"
#include "ocudu/adt/format.h"

using namespace ocudu;
using namespace ocucp;

rrc_du_metrics_aggregator::rrc_du_metrics_aggregator()
{
  connection_metrics.reset();
  inactive_connection_metrics.reset();
}

void rrc_du_metrics_aggregator::aggregate_successful_rrc_setup()
{
  connection_metrics.add_rrc_connection();
}

void rrc_du_metrics_aggregator::aggregate_successful_rrc_release(bool is_inactive)
{
  if (is_inactive) {
    inactive_connection_metrics.remove_rrc_connection();
    return;
  }
  connection_metrics.remove_rrc_connection();
}

void rrc_du_metrics_aggregator::aggregate_successful_rrc_inactive()
{
  connection_metrics.remove_rrc_connection();
  inactive_connection_metrics.add_rrc_connection();
}

void rrc_du_metrics_aggregator::aggregate_successful_rrc_resume()
{
  inactive_connection_metrics.remove_rrc_connection();
  connection_metrics.add_rrc_connection();
}

void rrc_du_metrics_aggregator::aggregate_attempted_connection_establishment(establishment_cause_t cause)
{
  connection_establishment_metrics.attempted_rrc_connection_establishments.increase(cause);
}

void rrc_du_metrics_aggregator::aggregate_successful_connection_establishment(establishment_cause_t cause)
{
  connection_establishment_metrics.successful_rrc_connection_establishments.increase(cause);
}

void rrc_du_metrics_aggregator::aggregate_failed_connection_establishment(establishment_fail_cause_t cause)
{
  connection_establishment_metrics.failed_rrc_connection_establishments.increase(cause);
}

void rrc_du_metrics_aggregator::aggregate_attempted_connection_reestablishment()
{
  ++connection_reestablishment_metrics.attempted_rrc_connection_reestablishments;
}

void rrc_du_metrics_aggregator::aggregate_successful_connection_reestablishment(bool with_ue_context)
{
  if (with_ue_context) {
    ++connection_reestablishment_metrics.successful_rrc_connection_reestablishments_with_ue_context;
  } else {
    ++connection_reestablishment_metrics.successful_rrc_connection_reestablishments_without_ue_context;
  }
}

void rrc_du_metrics_aggregator::aggregate_attempted_connection_resume(resume_cause_t cause)
{
  connection_resume_metrics.attempted_rrc_connection_resumes.increase(cause);
}

void rrc_du_metrics_aggregator::aggregate_successful_connection_resume(resume_cause_t cause)
{
  connection_resume_metrics.successful_rrc_connection_resumes.increase(cause);
}

void rrc_du_metrics_aggregator::aggregate_successful_connection_resume_with_fallback(resume_cause_t cause)
{
  connection_resume_metrics.successful_rrc_connection_resumes_with_fallback.increase(cause);
}

void rrc_du_metrics_aggregator::aggregate_connection_resume_followed_by_network_release(resume_cause_t cause)
{
  connection_resume_metrics.rrc_connection_resumes_followed_by_network_release.increase(cause);
}

void rrc_du_metrics_aggregator::aggregate_attempted_connection_resume_followed_by_rrc_setup(resume_cause_t cause)
{
  connection_resume_metrics.attempted_rrc_connection_resumes_followed_by_rrc_setup.increase(cause);
}

void rrc_du_metrics_aggregator::collect_metrics(rrc_du_metrics& metrics)
{
  metrics.mean_nof_rrc_connections          = connection_metrics.get_mean_nof_rrc_connections();
  metrics.max_nof_rrc_connections           = connection_metrics.get_max_nof_rrc_connections();
  metrics.mean_nof_inactive_rrc_connections = inactive_connection_metrics.get_mean_nof_rrc_connections();
  metrics.max_nof_inactive_rrc_connections  = inactive_connection_metrics.get_max_nof_rrc_connections();
  metrics.attempted_rrc_connection_establishments =
      connection_establishment_metrics.attempted_rrc_connection_establishments;
  metrics.successful_rrc_connection_establishments =
      connection_establishment_metrics.successful_rrc_connection_establishments;
  metrics.failed_rrc_connection_establishments = connection_establishment_metrics.failed_rrc_connection_establishments;
  metrics.attempted_rrc_connection_reestablishments =
      connection_reestablishment_metrics.attempted_rrc_connection_reestablishments;
  metrics.successful_rrc_connection_reestablishments_with_ue_context =
      connection_reestablishment_metrics.successful_rrc_connection_reestablishments_with_ue_context;
  metrics.successful_rrc_connection_reestablishments_without_ue_context =
      connection_reestablishment_metrics.successful_rrc_connection_reestablishments_without_ue_context;
  metrics.attempted_rrc_connection_resumes  = connection_resume_metrics.attempted_rrc_connection_resumes;
  metrics.successful_rrc_connection_resumes = connection_resume_metrics.successful_rrc_connection_resumes;
  metrics.successful_rrc_connection_resumes_with_fallback =
      connection_resume_metrics.successful_rrc_connection_resumes_with_fallback;
  metrics.rrc_connection_resumes_followed_by_network_release =
      connection_resume_metrics.rrc_connection_resumes_followed_by_network_release;
  metrics.attempted_rrc_connection_resumes_followed_by_rrc_setup =
      connection_resume_metrics.attempted_rrc_connection_resumes_followed_by_rrc_setup;
  connection_metrics.reset();
  inactive_connection_metrics.reset();
}
