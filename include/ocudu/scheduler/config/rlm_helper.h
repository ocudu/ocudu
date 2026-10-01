// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ran/csi_rs/csi_meas_config.h"
#include "ocudu/ran/radio_link_monitoring.h"

namespace ocudu {
namespace rlm_helper {

struct rlm_builder_params {
  /// Parameters that are needed only when the RLM uses SSB as detection resources.
  struct rlm_ssb_params {
    /// Transmitted SSB candidates, from which the SSB indexes used for RLM are taken.
    ssb_bitmap_t ssb_bitmap;
  };

  rlm_builder_params() = default;
  rlm_builder_params(rlm_resource_type resource_type_, uint8_t L_max_) : resource_type(resource_type_), L_max(L_max_)
  {
    // Meant for rlm_resource_type::csi_rs only.
  }
  rlm_builder_params(rlm_resource_type resource_type_, uint8_t L_max_, const ssb_bitmap_t& ssb_bitmap) :
    resource_type(resource_type_), L_max(L_max_), ssb_params(rlm_ssb_params{ssb_bitmap})
  {
  }

  /// Defines which resources (e.g, default, SSB, CSI-RS, SSB and CSI-RS) the UE should use for RLM.
  rlm_resource_type resource_type = rlm_resource_type::default_type;
  /// Max number of SSB candidates per SSB period. Possible values are {4, 8, 64}.
  /// \remark Even though L_max can be inferred from the SSB bitmap, we report it as a separate parameter, as it is
  /// needed also when no SSB resources are used (meaning there is no SSB parameters given).
  uint8_t                       L_max;
  std::optional<rlm_ssb_params> ssb_params;
};

/// Builds the Radio Link Monitoring configuration.
/// \param[in] params Parameters set by the user and other parameters that depend on the cell configuration.
/// \param [in] csi_rs_resources List of CSI-RS resources that can be used for Radio Link Monitoring.
radio_link_monitoring_config make_radio_link_monitoring_config(const rlm_builder_params&       params,
                                                               span<const nzp_csi_rs_resource> csi_rs_resources);
} // namespace rlm_helper
} // namespace ocudu
