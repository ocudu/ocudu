// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/du/du_low/o_du_low_factory.h"
#include "o_du_low_impl.h"
#include "ocudu/du/du_low/du_low_factory.h"
#include "ocudu/du/du_low/o_du_low_config.h"
#include "ocudu/fapi_adaptor/phy/phy_fapi_fastpath_adaptor_factory.h"
#include "ocudu/fapi_adaptor/precoding_matrix_table_generator.h"
#include "ocudu/fapi_adaptor/uci_part2_correspondence_generator.h"
#include "ocudu/ocudulog/ocudulog.h"

using namespace ocudu;
using namespace odu;

static fapi_adaptor::phy_fapi_fastpath_adaptor_dependencies
generate_fapi_fastpath_adaptor_dependencies(du_low&                                               du_low,
                                            const fapi_adaptor::phy_fapi_fastpath_adaptor_config& fapi_cfg,
                                            task_executor&                                        p5_executor)
{
  fapi_adaptor::phy_fapi_fastpath_adaptor_dependencies out_dependencies;

  auto& logger = ocudulog::fetch_basic_logger("FAPI");

  for (unsigned i = 0, e = fapi_cfg.sectors.size(); i != e; ++i) {
    upper_phy& upper = du_low.get_upper_phy(i);

    fapi_adaptor::phy_fapi_p5_sector_fastpath_adaptor_dependencies p5_dependencies = {
        .logger = logger, .executor = p5_executor, .upper_phy_controller = upper.get_operation_controller()};

    pmi_codebook_config codebook_config;
    switch (fapi_cfg.sectors[i].p7_config.carrier_cfg.num_tx_ant) {
      case 1:
        codebook_config = pmi_codebook_one_port{};
        break;
      case 2:
        codebook_config = pmi_codebook_two_port{};
        break;
      case 4:
        codebook_config =
            pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::two_one, pmi_codebook_typeI_mode::one};
        break;
      case 8:
        codebook_config =
            pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::four_one, pmi_codebook_typeI_mode::one};
        break;
      default:
        report_fatal_error(
            "Unsupported {} antenna ports in sector {}", fapi_cfg.sectors[i].p7_config.carrier_cfg.num_tx_ant, i);
    }

    fapi_adaptor::phy_fapi_p7_sector_fastpath_adaptor_dependencies p7_dependencies = {
        .logger               = logger,
        .dl_processor_pool    = upper.get_downlink_processor_pool(),
        .dl_rg_pool           = upper.get_downlink_resource_grid_pool(),
        .dl_pdu_validator     = upper.get_downlink_pdu_validator(),
        .ul_request_processor = upper.get_uplink_request_processor(),
        .ul_pdu_repository    = upper.get_uplink_pdu_slot_repository(),
        .ul_pdu_validator     = upper.get_uplink_pdu_validator(),
        .pm_repo              = std::move(std::get<std::unique_ptr<fapi_adaptor::precoding_matrix_repository>>(
            fapi_adaptor::generate_precoding_matrix_tables(codebook_config, i))),
        .part2_repo           = std::move(std::get<std::unique_ptr<fapi_adaptor::uci_part2_correspondence_repository>>(
            fapi_adaptor::generate_uci_part2_correspondence(1)))};

    out_dependencies.sectors.push_back(fapi_adaptor::phy_fapi_sector_fastpath_adaptor_dependencies{
        .p5_dependencies = p5_dependencies, .p7_dependencies = std::move(p7_dependencies)});
  }

  return out_dependencies;
}

std::unique_ptr<o_du_low> ocudu::odu::make_o_du_low(const o_du_low_config& config, o_du_low_dependencies& o_du_low_deps)
{
  du_low_dependencies& du_low_deps = o_du_low_deps.du_low_deps;

  auto du_lo = make_du_low(config.du_low_cfg, std::move(du_low_deps));
  report_error_if_not(du_lo != nullptr, "Unable to create DU low.");

  auto fapi = fapi_adaptor::create_phy_fapi_fastpath_adaptor_factory()->create(
      config.fapi_cfg,
      generate_fapi_fastpath_adaptor_dependencies(*du_lo, config.fapi_cfg, o_du_low_deps.fapi_p5_executor));
  report_error_if_not(fapi != nullptr, "Unable to create FAPI-PHY adpators.");

  auto o_du_lo = std::make_unique<o_du_low_impl>(std::move(du_lo), std::move(fapi), config.fapi_cfg.sectors.size());
  report_error_if_not(o_du_lo != nullptr, "Unable to create O-DU low.");

  return o_du_lo;
}
