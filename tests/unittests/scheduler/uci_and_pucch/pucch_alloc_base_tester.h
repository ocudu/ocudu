// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "lib/scheduler/cell/resource_grid.h"
#include "uci_test_utils.h"
#include "ocudu/ran/pucch/pucch_uci_bits.h"
#include <optional>

namespace ocudu {

class pucch_allocator_base_test
{
public:
  pucch_allocator_base_test(test_bench_params params_ = test_bench_params{.pucch_res_common = 11, .n_cces = 1}) :
    t_bench(params_)
  {
  }

protected:
  test_bench t_bench;

  static constexpr unsigned default_k1 = 4U;

  const cell_slot_resource_allocator& default_slot_grid = t_bench.res_grid[t_bench.k0 + default_k1];

  // Helper for adding a SR grant to a given UE.
  bool alloc_sr_opportunity(const ue& ue)
  {
    return t_bench.pucch_alloc.alloc_sr_opportunity(t_bench.res_grid[t_bench.k0 + default_k1], ue.get_pcell().cfg());
  }

  // Helper for adding a SR grant to a given UE.
  bool alloc_csi_opportunity(const ue& ue)
  {
    return t_bench.pucch_alloc.alloc_csi_opportunity(t_bench.res_grid[t_bench.k0 + default_k1], ue.get_pcell().cfg());
  }

  std::optional<unsigned> alloc_common_harq_ack(const ue& ue)
  {
    return t_bench.pucch_alloc.alloc_common_harq_ack(
        t_bench.res_grid, ue.crnti, t_bench.k0, default_k1, t_bench.dci_info);
  }

  std::optional<unsigned> alloc_ded_harq_ack(const ue& ue)
  {
    return t_bench.pucch_alloc.alloc_ded_harq_ack(t_bench.res_grid, ue.get_pcell().cfg(), t_bench.k0, default_k1);
  }

  std::optional<unsigned> alloc_common_and_ded_harq_ack(const ue& ue)
  {
    return t_bench.pucch_alloc.alloc_common_and_ded_harq_ack(
        t_bench.res_grid, ue.get_pcell().cfg(), t_bench.k0, default_k1, t_bench.dci_info);
  }

  pucch_uci_bits remove_ue_uci_from_pucch(const ue& ue)
  {
    return t_bench.pucch_alloc.remove_ue_uci_from_pucch(t_bench.res_grid[t_bench.k0 + default_k1],
                                                        ue.get_pcell().cfg());
  }
};

} // namespace ocudu
