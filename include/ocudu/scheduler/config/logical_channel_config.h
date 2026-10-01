// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/mac/mac_lc_config.h"
#include "ocudu/ran/logical_channel/lcid.h"
#include "ocudu/ran/qos/arp_prio_level.h"
#include "ocudu/ran/qos/five_qi_qos_mapping.h"
#include "ocudu/ran/qos/qos_parameters.h"
#include "ocudu/ran/rrm.h"
#include "ocudu/ran/sr_configuration.h"
#include "ocudu/scheduler/config/logical_channel_group.h"

namespace ocudu {

/// \c LogicalChannelConfig, TS 38.331.
/// Information relative to the scheduling of a logical channel in the MAC scheduler.
struct logical_channel_config {
  /// QoS specific features associated with a logical channel (only used by DRBs).
  struct qos_info {
    /// QoS characteristics associated with the logical channel.
    standardized_qos_characteristics qos;
    /// The ARP Priority Level indicates a priority in scheduling resources among QoS Flows. The lowest Priority Level
    /// value corresponds to the highest priority. See TS 23.501, clause 5.7.2.2.
    arp_prio_level_t arp_priority;
    /// QoS information present only for GBR QoS flows.
    std::optional<gbr_qos_flow_information> gbr_qos_info;

    bool operator==(const qos_info& rhs) const
    {
      return qos == rhs.qos && arp_priority == rhs.arp_priority && gbr_qos_info == rhs.gbr_qos_info;
    }
  };

  lcid_t   lcid;
  lcg_id_t lc_group;
  /// Slice associated with this Bearer.
  rrm_policy_member rrm_policy;
  /// QoS information associated with this logical channel.
  std::optional<qos_info> qos;
  // TODO: add remaining fields;
  std::optional<scheduling_request_id> sr_id;
  bool                                 lc_sr_mask;
  bool                                 lc_sr_delay_timer_applied;
  // [Implementation defined] Configuration of triggeded UL grant feature.
  std::optional<mac_lc_config::triggered_ul_grant_cfg> triggered_ul_grant;

  bool operator==(const logical_channel_config& rhs) const
  {
    return lcid == rhs.lcid and lc_group == rhs.lc_group and rrm_policy == rhs.rrm_policy and qos == rhs.qos and
           sr_id == rhs.sr_id and lc_sr_mask == rhs.lc_sr_mask and
           lc_sr_delay_timer_applied == rhs.lc_sr_delay_timer_applied and triggered_ul_grant == rhs.triggered_ul_grant;
  }
};

} // namespace ocudu
