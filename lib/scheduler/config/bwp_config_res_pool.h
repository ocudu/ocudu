// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/scheduler/config/bwp_configuration.h"
#include "ocudu/scheduler/config/cell_bwp_res_config.h"
#include "ocudu/scheduler/config/ran_cell_config.h"
#include "ocudu/scheduler/config/sched_bwp_config.h"

namespace ocudu {

/// \brief Holds all the information respective to the configuration and management of a BWP from the scheduler
/// perspective.
///
/// This config must represent the superset of all the possible common and UE-dedicated configurations for a given BWP.
class bwp_config_res_pool
{
public:
  bwp_config_res_pool(const ran_cell_config& ran_cfg, bwp_id_t bwp_id_) :
    bwpid(bwp_id_), base_dl_bwp_cmn(ran_cfg.dl_cfg_common.init_dl_bwp), res(make_cell_bwp_res_config(ran_cfg))
  {
  }

  bwp_id_t bwp_id() const { return bwpid; }

  /// BWP Downlink common config.
  const bwp_downlink_common& dl_common() const { return base_dl_bwp_cmn; }

  /// Dedicated Downlink resources.
  const cell_dl_bwp_res_config& dl() const { return res.dl; }

  /// Dedicated Uplink resources.
  const cell_ul_bwp_res_config& ul() const { return res.ul; }

private:
  bwp_id_t            bwpid;
  bwp_downlink_common base_dl_bwp_cmn;
  cell_bwp_res_config res;
};

} // namespace ocudu
