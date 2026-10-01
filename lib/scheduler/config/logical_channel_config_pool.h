// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "config_object_pool.h"
#include "logical_channel_list_config.h"
#include "ocudu/scheduler/config/logical_channel_config.h"
#include "ocudu/scheduler/config/logical_channel_config_factory.h"

namespace ocudu {

class logical_channel_config_pool
{
public:
  logical_channel_config_list_ptr create(span<const logical_channel_config> new_cfg)
  {
    // Obtain list of config_ptrs to logical channels.
    std::vector<config_ptr<logical_channel_config>> lc_chs;
    bool has_srb0 = std::any_of(new_cfg.begin(), new_cfg.end(), [](const auto& e) { return e.lcid == LCID_SRB0; });
    if (has_srb0) {
      lc_chs.reserve(new_cfg.size());
    } else {
      // SRB0 is always configured implicitly.
      // Create an SRB0 logical channel entry if it wasn't included in the config.
      lc_chs.reserve(new_cfg.size() + 1);
      lc_chs.push_back(lc_ch_pool.create(config_helpers::create_default_logical_channel_config(LCID_SRB0)));
    }
    for (const auto& lc : new_cfg) {
      lc_chs.push_back(lc_ch_pool.create(lc));
    }

    // Sort by LCID.
    std::sort(
        lc_chs.begin(), lc_chs.end(), [](const logical_channel_config_ptr& lhs, const logical_channel_config_ptr& rhs) {
          return lhs->lcid < rhs->lcid;
        });

    logical_channel_config_list lc_ch_list{std::move(lc_chs)};
    return lc_ch_list_pool.create(lc_ch_list);
  }

private:
  config_object_pool<logical_channel_config> lc_ch_pool;

  config_object_pool<logical_channel_config_list> lc_ch_list_pool;
};

} // namespace ocudu
