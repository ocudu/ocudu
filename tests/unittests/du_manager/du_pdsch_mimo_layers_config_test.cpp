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

/// Number of DL antenna ports, and therefore of PDSCH layers, of the cell under test.
constexpr unsigned cell_nof_layers = 4;

/// Builds the parameters of a cell with \ref cell_nof_layers DL antenna ports.
cell_config_builder_params make_cell_params()
{
  cell_config_builder_params params;
  params.dl_carrier.nof_ant = cell_nof_layers;
  return params;
}

/// Builds UE capabilities declaring the given maximum number of PDSCH layers for the given band.
ue_capability_summary make_caps(nr_band band, unsigned pdsch_max_rank)
{
  ue_capability_summary                 caps;
  ue_capability_summary::supported_band band_caps;
  band_caps.pdsch_max_rank = pdsch_max_rank;
  caps.bands.emplace(band, band_caps);
  return caps;
}

class du_pdsch_mimo_layers_config_tester : public ::testing::Test
{
protected:
  explicit du_pdsch_mimo_layers_config_tester(du_test_mode_config test_mode_cfg_ = {}) :
    test_mode_cfg(test_mode_cfg_),
    cell_cfg_list({config_helpers::make_default_du_cell_config(make_cell_params())}),
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

  /// Runs a UE configuration carrying the given capabilities.
  void update_ue_caps(const ue_capability_summary& caps)
  {
    f1ap_ue_context_update_request req;
    req.ue_index = ue_idx;
    req.srbs_to_setup.push_back(srb_id_t::srb1);

    const du_ue_resource_update_response resp = ue_res->update(to_du_cell_index(0), req, nullptr, &caps);
    report_fatal_error_if_not(not resp.failed(), "UE configuration failed");
  }

  const serving_cell_config& serv_cell_cfg() const
  {
    return ue_res->value().cell_group.cells.at(SERVING_PCELL_IDX).serv_cell_cfg;
  }

  /// \brief Returns the maxMIMO-Layers of the UE PDSCH-ServingCellConfig.
  ///
  /// The PDSCH layer limit of the scheduler must be the same value.
  unsigned max_mimo_layers() const
  {
    report_fatal_error_if_not(serv_cell_cfg().pdsch_serv_cell_cfg.has_value(), "No PDSCH-ServingCellConfig");
    report_fatal_error_if_not(serv_cell_cfg().pdsch_serv_cell_cfg->max_mimo_layers.has_value(), "No maxMIMO-Layers");
    const unsigned                 rrc_value = *serv_cell_cfg().pdsch_serv_cell_cfg->max_mimo_layers;
    const std::optional<unsigned>& sched_value =
        ue_res->value().cell_group.cells.at(SERVING_PCELL_IDX).bwps.front().dl.pdsch.max_nof_layers;
    report_fatal_error_if_not(sched_value == rrc_value, "The scheduler PDSCH layer limit differs from maxMIMO-Layers");
    return rrc_value;
  }

  /// Returns the ranks that the CSI reports of the UE can indicate, as per the Type-I single-panel RI restriction.
  bounded_bitset<8> ri_restriction() const
  {
    report_fatal_error_if_not(serv_cell_cfg().csi_meas_cfg.has_value(), "No CSI-MeasConfig");
    for (const csi_report_config& report_cfg : serv_cell_cfg().csi_meas_cfg->csi_report_cfg_list) {
      if (report_cfg.codebook_cfg.has_value()) {
        const auto& type1 = std::get<codebook_config::type1>(report_cfg.codebook_cfg->codebook_type);
        return std::get<codebook_config::type1::single_panel>(type1.sub_type).typei_single_panel_ri_restriction;
      }
    }
    report_fatal_error("No CSI report with a codebook config");
  }

  /// Returns the RI restriction of the cell, which allows the ranks from one to \ref cell_nof_layers.
  static bounded_bitset<8> cell_ri_restriction()
  {
    bounded_bitset<8> restriction(8);
    restriction.fill(0, cell_nof_layers);
    return restriction;
  }

  nr_band cell_band() const { return cell_cfg_list[0].ran.dl_carrier.band; }

  static constexpr du_ue_index_t ue_idx = to_du_ue_index(0);

  du_test_mode_config                         test_mode_cfg;
  std::vector<du_cell_config>                 cell_cfg_list;
  std::map<srb_id_t, du_srb_config>           srb_cfg_list;
  std::map<five_qi_t, du_qos_config>          qos_cfg_list;
  du_ran_resource_manager_impl                res_mng;
  std::optional<ue_ran_resource_configurator> ue_res;
};

/// Runs the same cell in test mode, where the UE capabilities are never decoded.
class du_pdsch_mimo_layers_config_test_mode_tester : public du_pdsch_mimo_layers_config_tester
{
protected:
  du_pdsch_mimo_layers_config_test_mode_tester() :
    du_pdsch_mimo_layers_config_tester(du_test_mode_config{.test_ue = du_test_mode_config::test_mode_ue_config{}})
  {
  }
};

} // namespace

TEST_F(du_pdsch_mimo_layers_config_tester, a_single_layer_is_configured_before_the_capabilities_are_decoded)
{
  // Before the UE capabilities are decoded, the UE is configured with a single layer, which every UE supports.
  ASSERT_EQ(max_mimo_layers(), 1);
}

TEST_F(du_pdsch_mimo_layers_config_test_mode_tester, the_cell_layers_are_configured_for_a_test_mode_ue)
{
  // The test mode UE reports no capabilities, so the cell configuration is applied as is.
  ASSERT_EQ(max_mimo_layers(), cell_nof_layers);
}

TEST_F(du_pdsch_mimo_layers_config_tester, the_layers_are_limited_to_the_ue_capability)
{
  update_ue_caps(make_caps(cell_band(), 2));

  ASSERT_EQ(max_mimo_layers(), 2);
}

TEST_F(du_pdsch_mimo_layers_config_tester, the_layers_are_limited_to_the_cell_layers)
{
  update_ue_caps(make_caps(cell_band(), 8));

  ASSERT_EQ(max_mimo_layers(), cell_nof_layers);
}

TEST_F(du_pdsch_mimo_layers_config_tester, a_single_layer_is_configured_for_a_ue_that_does_not_report_the_band)
{
  // The capabilities are reported per band, so a UE that does not list the band of the cell cannot be assumed to
  // support more than one layer in it.
  update_ue_caps(make_caps(static_cast<nr_band>(0xff), 4));

  ASSERT_EQ(max_mimo_layers(), 1);
}

TEST_F(du_pdsch_mimo_layers_config_tester, the_csi_configuration_does_not_depend_on_max_mimo_layers)
{
  // The RI restriction sets the size of the CSI reports, as per TS 38.212, Section 6.3.1.1.2. The scheduler applies a
  // new configuration before the UE does, so a change of size would make it decode the CSI reports in between wrongly.
  ASSERT_EQ(ri_restriction(), cell_ri_restriction());
  const csi_meas_config csi_cfg_before_caps = *serv_cell_cfg().csi_meas_cfg;

  for (unsigned ue_max_layers : {1U, 2U, cell_nof_layers}) {
    update_ue_caps(make_caps(cell_band(), ue_max_layers));

    ASSERT_EQ(max_mimo_layers(), ue_max_layers);
    ASSERT_EQ(*serv_cell_cfg().csi_meas_cfg, csi_cfg_before_caps);
  }
}
