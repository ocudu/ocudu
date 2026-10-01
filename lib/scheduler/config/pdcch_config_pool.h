// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/scheduler/config/cell_bwp_res_config.h"
#include "ocudu/scheduler/config/sched_pdcch_config.h"

namespace ocudu {

/// Pool of PDCCH configurations (common and dedicated) used within a given BWP.
class pdcch_config_pool
{
public:
  pdcch_config_pool(pci_t                         pci,
                    const bwp_configuration&      bwp_cfg,
                    const pdcch_config_common&    pdcch_common,
                    const cell_dl_bwp_res_config& cell_res);

  /// Retrieve PDCCH config common (used when UEs have no dedicated RRC config).
  const sched_pdcch_config& init_cfg() const { return init_pdcch_cfg; }

  /// Retrieve PDCCH dedicated config.
  span<const sched_pdcch_config> ded_cfgs() const { return ded_pdcch_cfgs; }

private:
  const sched_coreset_config&
  make_coreset(pci_t pci, const bwp_configuration& bwp_cfg, const coreset_configuration& cs_cfg);

  const sched_search_space_config& make_search_space(const sched_coreset_config&       cs_cfg,
                                                     const search_space_configuration& ss_cfg);

  std::vector<sched_coreset_config> coreset_pool;

  std::vector<sched_search_space_config> search_space_pool;

  sched_pdcch_config              init_pdcch_cfg;
  std::vector<sched_pdcch_config> ded_pdcch_cfgs;
};

} // namespace ocudu
