// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "apps/helpers/e2/e2_config_translators.h"
#include "apps/helpers/metrics/metrics_helpers.h"
#include "apps/helpers/network/sctp_config_translators.h"
#include "apps/services/app_execution_metrics/executor_metrics_manager.h"
#include "apps/services/app_resource_usage/app_resource_usage.h"
#include "apps/services/application_message_banners.h"
#include "apps/services/application_tracer.h"
#include "apps/services/buffer_pool/buffer_pool_manager.h"
#include "apps/services/cmdline/cmdline_command_dispatcher.h"
#include "apps/services/cmdline/stdout_metrics_command.h"
#include "apps/services/metrics/metrics_manager.h"
#include "apps/services/metrics/metrics_notifier_proxy.h"
#include "apps/services/remote_control/remote_server.h"
#include "apps/services/worker_manager/worker_manager.h"
#include "apps/units/flexible_o_du/flexible_o_du_application_unit.h"
#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config.h"
#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_pcap_factory.h"
#include "apps/units/o_cu_cp/o_cu_cp_application_unit.h"
#include "apps/units/o_cu_cp/o_cu_cp_unit_config.h"
#include "apps/units/o_cu_cp/pcap_factory.h"
#include "apps/units/o_cu_up/o_cu_up_application_unit.h"
#include "apps/units/o_cu_up/o_cu_up_unit_config.h"
#include "apps/units/o_cu_up/pcap_factory.h"
#include "gnb_appconfig.h"
#include "gnb_appconfig_cli11_schema.h"
#include "gnb_appconfig_translators.h"
#include "gnb_appconfig_validators.h"
#include "gnb_appconfig_yaml_writer.h"
#include "ocudu/adt/scope_exit.h"
#include "ocudu/cu_cp/cu_cp_operation_controller.h"
#include "ocudu/du/du_high/du_high_clock_controller.h"
#include "ocudu/du/du_operation_controller.h"
#include "ocudu/e1ap/gateways/e1_local_connector_factory.h"
#include "ocudu/e2/gateways/e2_network_client_factory.h"
#include "ocudu/f1ap/gateways/f1c_local_connector_factory.h"
#include "ocudu/f1u/local_connector/f1u_local_connector.h"
#include "ocudu/gtpu/gtpu_teid_pool_factory.h"
#include "ocudu/ngap/gateways/n2_connection_client_factory.h"
#include "ocudu/support/backtrace.h"
#include "ocudu/support/config_parsers.h"
#include "ocudu/support/cpu_features.h"
#include "ocudu/support/io/io_broker_factory.h"
#include "ocudu/support/signal_handling.h"
#include "ocudu/support/signal_observer.h"
#include "ocudu/support/sysinfo.h"
#include "ocudu/support/versioning/build_info.h"
#include "ocudu/support/versioning/version.h"
#include "ocudu/xnap/gateways/xnc_network_gateway_factory.h"
#include <atomic>
#ifdef DPDK_FOUND
#include "ocudu/hal/dpdk/dpdk_eal_factory.h"
#endif
// Include ThreadSanitizer (TSAN) options if thread sanitization is enabled.
// This include is not unused - it helps prevent false alarms from the thread sanitizer.
#include "ocudu/support/tsan_options.h"

using namespace ocudu;

/// \file
/// \brief Application of a co-located gNB with combined distributed unit (DU) and centralized unit (CU).
///
/// This application runs a gNB without the the F1 connection between CU and DU and without the E1 connection
/// between the CU-CP and CU-UP going over a real SCTP connection. However, its does expose the N2 and N3 interface
/// to the AMF and UPF over the standard SCTP ports.
/// The app serves as an example for a all-integrated, small-cell-style gNB.

static std::string config_file;

/// Flag that indicates if the application is running or being shutdown.
static std::atomic<bool> is_app_running = {true};
/// Maximum number of configuration files allowed to be concatenated in the command line.
static constexpr unsigned MAX_CONFIG_FILES = 10;

static void populate_cli11_generic_args(CLI::App& app)
{
  fmt::memory_buffer buffer;
  format_to(std::back_inserter(buffer), "OCUDU 5G gNB version {} ({})", get_version(), get_build_hash());
  app.set_version_flag("-v,--version", ocudu::to_c_str(buffer));
  app.set_config("-c,", config_file, "Read config from file", false)->expected(1, MAX_CONFIG_FILES);
}

/// Function to call when the application is interrupted.
static void interrupt_signal_handler(int signal)
{
  is_app_running = false;
}

static signal_dispatcher cleanup_signal_dispatcher;

/// Function to call when the application is going to be forcefully shutdown.
static void cleanup_signal_handler(int signal)
{
  cleanup_signal_dispatcher.notify_signal(signal);
  ocudulog::fetch_basic_logger("APP").error("Emergency flush of the logger");
  ocudulog::flush();
}

/// Function to call when an error is reported by the application.
static void app_error_report_handler()
{
  ocudulog::fetch_basic_logger("APP").error("Emergency flush of the logger");
  ocudulog::flush();
}

static void initialize_log(const std::string& filename)
{
  ocudulog::sink* log_sink =
      (filename == "stdout") ? ocudulog::create_stdout_sink() : ocudulog::create_file_sink(filename);
  if (log_sink == nullptr) {
    report_error("Could not create application main log sink.\n");
  }
  ocudulog::set_default_sink(*log_sink);
  ocudulog::init();
}

static void register_app_logs(const gnb_appconfig&            gnb_cfg,
                              o_cu_cp_application_unit&       cu_cp_app_unit,
                              o_cu_up_application_unit&       cu_up_app_unit,
                              flexible_o_du_application_unit& du_app_unit)
{
  const logger_appconfig& log_cfg = gnb_cfg.log_cfg;
  // Set log-level of app and all non-layer specific components to app level.
  for (const auto& id : {"ALL", "SCTP-GW", "IO-EPOLL", "UDP-GW", "PCAP", "ASN1"}) {
    auto& logger = ocudulog::fetch_basic_logger(id, false);
    logger.set_level(log_cfg.lib_level);
    logger.set_hex_dump_max_size(log_cfg.hex_max_size);
  }

  auto& app_logger = ocudulog::fetch_basic_logger("GNB", false);
  app_logger.set_level(ocudulog::basic_levels::info);
  app_services::application_message_banners::log_build_info(app_logger);
  app_logger.set_level(log_cfg.all_level);
  app_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  {
    auto& logger = ocudulog::fetch_basic_logger("APP", false);
    logger.set_level(log_cfg.all_level);
    logger.set_hex_dump_max_size(log_cfg.hex_max_size);
  }

  auto& config_logger = ocudulog::fetch_basic_logger("CONFIG", false);
  config_logger.set_level(log_cfg.config_level);
  config_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& e2ap_logger = ocudulog::fetch_basic_logger("E2AP", false);
  e2ap_logger.set_level(log_cfg.e2ap_level);
  e2ap_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  // Metrics log channels.
  const app_helpers::metrics_config& metrics_cfg = gnb_cfg.metrics_cfg.rusage_config.metrics_consumers_cfg;
  app_helpers::initialize_metrics_log_channels(metrics_cfg, log_cfg.hex_max_size);

  // Register units logs.
  cu_cp_app_unit.on_loggers_registration();
  cu_up_app_unit.on_loggers_registration();
  du_app_unit.on_loggers_registration();
}

static void autoderive_slicing_args(du_high_unit_config& du_hi_cfg, cu_cp_unit_config& cu_cp_config)
{
  std::vector<s_nssai_t> du_slices;
  for (const auto& cell_cfg : du_hi_cfg.cells_cfg) {
    for (const auto& slice : cell_cfg.cell.slice_cfg) {
      s_nssai_t nssai{slice_service_type{slice.sst}, slice_differentiator::create(slice.sd).value()};
      if (du_slices.end() == std::find(du_slices.begin(), du_slices.end(), nssai)) {
        du_slices.push_back(nssai);
      }
    }
  }
  // NOTE: A CU-CP can serve more slices than slices configured in the DU cells.
  // [Implementation-defined] Ensure that all slices served by DU cells are part of CU-CP served slices.
  for (const auto& slice : du_slices) {
    if (cu_cp_config.slice_cfg.end() ==
        std::find(cu_cp_config.slice_cfg.begin(), cu_cp_config.slice_cfg.end(), slice)) {
      cu_cp_config.slice_cfg.push_back(slice);
    }
  }
}

static void autoderive_cu_up_parameters_after_parsing(cu_up_unit_config& cu_up_cfg, const cu_cp_unit_config& cu_cp_cfg)
{
  // If no UPF is configured, we set the UPF configuration from the CU-CP AMF configuration.
  if (cu_up_cfg.ngu_cfg.ngu_socket_cfg.empty()) {
    cu_up_unit_ngu_socket_config sock_cfg;
    // If multiple AMF addresses are configured for SCTP multihoming, we use first address from the list.
    sock_cfg.bind_addr = cu_cp_cfg.amf_config.amf.bind_addrs[0];
    cu_up_cfg.ngu_cfg.ngu_socket_cfg.push_back(sock_cfg);
  }
}

int main(int argc, char** argv)
{
  // Set the application error handler.
  set_error_handler(app_error_report_handler);

  static constexpr std::string_view app_name = "gNB";
  app_services::application_message_banners::announce_app_and_version(app_name);

  // Set interrupt and cleanup signal handlers.
  register_interrupt_signal_handler(interrupt_signal_handler);
  register_cleanup_signal_handler(cleanup_signal_handler);

  // Enable backtrace.
  enable_backtrace();

  // Setup and configure config parsing.
  CLI::App app("OCUDU gNB application");
  app.config_formatter(create_yaml_config_parser());
  app.allow_config_extras(CLI::config_extras_mode::error);
  // Fill the generic application arguments to parse.
  populate_cli11_generic_args(app);

  gnb_appconfig gnb_cfg;
  // Configure CLI11 with the gNB application configuration schema.
  configure_cli11_with_gnb_appconfig_schema(app, gnb_cfg);

  auto o_cu_cp_app_unit = create_o_cu_cp_application_unit("gnb");
  o_cu_cp_app_unit->on_parsing_configuration_registration(app);

  auto o_cu_up_app_unit = create_o_cu_up_application_unit("gnb");
  o_cu_up_app_unit->on_parsing_configuration_registration(app);

  auto o_du_app_unit = create_flexible_o_du_application_unit("gnb");
  o_du_app_unit->on_parsing_configuration_registration(app);

  // Set the callback for the app calling all the autoderivation functions.
  app.callback([&app, &gnb_cfg, &o_du_app_unit, &o_cu_cp_app_unit, &o_cu_up_app_unit]() {
    autoderive_gnb_parameters_after_parsing(app, gnb_cfg);

    cu_cp_unit_config& cu_cp_cfg = o_cu_cp_app_unit->get_o_cu_cp_unit_config().cucp_cfg;

    autoderive_slicing_args(o_du_app_unit->get_o_du_high_unit_config().du_high_cfg.config, cu_cp_cfg);
    o_du_app_unit->on_configuration_parameters_autoderivation(app);

    // If test mode is enabled, we auto-enable "no_core" option and generate a amf config with no core.
    if (o_du_app_unit->get_o_du_high_unit_config().du_high_cfg.config.is_testmode_enabled()) {
      cu_cp_cfg.amf_config.no_core = true;
    } else {
      // If no-core or the default supported tas are configured and we are not using testmode, this will set the
      // supported TAs from the DU cell configuration.
      if (cu_cp_cfg.amf_config.no_core || cu_cp_cfg.amf_config.amf.is_default_supported_tas) {
        autoderive_supported_tas_for_amf_from_du_cells(o_du_app_unit->get_o_du_high_unit_config().du_high_cfg.config,
                                                       cu_cp_cfg);
      }
    }

    o_cu_cp_app_unit->on_configuration_parameters_autoderivation(app);
    o_cu_up_app_unit->on_configuration_parameters_autoderivation(app);
    autoderive_cu_up_parameters_after_parsing(o_cu_up_app_unit->get_o_cu_up_unit_config().cu_up_cfg, cu_cp_cfg);
  });

  // Parse arguments.
  CLI11_PARSE(app, argc, argv);

  // Dry run mode, exit.
  if (gnb_cfg.enable_dryrun) {
    return 0;
  }

  if (gnb_cfg.metrics_cfg.rusage_config.metrics_consumers_cfg.enable_json_metrics &&
      !gnb_cfg.remote_control_config.enabled) {
    fmt::println("NOTE: No JSON metrics will be generated as the remote server is disabled");
  }

  // Check the modified configuration.
  if (!validate_appconfig(gnb_cfg) || !o_cu_cp_app_unit->on_configuration_validation() ||
      !o_cu_up_app_unit->on_configuration_validation(not gnb_cfg.trace_cfg.filename.empty()) ||
      !o_du_app_unit->on_configuration_validation() ||
      !validate_plmn_and_tacs(o_du_app_unit->get_o_du_high_unit_config().du_high_cfg.config,
                              o_cu_cp_app_unit->get_o_cu_cp_unit_config().cucp_cfg)) {
    report_error("Invalid configuration detected.\n");
  }

  // Set up logging.
  initialize_log(gnb_cfg.log_cfg.filename);
  auto log_flusher = make_scope_exit([]() { ocudulog::flush(); });
  register_app_logs(gnb_cfg, *o_cu_cp_app_unit, *o_cu_up_app_unit, *o_du_app_unit);

  // Check the metrics and metrics consumers.
  ocudulog::basic_logger& gnb_logger = ocudulog::fetch_basic_logger("GNB");
  bool metrics_enabled = o_cu_cp_app_unit->are_metrics_enabled() || o_cu_up_app_unit->are_metrics_enabled() ||
                         o_du_app_unit->are_metrics_enabled() || gnb_cfg.metrics_cfg.rusage_config.enable_app_usage;

  if (!metrics_enabled && gnb_cfg.metrics_cfg.rusage_config.metrics_consumers_cfg.enabled()) {
    gnb_logger.warning("Logger or JSON metrics output enabled but no metrics will be reported as no layer was enabled");
    fmt::println("Logger or JSON metrics output enabled but no metrics will be reported as no layer was enabled");
  }

  // Log input configuration.
  ocudulog::basic_logger& config_logger = ocudulog::fetch_basic_logger("CONFIG");
  if (config_logger.debug.enabled()) {
    YAML::Node node;
    fill_gnb_appconfig_in_yaml_schema(node, gnb_cfg);
    o_cu_cp_app_unit->dump_config(node);
    o_cu_up_app_unit->dump_config(node);
    o_du_app_unit->dump_config(node);
    config_logger.debug("Input configuration (all values): \n{}", YAML::Dump(node));
  } else {
    config_logger.info("Input configuration (only non-default values): \n{}", app.config_to_str(false, false));
  }

  app_services::application_tracer app_tracer;
  if (not gnb_cfg.trace_cfg.filename.empty()) {
    app_tracer.enable_tracer(gnb_cfg.trace_cfg.filename,
                             gnb_cfg.trace_cfg.max_tracing_events_per_file,
                             gnb_cfg.trace_cfg.nof_tracing_events_after_severe,
                             gnb_logger);
  }

#ifdef DPDK_FOUND
  std::unique_ptr<dpdk::dpdk_eal> eal;
  if (gnb_cfg.hal_config) {
    // Prepend the application name in argv[0] as it is expected by EAL.
    eal = dpdk::create_dpdk_eal(std::string(argv[0]) + " " + gnb_cfg.hal_config->eal_args,
                                ocudulog::fetch_basic_logger("EAL", false));
  }
#endif

  // Buffer pool service.
  app_services::buffer_pool_manager buffer_pool_service(gnb_cfg.buffer_pool_config);

  // Log CPU architecture.
  cpu_architecture_info::get().print_cpu_info(gnb_logger);

  // Check and log included CPU features and check support by current CPU
  if (cpu_supports_included_features()) {
    gnb_logger.debug("Required CPU features: {}", get_cpu_feature_info());
  } else {
    // Quit here until we complete selection of the best matching implementation for the current CPU at runtime.
    gnb_logger.error("The CPU does not support the required CPU features that were configured during compile time: {}",
                     get_cpu_feature_info());
    report_error("The CPU does not support the required CPU features that were configured during compile time: {}\n",
                 get_cpu_feature_info());
  }

  // Check some common causes of performance issues and print a warning if required.
  check_cpu_governor(gnb_logger);
  check_drm_kms_polling(gnb_logger);

  // Setup application timers.
  timer_manager app_timers{1024};

  app_services::metrics_notifier_proxy_impl metrics_notifier_forwarder;

  std::unique_ptr<app_services::remote_server> remote_control_server =
      app_services::create_remote_server(gnb_cfg.remote_control_config);
  app_services::remote_server_metrics_gateway* remote_server_gateway =
      remote_control_server ? remote_control_server->get_metrics_gateway() : nullptr;

  // Instantiate executor metrics service.
  app_services::executor_metrics_service_and_metrics exec_metrics_service = build_executor_metrics_service(
      metrics_notifier_forwarder, app_timers, gnb_cfg.metrics_cfg.executors_metrics_cfg, remote_server_gateway);

  std::vector<app_services::metrics_config> metrics_configs = std::move(exec_metrics_service.metrics);

  // Instantiate worker manager.
  worker_manager_config worker_manager_cfg;
  o_cu_cp_app_unit->fill_worker_manager_config(worker_manager_cfg);
  o_cu_up_app_unit->fill_worker_manager_config(worker_manager_cfg);
  o_du_app_unit->fill_worker_manager_config(worker_manager_cfg);
  fill_gnb_worker_manager_config(worker_manager_cfg, gnb_cfg);
  worker_manager_cfg.app_timers                    = &app_timers;
  worker_manager_cfg.exec_metrics_channel_registry = exec_metrics_service.channel_registry;
  worker_manager workers{worker_manager_cfg};

  // Create IO broker.
  const auto&                main_pool_cpu_mask = gnb_cfg.expert_execution_cfg.affinities.main_pool_cpu_cfg.mask;
  io_broker_config           io_broker_cfg(os_thread_realtime_priority::min() + 5, main_pool_cpu_mask);
  std::unique_ptr<io_broker> epoll_broker = create_io_broker(io_broker_type::epoll, io_broker_cfg);

  // Create a DU-high timer source.
  auto time_ctrl = odu::create_du_high_clock_controller(app_timers, *epoll_broker, workers.get_timer_source_executor());

  // Create layer specific PCAPs.
  // In the gNB app, there is no point in instantiating two pcaps for each node of E1 and F1.
  // We disable one accordingly.
  auto on_pcap_close = make_scope_exit([&gnb_logger]() { gnb_logger.info("PCAP files successfully closed."); });
  o_cu_up_app_unit->get_o_cu_up_unit_config().cu_up_cfg.pcap_cfg.disable_e1_pcaps();
  o_du_app_unit->get_o_du_high_unit_config().du_high_cfg.config.pcaps.disable_f1_pcaps();
  o_cu_cp_dlt_pcaps cu_cp_dlt_pcaps = create_o_cu_cp_dlt_pcap(
      o_cu_cp_app_unit->get_o_cu_cp_unit_config(), workers.get_cu_cp_pcap_executors(), cleanup_signal_dispatcher);
  o_cu_up_dlt_pcaps cu_up_dlt_pcaps = create_o_cu_up_dlt_pcaps(
      o_cu_up_app_unit->get_o_cu_up_unit_config(), workers.get_cu_up_pcap_executors(), cleanup_signal_dispatcher);
  flexible_o_du_pcaps du_pcaps = create_o_du_pcaps(
      o_du_app_unit->get_o_du_high_unit_config(), workers.get_du_pcap_executors(), cleanup_signal_dispatcher);
  auto on_pcap_close_init = make_scope_exit([&gnb_logger]() { gnb_logger.info("Closing PCAP files..."); });

  // Create XN-C GW (TODO cleanup port and PPID args with factory)
  cu_cp_unit_config                              cp_unit_cfg = o_cu_cp_app_unit->get_o_cu_cp_unit_config().cucp_cfg;
  std::unique_ptr<ocucp::xnc_connection_gateway> xnc_gw;
  if (!cp_unit_cfg.xnap_config.connections.empty()) {
    sctp_network_gateway_config xnc_sctp_cfg = {};
    xnc_sctp_cfg.if_name                     = "XN-C";
    xnc_sctp_cfg.non_blocking_mode           = true;
    for (const auto& xnap_cfg : cp_unit_cfg.xnap_config.connections) {
      xnc_sctp_cfg.bind_addresses.insert(
          xnc_sctp_cfg.bind_addresses.end(), xnap_cfg.bind_addrs.begin(), xnap_cfg.bind_addrs.end());
    }

    fill_sctp_network_gateway_config_socket_params(xnc_sctp_cfg, cp_unit_cfg.xnap_config.sctp);
    xnc_sctp_cfg.bind_port = XNAP_PORT;
    xnc_sctp_cfg.ppid      = XNAP_PPID;
    xnc_sctp_gateway_config xnc_server_cfg({xnc_sctp_cfg,
                                            *epoll_broker,
                                            workers.get_cu_cp_executor_mapper().xnc_rx_executor(),
                                            workers.get_cu_cp_executor_mapper().ctrl_executor(),
                                            *cu_cp_dlt_pcaps.xnap});

    xnc_gw = create_xnc_connection_gateway(xnc_server_cfg);
  }

  std::unique_ptr<f1c_local_connector> f1c_gw =
      create_f1c_local_connector(f1c_local_connector_config{*cu_cp_dlt_pcaps.f1ap});
  std::unique_ptr<e1_local_connector> e1_gw =
      create_e1_local_connector(e1_local_connector_config{*cu_cp_dlt_pcaps.e1ap});

  // Create manager of timers for DU, CU-CP and CU-UP, which will be driven by the PHY slot ticks.
  timer_manager*                 cu_timers = &app_timers;
  std::unique_ptr<timer_manager> dummy_timers;
  if (o_du_app_unit->get_o_du_high_unit_config().du_high_cfg.config.is_testmode_enabled()) {
    // In case test mode is enabled, we pass dummy timers to the upper layers.
    dummy_timers = std::make_unique<timer_manager>(256);
    cu_timers    = dummy_timers.get();
  }

  // Create F1-U TEID allocator (CU-UP)
  gtpu_allocator_creation_request f1u_alloc_msg = {
      .max_nof_teids = o_cu_up_app_unit->get_o_cu_up_unit_config().cu_up_cfg.max_nof_ues * MAX_NOF_PDU_SESSIONS,
      .teid_release_linger_time = GTPU_DEFAULT_TEID_RELEASE_LINGER_TIME,
      .timers                   = *cu_timers};
  std::unique_ptr<gtpu_teid_pool> f1u_teid_allocator = create_gtpu_allocator(f1u_alloc_msg);

  // Create F1-U TEID allocator (DU)
  gtpu_allocator_creation_request du_f1u_alloc_msg = {.max_nof_teids            = MAX_NOF_DU_UES * MAX_NOF_DRBS,
                                                      .teid_release_linger_time = GTPU_DEFAULT_TEID_RELEASE_LINGER_TIME,
                                                      .timers                   = time_ctrl->get_timer_manager()};
  std::unique_ptr<gtpu_teid_pool> du_f1u_teid_allocator = create_gtpu_allocator(du_f1u_alloc_msg);

  // Create F1-U connector
  std::unique_ptr<f1u_local_connector> f1u_conn = std::make_unique<f1u_local_connector>();

  // Create app-level resource usage service and metrics.
  auto app_resource_usage_service = app_services::build_app_resource_usage_service(
      metrics_notifier_forwarder, gnb_cfg.metrics_cfg.rusage_config, gnb_logger, remote_server_gateway);

  for (auto& metric : app_resource_usage_service.metrics) {
    metrics_configs.push_back(std::move(metric));
  }

  buffer_pool_service.add_metrics_to_metrics_service(
      metrics_configs, gnb_cfg.buffer_pool_config.metrics_config, metrics_notifier_forwarder, remote_server_gateway);

  // Instantiate E2AP client gateways.
  std::unique_ptr<e2_connection_client> e2_gw_du = create_e2_gateway_client(
      generate_e2_client_gateway_config(o_du_app_unit->get_o_du_high_unit_config().e2_cfg.base_cfg,
                                        *epoll_broker,
                                        workers.get_du_high_executor_mapper().e2_rx_executor(),
                                        *du_pcaps.e2ap,
                                        E2_DU_PPID));
  std::unique_ptr<e2_connection_client> e2_gw_cu_cp = create_e2_gateway_client(
      generate_e2_client_gateway_config(o_cu_cp_app_unit->get_o_cu_cp_unit_config().e2_cfg.base_config,
                                        *epoll_broker,
                                        workers.get_cu_cp_executor_mapper().e2_rx_executor(),
                                        *cu_cp_dlt_pcaps.e2ap,
                                        E2_CP_PPID));
  std::unique_ptr<e2_connection_client> e2_gw_cu_up = create_e2_gateway_client(
      generate_e2_client_gateway_config(o_cu_up_app_unit->get_o_cu_up_unit_config().e2_cfg.base_config,
                                        *epoll_broker,
                                        workers.get_cu_up_executor_mapper().e2_rx_executor(),
                                        *cu_up_dlt_pcaps.e2ap,
                                        E2_UP_PPID));

  // Create O-CU-CP dependencies.
  o_cu_cp_unit_dependencies o_cucp_deps;
  o_cucp_deps.executor_mapper        = &workers.get_cu_cp_executor_mapper();
  o_cucp_deps.timers                 = cu_timers;
  o_cucp_deps.ngap_pcap              = cu_cp_dlt_pcaps.ngap.get();
  o_cucp_deps.broker                 = epoll_broker.get();
  o_cucp_deps.xnc_gw                 = xnc_gw.get();
  o_cucp_deps.metrics_notifier       = &metrics_notifier_forwarder;
  o_cucp_deps.e2_gw                  = e2_gw_cu_cp.get();
  o_cucp_deps.remote_metrics_gateway = remote_server_gateway;

  // Create O-CU-CP.
  auto            o_cucp_unit = o_cu_cp_app_unit->create_o_cu_cp(o_cucp_deps);
  ocucp::o_cu_cp& o_cucp_obj  = *o_cucp_unit.unit;
  for (auto& metric : o_cucp_unit.metrics) {
    metrics_configs.push_back(std::move(metric));
  }

  // Create O-CU-UP dependencies.
  o_cu_up_unit_dependencies o_cuup_unit_deps;
  o_cuup_unit_deps.workers                = &workers;
  o_cuup_unit_deps.e1ap_conn_client       = e1_gw.get();
  o_cuup_unit_deps.f1u_teid_allocator     = f1u_teid_allocator.get();
  o_cuup_unit_deps.f1u_gateway            = f1u_conn->get_f1u_cu_up_gateway();
  o_cuup_unit_deps.gtpu_pcap              = cu_up_dlt_pcaps.n3.get();
  o_cuup_unit_deps.timers                 = cu_timers;
  o_cuup_unit_deps.io_brk                 = epoll_broker.get();
  o_cuup_unit_deps.e2_gw                  = e2_gw_cu_up.get();
  o_cuup_unit_deps.metrics_notifier       = &metrics_notifier_forwarder;
  o_cuup_unit_deps.remote_metrics_gateway = remote_server_gateway;

  // Create O-CU-UP.
  auto            o_cuup_unit = o_cu_up_app_unit->create_o_cu_up_unit(o_cuup_unit_deps);
  ocuup::o_cu_up& o_cuup_obj  = *o_cuup_unit.unit;
  for (auto& metric : o_cuup_unit.metrics) {
    metrics_configs.push_back(std::move(metric));
  }
  // Create O-DU dependencies.
  o_du_unit_dependencies odu_dependencies;
  odu_dependencies.workers                = &workers;
  odu_dependencies.f1c_client_handler     = f1c_gw.get();
  odu_dependencies.f1u_teid_allocator     = du_f1u_teid_allocator.get();
  odu_dependencies.f1u_gw                 = f1u_conn->get_f1u_du_gateway();
  odu_dependencies.timer_ctrl             = time_ctrl.get();
  odu_dependencies.mac_p                  = du_pcaps.mac.get();
  odu_dependencies.rlc_p                  = du_pcaps.rlc.get();
  odu_dependencies.e2_client_handler      = e2_gw_du.get();
  odu_dependencies.metrics_notifier       = &metrics_notifier_forwarder;
  odu_dependencies.remote_metrics_gateway = remote_server_gateway;
  odu_dependencies.fapi_logger            = &ocudulog::fetch_basic_logger("FAPI");

  // Create O-DU.
  auto     o_du_unit = o_du_app_unit->create_flexible_o_du_unit(odu_dependencies);
  odu::du& o_du_obj  = *o_du_unit.unit;
  for (auto& metric : o_du_unit.metrics) {
    metrics_configs.push_back(std::move(metric));
  }

  // Create metrics manager.
  app_services::metrics_manager metrics_mngr(
      ocudulog::fetch_basic_logger("GNB"),
      workers.get_metrics_executor(),
      metrics_configs,
      app_timers,
      std::chrono::milliseconds(gnb_cfg.metrics_cfg.metrics_service_cfg.app_usage_report_period));

  // Connect the forwarder to the metrics manager.
  metrics_notifier_forwarder.connect(metrics_mngr);

  std::vector<std::unique_ptr<app_services::cmdline_command>> commands;
  commands.reserve(o_cucp_unit.commands.cmdline.commands.size() + o_du_unit.commands.cmdline.commands.size());
  for (auto& cmd : o_cucp_unit.commands.cmdline.commands) {
    commands.push_back(std::move(cmd));
  }
  for (auto& cmd : o_du_unit.commands.cmdline.commands) {
    commands.push_back(std::move(cmd));
  }

  // Add the metrics STDOUT command.
  if (std::unique_ptr<app_services::cmdline_command> cmd = app_services::create_stdout_metrics_app_command(
          {{o_du_unit.commands.cmdline.metrics_subcommands}, {o_cucp_unit.commands.cmdline.metrics_subcommands}},
          gnb_cfg.metrics_cfg.autostart_stdout_metrics)) {
    commands.push_back(std::move(cmd));
  }

  // Create console helper object for commands and metrics printing.
  app_services::cmdline_command_dispatcher command_parser(*epoll_broker, workers.get_cmd_line_executor(), commands);

  // Connect E1AP to O-CU-CP.
  e1_gw->attach_cu_cp(o_cucp_obj.get_cu_cp().get_e1_handler());

  if (xnc_gw != nullptr) {
    // Connect XN-C to O-CU-CP and start listening for new XN-C connection requests.
    xnc_gw->attach_cu_cp(o_cucp_obj.get_cu_cp().get_xnc_handler());
  }

  // Start O-CU-CP.
  gnb_logger.info("Starting CU-CP...");
  o_cucp_obj.get_operation_controller().start();
  gnb_logger.info("CU-CP started successfully");

  // Check connection to AMF.
  if (not o_cucp_obj.get_cu_cp().get_ng_handler().amfs_are_connected()) {
    report_error("CU-CP failed to connect to AMF");
  }

  // Configure the remote commands and start the service.
  if (remote_control_server) {
    remote_control_server->add_commands(o_du_unit.commands.remote);
    remote_control_server->get_operation_controller().start();
  }

  // Connect F1-C to O-CU-CP and start listening for new F1-C connection requests.
  f1c_gw->attach_cu_cp(o_cucp_obj.get_cu_cp().get_f1c_handler());

  // Start O-CU-UP.
  o_cuup_obj.get_operation_controller().start();

  // Start processing.
  o_du_obj.get_operation_controller().start();

  // Start metrics manager.
  metrics_mngr.start();

  {
    app_services::application_message_banners app_banner(
        app_name, gnb_cfg.log_cfg.filename == "stdout" ? std::string_view() : gnb_cfg.log_cfg.filename);

    auto exec_metrics_session = exec_metrics_service.service
                                    ? exec_metrics_service.service->create_session(workers.get_metrics_executor())
                                    : app_services::app_executor_metrics_service::create_dummy_session();

    while (is_app_running) {
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
  }

  // Stop metrics manager.
  metrics_mngr.stop();

  // Stop remote control server.
  if (remote_control_server) {
    remote_control_server->get_operation_controller().stop();
  }

  // Stop DU activity.
  o_du_obj.get_operation_controller().stop();

  // Stop O-CU-UP activity.
  o_cuup_obj.get_operation_controller().stop();

  // Stop O-CU-CP activity.
  o_cucp_obj.get_operation_controller().stop();

  // Stop gateway SCTP servers.
  f1c_gw->stop();
  e1_gw->stop();
  // Xn-C gateway is stopped by Xn-C connection manager.

  return 0;
}
