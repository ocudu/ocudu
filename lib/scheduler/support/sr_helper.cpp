// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "sr_helper.h"
#include "ocudu/adt/format.h"

bool ocudu::sr_helper::is_sr_opportunity_slot(const ue_uplink_bwp_config& ue_ul_cfg,
                                              sr_periodicity              sr_period,
                                              slot_point                  sl_tx)
{
  return (sl_tx - ue_ul_cfg.pucch.sr_offset).count() % sr_periodicity_to_slot(sr_period) == 0;
}
