// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "csi_rs_helpers.h"
#include "ocudu/adt/format.h"

bool ocudu::csi_helper::is_csi_rs_slot(const cell_configuration& cell_cfg, slot_point sl_tx)
{
  if (not cell_cfg.is_dl_enabled(sl_tx)) {
    return false;
  }

  if (not cell_cfg.zp_csi_rs_list.empty() and cell_cfg.nzp_csi_rs_list.empty()) {
    return false;
  }

  for (const auto& zp_csi : cell_cfg.zp_csi_rs_list) {
    if (zp_csi.offset.has_value() and zp_csi.period.has_value() and
        (sl_tx - *zp_csi.offset).count() % (unsigned)*zp_csi.period == 0) {
      return true;
    }
  }

  for (const auto& nzp_csi : cell_cfg.nzp_csi_rs_list) {
    if (nzp_csi.csi_res_offset.has_value() and nzp_csi.csi_res_period.has_value() and
        (sl_tx - *nzp_csi.csi_res_offset).count() % (unsigned)*nzp_csi.csi_res_period == 0) {
      return true;
    }
  }

  return false;
}
