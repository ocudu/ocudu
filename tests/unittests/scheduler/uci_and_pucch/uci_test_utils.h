// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../test_utils/config_generators.h"
#include "lib/scheduler/pucch_scheduling/pucch_allocator_impl.h"
#include "lib/scheduler/uci_scheduling/uci_allocator_impl.h"
#include "lib/scheduler/uci_scheduling/uci_scheduler_impl.h"
#include "lib/scheduler/ue_context/ue.h"
#include "lib/scheduler/ue_context/ue_repository.h"
#include "tests/test_doubles/scheduler/pucch_res_test_builder_helper.h"
#include "ocudu/ran/csi_rs/csi_meas_config.h"
#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/scheduler/config/pucch_resource_builder_params.h"

namespace ocudu {

namespace test_helpers {

pucch_info make_pucch_info(const cell_configuration& cell_cfg, const pucch_resource& res, pucch_uci_bits uci_bits);

// Returns pointers to all PUCCH PDUs allocated to the given UE, in storage order.
std::vector<const pucch_info*> find_ue_pucchs(span<const pucch_info> pucch_pdus, rnti_t rnti);

} // namespace test_helpers

// Wrapper for std::find_if() to find a PUCCH PDU in a range of PUCCH PDUs.
template <typename F>
bool find_pucch_pdu(const span<const pucch_info> pucch_pdus, const F& func)
{
  return std::find_if(pucch_pdus.begin(), pucch_pdus.end(), func) != pucch_pdus.end();
}

bool pucch_info_match(const pucch_info& expected, const pucch_info& test);

struct test_bench_params {
  pucch_resource_builder_params           pucch_ded_params{};
  max_pucch_code_rate                     max_code_rate = max_pucch_code_rate::dot_25;
  std::optional<unsigned>                 pucch_res_common;
  unsigned                                n_cces                 = 0;
  unsigned                                max_pucchs_per_slot    = 32U;
  unsigned                                max_ul_grants_per_slot = 32U;
  unsigned                                nof_ul_dl_ports        = 1U;
  bool                                    tdd                    = false;
  sr_periodicity                          sr_period              = sr_periodicity::sl_40;
  unsigned                                sr_offset              = 0;
  std::optional<csi_resource_periodicity> csi_period             = csi_resource_periodicity::slots320;
  unsigned                                csi_offset             = 9;
};

class test_bench
{
public:
  explicit test_bench(const test_bench_params& params);

  /// Return the main UE.
  const ue& get_main_ue() const;
  /// Return the UE with the given DU UE index.
  const ue& get_ue(du_ue_index_t ue_idx) const;

  /// Add an extra UE, whose RNTI will have RNTI +1 with respect to the last_allocated_rnti.
  void add_ue();

  /// Indicate the slot to the components under test.
  void slot_indication(slot_point slot_tx);

  /// Fill the whole UL resource grid for the given slot.
  void fill_all_grid(slot_point slot_tx);

  // Parameters.
  const test_bench_params params;

  // Configuration dependencies.
  const scheduler_expert_config           expert_cfg;
  test_helpers::test_sched_config_manager cfg_mng;
  const cell_configuration&               cell_cfg;
  ue_repository                           ues;
  ue_cell_repository&                     cell_ues;

  // Other dependencies.
  pucch_res_builder_test_helper pucch_builder;
  cell_resource_allocator       res_grid;
  ocudulog::basic_logger&       mac_logger  = ocudulog::fetch_basic_logger("SCHED", true);
  ocudulog::basic_logger&       test_logger = ocudulog::fetch_basic_logger("TEST");

  // Constants.
  const unsigned                 k0;
  const pdcch_dl_information     dci_info;
  static constexpr du_ue_index_t main_ue_idx{MIN_DU_UE_INDEX};

  // Components under test.
  pucch_allocator_impl pucch_alloc;
  uci_allocator_impl   uci_alloc;
  uci_scheduler_impl   uci_sched;

  // Helper variables.
  std::vector<const ue_configuration*> ue_ded_cfgs;
  rnti_t                               last_added_ue_rnti;
  du_ue_index_t                        last_added_ue_idx;
  slot_point                           sl_tx;
};

} // namespace ocudu
