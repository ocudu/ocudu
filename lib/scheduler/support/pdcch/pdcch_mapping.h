// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../config_helpers.h"
#include "ocudu/ran/pdcch/aggregation_level.h"
#include "ocudu/ran/pdcch/cce_to_prb_mapping.h"

namespace ocudu {
namespace pdcch_helper {

inline prb_index_list cce_to_prb_mapping(const bwp_configuration&     bwp_cfg,
                                         const coreset_configuration& cs_cfg,
                                         pci_t                        pci,
                                         aggregation_level            aggr_lvl,
                                         unsigned                     ncce)
{
  if (cs_cfg.get_id() == to_coreset_id(0)) {
    return cce_to_prb_mapping_coreset0(
        cs_cfg.get_coreset_start_crb(), get_coreset_nof_prbs(cs_cfg), cs_cfg.duration(), pci, aggr_lvl, ncce);
  }
  if (cs_cfg.interleaved_mapping().has_value()) {
    return cce_to_prb_mapping_interleaved(bwp_cfg.crbs.start(),
                                          cs_cfg.freq_domain_resources(),
                                          cs_cfg.duration(),
                                          cs_cfg.interleaved_mapping()->reg_bundle_sz,
                                          cs_cfg.interleaved_mapping()->interleaver_sz,
                                          cs_cfg.interleaved_mapping()->shift_index,
                                          aggr_lvl,
                                          ncce);
  }
  return cce_to_prb_mapping_non_interleaved(
      bwp_cfg.crbs.start(), cs_cfg.freq_domain_resources(), cs_cfg.duration(), aggr_lvl, ncce);
}

} // namespace pdcch_helper
} // namespace ocudu
