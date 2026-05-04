// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/du/du_high/du_high_ntn_time_provider_factory.h"
#include "du_high_ntn_time_provider_impl.h"

using namespace ocudu;

std::unique_ptr<ocudu_ntn::ntn_time_provider>
ocudu::create_du_high_ntn_time_provider(mac_subframe_time_mapper& du_time_mapper)
{
  auto time_provider = std::make_unique<odu::du_high_ntn_time_provider_impl>(du_time_mapper);
  return time_provider;
}
