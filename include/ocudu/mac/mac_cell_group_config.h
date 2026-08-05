// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/static_vector.h"
#include "ocudu/mac/bsr_config.h"
#include "ocudu/mac/phr_config.h"
#include "ocudu/ran/drx_config.h"
#include "ocudu/ran/sr_configuration.h"
#include "ocudu/ran/tar_config.h"
#include "ocudu/ran/time_alignment_config.h"
#include <chrono>
#include <optional>
#include <vector>

namespace ocudu {

/// \c MAC-CellGroupConfig, TS 38.331.
struct mac_cell_group_config {
  /// \c drx-Config.
  std::optional<drx_config> drx_cfg;
  /// \c schedulingRequestConfig.
  std::vector<scheduling_request_to_addmod> scheduling_request_config;
  /// \c bsr-Config.
  std::optional<bsr_config> bsr_cfg;
  /// \c tag-Config.
  static_vector<time_alignment_group, MAX_NOF_TIME_ALIGNMENT_GROUPS> tag_config;
  /// \c phr-Config.
  std::optional<phr_config> phr_cfg;
  /// \c skipUplinkTxDynamic.
  bool skip_uplink_tx_dynamic = false;
  /// \c tar-Config.
  std::optional<tar_config> tar_cfg;
  // TODO: add remaining fields.
};

} // namespace ocudu
