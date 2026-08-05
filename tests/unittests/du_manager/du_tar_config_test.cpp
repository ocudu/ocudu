// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/du/du_high/du_manager/ran_resource_management/du_ran_resource_manager_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/du/du_cell_config_helpers.h"
#include "ocudu/du/du_high/du_qos_config_helpers.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;

namespace {

/// tar-Config the cell is configured with, when it is an NTN cell that enables variation-triggered TA reporting.
constexpr tar_config cell_tar_cfg{std::chrono::microseconds{1000}, true};

enum class cell_type { terrestrial, ntn_without_tar, ntn_with_tar };

du_cell_config create_du_cell_config(const cell_config_builder_params& params, cell_type type)
{
  du_cell_config cell = config_helpers::make_default_du_cell_config(params);
  if (type == cell_type::terrestrial) {
    return cell;
  }

  ntn_cell_params ntn;
  ntn.ntn_cfg.cell_specific_koffset = std::chrono::milliseconds{16};
  if (type == cell_type::ntn_with_tar) {
    ntn.tar_cfg = cell_tar_cfg;
  }
  cell.ran.ntn_params = ntn;
  return cell;
}

/// Builds UE capabilities declaring support for the band of the cell under test.
ue_capability_summary make_ta_report_caps(nr_band band, bool ul_ta_reporting_supported, bool sr_supported)
{
  ue_capability_summary caps;
  caps.sr_triggered_by_ta_report_supported = sr_supported;

  ue_capability_summary::supported_band band_caps;
  band_caps.ul_ta_reporting_supported = ul_ta_reporting_supported;
  caps.bands.emplace(band, band_caps);

  return caps;
}

class du_tar_config_tester : public ::testing::TestWithParam<cell_type>
{
protected:
  du_tar_config_tester() :
    cell_cfg_list({create_du_cell_config(params, GetParam())}),
    qos_cfg_list(config_helpers::make_default_du_qos_config_list(/* warn_on_drop */ true, 1000)),
    res_mng(cell_cfg_list,
            scheduler_expert_config{.ue = {.max_pucchs_per_slot = 31}},
            srb_cfg_list,
            qos_cfg_list,
            dummy_test_mode_cfg)
  {
    auto result = res_mng.create_ue_resource_configurator(ue_idx, to_du_cell_index(0), true);
    report_fatal_error_if_not(result.has_value(), "Failed to create UE resources");
    ue_res.emplace(std::move(result.value()));
  }

  /// Runs a UE configuration carrying the given capabilities and returns the resulting tar-Config.
  const std::optional<tar_config>& update_ue_caps(const ue_capability_summary& caps)
  {
    f1ap_ue_context_update_request req;
    req.ue_index = ue_idx;
    req.srbs_to_setup.push_back(srb_id_t::srb1);

    const du_ue_resource_update_response resp = ue_res->update(to_du_cell_index(0), req, nullptr, &caps);
    report_fatal_error_if_not(not resp.failed(), "UE configuration failed");

    return ue_res->value().cell_group.mcg_cfg.tar_cfg;
  }

  nr_band cell_band() const { return cell_cfg_list[0].ran.dl_carrier.band; }

  static constexpr du_ue_index_t ue_idx = to_du_ue_index(0);

  cell_config_builder_params                  params{};
  du_test_mode_config                         dummy_test_mode_cfg{};
  std::vector<du_cell_config>                 cell_cfg_list;
  std::map<srb_id_t, du_srb_config>           srb_cfg_list;
  std::map<five_qi_t, du_qos_config>          qos_cfg_list;
  du_ran_resource_manager_impl                res_mng;
  std::optional<ue_ran_resource_configurator> ue_res;
};

} // namespace

TEST_P(du_tar_config_tester, tar_config_is_signalled_only_by_an_ntn_cell_that_configures_it)
{
  // The UE supports everything, so only the cell configuration decides.
  const std::optional<tar_config>& tar_cfg = update_ue_caps(make_ta_report_caps(cell_band(), true, true));

  if (GetParam() == cell_type::ntn_with_tar) {
    ASSERT_TRUE(tar_cfg.has_value());
    ASSERT_EQ(cell_tar_cfg, *tar_cfg);
  } else {
    ASSERT_FALSE(tar_cfg.has_value()) << "tar-Config must not be signalled by a cell that does not configure it";
  }
}

TEST_P(du_tar_config_tester, tar_config_is_not_signalled_to_a_ue_without_uplink_ta_reporting)
{
  // Asking for a procedure the UE does not implement gets no reports and risks the UE rejecting the reconfiguration.
  ASSERT_FALSE(update_ue_caps(make_ta_report_caps(cell_band(), false, true)).has_value());
}

TEST_P(du_tar_config_tester, tar_config_is_not_signalled_when_the_ue_does_not_support_the_band)
{
  // The capability is per band, so a UE that reports another band gets no tar-Config either.
  ue_capability_summary caps = make_ta_report_caps(cell_band(), true, true);
  caps.bands.clear();
  ASSERT_FALSE(update_ue_caps(caps).has_value());
}

TEST_P(du_tar_config_tester, timing_advance_sr_is_dropped_when_the_ue_cannot_raise_it)
{
  // sr-TriggeredBy-TA-Report-r17 is a capability of its own: without it, the rest of tar-Config still applies.
  const std::optional<tar_config>& tar_cfg = update_ue_caps(make_ta_report_caps(cell_band(), true, false));

  if (GetParam() == cell_type::ntn_with_tar) {
    ASSERT_TRUE(tar_cfg.has_value());
    ASSERT_EQ(cell_tar_cfg.offset_threshold_ta, tar_cfg->offset_threshold_ta);
    ASSERT_FALSE(tar_cfg->sr_enabled);
  } else {
    ASSERT_FALSE(tar_cfg.has_value());
  }
}

INSTANTIATE_TEST_SUITE_P(du_tar_config,
                         du_tar_config_tester,
                         ::testing::Values(cell_type::terrestrial,
                                           cell_type::ntn_without_tar,
                                           cell_type::ntn_with_tar));
