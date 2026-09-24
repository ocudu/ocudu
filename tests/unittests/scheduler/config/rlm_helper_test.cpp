// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "tests/ocudu_test_requirements.h"
#include "ocudu/scheduler/config/rlm_helper.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

/// N_RLM values in Table 5-1, TS 38.213, indexed by L_max.
constexpr unsigned N_RLM_L_MAX_4  = 2;
constexpr unsigned N_RLM_L_MAX_8  = 4;
constexpr unsigned N_RLM_L_MAX_64 = 8;

/// Number of CSI-RS resources for tracking that the CSI configuration provides.
constexpr unsigned NOF_TRACKING_CSI_RS_RESOURCES = 4;

class rlm_helper_test : public ::testing::Test
{
protected:
  /// Builds a list of CSI-RS resources that are all candidates for RLM, as per TS 38.213, Section 5.
  static std::vector<nzp_csi_rs_resource> make_csi_rs_resources(unsigned nof_resources)
  {
    std::vector<nzp_csi_rs_resource> resources(nof_resources);
    for (unsigned i = 0; i != nof_resources; ++i) {
      resources[i].res_id                   = static_cast<nzp_csi_rs_res_id_t>(i);
      resources[i].res_mapping.nof_ports    = 1;
      resources[i].res_mapping.cdm          = csi_rs_cdm_type::no_CDM;
      resources[i].res_mapping.freq_density = csi_rs_freq_density_type::one;
    }
    return resources;
  }

  /// Builds a bitmap in which the \c nof_ssbs lowest SSB candidates are transmitted.
  static ssb_bitmap_t make_ssb_bitmap(unsigned nof_ssbs, uint8_t l_max)
  {
    ssb_bitmap_t bitmap;
    bitmap.set_L_max(l_max);
    bitmap.reset();
    for (unsigned i = 0; i != nof_ssbs; ++i) {
      bitmap.set(i);
    }
    return bitmap;
  }

  /// Returns the SSB indexes of the RLM resources that use an SSB as detection resource.
  static std::vector<unsigned> ssb_indexes_of(const radio_link_monitoring_config& cfg)
  {
    std::vector<unsigned> indexes;
    for (const auto& res : cfg.rlm_resources) {
      if (std::holds_alternative<ssb_id_t>(res.detection_resource)) {
        indexes.push_back(std::get<ssb_id_t>(res.detection_resource).value());
      }
    }
    return indexes;
  }

  /// Returns the number of RLM resources that use a CSI-RS as detection resource.
  static unsigned nof_csi_rs_resources_of(const radio_link_monitoring_config& cfg)
  {
    return std::count_if(cfg.rlm_resources.begin(), cfg.rlm_resources.end(), [](const auto& res) {
      return std::holds_alternative<nzp_csi_rs_res_id_t>(res.detection_resource);
    });
  }
};

} // namespace

TEST_F(rlm_helper_test, default_resource_type_yields_no_resources)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-RLM-16-1");

  const rlm_helper::rlm_builder_params params(rlm_resource_type::default_type, 4);

  ASSERT_TRUE(rlm_helper::make_radio_link_monitoring_config(params, {}).rlm_resources.empty());
}

TEST_F(rlm_helper_test, single_transmitted_ssb_yields_one_ssb_resource)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-RLM-16-1");

  for (uint8_t l_max : {uint8_t{4}, uint8_t{8}, uint8_t{64}}) {
    const rlm_helper::rlm_builder_params params(rlm_resource_type::ssb, l_max, make_ssb_bitmap(1, l_max));
    const radio_link_monitoring_config   cfg = rlm_helper::make_radio_link_monitoring_config(params, {});

    ASSERT_EQ(ssb_indexes_of(cfg), std::vector<unsigned>{0}) << "L_max=" << static_cast<unsigned>(l_max);
    ASSERT_EQ(cfg.rlm_resources.size(), 1) << "L_max=" << static_cast<unsigned>(l_max);
    ASSERT_EQ(cfg.rlm_resources[0].resource_purpose,
              radio_link_monitoring_config::radio_link_monitoring_rs::purpose::rlf);
  }
}

TEST_F(rlm_helper_test, ssb_resource_takes_the_index_of_the_transmitted_candidate)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-RLM-16-1");

  static constexpr uint8_t  l_max   = 8;
  static constexpr unsigned ssb_idx = 5;

  ssb_bitmap_t bitmap;
  bitmap.set_L_max(l_max);
  bitmap.reset();
  bitmap.set(ssb_idx);

  const rlm_helper::rlm_builder_params params(rlm_resource_type::ssb, l_max, bitmap);

  ASSERT_EQ(ssb_indexes_of(rlm_helper::make_radio_link_monitoring_config(params, {})), std::vector<unsigned>{ssb_idx});
}

TEST_F(rlm_helper_test, one_ssb_resource_is_built_per_transmitted_candidate)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-RLM-16-1");

  static constexpr uint8_t  l_max    = 8;
  static constexpr unsigned nof_ssbs = 3;

  const rlm_helper::rlm_builder_params params(rlm_resource_type::ssb, l_max, make_ssb_bitmap(nof_ssbs, l_max));

  ASSERT_EQ(ssb_indexes_of(rlm_helper::make_radio_link_monitoring_config(params, {})),
            (std::vector<unsigned>{0, 1, 2}));
}

TEST_F(rlm_helper_test, nof_ssb_resources_is_capped_at_n_rlm)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-RLM-16-1");

  constexpr std::array<std::pair<uint8_t, unsigned>, 3> l_max_to_n_rlm = {
      {{4, N_RLM_L_MAX_4}, {8, N_RLM_L_MAX_8}, {64, N_RLM_L_MAX_64}}};

  for (const auto& [l_max, n_rlm] : l_max_to_n_rlm) {
    // Transmit every SSB candidate, which is more than N_RLM for every L_max.
    const rlm_helper::rlm_builder_params params(rlm_resource_type::ssb, l_max, make_ssb_bitmap(l_max, l_max));
    const radio_link_monitoring_config   cfg = rlm_helper::make_radio_link_monitoring_config(params, {});

    ASSERT_EQ(cfg.rlm_resources.size(), n_rlm) << "L_max=" << static_cast<unsigned>(l_max);
    ASSERT_EQ(nof_csi_rs_resources_of(cfg), 0) << "L_max=" << static_cast<unsigned>(l_max);
  }
}

TEST_F(rlm_helper_test, ssb_and_csi_rs_leaves_half_of_the_n_rlm_budget_for_csi_rs)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-RLM-16-1");

  const std::vector<nzp_csi_rs_resource> csi_rs_resources = make_csi_rs_resources(NOF_TRACKING_CSI_RS_RESOURCES);

  constexpr std::array<std::pair<uint8_t, unsigned>, 3> l_max_to_n_rlm = {
      {{4, N_RLM_L_MAX_4}, {8, N_RLM_L_MAX_8}, {64, N_RLM_L_MAX_64}}};

  for (const auto& [l_max, n_rlm] : l_max_to_n_rlm) {
    // Transmit every SSB candidate, so that the SSB budget is the only limit.
    const rlm_helper::rlm_builder_params params(
        rlm_resource_type::ssb_and_csi_rs, l_max, make_ssb_bitmap(l_max, l_max));
    const radio_link_monitoring_config cfg = rlm_helper::make_radio_link_monitoring_config(params, csi_rs_resources);

    const unsigned expected_nof_ssb_resources = std::max(n_rlm / 2, 1U);
    ASSERT_EQ(ssb_indexes_of(cfg).size(), expected_nof_ssb_resources) << "L_max=" << static_cast<unsigned>(l_max);
    // The remaining resources are filled with CSI-RS, as far as the CSI-RS list allows.
    ASSERT_EQ(nof_csi_rs_resources_of(cfg), std::min(n_rlm - expected_nof_ssb_resources, NOF_TRACKING_CSI_RS_RESOURCES))
        << "L_max=" << static_cast<unsigned>(l_max);
  }
}

TEST_F(rlm_helper_test, ssb_and_csi_rs_with_a_single_ssb_leaves_the_rest_to_csi_rs)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-RLM-16-1");

  static constexpr uint8_t l_max = 8;

  const std::vector<nzp_csi_rs_resource> csi_rs_resources = make_csi_rs_resources(NOF_TRACKING_CSI_RS_RESOURCES);
  const rlm_helper::rlm_builder_params   params(rlm_resource_type::ssb_and_csi_rs, l_max, make_ssb_bitmap(1, l_max));
  const radio_link_monitoring_config     cfg = rlm_helper::make_radio_link_monitoring_config(params, csi_rs_resources);

  ASSERT_EQ(ssb_indexes_of(cfg), std::vector<unsigned>{0});
  ASSERT_EQ(nof_csi_rs_resources_of(cfg), N_RLM_L_MAX_8 - 1);
}

TEST_F(rlm_helper_test, csi_rs_resource_type_yields_no_ssb_resource)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-RLM-16-1");

  const std::vector<nzp_csi_rs_resource> csi_rs_resources = make_csi_rs_resources(NOF_TRACKING_CSI_RS_RESOURCES);
  const rlm_helper::rlm_builder_params   params(rlm_resource_type::csi_rs, 8);
  const radio_link_monitoring_config     cfg = rlm_helper::make_radio_link_monitoring_config(params, csi_rs_resources);

  ASSERT_TRUE(ssb_indexes_of(cfg).empty());
  ASSERT_EQ(nof_csi_rs_resources_of(cfg), N_RLM_L_MAX_8);
}
