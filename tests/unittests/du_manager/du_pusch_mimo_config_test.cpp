// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/du/du_high/du_manager/ran_resource_management/du_ran_resource_manager_impl.h"
#include "ocudu/du/du_cell_config_helpers.h"
#include "ocudu/du/du_high/du_qos_config_helpers.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;

namespace {

/// Builds a cell whose PUSCH supports up to the given number of layers.
du_cell_config make_cell_config(unsigned pusch_max_rank)
{
  du_cell_config cell_cfg                    = config_helpers::make_default_du_cell_config();
  cell_cfg.ran.init_bwp.pusch.max_nof_layers = pusch_max_rank;
  return cell_cfg;
}

class du_pusch_mimo_config_tester : public ::testing::TestWithParam<unsigned>
{
protected:
  explicit du_pusch_mimo_config_tester(du_test_mode_config test_mode_cfg_ = {}) :
    test_mode_cfg(test_mode_cfg_),
    cell_cfg_list({make_cell_config(GetParam())}),
    qos_cfg_list(config_helpers::make_default_du_qos_config_list(/* warn_on_drop */ true, 1000)),
    res_mng(cell_cfg_list,
            scheduler_expert_config{.ue = {.max_pucchs_per_slot = 31}},
            srb_cfg_list,
            qos_cfg_list,
            test_mode_cfg)
  {
    auto result = res_mng.create_ue_resource_configurator(ue_idx, to_du_cell_index(0), true);
    report_fatal_error_if_not(result.has_value(), "Failed to create UE resources");
    ue_res.emplace(std::move(result.value()));
  }

  const ue_cell_config& pcell_cfg() const { return ue_res->value().cell_group.cells.at(SERVING_PCELL_IDX); }

  /// Returns the maximum PUSCH rank of the UE, checking that the RRC and scheduler configurations match.
  unsigned max_rank() const
  {
    const auto& rrc_tx_cfg = pcell_cfg().serv_cell_cfg.ul_config->init_ul_bwp.pusch_cfg->tx_cfg;
    report_fatal_error_if_not(rrc_tx_cfg.has_value() and std::holds_alternative<tx_scheme_codebook>(*rrc_tx_cfg),
                              "No codebook tx scheme");
    const tx_scheme_codebook& codebook = std::get<tx_scheme_codebook>(*rrc_tx_cfg);
    report_fatal_error_if_not(codebook == pcell_cfg().bwps.front().ul.pusch.tx_cfg,
                              "The scheduler PUSCH tx scheme differs from the RRC one");
    return codebook.max_rank.value();
  }

  /// Returns the number of SRS ports of the UE, checking that every SRS resource has the same number.
  unsigned nof_srs_ports() const
  {
    const auto nof_ports = pcell_cfg().bwps.front().ul.srs.nof_ports;
    for (const auto& srs_res : pcell_cfg().serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg->srs_res_list) {
      report_fatal_error_if_not(srs_res.nof_ports == nof_ports, "The SRS resource ports differ from the scheduler");
    }
    return static_cast<unsigned>(nof_ports);
  }

  static constexpr du_ue_index_t ue_idx = to_du_ue_index(0);

  du_test_mode_config                         test_mode_cfg;
  std::vector<du_cell_config>                 cell_cfg_list;
  std::map<srb_id_t, du_srb_config>           srb_cfg_list;
  std::map<five_qi_t, du_qos_config>          qos_cfg_list;
  du_ran_resource_manager_impl                res_mng;
  std::optional<ue_ran_resource_configurator> ue_res;
};

/// Runs the same cell in test mode, where the UE capabilities are never decoded.
class du_pusch_mimo_config_test_mode_tester : public du_pusch_mimo_config_tester
{
protected:
  du_pusch_mimo_config_test_mode_tester() :
    du_pusch_mimo_config_tester(du_test_mode_config{.test_ue = du_test_mode_config::test_mode_ue_config{}})
  {
  }
};

} // namespace

TEST_P(du_pusch_mimo_config_tester, a_single_layer_and_srs_port_are_configured_before_the_capabilities_are_decoded)
{
  ASSERT_EQ(max_rank(), 1);
  ASSERT_EQ(nof_srs_ports(), 1);
}

TEST_P(du_pusch_mimo_config_test_mode_tester, the_cell_rank_is_configured_for_a_test_mode_ue)
{
  const unsigned cell_max_rank = GetParam();

  ASSERT_EQ(max_rank(), cell_max_rank);
  // SRS resources have 1, 2 or 4 ports.
  ASSERT_EQ(nof_srs_ports(), cell_max_rank == 3 ? 4 : cell_max_rank);
}

INSTANTIATE_TEST_SUITE_P(pusch_max_rank, du_pusch_mimo_config_tester, ::testing::Values(1U, 2U, 4U));
INSTANTIATE_TEST_SUITE_P(pusch_max_rank, du_pusch_mimo_config_test_mode_tester, ::testing::Values(1U, 2U, 3U, 4U));
