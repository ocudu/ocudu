// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "typeii_ue_capabilities.h"
#include "tests/test_doubles/rrc/rrc_test_messages.h"
#include "ocudu/asn1/rrc_nr/ue_cap.h"
#include "ocudu/asn1/rrc_nr/ul_dcch_msg_ies.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;
using namespace asn1::rrc_nr;

/// Returns the enumerated value that reports the given number of CSI-RS ports per resource.
static supported_csi_rs_res_s::max_num_tx_ports_per_res_e_ to_asn1_nof_tx_ports(unsigned nof_tx_ports)
{
  supported_csi_rs_res_s::max_num_tx_ports_per_res_e_ result;
  switch (nof_tx_ports) {
    case 2:
      result.value = supported_csi_rs_res_s::max_num_tx_ports_per_res_opts::p2;
      break;
    case 4:
      result.value = supported_csi_rs_res_s::max_num_tx_ports_per_res_opts::p4;
      break;
    case 8:
      result.value = supported_csi_rs_res_s::max_num_tx_ports_per_res_opts::p8;
      break;
    default:
      report_fatal_error("Unsupported number of CSI-RS ports per resource {}.", nof_tx_ports);
  }

  return result;
}

static mimo_layers_dl_e to_asn1_nof_pdsch_layers(unsigned nof_pdsch_layers)
{
  mimo_layers_dl_e result;
  switch (nof_pdsch_layers) {
    case 2:
      result.value = mimo_layers_dl_opts::two_layers;
      break;
    case 4:
      result.value = mimo_layers_dl_opts::four_layers;
      break;
    case 8:
      result.value = mimo_layers_dl_opts::eight_layers;
      break;
    default:
      report_fatal_error("Unsupported number of PDSCH layers {}.", nof_pdsch_layers);
  }

  return result;
}

byte_buffer ocudu::test_helpers::create_typeii_ue_capability_container(nr_band  band,
                                                                       unsigned nof_beams,
                                                                       unsigned nof_tx_ports,
                                                                       unsigned nof_pdsch_layers)
{
  // Take the capabilities that the test doubles report, which already carry the Type I codebook of the band.
  ul_dcch_msg_s                msg      = create_ue_capability_info();
  ue_cap_rat_container_list_l& rat_list = msg.msg.c1().ue_cap_info().crit_exts.ue_cap_info().ue_cap_rat_container_list;
  report_fatal_error_if_not(rat_list.size() == 1, "The capabilities report an unexpected number of RAT containers.");

  ue_nr_cap_s ue_cap;
  {
    asn1::cbit_ref bref{rat_list[0].ue_cap_rat_container};
    report_fatal_error_if_not(asn1::unpack(ue_cap, bref) == asn1::OCUDUASN_SUCCESS,
                              "Failed to unpack the UE NR capabilities.");
  }

  report_fatal_error_if_not(ue_cap.rf_params.supported_band_list_nr.size() != 0, "The capabilities report no band.");
  band_nr_s& band_cap = ue_cap.rf_params.supported_band_list_nr[0];
  band_cap.band_nr    = static_cast<uint16_t>(band);

  // Report the codebooks of the band, as per TS 38.331 codebookParameters.
  band_cap.mimo_params_per_band_present = true;
  // The codebook parameters belong to the first extension group of the MIMO parameters.
  if (not band_cap.mimo_params_per_band.codebook_params.is_present()) {
    band_cap.mimo_params_per_band.codebook_params = asn1::make_copy_ptr(codebook_params_s{});
  }
  codebook_params_s& codebook = *band_cap.mimo_params_per_band.codebook_params;

  supported_csi_rs_res_s csi_rs_res;
  csi_rs_res.max_num_tx_ports_per_res    = to_asn1_nof_tx_ports(nof_tx_ports);
  csi_rs_res.max_num_res_per_band        = 1;
  csi_rs_res.total_num_tx_ports_per_band = static_cast<uint16_t>(nof_tx_ports);

  // The Type I codebook is mandatory in the codebook parameters.
  codebook_params_s::type1_s_::single_panel_s_& type1 = codebook.type1.single_panel;
  type1.supported_csi_rs_res_list.resize(1);
  type1.supported_csi_rs_res_list[0] = csi_rs_res;
  type1.modes.value                  = codebook_params_s::type1_s_::single_panel_s_::modes_opts::mode1;
  type1.max_num_csi_rs_per_res_set   = 1;

  codebook.type2_present             = true;
  codebook_params_s::type2_s_& type2 = codebook.type2;
  type2.param_lx                     = static_cast<uint8_t>(nof_beams);
  type2.amplitude_scaling_type.value = codebook_params_s::type2_s_::amplitude_scaling_type_opts::wideband;
  type2.supported_csi_rs_res_list.resize(1);
  type2.supported_csi_rs_res_list[0] = csi_rs_res;

  // Report the number of PDSCH layers of the band, as per TS 38.331, FeatureSetDownlinkPerCC, in a single-carrier band
  // combination.
  feature_set_dl_per_cc_s dl_per_cc;
  dl_per_cc.supported_subcarrier_spacing_dl.value = subcarrier_spacing_opts::khz30;
  dl_per_cc.supported_bw_dl.set_fr1().value       = supported_bw_c::fr1_opts::mhz100;
  dl_per_cc.max_num_mimo_layers_pdsch_present     = true;
  dl_per_cc.max_num_mimo_layers_pdsch             = to_asn1_nof_pdsch_layers(nof_pdsch_layers);
  ue_cap.feature_sets_present                     = true;
  ue_cap.feature_sets.feature_sets_dl_per_cc.push_back(dl_per_cc);

  feature_set_dl_s dl_set;
  dl_set.feature_set_list_per_dl_cc.push_back(ue_cap.feature_sets.feature_sets_dl_per_cc.size());
  ue_cap.feature_sets.feature_sets_dl.push_back(dl_set);

  feature_set_combination_l feature_set_combination(1);
  feature_set_combination[0].resize(1);
  feature_set_combination[0][0].set_nr().dl_set_nr = ue_cap.feature_sets.feature_sets_dl.size();
  ue_cap.feature_set_combinations.push_back(feature_set_combination);

  band_combination_s band_combination;
  band_combination.band_list.resize(1);
  band_combination.band_list[0].set_nr().band_nr = static_cast<uint16_t>(band);
  band_combination.feature_set_combination       = ue_cap.feature_set_combinations.size() - 1;
  ue_cap.rf_params.supported_band_combination_list.push_back(band_combination);

  // Pack the capabilities back into the container of their RAT.
  byte_buffer packed_ue_cap;
  {
    asn1::bit_ref bref{packed_ue_cap};
    report_fatal_error_if_not(asn1::pack(bref, ue_cap) == asn1::OCUDUASN_SUCCESS,
                              "Failed to pack the UE NR capabilities.");
  }
  rat_list[0].ue_cap_rat_container = std::move(packed_ue_cap);

  byte_buffer result;
  {
    asn1::bit_ref bref{result};
    report_fatal_error_if_not(asn1::pack_dyn_seq_of(bref, rat_list, 0, 8) == asn1::OCUDUASN_SUCCESS,
                              "Failed to pack the UE Capability RAT Container List.");
  }

  return result;
}
