// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/config/sched_config_manager.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/dummy_test_components.h"
#include "ocudu/ran/pdsch/pdsch_constants.h"
#include "ocudu/scheduler/config/scheduler_expert_config_factory.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

// Enlarges the cell's PUCCH Format 2 so it can carry the ~16-bit CSI report of an 8-port cell. The default F2 (1 RB,
// code rate 0.25) only fits 8 bits, so an 8-port UE creation request would otherwise be rejected before the UE
// configuration (and its PDSCH config precompute) is built.
void widen_pucch_f2_for_csi(sched_cell_configuration_request_message& sched_cfg)
{
  auto& f2_params         = std::get<pucch_f2_params>(sched_cfg.ran.init_bwp.pucch.resources.f2_or_f3_or_f4_params);
  f2_params.max_code_rate = max_pucch_code_rate::dot_80;
  f2_params.max_nof_rbs   = pucch_f2_params::nof_rbs{3};
}

// Builds a UE configuration for a cell with the given number of DL antennas and returns the number of PDSCH configs
// precomputed per time-domain resource for the UE-specific (DCI format 1_1) search space.
unsigned precomputed_pdsch_layer_configs(unsigned nof_dl_antennas)
{
  cell_config_builder_params builder_params;
  builder_params.dl_carrier.nof_ant = nof_dl_antennas;

  scheduler_expert_config  expert_cfg = config_helpers::make_default_scheduler_expert_config();
  sched_cfg_dummy_notifier metric_notif;
  sched_config_manager     cfg_mng(scheduler_config{expert_cfg, metric_notif});

  sched_cell_configuration_request_message sched_cfg =
      sched_config_helper::make_default_sched_cell_configuration_request(builder_params);
  widen_pucch_f2_for_csi(sched_cfg);
  const cell_configuration& cell_cfg = *cfg_mng.add_cell(sched_cfg);

  sched_ue_creation_request_message ue_req = sched_config_helper::create_default_sched_ue_creation_request(
      cell_cfg.params, std::array<lcid_t, 3>{lcid_t::LCID_SRB1, lcid_t::LCID_SRB2, lcid_t::LCID_MIN_DRB});

  ue_config_update_event ev = cfg_mng.add_ue(ue_req);
  EXPECT_TRUE(ev.valid()) << "UE creation request was rejected for nof_dl_antennas=" << nof_dl_antennas;
  const ue_configuration& ue_cfg = ev.next_config();

  const ue_cell_configuration& ue_cell_cfg = ue_cfg.pcell_cfg();
  unsigned                     max_configs = 0;
  for (unsigned id = 0; id != MAX_NOF_SEARCH_SPACES; ++id) {
    const search_space_info* ss = ue_cell_cfg.find_search_space(to_search_space_id(id));
    if (ss == nullptr or ss->get_dl_dci_format() != dci_dl_format::f1_1) {
      continue;
    }
    for (unsigned td = 0, e = ss->bwp->dl.td_mapper().pdsch_td_resources(ss->get_dl_dci_format()).size(); td != e;
         ++td) {
      max_configs = std::max(max_configs, ss->get_nof_pdsch_layer_configs(td));
    }
  }

  // The RRC maxMIMO-Layers (PDSCH-ServingCellConfig) signalled to the UE should match the resolved maximum number of DL
  // layers.
  const pdsch_serving_cell_config* pdsch_serv_cell = ue_cell_cfg.pdsch_serving_cell_cfg();
  EXPECT_NE(pdsch_serv_cell, nullptr);
  if (pdsch_serv_cell != nullptr) {
    EXPECT_EQ(pdsch_serv_cell->max_mimo_layers, max_configs)
        << "RRC maxMIMO-Layers must match the resolved max DL layers for nof_dl_antennas=" << nof_dl_antennas;
  }

  ev.notify_completion();
  return max_configs;
}

} // namespace

// The UE-specific PDSCH config precompute is bounded by min(nof_dl_ports, MAX_NOF_LAYERS_PER_CODEWORD): it tracks the
// number of antenna ports below the single-codeword MAX_NOF_LAYERS_PER_CODEWORD limit.
// Without the limit an 8-port (8T8R) cell would build configs for up to 8 layers, which are not yet supported.
TEST(ue_configuration_test, pdsch_config_precompute_is_bounded_by_layers_not_ports)
{
  // Below the single-codeword limit, one config per port.
  EXPECT_EQ(precomputed_pdsch_layer_configs(1), 1);
  EXPECT_EQ(precomputed_pdsch_layer_configs(2), 2);
  EXPECT_EQ(precomputed_pdsch_layer_configs(4), 4);
  // At and above the limit (MAX_NOF_LAYERS_PER_CODEWORD == 4), the count is clamped.
  EXPECT_EQ(precomputed_pdsch_layer_configs(8), pdsch_constants::MAX_NOF_LAYERS_PER_CODEWORD);
}
