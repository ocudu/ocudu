// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/csi_report/csi_report_config_helpers.h"
#include "ocudu/ran/csi_rs/csi_meas_config.h"
#include "ocudu/ran/csi_rs/csi_report_config.h"
#include "ocudu/ran/csi_rs/csi_resource_config.h"
#include <gtest/gtest.h>

using namespace ocudu;

using typeii_restriction_type = codebook_config::type2::typeii::n1_n2_codebook_subset_restriction_type_t;

/// Creates a minimal CSI measurement configuration with a single CSI report using the given codebook.
static csi_meas_config make_csi_meas_config(const codebook_config& codebook)
{
  csi_meas_config csi_meas;

  nzp_csi_rs_resource_set& res_set = csi_meas.nzp_csi_rs_res_set_list.emplace_back();
  res_set.res_set_id               = static_cast<nzp_csi_rs_res_set_id_t>(0);
  res_set.nzp_csi_rs_res.push_back(static_cast<nzp_csi_rs_res_id_t>(0));

  csi_resource_config& res_cfg = csi_meas.csi_res_cfg_list.emplace_back();
  res_cfg.res_cfg_id           = static_cast<csi_res_config_id_t>(0);
  res_cfg.csi_rs_res_set_list =
      csi_resource_config::nzp_csi_rs_ssb{.nzp_csi_rs_res_set_list = {static_cast<nzp_csi_rs_res_set_id_t>(0)}};
  res_cfg.res_type = csi_resource_config::resource_type::aperiodic;

  csi_report_config& report_cfg   = csi_meas.csi_report_cfg_list.emplace_back();
  report_cfg.report_cfg_id        = static_cast<csi_report_config_id_t>(0);
  report_cfg.res_for_channel_meas = static_cast<csi_res_config_id_t>(0);
  report_cfg.report_qty_type      = csi_report_config::report_quantity_type_t::cri_ri_pmi_cqi;
  report_cfg.codebook_cfg         = codebook;

  return csi_meas;
}

/// Creates a Type II codebook configuration.
static codebook_config make_typeii_codebook_config(typeii_restriction_type        restriction,
                                                   unsigned                       nof_beams,
                                                   pmi_codebook_typeII_phase_size phase_alphabet_size,
                                                   bool                           subband_amplitude = false,
                                                   unsigned                       max_rank          = 2)
{
  codebook_config::type2::typeii typeii;
  typeii.n1_n2_codebook_subset_restriction_type = restriction;
  typeii.typeii_ri_restriction.resize(2);
  typeii.typeii_ri_restriction.fill(0, max_rank, true);

  codebook_config::type2 type2;
  type2.sub_type            = typeii;
  type2.nof_beams           = nof_beams;
  type2.phase_alphabet_size = phase_alphabet_size;
  type2.subband_amplitude   = subband_amplitude;

  codebook_config codebook;
  codebook.codebook_type = type2;

  return codebook;
}

// An eight-port Type II codebook configuration must translate into the equivalent PMI codebook.
TEST(csi_report_config_helpers, typeii_eight_port_translation)
{
  const csi_report_configuration config = create_csi_report_configuration(
      make_csi_meas_config(make_typeii_codebook_config(typeii_restriction_type::four_one,
                                                       /* nof_beams = */ 4,
                                                       pmi_codebook_typeII_phase_size::psk8,
                                                       /* subband_amplitude = */ true)));

  ASSERT_TRUE(std::holds_alternative<pmi_codebook_typeII>(config.pmi_codebook));

  const auto& codebook = std::get<pmi_codebook_typeII>(config.pmi_codebook);
  ASSERT_EQ(codebook.n1_n2, pmi_codebook_single_panel_config::four_one);
  ASSERT_EQ(codebook.nof_beams, 4);
  ASSERT_EQ(codebook.phase_alphabet_size, pmi_codebook_typeII_phase_size::psk8);
  ASSERT_TRUE(codebook.subband_amplitude);

  ASSERT_EQ(get_precoding_codebook_antenna_ports(config.pmi_codebook), 8);
  ASSERT_TRUE(is_valid(config));
}

// A four-port Type II codebook configuration must translate into the equivalent PMI codebook.
TEST(csi_report_config_helpers, typeii_four_port_translation)
{
  const csi_report_configuration config = create_csi_report_configuration(
      make_csi_meas_config(make_typeii_codebook_config(typeii_restriction_type::two_one,
                                                       /* nof_beams = */ 2,
                                                       pmi_codebook_typeII_phase_size::qpsk)));

  ASSERT_TRUE(std::holds_alternative<pmi_codebook_typeII>(config.pmi_codebook));

  const auto& codebook = std::get<pmi_codebook_typeII>(config.pmi_codebook);
  ASSERT_EQ(codebook.n1_n2, pmi_codebook_single_panel_config::two_one);
  ASSERT_EQ(codebook.nof_beams, 2);
  ASSERT_EQ(codebook.phase_alphabet_size, pmi_codebook_typeII_phase_size::qpsk);
  ASSERT_FALSE(codebook.subband_amplitude);

  ASSERT_EQ(get_precoding_codebook_antenna_ports(config.pmi_codebook), 4);
  ASSERT_TRUE(is_valid(config));
}

// The Type II RI restriction must be carried over, as it selects the reported ranks.
TEST(csi_report_config_helpers, typeii_ri_restriction_is_translated)
{
  for (unsigned max_rank : {1, 2}) {
    const csi_report_configuration config = create_csi_report_configuration(
        make_csi_meas_config(make_typeii_codebook_config(typeii_restriction_type::four_one,
                                                         /* nof_beams = */ 2,
                                                         pmi_codebook_typeII_phase_size::qpsk,
                                                         /* subband_amplitude = */ false,
                                                         max_rank)));

    ASSERT_EQ(config.ri_restriction.count(), max_rank) << "Maximum rank " << max_rank << ".";
    ASSERT_EQ(config.ri_restriction.find_highest(), static_cast<int>(max_rank) - 1);
    ASSERT_TRUE(is_valid(config));
  }
}

// As per TS38.214 Section 5.2.2.2.3, four CSI-RS ports only support two beams.
TEST(csi_report_config_helpers, typeii_four_port_rejects_more_than_two_beams)
{
  for (unsigned nof_beams : {3, 4}) {
    const csi_report_configuration config =
        create_csi_report_configuration(make_csi_meas_config(make_typeii_codebook_config(
            typeii_restriction_type::two_one, nof_beams, pmi_codebook_typeII_phase_size::qpsk)));

    ASSERT_FALSE(is_valid(config)) << "Number of beams " << nof_beams << ".";
  }
}

// The Type I codebook translation must not be affected.
TEST(csi_report_config_helpers, type1_translation_is_preserved)
{
  codebook_config::type1::single_panel::more_than_two_antenna_ports ports;
  ports.n1_n2_restriction_type = pmi_codebook_single_panel_config::four_one;

  codebook_config::type1::single_panel single_panel;
  single_panel.nof_antenna_ports = ports;
  single_panel.typei_single_panel_ri_restriction.resize(8);
  single_panel.typei_single_panel_ri_restriction.fill(0, 4, true);

  codebook_config::type1 type1;
  type1.sub_type      = single_panel;
  type1.codebook_mode = pmi_codebook_typeI_mode::one;

  codebook_config codebook;
  codebook.codebook_type = type1;

  const csi_report_configuration config = create_csi_report_configuration(make_csi_meas_config(codebook));

  ASSERT_TRUE(std::holds_alternative<pmi_codebook_typeI_single_panel>(config.pmi_codebook));
  ASSERT_EQ(std::get<pmi_codebook_typeI_single_panel>(config.pmi_codebook).n1_n2,
            pmi_codebook_single_panel_config::four_one);
  ASSERT_EQ(config.ri_restriction.count(), 4);
  ASSERT_TRUE(is_valid(config));
}
