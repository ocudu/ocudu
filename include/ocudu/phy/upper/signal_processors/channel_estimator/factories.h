// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/time_alignment_estimator/time_alignment_estimator_factories.h"
#include "ocudu/phy/upper/signal_processors/channel_estimator/port_channel_estimator.h"
#include "ocudu/phy/upper/signal_processors/channel_estimator/port_channel_estimator_parameters.h"

namespace ocudu {

class port_channel_estimator_factory
{
public:
  virtual ~port_channel_estimator_factory() = default;
  virtual std::unique_ptr<port_channel_estimator>
  create(port_channel_estimator_fd_smoothing_strategy     fd_smoothing_strategy,
         port_channel_estimator_td_interpolation_strategy td_interpolation_strategy,
         bool                                             complensate_cfo) = 0;
};

std::shared_ptr<port_channel_estimator_factory>
create_port_channel_estimator_factory_sw(std::shared_ptr<time_alignment_estimator_factory> ta_estimator_factory);

} // namespace ocudu
