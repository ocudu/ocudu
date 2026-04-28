// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "flexible_o_du_factory.h"
#include "apps/helpers/e2/e2_metric_connector_manager.h"
#include "apps/services/worker_manager/worker_manager.h"
#include "apps/units/flexible_o_du/flexible_o_du_commands.h"
#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config_translators.h"
#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_factory.h"
#include "apps/units/flexible_o_du/o_du_low/o_du_low_unit_factory.h"
#include "commands/ntn_config_update_remote_command_factory.h"
#include "flexible_o_du_impl.h"
#include "flexible_o_du_ntn_configuration_manager_factory.h"
#include "metrics/flexible_o_du_metrics_builder.h"
#include "ocudu/du/du_high/du_high.h"
#include "ocudu/du/du_high/du_high_clock_controller.h"
#include "ocudu/du/o_du_factory.h"
#include "ocudu/e2/e2_du_metrics_connector.h"
#include "ocudu/fapi_adaptor/mac/mac_fapi_fastpath_adaptor.h"
#include "ocudu/fapi_adaptor/mac/mac_fapi_sector_fastpath_adaptor.h"
#include "ocudu/fapi_adaptor/mac/p5/mac_fapi_p5_sector_fastpath_adaptor.h"
#include "ocudu/fapi_adaptor/mac/p7/mac_fapi_p7_sector_fastpath_adaptor.h"
#include "ocudu/fapi_adaptor/phy/p5/phy_fapi_p5_sector_adaptor.h"
#include "ocudu/fapi_adaptor/phy/p7/phy_fapi_p7_sector_fastpath_adaptor.h"
#include "ocudu/fapi_adaptor/phy/phy_fapi_fastpath_adaptor.h"
#include "ocudu/fapi_adaptor/phy/phy_fapi_sector_fastpath_adaptor.h"
#include "ocudu/ntn/ntn_configuration_manager_config.h"

using namespace ocudu;

static fapi::carrier_config generate_carrier_config_tlv(const odu::du_cell_config& du_cell)
{
  const subcarrier_spacing scs_common       = du_cell.ran.dl_cfg_common.init_dl_bwp.generic_params.scs;
  unsigned                 grid_size_bw_prb = band_helper::get_n_rbs_from_bw(
      du_cell.ran.dl_carrier.carrier_bw, scs_common, band_helper::get_freq_range(du_cell.ran.dl_carrier.band));

  fapi::carrier_config fapi_config = {};

  fapi_config.dl_grid_size = grid_size_bw_prb;
  fapi_config.ul_grid_size = grid_size_bw_prb;

  // Number of transmit and receive antenna ports.
  fapi_config.num_tx_ant = du_cell.ran.dl_carrier.nof_ant;
  fapi_config.num_rx_ant = du_cell.ran.ul_carrier.nof_ant;

  return fapi_config;
}

static o_du_low_unit_config generate_o_du_low_config(const du_low_unit_config&            du_low_unit_cfg,
                                                     float                                dBFS_calibration_value,
                                                     span<const odu::du_cell_config>      cells,
                                                     span<const du_high_unit_cell_config> du_hi_cells)
{
  o_du_low_unit_config odu_low_cfg = {du_low_unit_cfg, {}, {}};

  for (unsigned i = 0, e = cells.size(); i != e; ++i) {
    const auto&              cell       = cells[i];
    const auto&              du_hi_cell = du_hi_cells[i];
    const subcarrier_spacing scs_common = cell.ran.dl_cfg_common.init_dl_bwp.generic_params.scs;

    fapi_adaptor::phy_fapi_p5_sector_fastpath_adaptor_config p5_cfg{.sector_id = i};

    fapi_adaptor::phy_fapi_p7_sector_fastpath_adaptor_config p7_cfg = {
        .sector_id                     = i,
        .nof_slots_request_headroom    = du_low_unit_cfg.expert_phy_cfg.nof_slots_request_headroom,
        .allow_request_on_empty_ul_tti = du_low_unit_cfg.expert_phy_cfg.allow_request_on_empty_uplink_slot,
        .scs                           = scs_common,
        .scs_common                    = scs_common,
        .carrier_cfg                   = generate_carrier_config_tlv(cell),
        .prach_cfg                     = *cell.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common,
        .prach_ports                   = du_hi_cell.cell.prach_cfg.ports,
        .dBFS_calibration_value        = dBFS_calibration_value};

    odu_low_cfg.fapi_cfg.sectors.push_back({.p5_config = p5_cfg, .p7_config = p7_cfg});

    report_error_if_not(cell.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common,
                        "RACH configuration for the cell is not present");

    auto&   du_low_cell    = odu_low_cfg.cells.emplace_back();
    nr_band band           = cell.ran.dl_carrier.band;
    du_low_cell.duplex     = band_helper::get_duplex_mode(band);
    du_low_cell.freq_range = band_helper::get_freq_range(band);
    du_low_cell.bw_rb =
        band_helper::get_n_rbs_from_bw(cell.ran.dl_carrier.carrier_bw, scs_common, du_low_cell.freq_range);
    du_low_cell.nof_rx_antennas = cell.ran.ul_carrier.nof_ant;
    du_low_cell.nof_tx_antennas = cell.ran.dl_carrier.nof_ant;
    du_low_cell.prach_ports     = du_hi_cell.cell.prach_cfg.ports;
    du_low_cell.scs_common      = scs_common;
    du_low_cell.prach_config_index =
        cell.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index;
    du_low_cell.max_puschs_per_slot  = du_hi_cell.cell.pusch_cfg.max_puschs_per_slot;
    du_low_cell.pusch_max_nof_layers = cell.ran.init_bwp.pusch.max_nof_layers;
    du_low_cell.tdd_pattern          = cell.ran.tdd_cfg;
  }

  return odu_low_cfg;
}

static flexible_o_du_ru_config
generate_o_du_ru_config(span<const odu::du_cell_config> cells, unsigned max_processing_delay, unsigned prach_nof_ports)
{
  flexible_o_du_ru_config out_cfg;
  out_cfg.prach_nof_ports      = prach_nof_ports;
  out_cfg.max_processing_delay = max_processing_delay;

  for (const auto& cell : cells) {
    auto&                    out_cell   = out_cfg.cells.emplace_back();
    const subcarrier_spacing scs_common = cell.ran.dl_cfg_common.init_dl_bwp.generic_params.scs;
    out_cell.nof_tx_antennas            = cell.ran.dl_carrier.nof_ant;
    out_cell.nof_rx_antennas            = cell.ran.ul_carrier.nof_ant;
    out_cell.scs                        = scs_common;
    out_cell.dl_arfcn                   = cell.ran.dl_carrier.arfcn_f_ref.value();
    out_cell.ul_arfcn                   = cell.ran.ul_carrier.arfcn_f_ref.value();
    out_cell.tdd_config                 = cell.ran.tdd_cfg;
    out_cell.bw                         = cell.ran.dl_carrier.carrier_bw;
    out_cell.freq_range                 = band_helper::get_freq_range(cell.ran.dl_carrier.band);
    out_cell.cp                         = cell.ran.dl_cfg_common.init_dl_bwp.generic_params.cp;
  }

  return out_cfg;
}

/// Converts app-level ntn_config to library-level ntn_assistance_info.
static ocudu_ntn::ntn_assistance_info convert_ntn_config_to_assistance_info(const du_high_unit_cell_ntn_config& cfg)
{
  ocudu_ntn::ntn_assistance_info info = {};

  // SIB19 fields exempt from valuetag.
  info.moving_reference_location = cfg.moving_ref_location;
  info.ephemeris_info            = cfg.ephemeris_info;
  info.ta_info                   = cfg.ta_info;
  info.epoch_time                = cfg.epoch_time;
  info.ntn_ul_sync_validity_dur  = cfg.ntn_ul_sync_validity_dur;

  // SIB19 fields tracked by valuetag.
  info.reference_location    = cfg.reference_location;
  info.distance_threshold    = cfg.distance_threshold;
  info.t_service             = cfg.t_service;
  info.cell_specific_koffset = cfg.cell_specific_koffset;
  info.k_mac                 = cfg.k_mac;
  info.polarization          = cfg.polarization;
  info.ta_report             = cfg.ta_report;

  // SIB19 neighbor cells.
  for (const auto& ncell : cfg.ncells) {
    info.ncells.push_back(ncell);
  }

  // SIB19 tracked fields (R18 extensions).
  info.sat_switch_with_resync = cfg.sat_switch_with_resync;

  // Metadata fields.
  info.epoch_timestamp      = cfg.epoch_timestamp;
  info.epoch_sfn_offset     = cfg.epoch_sfn_offset;
  info.use_state_vector     = cfg.use_state_vector;
  info.feeder_link_info     = cfg.feeder_link_info;
  info.ntn_gateway_location = cfg.ntn_gateway_location;

  return info;
}

static ocudu_ntn::ntn_configuration_manager_config
generate_ntn_configuration_manager_config(const gnb_id_t& gnb_id, span<const du_high_unit_cell_config> du_hi_cells)
{
  ocudu_ntn::ntn_configuration_manager_config out_cfg = {};
  for (const auto& cell : du_hi_cells) {
    const auto& cell_cfg = cell.cell;
    if (cell_cfg.ntn_cfg.has_value()) {
      auto&                      out_cell = out_cfg.cells.emplace_back();
      expected<plmn_identity>    plmn     = plmn_identity::parse(cell_cfg.plmn);
      expected<nr_cell_identity> nci      = nr_cell_identity::create(gnb_id, cell_cfg.sector_id.value());
      if (not plmn.has_value()) {
        report_error("Invalid PLMN: {}", cell_cfg.plmn);
      }
      if (not nci.has_value()) {
        report_error("Invalid NR-NCI");
      }
      out_cell.sector_id       = cell_cfg.sector_id;
      out_cell.nr_cgi.plmn_id  = plmn.value();
      out_cell.nr_cgi.nci      = nci.value();
      out_cell.assistance_info = convert_ntn_config_to_assistance_info(*cell_cfg.ntn_cfg);

      // SIB19 Scheduling info.
      const auto& sib_cfg = cell_cfg.sib_cfg;
      for (unsigned i = 0, ie = sib_cfg.si_sched_info.size(); i != ie; ++i) {
        const auto& si_msg = sib_cfg.si_sched_info[i];
        for (unsigned j = 0, je = si_msg.sib_mapping_info.size(); j != je; ++j) {
          if (si_msg.sib_mapping_info[j] == 19) {
            out_cell.si_msg_idx          = i;
            out_cell.si_period_rf        = si_msg.si_period_rf;
            out_cell.si_window_len_slots = sib_cfg.si_window_len_slots;
            out_cell.si_window_position  = si_msg.si_window_position.value();
          }
        }
      }
    }
  }
  return out_cfg;
}

o_du_unit flexible_o_du_factory::create_flexible_o_du(const o_du_unit_dependencies& dependencies)
{
  o_du_unit      o_du;
  const unsigned nof_cells  = config.odu_high_cfg.du_high_cfg.config.cells_cfg.size();
  o_du.e2_metric_connectors = std::make_unique<
      e2_metric_connector_manager<e2_du_metrics_connector, e2_du_metrics_notifier, e2_du_metrics_interface>>(nof_cells);

  const du_high_unit_config& du_hi = config.odu_high_cfg.du_high_cfg.config;
  const du_low_unit_config&  du_lo = config.du_low_cfg;

  auto du_cells = generate_du_cell_config(du_hi);

  std::vector<pci_t> pci_cell_mapper;

  for (const auto& cell : du_cells) {
    pci_cell_mapper.push_back(cell.ran.pci);
  }

  std::chrono::nanoseconds symbol_duration(
      static_cast<int64_t>(1e6 / (get_nsymb_per_slot(cyclic_prefix::NORMAL) *
                                  get_nof_slots_per_subframe(du_hi.cells_cfg.front().cell.common_scs))));

  std::vector<std::unique_ptr<app_services::toggle_stdout_metrics_app_command::metrics_subcommand>>
      ru_metrics_subcommands;
  // Create flexible O-DU metrics configuration.
  flexible_o_du_metrics_notifier* flexible_odu_metrics_notifier =
      build_flexible_o_du_metrics_config(o_du.metrics,
                                         ru_metrics_subcommands,
                                         *dependencies.metrics_notifier,
                                         dependencies.remote_metrics_gateway,
                                         config.ru_cfg.config,
                                         std::move(pci_cell_mapper),
                                         symbol_duration);

  // Create flexible O-DU implementation.
  auto du_impl = std::make_unique<flexible_o_du_impl>(nof_cells, flexible_odu_metrics_notifier);

  o_du_low_unit_config odu_low_cfg =
      generate_o_du_low_config(du_lo, config.ru_cfg.dBFS_calibration_value, du_cells, du_hi.cells_cfg);
  o_du_low_unit_dependencies odu_low_dependencies = {.rg_gateway = du_impl->get_upper_ru_dl_rg_adapter(),
                                                     .rx_symbol_request_notifier =
                                                         du_impl->get_upper_ru_ul_request_adapter(),
                                                     .workers = dependencies.workers->get_du_low_executor_mapper(),
                                                     .fapi_p5_executor = dependencies.workers->get_cmd_line_executor()};
  o_du_low_unit_factory      odu_low_factory(du_lo.hal_config);
  auto                       odu_lo_unit = odu_low_factory.create(odu_low_cfg, odu_low_dependencies);

  std::for_each(odu_lo_unit.metrics.begin(), odu_lo_unit.metrics.end(), [&](auto& e) {
    o_du.metrics.emplace_back(std::move(e));
  });

  o_du_high_unit_dependencies odu_hi_unit_dependencies = {dependencies.workers->get_du_high_executor_mapper(),
                                                          *dependencies.f1c_client_handler,
                                                          *dependencies.f1u_teid_allocator,
                                                          *dependencies.f1u_gw,
                                                          *dependencies.timer_ctrl,
                                                          *dependencies.mac_p,
                                                          *dependencies.rlc_p,
                                                          *dependencies.e2_client_handler,
                                                          *(o_du.e2_metric_connectors),
                                                          *dependencies.metrics_notifier,
                                                          dependencies.remote_metrics_gateway,
                                                          {}};

  // Adjust the dependencies.
  for (unsigned i = 0, e = du_cells.size(); i != e; ++i) {
    odu::o_du_high_sector_dependencies sector_dependencies = {
        .p5_gateway = odu_lo_unit.o_du_lo->get_phy_fapi_fastpath_adaptor()
                          .get_sector_adaptor(i)
                          .get_p5_sector_adaptor()
                          .get_p5_requests_gateway(),
        .p7_gateway = odu_lo_unit.o_du_lo->get_phy_fapi_fastpath_adaptor()
                          .get_sector_adaptor(i)
                          .get_p7_sector_adaptor()
                          .get_p7_requests_gateway(),
        .p7_last_req_notifier = odu_lo_unit.o_du_lo->get_phy_fapi_fastpath_adaptor()
                                    .get_sector_adaptor(i)
                                    .get_p7_sector_adaptor()
                                    .get_p7_last_request_notifier(),
        .timer_mng          = dependencies.timer_ctrl->get_timer_manager(),
        .fapi_ctrl_executor = dependencies.workers->get_cmd_line_executor(),
        .mac_ctrl_executor  = dependencies.workers->get_du_high_executor_mapper().du_control_executor(),
        .fapi_logger        = *dependencies.fapi_logger,
    };

    odu_hi_unit_dependencies.o_du_hi_dependencies.sectors.push_back(sector_dependencies);
  }

  o_du_high_unit odu_hi_unit = make_o_du_high_unit(config.odu_high_cfg, std::move(odu_hi_unit_dependencies));

  // Connect the adaptors.
  for (unsigned i = 0, e = du_cells.size(); i != e; ++i) {
    auto& p5_phy_adaptor =
        odu_lo_unit.o_du_lo->get_phy_fapi_fastpath_adaptor().get_sector_adaptor(i).get_p5_sector_adaptor();
    auto& p5_mac_adaptor =
        odu_hi_unit.o_du_hi->get_mac_fapi_fastpath_adaptor().get_sector_adaptor(i).get_p5_sector_fastpath_adaptor();

    p5_phy_adaptor.set_p5_responses_notifier(p5_mac_adaptor.get_p5_responses_notifier());
    p5_phy_adaptor.set_error_indication_notifier(p5_mac_adaptor.get_error_indication_notifier());

    auto& p7_phy_adaptor =
        odu_lo_unit.o_du_lo->get_phy_fapi_fastpath_adaptor().get_sector_adaptor(i).get_p7_sector_adaptor();
    auto& p7_mac_adaptor =
        odu_hi_unit.o_du_hi->get_mac_fapi_fastpath_adaptor().get_sector_adaptor(i).get_p7_sector_adaptor();

    // Connect P7 O-DU low with O-DU high.
    p7_phy_adaptor.set_p7_slot_indication_notifier(p7_mac_adaptor.get_p7_slot_indication_notifier());
    p7_phy_adaptor.set_error_indication_notifier(p7_mac_adaptor.get_error_indication_notifier());
    p7_phy_adaptor.set_p7_indications_notifier(p7_mac_adaptor.get_p7_indications_notifier());
  }

  std::for_each(odu_hi_unit.metrics.begin(), odu_hi_unit.metrics.end(), [&](auto& e) {
    o_du.metrics.emplace_back(std::move(e));
  });

  // Manage commands.
  o_du.commands = std::move(odu_hi_unit.commands);

  odu::o_du_dependencies odu_dependencies;
  odu_dependencies.odu_hi           = std::move(odu_hi_unit.o_du_hi);
  odu_dependencies.odu_lo           = std::move(odu_lo_unit.o_du_lo);
  odu_dependencies.metrics_notifier = &du_impl->get_o_du_metrics_notifier();

  auto odu_instance = make_o_du(std::move(odu_dependencies));
  report_error_if_not(odu_instance, "Invalid Distributed Unit");

  flexible_o_du_ru_config ru_config = generate_o_du_ru_config(
      du_cells, du_lo.expert_phy_cfg.max_processing_delay_slots, du_hi.cells_cfg.front().cell.prach_cfg.ports.size());
  flexible_o_du_ru_dependencies ru_dependencies{*dependencies.workers,
                                                du_impl->get_upper_ru_ul_adapter(),
                                                du_impl->get_upper_ru_timing_adapter(),
                                                du_impl->get_upper_ru_error_adapter()};

  std::unique_ptr<radio_unit> ru = create_radio_unit(ru_config, ru_dependencies);

  ocudu_assert(ru, "Invalid Radio Unit");

  // Add RU metrics subcommands.
  for (auto& subcmd : ru_metrics_subcommands) {
    o_du.commands.cmdline.metrics_subcommands.emplace_back(std::move(subcmd));
  }

  // Add RU command-line commands.
  o_du.commands.cmdline.commands.push_back(std::make_unique<change_log_level_app_command>());

  // Create the RU gain commands.
  if (auto* controller = ru->get_controller().get_gain_controller()) {
    o_du.commands.cmdline.commands.push_back(std::make_unique<tx_gain_app_command>(*controller));
    o_du.commands.cmdline.commands.push_back(std::make_unique<rx_gain_app_command>(*controller));
  }

  // Create the RU CFO command.
  if (auto* controller = ru->get_controller().get_cfo_controller()) {
    o_du.commands.cmdline.commands.push_back(std::make_unique<cfo_app_command>(*controller));
  }

  // Create the RU transmit time offset command.
  if (auto* controller = ru->get_controller().get_tx_time_offset_controller()) {
    o_du.commands.cmdline.commands.push_back(std::make_unique<tx_time_offset_app_command>(*controller));
  }

  // Create the NTN Configuration Manager if at least one NTN cell is present.
  ocudu_ntn::ntn_configuration_manager_config ntn_manager_config =
      generate_ntn_configuration_manager_config(du_hi.gnb_id, du_hi.cells_cfg);

  if (not ntn_manager_config.cells.empty()) {
    o_du.ntn_configurator_manager =
        create_ntn_configuration_manager(ntn_manager_config,
                                         odu_instance->get_o_du_high().get_du_high().get_du_configurator(),
                                         odu_instance->get_o_du_high().get_du_high().get_subframe_time_mapper(),
                                         ru->get_controller(),
                                         dependencies.timer_ctrl->get_timer_manager(),
                                         dependencies.workers->get_du_high_executor_mapper().du_control_executor());

    if (o_du.ntn_configurator_manager) {
      add_ntn_config_update_remote_command(o_du.commands, *o_du.ntn_configurator_manager);
    }
  }

  // Configure the RU and DU in the dynamic DU.
  du_impl->add_ru(std::move(ru));
  du_impl->add_du(std::move(odu_instance));

  o_du.unit = std::move(du_impl);

  announce_du_high_cells(du_hi);

  return o_du;
}
