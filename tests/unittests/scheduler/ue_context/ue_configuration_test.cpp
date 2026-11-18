// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/config/sched_config_manager.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/dummy_test_components.h"
#include "ocudu/ran/pdsch/pdsch_constants.h"
#include "ocudu/ran/sch/tbs_calculator.h"
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
  sched_cfg.rrm           = make_cell_rrm_config(sched_cfg.ran);
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

  // The PDSCH layer limit of the UE (maxMIMO-Layers signalled to the UE) should match the resolved maximum number of DL
  // layers.
  const ue_downlink_bwp_config* ue_dl_bwp = ue_cell_cfg.init_bwp().dl.ue_cfg();
  EXPECT_NE(ue_dl_bwp, nullptr);
  if (ue_dl_bwp != nullptr) {
    EXPECT_EQ(ue_dl_bwp->pdsch.max_nof_layers, max_configs)
        << "The PDSCH layer limit of the UE must match the resolved max DL layers for nof_dl_antennas="
        << nof_dl_antennas;

    // The UE derives TBS_LBRM from maxMIMO-Layers, the MCS table and the size of its BWP (TS 38.212, Section 5.4.2.1).
    EXPECT_EQ(ue_cell_cfg.init_bwp().dl.ue_cfg()->pdsch.mcs_table, pdsch_mcs_table::qam256);
    EXPECT_EQ(ue_cell_cfg.pdsch_tbs_lbrm(),
              tbs_calculator_dlsch_lbrm(
                  max_configs, modulation_scheme::QAM256, ue_cell_cfg.init_bwp().dl.cfg().crbs.length()))
        << "TBS_LBRM must follow the UE configuration for nof_dl_antennas=" << nof_dl_antennas;
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

/// Test fixture with a scheduler configuration manager.
class ue_configuration_tester : public ::testing::Test
{
protected:
  /// Adds a cell to the configuration manager.
  const cell_configuration& add_cell(const sched_cell_configuration_request_message& req =
                                         sched_config_helper::make_default_sched_cell_configuration_request())
  {
    return *cfg_mng.add_cell(req);
  }

  /// Creates a UE creation request for the cell, with the default configuration of the UE.
  static sched_ue_creation_request_message make_ue_creation_request(const cell_configuration& cell_cfg)
  {
    return sched_config_helper::create_default_sched_ue_creation_request(
        cell_cfg.params, std::array<lcid_t, 3>{lcid_t::LCID_SRB1, lcid_t::LCID_SRB2, lcid_t::LCID_MIN_DRB});
  }

  scheduler_expert_config  expert_cfg = config_helpers::make_default_scheduler_expert_config();
  sched_cfg_dummy_notifier metric_notif;
  sched_config_manager     cfg_mng{scheduler_config{expert_cfg, metric_notif}};
};

// The scheduler takes the PUCCH, CSI, HARQ and xOverhead values of the UE from the RRM configurations.
TEST_F(ue_configuration_tester, ue_config_comes_from_the_rrm_configs)
{
  sched_cell_configuration_request_message sched_cfg =
      sched_config_helper::make_default_sched_cell_configuration_request();
  sched_cfg.ran.init_bwp.pdsch.x_ov_head     = x_overhead::xoh12;
  sched_cfg.ran.init_bwp.pusch.x_ov_head     = x_overhead::xoh6;
  sched_cfg.rrm                              = make_cell_rrm_config(sched_cfg.ran);
  const cell_configuration&         cell_cfg = add_cell(sched_cfg);
  sched_ue_creation_request_message ue_req   = make_ue_creation_request(cell_cfg);
  ue_rrm_config&                    rrm      = ue_req.cfg.cells->front().ded->rrm;
  ASSERT_TRUE(rrm.csi.periodic_report.has_value());
  rrm.pucch.sr_offset          = rrm.pucch.sr_offset + 1;
  rrm.pucch.rep_f1_3_4_enabled = true;
  rrm.csi.periodic_report->offset += 1;
  rrm.pdsch.nof_harqs = 4;
  rrm.pusch.nof_harqs = 32;
  rrm.pdsch.harq_feedback_disabled.set(3);
  rrm.pusch.harq_mode.reset(1);

  ue_config_update_event ev = cfg_mng.add_ue(ue_req);
  ASSERT_TRUE(ev.valid());
  const ue_cell_configuration& ue_cell_cfg = ev.next_config().pcell_cfg();
  const ue_uplink_bwp_config&  ue_ul       = *ue_cell_cfg.init_bwp().ul.ue_cfg();
  EXPECT_EQ(ue_ul.pucch.res_set_cfg_id, rrm.pucch.res_set_cfg_id);
  EXPECT_EQ(ue_ul.pucch.sr_res_id, rrm.pucch.sr_res_id);
  EXPECT_EQ(ue_ul.pucch.sr_offset, rrm.pucch.sr_offset);
  EXPECT_TRUE(ue_ul.pucch.rep_f1_3_4_enabled);
  EXPECT_FALSE(ue_ul.pucch.rep_f0_2_enabled);
  ASSERT_TRUE(ue_ul.csi.periodic_report.has_value());
  EXPECT_EQ(ue_ul.csi.periodic_report->pucch_res_id, rrm.csi.periodic_report->res_id);
  EXPECT_EQ(ue_ul.csi.periodic_report->offset, rrm.csi.periodic_report->offset);
  const ue_downlink_bwp_config& ue_dl = *ue_cell_cfg.init_bwp().dl.ue_cfg();
  EXPECT_EQ(ue_dl.pdsch.nof_harqs, rrm.pdsch.nof_harqs);
  EXPECT_EQ(ue_dl.pdsch.harq_feedback_disabled, rrm.pdsch.harq_feedback_disabled);
  EXPECT_EQ(ue_ul.pusch.nof_harqs, rrm.pusch.nof_harqs);
  EXPECT_EQ(ue_ul.pusch.harq_mode, rrm.pusch.harq_mode);

  // The scheduler assumes the xOverhead that the DU signals to the UE, also in its PDSCH parameters.
  EXPECT_EQ(ue_cell_cfg.pdsch_nof_oh_prb(), 12);
  EXPECT_EQ(ue_cell_cfg.pusch_nof_oh_prb(), 6);
  unsigned nof_checked = 0;
  for (unsigned id = 0; id != MAX_NOF_SEARCH_SPACES; ++id) {
    if (const search_space_info* ss = ue_cell_cfg.find_search_space(to_search_space_id(id)); ss != nullptr) {
      EXPECT_EQ(ss->get_pdsch_config(0, 1).nof_oh_prb, 12) << "ss=" << id;
      ++nof_checked;
    }
  }
  EXPECT_GT(nof_checked, 0);
  ev.notify_completion();

  // A UE that only has the common configuration of the cell, e.g. to get an RRC Reject, has no overhead.
  sched_ue_creation_request_message reject_ue_req = make_ue_creation_request(cell_cfg);
  reject_ue_req.ue_index                          = to_du_ue_index(1);
  reject_ue_req.crnti                             = to_rnti(0x4602);
  reject_ue_req.cfg.cells->front().ded.reset();
  ue_config_update_event reject_ev = cfg_mng.add_ue(reject_ue_req);
  ASSERT_TRUE(reject_ev.valid());
  EXPECT_EQ(reject_ev.next_config().pcell_cfg().pdsch_nof_oh_prb(), 0);
  EXPECT_EQ(reject_ev.next_config().pcell_cfg().pusch_nof_oh_prb(), 0);
  reject_ev.notify_completion();
}
