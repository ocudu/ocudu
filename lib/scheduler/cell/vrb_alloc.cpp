// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/scheduler/result/vrb_alloc.h"
#include "ocudu/support/enum_utils.h"

using namespace ocudu;

prb_bitmap ocudu::convert_rbgs_to_prbs(const rbg_bitmap& rbgs, crb_interval bwp_rbs, nominal_rbg_size P)
{
  int      idx            = 0;
  unsigned first_rbg_size = get_rbg_size(bwp_rbs, P, 0);

  prb_bitmap prbs(bwp_rbs.length());
  do {
    idx = rbgs.find_lowest(idx, rbgs.size(), true);
    if (idx < 0) {
      break;
    }
    uint32_t prb_idx = (idx - 1) * to_underlying(P) + first_rbg_size;
    uint32_t prb_end =
        std::min(prb_idx + ((idx == 0) ? first_rbg_size : to_underlying(P)), static_cast<uint32_t>(prbs.size()));
    prbs.fill(prb_idx, prb_end);
    idx++;
  } while (idx != static_cast<int>(prbs.size()));
  return prbs;
}
