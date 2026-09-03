// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "csi_report_helpers.h"
#include "ocudu/adt/format.h"

bool ocudu::csi_helper::is_csi_reporting_slot(const ue_periodic_csi_config& periodic_csi,
                                              csi_resource_periodicity      csi_period,
                                              slot_point                    sl_tx)
{
  return (sl_tx - periodic_csi.offset).count() % csi_resource_periodicity_to_uint(csi_period) == 0;
}
