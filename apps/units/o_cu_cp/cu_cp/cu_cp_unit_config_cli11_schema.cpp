// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_unit_config_cli11_schema.h"
#include "apps/helpers/logger/logger_appconfig_cli11_utils.h"
#include "apps/helpers/metrics/metrics_config_cli11_schema.h"
#include "apps/helpers/network/sctp_cli11_schema.h"
#include "apps/helpers/ntn/ntn_config_cli11_schema.h"
#include "cu_cp_unit_config.h"
#include "cu_cp_unit_config_helpers.h"
#include "ocudu/ran/nr_cell_identity.h"
#include "ocudu/support/cli11_utils.h"
#include "ocudu/support/config_parsers.h"
#include <algorithm>
#include <istream>
#include <map>

using namespace ocudu;

/// Registers CLI11 lat/lon options on \p app bound to \p loc.
static void configure_cli11_reference_location(CLI::App& app, reference_location& loc)
{
  add_option(app, "--latitude", loc.latitude, "Latitude [degrees, -90..90]")->range(-90.0, 90.0);
  add_option(app, "--longitude", loc.longitude, "Longitude [degrees, -180..180]")->range(-180.0, 180.0);
}

/// Configures the CLI11 logging arguments.
static void configure_cli11_log_args(CLI::App& app, cu_cp_unit_logger_config& log_params)
{
  app_helpers::add_log_option(app, log_params.pdcp_level, "--pdcp_level", "PDCP log level");
  app_helpers::add_log_option(app, log_params.rrc_level, "--rrc_level", "RRC log level");
  app_helpers::add_log_option(app, log_params.ngap_level, "--ngap_level", "NGAP log level");
  app_helpers::add_log_option(app, log_params.xnap_level, "--xnap_level", "XNAP log level");
  app_helpers::add_log_option(app, log_params.nrppa_level, "--nrppa_level", "NRPPA log level");
  app_helpers::add_log_option(app, log_params.e1ap_level, "--e1ap_level", "E1AP log level");
  app_helpers::add_log_option(app, log_params.f1ap_level, "--f1ap_level", "F1AP log level");
  app_helpers::add_log_option(app, log_params.cu_level, "--cu_level", "Log level for the CU");
  app_helpers::add_log_option(app, log_params.sec_level, "--sec_level", "Security functions log level");

  add_option(app,
             "--hex_max_size",
             log_params.hex_max_size,
             "Maximum number of bytes to print in hex (zero for no hex dumps, -1 for unlimited bytes)")
      ->capture_default_str()
      ->range(-1, 1024);

  add_option(app, "--e1ap_json_enabled", log_params.e1ap_json_enabled, "Enable JSON logging of E1AP PDUs")
      ->always_capture_default();
  add_option(app, "--f1ap_json_enabled", log_params.f1ap_json_enabled, "Enable JSON logging of F1AP PDUs")
      ->always_capture_default();
}

/// Configures the CLI11 PCAP arguments.
static void configure_cli11_pcap_args(CLI::App& app, cu_cp_unit_pcap_config& pcap_params)
{
  add_option(app, "--ngap_filename", pcap_params.ngap.filename, "N3 GTP-U PCAP file output path")
      ->capture_default_str();
  add_option(app, "--ngap_enable", pcap_params.ngap.enabled, "Enable N3 GTP-U packet capture")
      ->always_capture_default();
  add_option(app, "--xnap_filename", pcap_params.xnap.filename, "XNAP PCAP file output path")->capture_default_str();
  add_option(app, "--xnap_enable", pcap_params.xnap.enabled, "Enable XNAP packet capture")->always_capture_default();
  add_option(app, "--f1ap_filename", pcap_params.f1ap.filename, "F1AP PCAP file output path")->capture_default_str();
  add_option(app, "--f1ap_enable", pcap_params.f1ap.enabled, "Enable F1AP packet capture")->always_capture_default();
  add_option(app, "--e1ap_filename", pcap_params.e1ap.filename, "E1AP PCAP file output path")->capture_default_str();
  add_option(app, "--e1ap_enable", pcap_params.e1ap.enabled, "Enable E1AP packet capture")->always_capture_default();
}

/// Configures the TAI slice support arguments.
static void configure_cli11_tai_slice_support_args(CLI::App& app, cu_cp_unit_plmn_item::tai_slice_t& config)
{
  add_option(app, "--sst", config.sst, "Slice Service Type")->capture_default_str()->range(0, 255);
  add_option(app, "--sd", config.sd, "Service Differentiator")->capture_default_str()->range(0, 0xffffff);
}

/// Configures the CLI11 PLMD item arguments.
static void configure_cli11_plmn_item_args(CLI::App& app, cu_cp_unit_plmn_item& config)
{
  add_option(app, "--plmn", config.plmn_id, "PLMN to be configured")
      ->pattern("^[0-9]{5,6}$")
      ->min_length(5)
      ->max_length(6);

  // TAI slice support list.
  add_option_object_list<cu_cp_unit_plmn_item::tai_slice_t>(app,
                                                            "--tai_slice_support_list",
                                                            config.tai_slice_support_list,
                                                            configure_cli11_tai_slice_support_args,
                                                            "Sets the list of TAI slices for this PLMN");
}

/// Configures the CLI11 supported tracking areas arguments.
static void configure_cli11_supported_ta_args(CLI::App& app, cu_cp_unit_supported_ta_item& config)
{
  add_option(app, "--tac", config.tac, "TAC to be configured")->check([](const std::string& value) {
    std::stringstream ss(value);
    unsigned          tac;
    ss >> tac;

    // Values 0 and 0xfffffe are reserved.
    if (tac == 0U || tac == 0xfffffeU) {
      return "TAC values 0 or 0xfffffe are reserved";
    }

    return (tac <= 0xffffffU) ? "" : "TAC value out of range";
  });

  // PLMN item list.
  add_option_object_list<cu_cp_unit_plmn_item>(app,
                                               "--plmn_list",
                                               config.plmn_list,
                                               configure_cli11_plmn_item_args,
                                               "Sets the list of PLMN items for this tracking area");
}

/// Configures the CLI11 AMF item arguments.
static void configure_cli11_amf_item_args(CLI::App& app, cu_cp_unit_amf_config_item& config)
{
  add_option(app,
             "--addrs,--addr",
             config.ip_addrs,
             "AMF addresses to be used for N2 interface. Multiple addresses can be specified for SCTP multi-homing. "
             "The '--addr' name is a deprecated alias and should not be used.");
  add_option(app, "--port", config.port, "AMF port")->capture_default_str()->range(20000, 40000);
  add_option(
      app,
      "--bind_addrs,--bind_addr",
      config.bind_addrs,
      "CU-CP bind addresses to be used for N2 interface. Multiple addresses can be specified for SCTP "
      "multi-homing. If left empty, implicit bind is performed. The '--bind_addr' name is a deprecated alias and "
      "should not be used.");
  add_option(app, "--bind_interface", config.bind_interface, "Network device to bind for N2 interface")
      ->capture_default_str();
  configure_cli11_sctp_socket_args(app, config.sctp);

  // Supported tracking areas configuration parameters.
  declare_object_list_schema<cu_cp_unit_supported_ta_item>(app,
                                                           "--supported_tracking_areas",
                                                           configure_cli11_supported_ta_args,
                                                           "Sets the list of tracking areas supported by this AMF");
  add_option_function<std::vector<std::string>>(
      app,
      "--supported_tracking_areas",
      [&config](const std::vector<std::string>& values) {
        // If supported tracking areas are configured clear default values.
        config.supported_tas.clear();
        config.is_default_supported_tas = false;
        config.supported_tas.resize(values.size());

        for (unsigned i = 0, e = values.size(); i != e; ++i) {
          CLI::App subapp("Supported tracking areas of AMF");
          subapp.config_formatter(create_yaml_config_parser());
          subapp.allow_config_extras(CLI::config_extras_mode::error);
          configure_cli11_supported_ta_args(subapp, config.supported_tas[i]);
          std::istringstream ss(values[i]);
          subapp.parse_from_stream(ss);
        }
      },
      "Sets the list of tracking areas supported by this AMF");
}

/// Configures the CLI11 AMF arguments.
static void configure_cli11_amf_args(CLI::App& app, cu_cp_unit_amf_config& config)
{
  add_option(app, "--no_core", config.no_core, "Allow CU-CP to run without a core")->capture_default_str();
  add_option(app,
             "--amf_reconnection_retry_time",
             config.amf_reconnection_retry_time,
             "Time to wait after a failed AMF reconnection attempt in ms")
      ->capture_default_str();
  add_option(app,
             "--procedure_timeout",
             config.procedure_timeout,
             "Time that the NGAP waits for a response from the AMF in milliseconds")
      ->capture_default_str();

  // AMF parameters.
  configure_cli11_amf_item_args(app, config.amf);
}

/// Configures the XNAP peer arguments.
static void configure_cli11_xnap_peer_args(CLI::App& app, cu_cp_unit_xnap_peer_config& config)
{
  add_option(
      app,
      "--peer_addrs",
      config.peer_addrs,
      "Peer gNB IP addresses to connect for XnAP interface. Multiple addresses can be specified for SCTP multi-homing");
}

/// Configures the CLI11 XNAP gateway arguments.
static void configure_cli11_xnap_gateway_args(CLI::App& app, cu_cp_unit_xnap_gateway_config& config)
{
  add_option(app,
             "--bind_addrs",
             config.bind_addrs,
             "Local IP addresses to bind for this XnAP gateway. Multiple addresses can be specified for SCTP "
             "multi-homing. If left empty, implicit bind is performed");

  // SCTP socket parameters specific to this gateway, nested under `sctp:`.
  CLI::App* sctp_subcmd = add_subcommand(app, "sctp", "SCTP socket options");
  configure_cli11_sctp_socket_args(*sctp_subcmd, config.sctp);

  add_option_object_list<cu_cp_unit_xnap_peer_config>(
      app,
      "--connections",
      config.connections,
      configure_cli11_xnap_peer_args,
      "Sets the list of Xn-C peer connections reachable via this gateway");
}

/// Configures the CLI11 XNAP arguments.
static void configure_cli11_xnap_args(CLI::App& app, cu_cp_unit_xnap_config& config)
{
  add_option(
      app, "--procedure_timeout", config.procedure_timeout, "Time that the XNAP waits for a response in milliseconds")
      ->capture_default_str();
  add_option(app,
             "--reconnect_timer",
             config.reconnect_timer,
             "Time that the XNAP waits before trying to reconnect in milliseconds")
      ->capture_default_str();
  add_option(app,
             "--no_connection_init",
             config.no_connection_init,
             "When true, the CU-CP will not initiate XNAP connections, but will only accept inbound ones")
      ->capture_default_str()
      ->group(""); // hide this parameter from --help

  add_option_object_list<cu_cp_unit_xnap_gateway_config>(
      app,
      "--gateways",
      config.gateways,
      configure_cli11_xnap_gateway_args,
      "Sets the list of XnAP gateways, each with its own bind addresses, SCTP options, and Xn-C peer connections");
}

/// Configures the CLI11 report arguments.
static void configure_cli11_report_args(CLI::App& app, cu_cp_unit_report_config& report_params)
{
  add_option(app, "--report_cfg_id", report_params.report_cfg_id, "Report configuration id to be configured")
      ->range(1, 64);
  add_option(app, "--report_type", report_params.report_type, "Type of the report configuration")
      ->enum_values({"periodical", "event_triggered", "cond_trigger"});
  add_option_function<std::string>(
      app,
      "--event_triggered_report_type",
      [&report_params](const std::string& value) {
        std::optional<ocucp::rrc_event_id::event_id_t> id = ocucp::from_string(value);
        report_error_if_not(id.has_value(), "Invalid --event_triggered_report_type argument.");
        report_params.event_triggered_report_type = *id;
      },
      "Type of the event triggered report")
      ->enum_values({"a1", "a2", "a3", "a4", "a5", "a6", "d1", "t1", "d2"});
  add_option(app, "--report_interval_ms", report_params.report_interval_ms, "Report interval in ms")
      ->enum_values({120, 240, 480, 640, 1024, 2048, 5120, 10240, 20480, 40960, 60000, 360000, 720000, 1800000});
  add_option(app,
             "--periodic_ho_rsrp_offset_db",
             report_params.periodic_ho_rsrp_offset,
             "Measurement trigger quantity offset in dB used to trigger handovers by periodic measurement reports. "
             "When set to -1 no handover will be triggered from periodical measurements. Note the "
             "actual value is field value * 0.5 dB")
      ->range(-1, 30)
      ->capture_default_str();
  add_option(app,
             "--meas_trigger_quantity",
             report_params.meas_trigger_quantity,
             "Measurement trigger quantity (RSRP/RSRQ/SINR)")
      ->enum_values({"rsrp", "rsrq", "sinr"});
  add_option(app,
             "--meas_trigger_quantity_threshold_db",
             report_params.meas_trigger_quantity_threshold_db,
             "Measurement trigger quantity threshold in dB used for measurement report trigger of event A1/A2/A4/A5"
             "Valid ranges: RSRP [-156..-31] dBm, RSRQ [-43..20] dB, SINR [-23..40] dB")
      ->range(-156, 40);
  add_option(app,
             "--meas_trigger_quantity_threshold_2_db",
             report_params.meas_trigger_quantity_threshold_2_db,
             "Measurement trigger quantity threshold 2 in dB used for measurement report trigger of event A5"
             "Valid ranges: RSRP [-156..-31] dBm, RSRQ [-43..20] dB, SINR [-23..40] dB")
      ->range(-156, 40);
  add_option(app,
             "--meas_trigger_quantity_offset_db",
             report_params.meas_trigger_quantity_offset_db,
             "Measurement trigger quantity offset in dB used for measurement report trigger of event A3/A6.")
      ->range(-15, 15);
  add_option(
      app, "--hysteresis_db", report_params.hysteresis_db, "Hysteresis in dB used for measurement report trigger.")
      ->range(0, 15);
  add_option(app,
             "--time_to_trigger_ms",
             report_params.time_to_trigger_ms,
             "Time in ms during which a condition must be met before measurement report trigger")
      ->enum_values({0, 40, 64, 80, 100, 128, 160, 256, 320, 480, 512, 640, 1024, 1280, 2560, 5120});
  add_option(app,
             "--t312",
             report_params.t312_ms,
             "T312 timer in ms. This timer is started by the UE on event triggered measurement report, when T310 "
             "(out-of-sync) timer is already running and on its expiration triggers the RLF to speed up "
             "reestablishment to different cell.")
      ->enum_values({0, 50, 100, 200, 300, 400, 500, 1000});

  // D1/D2 distance-based conditional event options.
  add_option(app,
             "--distance_thresh_from_ref1_km",
             report_params.distance_thresh_from_ref1_km,
             "D1/D2: distance threshold 1 in km [0..3276.75] (50m steps, D1 max is 3276.25)")
      ->range(0.0, 3276.75);
  add_option(app,
             "--distance_thresh_from_ref2_km",
             report_params.distance_thresh_from_ref2_km,
             "D1/D2: distance threshold 2 in km [0..3276.75] (50m steps, D1 max is 3276.25)")
      ->range(0.0, 3276.75);
  add_option(app,
             "--hysteresis_location_km",
             report_params.hysteresis_location_km,
             "D1/D2: location hysteresis in km [0..327.68] (10m steps)")
      ->range(0.0, 327.68);

  // D1 reference locations (nested subcommands for lat/lon).
  static reference_location ref_location1;
  CLI::App* ref_loc1_sub = add_subcommand(app, "ref_location1", "D1: reference location 1 (serving cell)");
  configure_cli11_reference_location(*ref_loc1_sub, ref_location1);
  ref_loc1_sub->parse_complete_callback([&]() {
    if (app.get_subcommand("ref_location1")->count() != 0) {
      report_params.ref_location1 = ref_location1;
    }
  });

  static reference_location ref_location2;
  CLI::App* ref_loc2_sub = add_subcommand(app, "ref_location2", "D1: reference location 2 (target cell)");
  configure_cli11_reference_location(*ref_loc2_sub, ref_location2);
  ref_loc2_sub->parse_complete_callback([&]() {
    if (app.get_subcommand("ref_location2")->count() != 0) {
      report_params.ref_location2 = ref_location2;
    }
  });

  // T1 time-based conditional event options.
  add_option_function<std::string>(
      app,
      "--t1_thres",
      [&report_params](const std::string& v) {
        auto result = parse_timestamp_ms(v);
        report_fatal_error_if_not(result, result.error().c_str());
        report_params.t1_thres = result.value();
      },
      "T1: time threshold (Unix ms integer or YYYY-MM-DDTHH:MM:SS[.mmm])")
      ->check([](const std::string& input) -> std::string {
        if (!is_number(input) && !is_valid_timestamp(input)) {
          return "Invalid timestamp format. Expected Unix time (ms) or YYYY-MM-DDTHH:MM:SS[.mmm]";
        }
        return {};
      });
  add_option_function<double>(
      app,
      "--duration_s",
      [&report_params](double v) { report_params.duration = std::chrono::duration<double>{v}; },
      "T1: duration in seconds (each step=100ms, range [0.1..600])")
      ->range(0.1, 600.0);
}

/// Configures the CLI11 neighbor cell arguments.
static void configure_cli11_ncell_args(CLI::App& app, cu_cp_unit_neighbor_cell_config_item& config)
{
  add_option(app, "--nr_cell_id", config.nr_cell_id, "Neighbor cell id")
      ->range(static_cast<uint64_t>(0U), nr_cell_identity::max().value());
  add_option(
      app, "--report_configs", config.report_cfg_ids, "Report configurations to configure for this neighbor cell");
}

/// Configues the CLI11 cells arguments.
static void configure_cli11_cells_args(CLI::App& app, cu_cp_unit_cell_config_item& config)
{
  add_option(app, "--nr_cell_id", config.nr_cell_id, "Cell id to be configured")
      ->range(static_cast<uint64_t>(0U), nr_cell_identity::max().value());
  add_option(app,
             "--periodic_report_cfg_id",
             config.periodic_report_cfg_id,
             "Periodical report configuration for the serving cell")
      ->range(1, 64);

  add_auto_enum_option(app, "--band", config.band, "NR frequency band");

  add_option(app,
             "--gnb_id_bit_length",
             config.gnb_id_bit_length,
             "gNodeB identifier bit length. If not set, it will be automatically set to be equal to the gNodeB Id of "
             "the CU-CP")
      ->range(22, 32);
  add_option(app, "--pci", config.pci, "Physical Cell Id")->range(0, 1007);
  add_option(app, "--plmn", config.plmn_id, "PLMN of the cell")->pattern("^[0-9]{5,6}$")->min_length(5)->max_length(6);
  add_option(app, "--tac", config.tac, "Tracking Area Code")->range(0, 0xffffff);
  add_option(app, "--ssb_arfcn", config.ssb_arfcn, "SSB ARFCN");
  add_option(app, "--ssb_scs", config.ssb_scs, "SSB subcarrier spacing")->enum_values({15, 30, 60, 120, 240});
  add_option(app, "--ssb_period", config.ssb_period, "SSB period in ms")->enum_values({5, 10, 20, 40, 80, 160});
  add_option(app, "--ssb_offset", config.ssb_offset, "SSB offset");
  add_option(app, "--ssb_duration", config.ssb_duration, "SSB duration")->enum_values({1, 2, 3, 4, 5});

  // NTN configuration (ntn-NeighbourCellInfo-r18): satellite reference (globally-defined satellite_idx or inline
  // definition, same options as in the DU NTN cell config) plus an optional 2-D reference location. Each cell is
  // parsed immediately by its own subapp, so the static buffers are copied out before the next cell is parsed.
  static cu_cp_unit_cell_ntn_config ntn_buf;
  ntn_buf              = {};
  CLI::App* ntn_subcmd = add_subcommand(app, "ntn", "NTN configuration of this cell");
  configure_cli11_ntn_satellite_args(*ntn_subcmd, ntn_buf.sat_ref);

  static geodetic_coordinates_t ntn_ref_loc_buf;
  ntn_ref_loc_buf      = {};
  CLI::App* ref_subcmd = add_subcommand(*ntn_subcmd, "reference_location", "2-D reference location of the cell");
  configure_cli11_geodetic_coordinates(*ref_subcmd, ntn_ref_loc_buf, false);
  ref_subcmd->parse_complete_callback([]() { ntn_buf.reference_location = ntn_ref_loc_buf; });

  static ntn_polarization_t ntn_pol_buf;
  ntn_pol_buf          = {};
  CLI::App* pol_subcmd = add_subcommand(*ntn_subcmd, "polarization", "Service link DL/UL polarization of the cell");
  configure_cli11_ntn_polarization(*pol_subcmd, ntn_pol_buf);
  pol_subcmd->parse_complete_callback([]() { ntn_buf.polarization = ntn_pol_buf; });

  ntn_subcmd->parse_complete_callback([&config, ntn_subcmd]() {
    if (ntn_subcmd->count() != 0) {
      config.ntn_cfg = ntn_buf;
    }
  });

  // report configuration parameters.
  add_option_object_list<cu_cp_unit_neighbor_cell_config_item>(
      app, "--ncells", config.ncells, configure_cli11_ncell_args, "Sets the list of neighbor cells known to the CU-CP");
}

/// Configures the CLI11 mobility arguments.
static void configure_cli11_mobility_args(CLI::App& app, cu_cp_unit_mobility_config& config)
{
  add_option(app,
             "--trigger_handover_from_measurements",
             config.trigger_handover_from_measurements,
             "Whether to start HO if neighbor cells become stronger")
      ->capture_default_str();
  add_option(app,
             "--trigger_cho_on_ue_setup",
             config.trigger_cho_on_ue_setup,
             "Whether to auto-trigger CHO after UE setup when readiness checks pass")
      ->capture_default_str();
  add_option(app,
             "--cho_timeout_ms",
             config.cho_timeout_ms,
             "Timeout in milliseconds used for auto-triggered CHO and as default timeout for manual CHO command")
      ->capture_default_str()
      ->range(1, 600000);
  add_option(app,
             "--ntn_update_period_ms",
             config.ntn_update_period_ms,
             "Period in milliseconds of the NTN neighbour cell info updates in the measurement configuration")
      ->capture_default_str()
      ->range(100, 2000);

  // Cell map parameters.
  add_option_object_list<cu_cp_unit_cell_config_item>(
      app, "--cells", config.cells, configure_cli11_cells_args, "Sets the list of cells known to the CU-CP");

  // report configuration parameters.
  add_option_object_list<cu_cp_unit_report_config>(
      app, "--report_configs", config.report_configs, configure_cli11_report_args, "Sets report configurations");
}

/// Configures the CLI11 RRC arguments.
static void configure_cli11_rrc_args(CLI::App& app, cu_cp_unit_rrc_config& config)
{
  add_option(app,
             "--force_reestablishment_fallback",
             config.force_reestablishment_fallback,
             "Force RRC re-establishment fallback to RRC setup")
      ->capture_default_str();

  add_option(app, "--force_resume_fallback", config.force_resume_fallback, "Force RRC resume fallback to RRC setup")
      ->capture_default_str();

  add_option(app,
             "--rrc_procedure_guard_time_ms",
             config.rrc_procedure_guard_time_ms,
             "Guard time in ms used for RRC message exchange with UE. This is added to the RRC procedure timeout.")
      ->capture_default_str();
  add_option(app,
             "--rrc_reject_wait_time_s",
             config.rrc_reject_wait_time_s,
             "Optional: WaitTime [s] (1..16) signalled in the RRC Reject waitTime IE. "
             "If not provided, no waitTime will be sent with RRC Reject.")
      ->range(1, 16);
}

/// Strips whitespace and lower-cases an algorithm preference list, e.g. "NEA0, NEA2" -> "nea0,nea2".
static std::string normalize_algo_pref_list(std::string value)
{
  value.erase(std::remove_if(value.begin(), value.end(), ::isspace), value.end());
  std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return std::tolower(c); });
  return value;
}

/// Splits teh given string into a vector of strings splitting the items with ",".
static std::vector<std::string> split_algo_pref_list(const std::string& value)
{
  std::vector<std::string> tokens;
  std::istringstream       ss(value);
  for (std::string token; std::getline(ss, token, ','); tokens.push_back(token)) {
  }
  return tokens;
}

/// Configures the CLI11 security arguments.
static void configure_cli11_security_args(CLI::App& app, cu_cp_unit_security_config& config)
{
  auto sec_check = [](const std::string& value) -> std::string {
    if (value == "required" || value == "preferred" || value == "not_needed") {
      return {};
    }
    return "Security indication value not supported. Accepted values [required,preferred,not_needed]";
  };

  add_option_function<std::string>(
      app,
      "--integrity",
      [&config](const std::string& value) {
        report_error_if_not(from_string(config.integrity_protection, value), "Invalid --integrity argument.");
      },
      "Default integrity protection indication for DRBs")
      ->default_str(to_string(config.integrity_protection))
      ->check(sec_check);

  add_option_function<std::string>(
      app,
      "--confidentiality",
      [&config](const std::string& value) {
        std::optional<confidentiality_protection_indication_t> confidentiality_protection = from_string(value);
        report_error_if_not(confidentiality_protection, "Invalid --confidenciality argument.");
        config.confidentiality_protection = *confidentiality_protection;
      },
      "Default confidentiality protection indication for DRBs")
      ->default_str(to_string(config.confidentiality_protection))
      ->check(sec_check);

  add_option_function<std::string>(
      app,
      "--nea_pref_list",
      [&config](const std::string& value) {
        config.nea_preference_list = {};
        unsigned idx               = 0;
        for (const std::string& algo : split_algo_pref_list(value)) {
          if (algo == "nea0") {
            config.nea_preference_list[idx] = security::ciphering_algorithm::nea0;
          } else if (algo == "nea1") {
            config.nea_preference_list[idx] = security::ciphering_algorithm::nea1;
          } else if (algo == "nea2") {
            config.nea_preference_list[idx] = security::ciphering_algorithm::nea2;
          } else if (algo == "nea3") {
            config.nea_preference_list[idx] = security::ciphering_algorithm::nea3;
          }
          ++idx;
        }
      },
      "Ordered preference list for the selection of encryption algorithm (NEA) (default: NEA0, NEA2, NEA1)")
      ->default_str(to_string(config.nea_preference_list))
      ->transform(normalize_algo_pref_list)
      ->check([](const std::string& value) -> std::string {
        std::vector<std::string> tokens = split_algo_pref_list(value);
        if (tokens.size() > 4) {
          return fmt::format("Too many ciphering algorithms specified ({}); at most 4 are allowed.", tokens.size());
        }
        for (const std::string& algo : tokens) {
          if (algo != "nea0" && algo != "nea1" && algo != "nea2" && algo != "nea3") {
            return fmt::format(
                "Invalid ciphering algorithm. Valid values are \"nea0\", \"nea1\", \"nea2\" and \"nea3\". algo={}",
                algo);
          }
        }
        return {};
      });

  add_option_function<std::string>(
      app,
      "--nia_pref_list",
      [&config](const std::string& value) {
        config.nia_preference_list = {};
        unsigned idx               = 0;
        for (const std::string& algo : split_algo_pref_list(value)) {
          if (algo == "nia1") {
            config.nia_preference_list[idx] = security::integrity_algorithm::nia1;
          } else if (algo == "nia2") {
            config.nia_preference_list[idx] = security::integrity_algorithm::nia2;
          } else if (algo == "nia3") {
            config.nia_preference_list[idx] = security::integrity_algorithm::nia3;
          }
          ++idx;
        }
      },
      "Ordered preference list for the selection of encryption algorithm (NIA) (default: NIA2, NIA1)")
      ->default_str(to_string(config.nia_preference_list))
      ->transform(normalize_algo_pref_list)
      ->check([](const std::string& value) -> std::string {
        std::vector<std::string> tokens = split_algo_pref_list(value);
        if (tokens.size() > 3) {
          return fmt::format("Too many integrity algorithms specified ({}); at most 3 are allowed (NIA0 is implicit).",
                             tokens.size());
        }
        for (const std::string& algo : tokens) {
          if (algo == "nia0") {
            return "NIA0 cannot be selected in the algorithm preferences.";
          }
          if (algo != "nia1" && algo != "nia2" && algo != "nia3") {
            return fmt::format("Invalid integrity algorithm. Valid values are \"nia1\", \"nia2\" and \"nia3\". algo={}",
                               algo);
          }
        }
        return {};
      });
}

/// Configures the CLI11 reference time reporting arguments.
static void configure_cli11_ref_time_reporting_args(CLI::App& app, cu_cp_unit_f1ap_config& f1ap_params)
{
  add_option(app,
             "--enabled",
             f1ap_params.ref_time_reporting_enabled,
             "Send Reference Time Information Reporting Control to each DU on connection")
      ->capture_default_str();
  add_option(app,
             "--event_type",
             f1ap_params.ref_time_reporting_event_type,
             "Reporting mode: \"on_demand\" (single report) or \"periodic\" (recurring reports)")
      ->capture_default_str()
      ->enum_values({"on_demand", "periodic"});
  add_option(app,
             "--periodicity_rf",
             f1ap_params.ref_time_reporting_periodicity_rf,
             "Reporting period in radio frames (1 RF = 10 ms). Used only when event_type is \"periodic\"")
      ->capture_default_str()
      ->range(1, 512);
}

/// Configures the CLI11 F1AP arguments.
static void configure_cli11_f1ap_args(CLI::App& app, cu_cp_unit_f1ap_config& f1ap_params)
{
  add_option(app,
             "--procedure_timeout",
             f1ap_params.procedure_timeout,
             "Time that the F1AP waits for a DU response in milliseconds")
      ->capture_default_str();
  CLI::App* rtr_subcmd =
      add_subcommand(app, "ref_time_reporting", "Reference Time Information Reporting Control (TS 38.473 section 8.12)")
          ->configurable();
  configure_cli11_ref_time_reporting_args(*rtr_subcmd, f1ap_params);
}

static void configure_cli11_pws_args(CLI::App& app, cu_cp_unit_pws_config& pws_params)
{
  add_option(app,
             "--max_warning_message_segment_size",
             pws_params.max_warning_message_segment_size,
             "Maximum bytes per SIB7/SIB8 warning message segment. The warning message contents (up to 9600 bytes) is "
             "split into chunks of this size, each encoded as a separate SIB.")
      ->capture_default_str()
      ->positive();
}

static void configure_cli11_e1ap_args(CLI::App& app, cu_cp_unit_e1ap_config& e1ap_params)
{
  add_option(app,
             "--procedure_timeout",
             e1ap_params.procedure_timeout,
             "Time that the E1AP waits for a CU-UP response in milliseconds")
      ->capture_default_str();
}

/// Configures the CLI11 logical cell arguments.
static void configure_cli11_logical_cell_args(CLI::App& app, cu_cp_unit_logical_cell_config& cell_params)
{
  add_option(app,
             "--sector_id",
             cell_params.sector_id,
             "Sector ID (4-14 bits). This value is concatenated with the gNB Id to form the NR Cell Identity "
             "(NCI) of the logical cell, and must match the sector ID of the corresponding DU cell")
      ->required()
      ->check(CLI::Range(0U, (1U << 14) - 1U));
  add_option(app,
             "--admin_state",
             cell_params.admin_state,
             "Administrative state of the cell: locked cells are not activated by the CU-CP until unlocked by command")
      ->default_str("unlocked")
      ->transform(CLI::CheckedTransformer(
          std::map<std::string, ocucp::cell_admin_state>{{"unlocked", ocucp::cell_admin_state::unlocked},
                                                         {"locked", ocucp::cell_admin_state::locked}},
          CLI::ignore_case));
  add_option(app, "--cell_barred", cell_params.cell_barred, "Intended MIB cellBarred state of the cell")
      ->capture_default_str();
}

/// Configues the CLI11 CU-CP arguments.
static void configure_cli11_cu_cp_args(CLI::App& app, cu_cp_unit_config& cu_cp_params)
{
  add_option(
      app, "--max_nof_dus", cu_cp_params.max_nof_dus, "Maximum number of DU connections that the CU-CP may accept");

  add_option(app,
             "--max_nof_cu_ups",
             cu_cp_params.max_nof_cu_ups,
             "Maximum number of CU-UP connections that the CU-CP may accept");

  add_option(app, "--max_nof_ues", cu_cp_params.max_nof_ues, "Maximum number of UEs that the CU-CP may accept");

  add_option(app, "--max_nof_drbs_per_ue", cu_cp_params.max_nof_drbs_per_ue, "Maximum number of DRBs per UE")
      ->capture_default_str()
      ->range(1, 29);

  add_option(app, "--inactivity_timer", cu_cp_params.inactivity_timer, "UE/PDU Session/DRB inactivity timer in seconds")
      ->capture_default_str()
      ->range(1, 7200);

  add_option(
      app,
      "--enable_rrc_inactive",
      cu_cp_params.enable_rrc_inactive,
      "Enable RRC inactive state for UEs based on inactivity timer. When disabled, UEs will be released on inactivity")
      ->capture_default_str();

  add_option(app,
             "--ran_paging_cycle",
             cu_cp_params.ran_paging_cycle,
             "RAN Paging cycle for RRC inactive UEs in nof. Radio Frames")
      ->capture_default_str()
      ->enum_values({32, 64, 128, 256});

  add_option(app,
             "--t380",
             cu_cp_params.t380,
             "RRC inactivity timer T380 in minutes. The timer is started when the UE receives a RRC Release message "
             "including a suspend config and is stopped on the reception of RRCResume.")
      ->capture_default_str()
      ->enum_values({5, 10, 20, 30, 60, 120, 360, 720});

  add_option(app,
             "--full_i_rnti_profile",
             cu_cp_params.full_i_rnti_profile,
             "I-RNTI profile of the Full-I-RNTI, which sets the width of the Local NG-RAN Node Identifier it carries")
      ->capture_default_str()
      ->enum_values({"profile0", "profile1", "profile2", "profile3"});

  add_option(app,
             "--short_i_rnti_profile",
             cu_cp_params.short_i_rnti_profile,
             "I-RNTI profile of the Short-I-RNTI, which sets the width of the Local NG-RAN Node Identifier it carries")
      ->capture_default_str()
      ->enum_values({"profile0", "profile1"});

  add_option(app,
             "--request_pdu_session_timeout",
             cu_cp_params.request_pdu_session_timeout,
             "Timeout for requesting a PDU session after the InitialUeMessage was sent to the core, in "
             "seconds. The timeout must be larger than T310. If the value is reached, the UE will be released.")
      ->capture_default_str();

  // Logical cells: the CU-CP-side declaration of cells (administrative intent), addressed by sector ID.
  // Distinct from the DU's top-level cells section, which carries the radio configuration.
  app.add_option_function<std::vector<std::string>>(
      "--logical_cells",
      [&cu_cp_params](const std::vector<std::string>& values) {
        cu_cp_params.cells_cfg.resize(values.size());

        for (unsigned i = 0, e = values.size(); i != e; ++i) {
          CLI::App subapp("CU-CP logical cell list");
          subapp.config_formatter(create_yaml_config_parser());
          subapp.allow_config_extras(CLI::config_extras_mode::error);
          configure_cli11_logical_cell_args(subapp, cu_cp_params.cells_cfg[i]);
          std::istringstream ss(values[i]);
          subapp.parse_from_stream(ss);
        }
      },
      "Sets the list of logical cells declared to the CU-CP (sector_id, admin_state, cell_barred)");

  CLI::App* amf_subcmd = add_subcommand(app, "amf", "AMF configuration");
  configure_cli11_amf_args(*amf_subcmd, cu_cp_params.amf_config);

  // AMF parameters.
  add_option_object_list<cu_cp_unit_amf_config_item>(app,
                                                     "--extra_amfs",
                                                     cu_cp_params.extra_amfs,
                                                     configure_cli11_amf_item_args,
                                                     "Sets the list of extra AMFs for the CU-CP to connect to");

  // XN-C parameters.
  CLI::App* xnap_subcmd = add_subcommand(app, "xnap", "XNAP configuration");
  configure_cli11_xnap_args(*xnap_subcmd, cu_cp_params.xnap_config);

  CLI::App* mobility_subcmd = add_subcommand(app, "mobility", "Mobility configuration");
  configure_cli11_mobility_args(*mobility_subcmd, cu_cp_params.mobility_config);

  CLI::App* rrc_subcmd = add_subcommand(app, "rrc", "RRC specific configuration");
  configure_cli11_rrc_args(*rrc_subcmd, cu_cp_params.rrc_config);

  CLI::App* security_subcmd = add_subcommand(app, "security", "Security configuration");
  configure_cli11_security_args(*security_subcmd, cu_cp_params.security_config);

  CLI::App* f1ap_subcmd = add_subcommand(app, "f1ap", "F1AP configuration parameters");
  configure_cli11_f1ap_args(*f1ap_subcmd, cu_cp_params.f1ap_config);

  CLI::App* e1ap_subcmd = add_subcommand(app, "e1ap", "E1AP configuration parameters");
  configure_cli11_e1ap_args(*e1ap_subcmd, cu_cp_params.e1ap_config);

  CLI::App* pws_subcmd = add_subcommand(app, "pws", "Public Warning System configuration");
  configure_cli11_pws_args(*pws_subcmd, cu_cp_params.pws_config);
}

/// Configures the CLI11 RLC-UM arguments.
static void configure_cli11_rlc_um_args(CLI::App& app, cu_cp_unit_rlc_um_config& rlc_um_params)
{
  CLI::App* rlc_tx_um_subcmd = add_subcommand(app, "tx", "UM TX parameters");
  add_option(*rlc_tx_um_subcmd, "--sn", rlc_um_params.tx.sn_field_length, "RLC UM TX SN")->capture_default_str();
  add_option(*rlc_tx_um_subcmd, "--queue-size", rlc_um_params.tx.queue_size, "RLC UM TX SDU queue size")
      ->capture_default_str();
  CLI::App* rlc_rx_um_subcmd = add_subcommand(app, "rx", "UM TX parameters");
  add_option(*rlc_rx_um_subcmd, "--sn", rlc_um_params.rx.sn_field_length, "RLC UM RX SN")->capture_default_str();
  add_option(*rlc_rx_um_subcmd, "--t-reassembly", rlc_um_params.rx.t_reassembly, "RLC UM t-Reassembly")
      ->capture_default_str();
}

/// Configures the CLI11 RLC-AM arguments.
static void configure_cli11_rlc_am_args(CLI::App& app, cu_cp_unit_rlc_am_config& rlc_am_params)
{
  CLI::App* tx_subcmd = add_subcommand(app, "tx", "AM TX parameters");
  add_option(*tx_subcmd, "--sn", rlc_am_params.tx.sn_field_length, "RLC AM TX SN size")->capture_default_str();
  add_option(*tx_subcmd, "--t-poll-retransmit", rlc_am_params.tx.t_poll_retx, "RLC AM TX t-PollRetransmit (ms)")
      ->capture_default_str();
  add_option(*tx_subcmd, "--max-retx-threshold", rlc_am_params.tx.max_retx_thresh, "RLC AM max retx threshold")
      ->capture_default_str();
  add_option(*tx_subcmd, "--poll-pdu", rlc_am_params.tx.poll_pdu, "RLC AM TX PollPdu")->capture_default_str();
  add_option(*tx_subcmd, "--poll-byte", rlc_am_params.tx.poll_byte, "RLC AM TX PollByte")->capture_default_str();
  add_option(*tx_subcmd,
             "--max_window",
             rlc_am_params.tx.max_window,
             "Non-standard parameter that limits the tx window size. Can be used for limiting memory usage with "
             "large windows. 0 means no limits other than the SN size (i.e. 2^[sn_size-1]).");
  add_option(*tx_subcmd, "--queue-size", rlc_am_params.tx.queue_size, "RLC AM TX SDU queue size")
      ->capture_default_str();

  CLI::App* rx_subcmd = add_subcommand(app, "rx", "AM RX parameters");
  add_option(*rx_subcmd, "--sn", rlc_am_params.rx.sn_field_length, "RLC AM RX SN")->capture_default_str();
  add_option(*rx_subcmd, "--t-reassembly", rlc_am_params.rx.t_reassembly, "RLC AM RX t-Reassembly")
      ->capture_default_str();
  add_option(*rx_subcmd, "--t-status-prohibit", rlc_am_params.rx.t_status_prohibit, "RLC AM RX t-StatusProhibit")
      ->capture_default_str();
  add_option(*rx_subcmd, "--max_sn_per_status", rlc_am_params.rx.max_sn_per_status, "RLC AM RX status SN limit")
      ->capture_default_str();
}

/// Configures the CLI11 RLC arguments.
static void configure_cli11_rlc_args(CLI::App& app, cu_cp_unit_rlc_config& rlc_params)
{
  add_option_function<std::string>(
      app,
      "--mode",
      [&rlc_params](const std::string& value) {
        report_error_if_not(from_string(rlc_params.mode, value), "Invalid --mode argument.");
      },
      "RLC mode")
      ->default_str(format_as(rlc_params.mode))
      ->check(CLI::IsMember({"tm", "um-bidir", "um-unidir-ul", "um-unidir-dl", "am"}));

  // UM section.
  CLI::App* rlc_um_subcmd = add_subcommand(app, "um-bidir", "UM parameters");
  configure_cli11_rlc_um_args(*rlc_um_subcmd, rlc_params.um);

  // AM section.
  CLI::App* rlc_am_subcmd = add_subcommand(app, "am", "AM parameters");
  configure_cli11_rlc_am_args(*rlc_am_subcmd, rlc_params.am);
}

/// Configures the CLI11 PDCP ROHC arguments.
static void configure_cli11_pdcp_rohc_args(CLI::App& app, cu_cp_unit_pdcp_rohc_config& pdcp_rohc_params)
{
  add_option_function<std::string>(
      app,
      "--rohc_type",
      [&pdcp_rohc_params](const std::string& value) {
        if (value == "none") {
          pdcp_rohc_params.rohc_type = cu_cp_unit_pdcp_rohc_type::none;
        } else if (value == "rohc") {
          pdcp_rohc_params.rohc_type = cu_cp_unit_pdcp_rohc_type::rohc;
        } else if (value == "uplink_only_rohc") {
          pdcp_rohc_params.rohc_type = cu_cp_unit_pdcp_rohc_type::uplink_only_rohc;
        }
      },
      "ROHC type (none/rohc/ul_only_rohc). Values: {none, rohc, ul_only_rohc}. Default: none")
      ->default_str("none")
      ->enum_values({"none", "rohc", "uplink_only_rohc"});
  add_option(app, "--max_cid", pdcp_rohc_params.max_cid, "Maximum CID")->capture_default_str();
  add_option(app, "--profile0x0001", pdcp_rohc_params.profile0x0001, "Configure profile0x0001 (ROHCv1 RTP/UDP/IP)")
      ->always_capture_default();
  add_option(app, "--profile0x0002", pdcp_rohc_params.profile0x0002, "Configure profile0x0002 (ROHCv1 UDP/IP)")
      ->always_capture_default();
  add_option(app, "--profile0x0003", pdcp_rohc_params.profile0x0003, "Configure profile0x0003 (ROHCv1 ESP/IP)")
      ->always_capture_default();
  add_option(app, "--profile0x0004", pdcp_rohc_params.profile0x0004, "Configure profile0x0004 (ROHCv1 IP)")
      ->always_capture_default();
  add_option(app, "--profile0x0006", pdcp_rohc_params.profile0x0006, "Configure profile0x0006 (ROHCv1 TCP/IP)")
      ->always_capture_default();
  add_option(app, "--profile0x0101", pdcp_rohc_params.profile0x0101, "Configure profile0x0101 (ROHCv2 RTP/UDP/IP)")
      ->always_capture_default();
  add_option(app, "--profile0x0102", pdcp_rohc_params.profile0x0102, "Configure profile0x0102 (ROHCv2 UDP/IP)")
      ->always_capture_default();
  add_option(app, "--profile0x0103", pdcp_rohc_params.profile0x0103, "Configure profile0x0103 (ROHCv2 ESP/IP)")
      ->always_capture_default();
  add_option(app, "--profile0x0104", pdcp_rohc_params.profile0x0104, "Configure profile0x0104 (ROHCv2 IP)")
      ->always_capture_default();
}

/// Configures the CLI11 PDCP transmission arguments.
static void configure_cli11_pdcp_tx_args(CLI::App& app, cu_cp_unit_pdcp_tx_config& pdcp_tx_params)
{
  add_option(app, "--sn", pdcp_tx_params.sn_field_length, "PDCP TX SN size")
      ->default_str(std::to_string(pdcp_sn_size_to_uint(pdcp_tx_params.sn_field_length)))
      ->check(CLI::IsMember({"12", "18"}));
  add_option(app, "--discard_timer", pdcp_tx_params.discard_timer, "PDCP TX discard timer (ms)")
      ->default_str(std::to_string(pdcp_discard_timer_to_int(pdcp_tx_params.discard_timer)))
      ->check([](const std::string& value) -> std::string {
        int number = 0;
        try {
          number = std::stoi(value);
        } catch (const std::exception&) {
          return fmt::format("Invalid PDCP discard timer value \"{}\"", value);
        }
        pdcp_discard_timer discard_timer;
        if (!pdcp_discard_timer_from_int(discard_timer, number)) {
          return fmt::format("Invalid PDCP discard timer value \"{}\"", value);
        }
        return {};
      });
  add_option(app, "--status_report_required", pdcp_tx_params.status_report_required, "PDCP TX status report required")
      ->capture_default_str();
}

/// Configures the CLI11 PDCP reception arguments.
static void configure_cli11_pdcp_rx_args(CLI::App& app, cu_cp_unit_pdcp_rx_config& pdcp_rx_params)
{
  add_option(app, "--sn", pdcp_rx_params.sn_field_length, "PDCP RX SN size")
      ->default_str(std::to_string(pdcp_sn_size_to_uint(pdcp_rx_params.sn_field_length)))
      ->check(CLI::IsMember({"12", "18"}));
  add_option(app, "--t_reordering", pdcp_rx_params.t_reordering, "PDCP RX t-Reordering (ms)")
      ->default_str(std::to_string(pdcp_t_reordering_to_int(pdcp_rx_params.t_reordering)))
      ->check([](const std::string& value) -> std::string {
        int number = 0;
        try {
          number = std::stoi(value);
        } catch (const std::exception&) {
          return fmt::format("Invalid PDCP t-Reordering value \"{}\"", value);
        }
        pdcp_t_reordering t_reordering;
        if (!pdcp_t_reordering_from_int(t_reordering, number)) {
          return fmt::format("Invalid PDCP t-Reordering value \"{}\"", value);
        }
        return {};
      });
  add_option(
      app, "--out_of_order_delivery", pdcp_rx_params.out_of_order_delivery, "PDCP RX enable out-of-order delivery")
      ->capture_default_str();
}

/// Configures the CLI11 PDCP arguments.
static void configure_cli11_pdcp_args(CLI::App& app, cu_cp_unit_pdcp_config& pdcp_params)
{
  // Header compression section.
  CLI::App* pdcp_rohc_subcmd = add_subcommand(app, "rohc", "Header compression parameters");
  configure_cli11_pdcp_rohc_args(*pdcp_rohc_subcmd, pdcp_params.rohc);

  // Transmission section.
  CLI::App* pdcp_tx_subcmd = add_subcommand(app, "tx", "PDCP TX parameters");
  configure_cli11_pdcp_tx_args(*pdcp_tx_subcmd, pdcp_params.tx);

  // Reception section.
  CLI::App* pdcp_rx_subcmd = add_subcommand(app, "rx", "PDCP RX parameters");
  configure_cli11_pdcp_rx_args(*pdcp_rx_subcmd, pdcp_params.rx);
}

/// Configures the CLI11 Quality of Service arguments.
static void configure_cli11_qos_args(CLI::App& app, cu_cp_unit_qos_config& qos_params)
{
  add_option(app, "--five_qi", qos_params.five_qi, "5QI")->capture_default_str()->range(0, 255);

  // RLC section.
  CLI::App* rlc_subcmd = add_subcommand(app, "rlc", "RLC parameters");
  configure_cli11_rlc_args(*rlc_subcmd, qos_params.rlc);

  // PDCP section.
  CLI::App* pdcp_subcmd = add_subcommand(app, "pdcp", "PDCP parameters");
  configure_cli11_pdcp_args(*pdcp_subcmd, qos_params.pdcp);

  // Mark the application that these subcommands need to be present.
  app.needs(rlc_subcmd);
  app.needs(pdcp_subcmd);
}

/// Configures the CLI11 metrics layers arguments.
static void configure_cli11_metrics_layers_args(CLI::App& app, cu_cp_unit_metrics_layer_config& metrics_params)
{
  add_option(app, "--enable_ngap", metrics_params.enable_ngap_metrics, "Enable NGAP metrics")->capture_default_str();
  add_option(app, "--enable_pdcp", metrics_params.enable_pdcp_metrics, "Enable PDCP metrics")->capture_default_str();
  add_option(app, "--enable_rrc", metrics_params.enable_rrc_metrics, "Enable CU-CP RRC metrics")->capture_default_str();
}

/// Configures the CLI11 metrics arguments.
static void configure_cli11_metrics_args(CLI::App& app, cu_cp_unit_metrics_config& metrics_params)
{
  auto* periodicity_subcmd = add_subcommand(app, "periodicity", "Metrics periodicity configuration")->configurable();
  add_option(*periodicity_subcmd,
             "--cu_cp_report_period",
             metrics_params.cu_cp_report_period,
             "CU-CP metrics report period in milliseconds")
      ->capture_default_str();

  auto* layers_subcmd = add_subcommand(app, "layers", "Layer basis metrics configuration")->configurable();
  configure_cli11_metrics_layers_args(*layers_subcmd, metrics_params.layers_cfg);
}

void ocudu::configure_cli11_with_cu_cp_unit_config_schema(CLI::App& app, cu_cp_unit_config& unit_cfg)
{
  add_option(app, "--gnb_id", unit_cfg.gnb_id.id, "gNodeB identifier")->capture_default_str();
  add_option(app, "--gnb_id_bit_length", unit_cfg.gnb_id.bit_length, "gNodeB identifier length in bits")
      ->capture_default_str()
      ->range(22, 32);
  add_option(app, "--ran_node_name", unit_cfg.ran_node_name, "RAN node name")->capture_default_str();

  // CU-CP section
  CLI::App* cu_cp_subcmd = add_subcommand(app, "cu_cp", "CU-CP parameters")->configurable();
  configure_cli11_cu_cp_args(*cu_cp_subcmd, unit_cfg);

  // Loggers section.
  CLI::App* log_subcmd = add_subcommand(app, "log", "Logging configuration")->configurable();
  configure_cli11_log_args(*log_subcmd, unit_cfg.loggers);

  // PCAP section.
  CLI::App* pcap_subcmd = add_subcommand(app, "pcap", "PCAP configuration")->configurable();
  configure_cli11_pcap_args(*pcap_subcmd, unit_cfg.pcap_cfg);

  // Metrics section.
  CLI::App* metrics_subcmd = add_subcommand(app, "metrics", "Metrics configuration")->configurable();
  configure_cli11_metrics_args(*metrics_subcmd, unit_cfg.metrics);
  app_helpers::configure_cli11_with_metrics_appconfig_schema(app, unit_cfg.metrics.common_metrics_cfg);

  // QoS section.
  add_option_object_list<cu_cp_unit_qos_config>(
      app,
      "--qos",
      unit_cfg.qos_cfg,
      configure_cli11_qos_args,
      "Configures RLC and PDCP radio bearers on a per 5QI basis.",
      // The gNB declares --qos in the DU, CU-CP and CU-UP alike, so every unit parses each element and must
      // tolerate the keys owned by the sibling units.
      nullptr,
      object_list_extras::tolerate_unknown);

  // Global NTN section (shared satellite definitions referenced by satellite_idx in neighbor cell NTN configs).
  // In the gnb application this reuses the same section as the DU, so the satellites are defined once.
  CLI::App* global_ntn_subcmd = add_subcommand(app, "ntn", "Global NTN configuration");
  configure_cli11_ntn_satellites_args(*global_ntn_subcmd, unit_cfg.ntn_satellites);
}

void ocudu::autoderive_cu_cp_parameters_after_parsing(const CLI::App& app, cu_cp_unit_config& unit_cfg)
{
  auto cu_cp_app = app.get_subcommand_ptr("cu_cp");
  for (auto& cell : unit_cfg.mobility_config.cells) {
    // Set gNB ID bit length of the neighbor cell to be equal to the current unit gNB ID bit length, if not explicitly
    // set.
    if (not cell.gnb_id_bit_length.has_value()) {
      cell.gnb_id_bit_length = unit_cfg.gnb_id.bit_length;
    }
  }
}
