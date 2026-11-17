// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "o_du_high_unit_factory.h"
#include "apps/helpers/e2/e2_metric_connector_manager.h"
#include "apps/helpers/metrics/metrics_helpers.h"
#include "apps/services/metrics/metrics_channel.h"
#include "du_high/commands/du_high_cmdline_commands.h"
#include "du_high/commands/du_high_remote_commands.h"
#include "du_high/du_high_config_translators.h"
#include "du_high/metrics/du_high_rlc_metrics.h"
#include "du_high/metrics/du_high_rlc_metrics_consumers.h"
#include "du_high/metrics/du_high_rlc_metrics_producer.h"
#include "du_high/metrics/du_metrics.h"
#include "du_high/metrics/du_metrics_consumers.h"
#include "du_high/metrics/du_metrics_producer.h"
#include "e2/o_du_high_e2_config_translators.h"
#include "o_du_high_unit_config.h"
#include "ocudu/du/du_high/du_high.h"
#include "ocudu/du/du_high/du_high_configuration.h"
#include "ocudu/du/du_high/o_du_high_config.h"
#include "ocudu/du/du_high/o_du_high_factory.h"
#include "ocudu/du/du_update_config_helpers.h"
#include "ocudu/e2/e2_du_metrics_connector.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/scheduler/config/pucch_resource_generator.h"

using namespace ocudu;

void ocudu::announce_du_high_cells(const du_high_unit_config& du_high_unit_cfg)
{
  ocudulog::basic_logger& logger = ocudulog::fetch_basic_logger("GNB", false);

  // Generate DU cells.
  auto cells = generate_du_cell_config(du_high_unit_cfg);

  for (const auto& cell : cells) {
    fmt::print("Cell pci={}, bw={} MHz, {}T{}R, dl_arfcn={} (n{}), dl_freq={} MHz, dl_ssb_arfcn={}, ul_freq={} MHz\n",
               cell.ran.pci,
               bs_channel_bandwidth_to_MHz(cell.ran.dl_carrier.carrier_bw),
               cell.ran.dl_carrier.nof_ant,
               cell.ran.ul_carrier.nof_ant,
               cell.ran.dl_carrier.arfcn_f_ref,
               to_underlying(cell.ran.dl_carrier.band),
               band_helper::nr_arfcn_to_freq(cell.ran.dl_carrier.arfcn_f_ref) / 1e6,
               cell.ran.dl_cfg_common.freq_info_dl.absolute_frequency_ssb,
               band_helper::nr_arfcn_to_freq(cell.ran.ul_carrier.arfcn_f_ref) / 1e6);

    const auto ss0_idx = cell.ran.dl_cfg_common.init_dl_bwp.pdcch_common.get_searchspace0();
    ocudu_assert(ss0_idx.has_value(), "SearchSpace#0 not found in common SearchSpace list");
    const auto cs0_idx = cell.ran.dl_cfg_common.init_dl_bwp.pdcch_common.get_coreset0();
    ocudu_assert(cs0_idx.has_value(), "CORESET#0 index not found in common PDCCH configuration");

    logger.info(
        "SSB derived parameters for cell: {}, band: {}, dl_arfcn:{}, nof_crbs: {} scs:{}, ssb_scs:{}:\n\t - SSB offset "
        "pointA:{} \n\t - k_SSB:{} \n\t - SSB arfcn:{} \n\t - Coreset index:{} \n\t - Searchspace index:{}",
        cell.ran.pci,
        fmt::underlying(cell.ran.dl_carrier.band),
        cell.ran.dl_carrier.arfcn_f_ref,
        cell.ran.dl_cfg_common.init_dl_bwp.generic_params.crbs.length(),
        to_string(cell.ran.dl_cfg_common.init_dl_bwp.generic_params.scs),
        to_string(cell.ran.ssb_cfg.scs),
        cell.ran.ssb_cfg.offset_to_point_A.value(),
        cell.ran.ssb_cfg.k_ssb.value(),
        cell.ran.dl_cfg_common.freq_info_dl.absolute_frequency_ssb,
        cs0_idx.value(),
        ss0_idx.value());
  }

  fmt::print("\n");
}

/// Returns true if the DU metrics are enabled, i.e. scheduler or MAC metrics are enabled or there is somewhere to
/// report the F1AP metrics.
static bool is_du_metrics_enabled(const du_high_unit_metrics_config& metrics)
{
  return metrics.layers_cfg.enable_mac || metrics.layers_cfg.enable_scheduler ||
         metrics.common_metrics_cfg.enable_json_metrics || metrics.common_metrics_cfg.enable_log_metrics;
}

/// Adds the RLC metrics consumers to the given channel.
static void add_rlc_metrics_consumers(app_services::metrics_channel<rlc_metrics_impl>& rlc_metrics_channel,
                                      app_services::remote_server_metrics_gateway*     remote_metrics_gateway,
                                      const o_du_high_unit_config&                     o_du_high_unit_cfg,
                                      e2_du_metrics_notifier&                          e2_notifier)
{
  const app_helpers::metrics_config& metrics_config = o_du_high_unit_cfg.du_high_cfg.config.metrics.common_metrics_cfg;

  if (metrics_config.enable_log_metrics) {
    rlc_metrics_channel.add_consumer(
        std::make_unique<rlc_metrics_consumer_log>(app_helpers::fetch_logger_metrics_log_channel()));
  }

  if (metrics_config.enable_json_metrics) {
    report_error_if_not(remote_metrics_gateway,
                        "Invalid remote server gateway for sending JSON metrics. Check that remote server is enabled");
    rlc_metrics_channel.add_consumer(std::make_unique<rlc_metrics_consumer_json>(*remote_metrics_gateway));
  }

  if (o_du_high_unit_cfg.e2_cfg.base_cfg.enable_unit_e2) {
    rlc_metrics_channel.add_consumer(std::make_unique<rlc_metrics_consumer_e2>(e2_notifier));
  }
}

/// Adds the DU metrics consumers to the given channel and the STDOUT metrics subcommand to the given list.
static void add_du_metrics_consumers(
    app_services::metrics_channel<du_metrics_impl>& du_metrics_channel,
    std::vector<std::unique_ptr<app_services::toggle_stdout_metrics_app_command::metrics_subcommand>>&
                                                 metrics_subcommands,
    app_services::remote_server_metrics_gateway* remote_metrics_gateway,
    const o_du_high_unit_config&                 o_du_high_unit_cfg,
    e2_du_metrics_notifier&                      e2_notifier)
{
  const app_helpers::metrics_config& metrics_config = o_du_high_unit_cfg.du_high_cfg.config.metrics.common_metrics_cfg;

  // Create the consumer for STDOUT. Also create the command for toggle the metrics.
  auto metrics_stdout = std::make_unique<du_metrics_consumer_stdout>();
  metrics_subcommands.push_back(std::make_unique<du_high_metrics_subcommand_stdout>(*metrics_stdout));
  du_metrics_channel.add_consumer(std::move(metrics_stdout));

  if (metrics_config.enable_log_metrics) {
    du_metrics_channel.add_consumer(
        std::make_unique<du_metrics_consumer_log>(app_helpers::fetch_logger_metrics_log_channel()));
  }

  if (metrics_config.enable_json_metrics) {
    report_error_if_not(remote_metrics_gateway,
                        "Invalid remote server gateway for sending JSON metrics. Check that remote server is enabled");
    du_metrics_channel.add_consumer(std::make_unique<du_metrics_consumer_json>(*remote_metrics_gateway));
  }

  // Connect E2 agent to DU Scheduler UE metrics.
  if (o_du_high_unit_cfg.e2_cfg.base_cfg.enable_unit_e2) {
    du_metrics_channel.add_consumer(std::make_unique<du_metrics_consumer_e2>(e2_notifier));
  }
}

namespace {

/// O-DU high metrics: the channels and STDOUT subcommands owned by the unit, plus the notifiers handed to the O-DU
/// high library.
struct o_du_high_metrics {
  app_services::metrics_channel_collection                                                          channels;
  std::vector<std::unique_ptr<app_services::toggle_stdout_metrics_app_command::metrics_subcommand>> metrics_subcommands;
  odu::du_metrics_notifier* du_notifier  = nullptr;
  rlc_metrics_notifier*     rlc_notifier = nullptr;
};

} // namespace

/// Builds the O-DU high metrics channels, consumers and producers.
static o_du_high_metrics build_o_du_high_metrics(const o_du_high_unit_config&       o_du_high_unit_cfg,
                                                 const o_du_high_unit_dependencies& dependencies)
{
  const du_high_unit_config& du_hi_cfg   = o_du_high_unit_cfg.du_high_cfg.config;
  e2_du_metrics_notifier&    e2_notifier = dependencies.e2_metric_connectors.get_e2_metric_notifier(0);

  o_du_high_metrics metrics;

  if (is_du_metrics_enabled(du_hi_cfg.metrics)) {
    auto& channel = app_services::make_channel<du_metrics_impl>(metrics.channels);
    add_du_metrics_consumers(
        channel, metrics.metrics_subcommands, dependencies.remote_metrics_gateway, o_du_high_unit_cfg, e2_notifier);
    metrics.du_notifier = &channel.emplace_producer<du_metrics_producer_impl>(channel);
  }

  if (du_hi_cfg.metrics.layers_cfg.enable_rlc) {
    auto& channel = app_services::make_channel<rlc_metrics_impl>(metrics.channels);
    add_rlc_metrics_consumers(channel, dependencies.remote_metrics_gateway, o_du_high_unit_cfg, e2_notifier);
    metrics.rlc_notifier = &channel.emplace_producer<rlc_metrics_producer_impl>(channel);
  }

  return metrics;
}

/// Builds the O-DU high library dependencies from the unit dependencies.
static odu::o_du_high_dependencies build_o_du_high_dependencies(const o_du_high_unit_dependencies& dependencies,
                                                                odu::du_metrics_notifier*          du_notifier,
                                                                rlc_metrics_notifier*              rlc_notifier,
                                                                bool                               is_e2_enabled)
{
  return odu::o_du_high_dependencies{
      .sectors            = dependencies.sectors,
      .du_hi              = odu::du_high_dependencies{.exec_mapper             = &dependencies.execution_mapper,
                                                      .f1c_client              = &dependencies.f1c_client_handler,
                                                      .f1u_teid_allocator      = &dependencies.f1u_teid_allocator,
                                                      .f1u_gw                  = &dependencies.f1u_gw,
                                                      .phy_adapter             = nullptr,
                                                      .timer_ctrl              = &dependencies.timer_ctrl,
                                                      .du_notifier             = du_notifier,
                                                      .rlc_metrics_notif       = rlc_notifier,
                                                      .mac_p                   = &dependencies.mac_p,
                                                      .rlc_p                   = &dependencies.rlc_p,
                                                      .f1_setup_notifier       = nullptr,
                                                      .ntn_doppler_handler     = dependencies.ntn_doppler_handler,
                                                      .phy_cell_op_controllers = {}},
      .e2_client          = is_e2_enabled ? &dependencies.e2_client_handler : nullptr,
      .e2_du_metric_iface = is_e2_enabled ? &dependencies.e2_metric_connectors.get_e2_metrics_interface(0) : nullptr};
}

/// Creates the E2AP configuration if it is enabled.
static e2ap_config create_e2ap_config(const o_du_high_unit_config& o_du_high_unit_cfg)
{
  if (!o_du_high_unit_cfg.e2_cfg.base_cfg.enable_unit_e2) {
    return e2ap_config{};
  }

  const du_high_unit_config& du_high_unit_cfg = o_du_high_unit_cfg.du_high_cfg.config;

  return generate_e2_config(o_du_high_unit_cfg.e2_cfg,
                            du_high_unit_cfg.gnb_id,
                            du_high_unit_cfg.cells_cfg.front().cell.plmn,
                            du_high_unit_cfg.gnb_du_id);
}

/// Creates the application unit commands, taking ownership of the given STDOUT metrics subcommands.
static application_unit_commands create_application_unit_commands(
    odu::o_du_high& o_du_hi,
    std::vector<std::unique_ptr<app_services::toggle_stdout_metrics_app_command::metrics_subcommand>>
        metrics_subcommands)
{
  application_unit_commands commands;

  odu::du_configurator& configurator = o_du_hi.get_du_high().get_du_configurator();

  commands.cmdline.metrics_subcommands = std::move(metrics_subcommands);
  commands.remote.push_back(std::make_unique<ssb_modify_remote_command>(configurator));
  commands.remote.push_back(std::make_unique<rrm_policy_ratio_remote_command>(configurator));
  commands.remote.push_back(std::make_unique<sib_update_remote_command>(configurator));

  return commands;
}

/// \brief Validates the derived parameters of the given DU cells.
///
/// Reports through the given logger and STDOUT when the PRACH opportunities overlap with the PUCCH resources.
static void validate_derived_du_params(span<const odu::du_cell_config> cells, ocudulog::basic_logger& logger)
{
  for (const auto& cell_cfg : cells) {
    const rach_config_common& rach_cfg = cell_cfg.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common.value();

    frequency_range freq_range = band_helper::get_freq_range(cell_cfg.ran.dl_carrier.band);
    duplex_mode     dplx_mode  = band_helper::get_duplex_mode(cell_cfg.ran.dl_carrier.band);

    const auto prach_cfg = prach_configuration_get(freq_range, dplx_mode, rach_cfg.rach_cfg_generic.prach_config_index);

    const unsigned bwp_size                 = cell_cfg.ran.ul_cfg_common.init_ul_bwp.generic_params.crbs.length();
    const unsigned dedicated_pucch_rb_start = config_helpers::get_dedicated_pucch_rb_start(
        cell_cfg.ran.init_bwp.pucch.resources,
        cell_cfg.ran.ul_cfg_common.init_ul_bwp.pucch_cfg_common->pucch_resource_common,
        bwp_size);
    prb_interval prb_interval_no_pucch = config_helpers::find_largest_prb_interval_without_pucch(
        cell_cfg.ran.init_bwp.pucch.resources, bwp_size, dedicated_pucch_rb_start);

    // This is to preserve a guardband between the PUCCH and PRACH.
    const unsigned pucch_to_prach_guardband = is_long_preamble(prach_cfg.format) ? 0U : 3U;

    if (rach_cfg.rach_cfg_generic.msg1_frequency_start < prb_interval_no_pucch.start() + pucch_to_prach_guardband) {
      fmt::print("Warning: With the given prach_frequency_start={}, the PRACH opportunities overlap with the PUCCH "
                 "resources/guardband in prbs=[0, {}). Some interference between PUCCH and PRACH should be "
                 "expected\n",
                 rach_cfg.rach_cfg_generic.msg1_frequency_start,
                 prb_interval_no_pucch.start() + pucch_to_prach_guardband);

      logger.warning(
          "With the given prach_frequency_start={}, the PRACH opportunities overlap with the PUCCH resources/guardband "
          "in prbs=[0, {}). Some interference between PUCCH and PRACH should be expected",
          rach_cfg.rach_cfg_generic.msg1_frequency_start,
          prb_interval_no_pucch.start() + pucch_to_prach_guardband);
    }
  }
}

o_du_high_unit ocudu::make_o_du_high_unit(const o_du_high_unit_config&       o_du_high_unit_cfg,
                                          const o_du_high_unit_dependencies& dependencies)
{
  // Generate the DU high configuration and validate its derived parameters.
  odu::du_high_configuration du_hi_cfg = generate_du_high_config(o_du_high_unit_cfg.du_high_cfg.config);
  validate_derived_du_params(du_hi_cfg.ran.cells, dependencies.du_logger);

  // Create the DU high metrics.
  o_du_high_metrics metrics = build_o_du_high_metrics(o_du_high_unit_cfg, dependencies);

  // Create the O-DU high.
  std::unique_ptr<odu::o_du_high> o_du_hi = odu::make_o_du_high(
      odu::o_du_high_config{.du_hi              = std::move(du_hi_cfg),
                            .e2ap_cfg           = create_e2ap_config(o_du_high_unit_cfg),
                            .is_rt_mode_enabled = o_du_high_unit_cfg.fapi_cfg.is_rt_mode_enabled},
      build_o_du_high_dependencies(
          dependencies, metrics.du_notifier, metrics.rlc_notifier, o_du_high_unit_cfg.e2_cfg.base_cfg.enable_unit_e2));
  report_error_if_not(o_du_hi, "Invalid O-DU high");

  // Build the application unit commands.
  application_unit_commands commands =
      create_application_unit_commands(*o_du_hi, std::move(metrics.metrics_subcommands));

  // Create the O-DU high unit and return it.
  return o_du_high_unit{
      .o_du_hi = std::move(o_du_hi), .commands = std::move(commands), .metrics = std::move(metrics.channels)};
}
