// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/csi_report/csi_report_config_helpers.h"

using namespace ocudu;

csi_report_configuration ocudu::create_csi_report_configuration(const csi_meas_config& csi_meas)
{
  csi_report_configuration csi_rep = {};
  csi_rep.pmi_codebook             = pmi_codebook_one_port{};

  // TODO: support more CSI reports.
  const csi_report_config& csi_rep_cfg = csi_meas.csi_report_cfg_list[0];

  // TODO: support more CSI resource sets.
  nzp_csi_rs_res_set_id_t nzp_csi_set_id =
      std::get<csi_resource_config::nzp_csi_rs_ssb>(
          csi_meas.csi_res_cfg_list[csi_rep_cfg.res_for_channel_meas].csi_rs_res_set_list)
          .nzp_csi_rs_res_set_list[0];
  csi_rep.nof_csi_rs_resources = csi_meas.nzp_csi_rs_res_set_list[nzp_csi_set_id].nzp_csi_rs_res.size();

  // Select the given number of reported Resource Sets if group based beamforming is disabled and the number of resource
  // sets to report is present.
  if (!csi_rep_cfg.is_group_based_beam_reporting_enabled && csi_rep_cfg.nof_reported_rs.has_value()) {
    csi_rep.nof_reported_rs = csi_rep_cfg.nof_reported_rs.value();
  } else {
    csi_rep.nof_reported_rs = 1;
  }

  // Enable indicators
  switch (csi_rep_cfg.report_qty_type) {
    case csi_report_config::report_quantity_type_t::none:
      break;
    case csi_report_config::report_quantity_type_t::cri_ri_li_pmi_cqi:
      csi_rep.quantities = csi_report_quantities::cri_ri_li_pmi_cqi;
      break;
    case csi_report_config::report_quantity_type_t::cri_ri_pmi_cqi:
      csi_rep.quantities = csi_report_quantities::cri_ri_pmi_cqi;
      break;
    case csi_report_config::report_quantity_type_t::cri_ri_cqi:
      csi_rep.quantities = csi_report_quantities::cri_ri_cqi;
      break;
    default:
      csi_rep.quantities = csi_report_quantities::other;
      break;
  }

  if (csi_rep_cfg.codebook_cfg.has_value()) {
    if (const auto* type1 = std::get_if<codebook_config::type1>(&csi_rep_cfg.codebook_cfg->codebook_type)) {
      if (const auto* panel = std::get_if<codebook_config::type1::single_panel>(&type1->sub_type)) {
        using single_panel = codebook_config::type1::single_panel;

        if (std::holds_alternative<single_panel::two_antenna_ports_two_tx_codebook_subset_restriction>(
                panel->nof_antenna_ports)) {
          csi_rep.pmi_codebook = pmi_codebook_two_port{};
        } else if (std::holds_alternative<single_panel::more_than_two_antenna_ports>(panel->nof_antenna_ports)) {
          const single_panel::more_than_two_antenna_ports& nof_antenna_ports =
              std::get<single_panel::more_than_two_antenna_ports>(panel->nof_antenna_ports);
          csi_rep.pmi_codebook =
              pmi_codebook_typeI_single_panel{nof_antenna_ports.n1_n2_restriction_type, type1->codebook_mode};
        } else {
          csi_rep.pmi_codebook = std::monostate();
        }

        csi_rep.ri_restriction = panel->typei_single_panel_ri_restriction;
      } else {
        report_fatal_error("Codebook panel type not supported");
      }
    } else {
      report_fatal_error("Codebook type not supported");
    }
  }

  return csi_rep;
}

bool ocudu::is_valid(const csi_report_configuration& config)
{
  // The number of CSI resources in the corresponding resource set must be at least one and up to 64 (see TS38.331
  // Section 6.3.2, Information Element \c NZP-CSI-RS-ResourceSet).
  constexpr interval<unsigned, true> nof_csi_res_range(1, 64);
  if (!nof_csi_res_range.contains(config.nof_csi_rs_resources)) {
    return false;
  }

  // PMI codebook is not supported if the measurement is for cri-RSRP or ssb-Index-RSRP.
  if (std::holds_alternative<std::monostate>(config.pmi_codebook) &&
      (config.quantities != csi_report_quantities::cri_rsrp) &&
      (config.quantities != csi_report_quantities::ssb_index_rsrp)) {
    return false;
  }

  if (!std::holds_alternative<pmi_codebook_one_port>(config.pmi_codebook)) {
    unsigned nof_csi_rs_ports = get_precoding_codebook_antenna_ports(config.pmi_codebook);

    // The RI restriction set size is too small to cover all possible ranks given the number of CSI-RS ports.
    if (config.ri_restriction.size() < nof_csi_rs_ports) {
      return false;
    }

    // The RI Restriction set cannot allow a higher rank than the number of CSI-RS ports.
    if (config.ri_restriction.find_highest() >= static_cast<int>(nof_csi_rs_ports)) {
      return false;
    }
  }

  // The CSI report quantities are not supported.
  if (config.quantities == csi_report_quantities::other) {
    return false;
  }

  return true;
}

bool ocudu::is_pusch_configured(const csi_meas_config& csi_meas)
{
  ocudu_assert(csi_meas.csi_report_cfg_list.size() == 1, "Only one CSI report configuration is supported");
  return not std::holds_alternative<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
      csi_meas.csi_report_cfg_list[0].report_cfg_type);
}
