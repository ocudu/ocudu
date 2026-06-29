// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/du/du_high/o_du_high_factory.h"
#include "adapters/f1_setup_e2_adapter.h"
#include "o_du_high_impl.h"
#include "ocudu/du/du_high/du_high_clock_controller.h"
#include "ocudu/du/du_high/du_high_factory.h"
#include "ocudu/du/du_high/o_du_high_config.h"
#include "ocudu/e2/e2_du_factory.h"
#include "ocudu/fapi_adaptor/mac/mac_fapi_fastpath_adaptor_factory.h"
#include "ocudu/fapi_adaptor/precoding_matrix_table_generator.h"
#include "ocudu/fapi_adaptor/uci_part2_correspondence_generator.h"
#include "ocudu/ran/band_helper.h"
#include "ocudu/ran/ssb/ssb_mapping.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;
using namespace odu;

static fapi_adaptor::mac_fapi_p5_sector_fastpath_adaptor_dependencies
generate_mac_fapi_p5_sector_adaptor_dependencies(const o_du_high_sector_dependencies& sector_dependencies)
{
  return {.logger             = ocudulog::fetch_basic_logger("FAPI"),
          .p5_gateway         = sector_dependencies.p5_gateway,
          .timers             = sector_dependencies.timer_mng,
          .fapi_ctrl_executor = sector_dependencies.fapi_ctrl_executor,
          .mac_ctrl_executor  = sector_dependencies.mac_ctrl_executor};
}

static fapi_adaptor::mac_fapi_p5_sector_fastpath_adaptor_config
generate_fapi_p5_cell_config(const du_cell_config& du_cell)
{
  fapi::cell_configuration cell_cfg;
  const subcarrier_spacing scs_common = du_cell.ran.dl_cfg_common.init_dl_bwp.generic_params.scs;

  cell_cfg.scs_common = scs_common;
  cell_cfg.cp         = du_cell.ran.dl_cfg_common.init_dl_bwp.generic_params.cp;

  cell_cfg.duplex = (du_cell.ran.tdd_cfg) ? duplex_mode::TDD : duplex_mode::FDD;
  cell_cfg.pci    = du_cell.ran.pci;

  unsigned grid_size_bw_prb = band_helper::get_n_rbs_from_bw(
      du_cell.ran.dl_carrier.carrier_bw,
      scs_common,
      band_helper::get_freq_range(band_helper::get_band_from_dl_arfcn(du_cell.ran.dl_carrier.arfcn_f_ref)));

  cell_cfg.carrier_cfg.dl_bandwidth = bs_channel_bandwidth_to_MHz(du_cell.ran.dl_carrier.carrier_bw);
  cell_cfg.carrier_cfg.ul_bandwidth = bs_channel_bandwidth_to_MHz(du_cell.ran.ul_carrier.carrier_bw);

  cell_cfg.carrier_cfg.dl_f_ref_arfcn = du_cell.ran.dl_carrier.arfcn_f_ref.value();
  cell_cfg.carrier_cfg.ul_f_ref_arfcn = du_cell.ran.ul_carrier.arfcn_f_ref.value();

  cell_cfg.carrier_cfg.dl_grid_size = grid_size_bw_prb;
  cell_cfg.carrier_cfg.ul_grid_size = grid_size_bw_prb;

  // Number of transmit and receive antenna ports.
  cell_cfg.carrier_cfg.num_tx_ant     = du_cell.ran.dl_carrier.nof_ant;
  cell_cfg.carrier_cfg.num_rx_ant     = du_cell.ran.ul_carrier.nof_ant;
  cell_cfg.carrier_cfg.dmrs_typeA_pos = du_cell.ran.dmrs_typeA_pos;

  cell_cfg.ssb_cfg              = du_cell.ran.ssb_cfg;
  cell_cfg.tdd_ul_dl_cfg_common = du_cell.ran.tdd_cfg;
  report_error_if_not(du_cell.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common, "RACH configuration not present");

  cell_cfg.prach_cfg = *du_cell.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common;

  return {cell_cfg};
}

static fapi_adaptor::mac_fapi_fastpath_adaptor_config
generate_fapi_fastpath_adaptor_config(const o_du_high_config& config)
{
  fapi_adaptor::mac_fapi_fastpath_adaptor_config out_config;

  for (unsigned i = 0, e = config.du_hi.ran.cells.size(); i != e; ++i) {
    const auto&              du_cell    = config.du_hi.ran.cells[i];
    const subcarrier_spacing scs_common = du_cell.ran.dl_cfg_common.init_dl_bwp.generic_params.scs;
    unsigned                 nof_prb    = get_max_Nprb(
        du_cell.ran.dl_carrier.carrier_bw, scs_common, band_helper::get_freq_range(du_cell.ran.dl_carrier.band));
    fapi_adaptor::mac_fapi_p7_sector_fastpath_adaptor_config p7_cfg = {
        .sector_id = i, .cell_nof_prbs = nof_prb, .scs = scs_common};

    out_config.sectors.push_back({.p5_config = generate_fapi_p5_cell_config(du_cell), .p7_config = p7_cfg});
  }

  return out_config;
}

static fapi_adaptor::mac_fapi_p7_sector_fastpath_adaptor_dependencies
generate_mac_fapi_p7_sector_adaptor_dependencies(const o_du_high_sector_dependencies& sector_dependencies,
                                                 const pmi_codebook_config&           codebook_config,
                                                 unsigned                             sector)
{
  return {.p7_gateway           = sector_dependencies.p7_gateway,
          .p7_last_req_notifier = sector_dependencies.p7_last_req_notifier,
          .pm_mapper            = std::move(std::get<std::unique_ptr<fapi_adaptor::precoding_matrix_mapper>>(
              fapi_adaptor::generate_precoding_matrix_tables(codebook_config, sector))),
          .part2_mapper         = std::move(std::get<std::unique_ptr<fapi_adaptor::uci_part2_correspondence_mapper>>(
              fapi_adaptor::generate_uci_part2_correspondence(1))),
          .fapi_logger          = sector_dependencies.fapi_logger};
}

static fapi_adaptor::mac_fapi_fastpath_adaptor_dependencies
generate_fapi_fastpath_adaptor_dependencies(const o_du_high_config& config, o_du_high_dependencies& odu_dependencies)
{
  fapi_adaptor::mac_fapi_fastpath_adaptor_dependencies out_dependencies;

  for (unsigned i = 0, e = config.du_hi.ran.cells.size(); i != e; ++i) {
    pmi_codebook_config codebook_config;
    switch (config.du_hi.ran.cells[i].ran.dl_carrier.nof_ant) {
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
            "Unsupported {} antenna ports in sector {}", config.du_hi.ran.cells[i].ran.dl_carrier.nof_ant, i);
    }

    const auto& sector_dependencies = odu_dependencies.sectors[i];
    out_dependencies.sectors.push_back(
        {.p5_dependencies = generate_mac_fapi_p5_sector_adaptor_dependencies(sector_dependencies),
         .p7_dependencies = generate_mac_fapi_p7_sector_adaptor_dependencies(sector_dependencies, codebook_config, i)});
  }

  return out_dependencies;
}

std::unique_ptr<o_du_high> ocudu::odu::make_o_du_high(const o_du_high_config&  config,
                                                      o_du_high_dependencies&& odu_dependencies)
{
  o_du_high_impl_dependencies dependencies;
  ocudulog::basic_logger*     logger = &ocudulog::fetch_basic_logger("DU");
  dependencies.logger                = logger;
  dependencies.fapi_fastpath_adaptor = fapi_adaptor::create_mac_fapi_fastpath_adaptor_factory()->create(
      generate_fapi_fastpath_adaptor_config(config),
      generate_fapi_fastpath_adaptor_dependencies(config, odu_dependencies));
  dependencies.metrics_notifier = odu_dependencies.du_hi.du_notifier;

  logger->debug("FAPI adaptors created successfully");

  odu::du_high_configuration du_hi_cfg = config.du_hi;

  auto odu = std::make_unique<o_du_high_impl>(config.du_hi.ran.cells.size(), std::move(dependencies));

  // Resolve dependencies for DU-high.
  odu_dependencies.du_hi.phy_adapter = &odu->get_mac_result_notifier();
  odu_dependencies.du_hi.du_notifier = &odu->get_du_metrics_notifier();

  if (!odu_dependencies.e2_client) {
    odu->set_du_high(make_du_high(du_hi_cfg, odu_dependencies.du_hi));
    logger->info("DU created successfully");

    return odu;
  }

  auto collector =
      std::make_unique<e2_node_component_config_collector>(odu_dependencies.du_hi.exec_mapper->du_e2_executor(), 1);
  auto adapter                             = std::make_unique<f1_setup_e2_adapter>(*collector);
  odu_dependencies.du_hi.f1_setup_notifier = adapter.get();

  auto du_hi = make_du_high(du_hi_cfg, odu_dependencies.du_hi);

  auto e2agent = create_e2_du_agent(config.e2ap_config,
                                    *odu_dependencies.e2_client,
                                    odu_dependencies.e2_du_metric_iface,
                                    &du_hi->get_f1ap_ue_id_translator(),
                                    &du_hi->get_du_configurator(),
                                    timer_factory{odu_dependencies.du_hi.timer_ctrl->get_timer_manager(),
                                                  odu_dependencies.du_hi.exec_mapper->du_e2_executor()},
                                    odu_dependencies.du_hi.exec_mapper->du_e2_executor(),
                                    std::move(collector));

  odu->set_e2_components(std::move(e2agent), std::move(adapter));
  odu->set_du_high(std::move(du_hi));
  logger->info("DU created successfully");

  return odu;
}
