// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_high_ntn_config_cli11_schema.h"
#include "apps/helpers/ntn/ntn_config_cli11_schema.h"
#include "apps/helpers/ntn/ntn_satellite_config.h"
#include "du_high_unit_cell_ntn_config.h"
#include "ocudu/ran/ntn.h"
#include "ocudu/support/cli11_utils.h"
#include "ocudu/support/config_parsers.h"
#include "ocudu/support/string_parsing_utils.h"

using namespace ocudu;

static void configure_cli11_feeder_link(CLI::App& app, feeder_link_info_t& feeder_link_info)
{
  add_option(app,
             "--enable_doppler_compensation",
             feeder_link_info.enable_doppler_compensation,
             "Enable/disable Feeder Link Doppler compensation.")
      ->capture_default_str();
  add_option(app, "--dl_freq", feeder_link_info.dl_freq, "Downlink feeder link carrier frequency (gnb->sat) [Hz]")
      ->capture_default_str()
      ->range(0.0, 100e9);
  add_option(app, "--ul_freq", feeder_link_info.ul_freq, "Uplink feeder link carrier frequency (sat->gnb) [Hz]")
      ->capture_default_str()
      ->range(0.0, 100e9);
}

static void configure_cli11_ntn_neighbor_cell_args(CLI::App& app, du_high_unit_ntn_neighbor_cell_config& ncell);

static void configure_cli11_ncells(CLI::App& app, std::vector<du_high_unit_ntn_neighbor_cell_config>& ncells)
{
  // The maximum number of neighbour cells is enforced natively via expected() rather than a hand-written size check.
  add_option_object_list<du_high_unit_ntn_neighbor_cell_config>(
      app, "--ncells", ncells, configure_cli11_ntn_neighbor_cell_args, "List of NTN neighbor cells")
      ->expected(0, static_cast<int>(MAX_NOF_NTN_NEIGHBORS));
}

static void configure_cli11_ntn_neighbor_cell_args(CLI::App& app, du_high_unit_ntn_neighbor_cell_config& ncell)
{
  configure_cli11_ntn_satellite_args(app, ncell.sat_ref);

  add_option_function<unsigned>(
      app, "--pci", [&ncell](unsigned val) { ncell.phys_cell_id = static_cast<pci_t>(val); }, "Physical Cell ID")
      ->range(0, static_cast<int>(MAX_PCI));

  add_option_function<unsigned>(
      app,
      "--carrier_freq",
      [&ncell](unsigned val) { ncell.carrier_freq = arfcn_t{val}; },
      "Carrier frequency (NR-ARFCN)")
      ->range(0U, 3279165U);

  add_option_function<unsigned>(
      app,
      "--cell_specific_koffset",
      [&ncell](unsigned value) { ncell.cell_specific_koffset = std::chrono::milliseconds(value); },
      "Cell-specific k-offset [ms]")
      ->range(1U, 1023U);

  add_option(app, "--ntn_ul_sync_validity_dur", ncell.ntn_ul_sync_validity_dur, "UL sync validity duration [s]")
      ->enum_values({5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 120, 180, 240, 900});

  add_option(app, "--k_mac", ncell.k_mac, "K_mac offset")->range(1U, 512U);

  static ntn_polarization_t polarization;
  CLI::App*                 pol_subcmd = add_subcommand(app, "polarization", "Polarization for this neighbor");
  configure_cli11_ntn_polarization(*pol_subcmd, polarization);
  pol_subcmd->parse_complete_callback([&ncell]() { ncell.polarization = polarization; });

  add_option(app, "--ta_report", ncell.ta_report, "Enable TA reporting");

  add_option(app,
             "--has_feeder_link",
             ncell.has_feeder_link,
             "Whether this neighbour has a feeder link (transparent payload); if so, its ta-Info is broadcast in "
             "SIB19");

  add_option(app,
             "--use_state_vector",
             ncell.use_state_vector,
             "Whether to broadcast EphemerisInfo as ECEF state vectors (if true) or ECI Orbital parameters (if false)");
}

static void configure_cli11_sat_switch_with_resync(CLI::App& app, du_high_unit_sat_switch_config& sat_switch_config)
{
  configure_cli11_ntn_satellite_args(app, sat_switch_config.sat_ref);

  add_ntn_timestamp_option(
      app,
      "--t_service_start",
      sat_switch_config.t_service_start,
      "Time when target satellite starts serving (Unix time in ms or ISO 8601: YYYY-MM-DDTHH:MM:SS[.mmm])");

  add_option(app, "--ssb_time_offset_sf", sat_switch_config.ssb_time_offset_sf, "SSB time offset in subframes (0-159)")
      ->range(0U, 159U);

  add_option(app,
             "--ntn_ul_sync_validity_dur",
             sat_switch_config.ntn_ul_sync_validity_dur,
             "UL sync validity duration after switch. If not set, the serving cell value is used")
      ->enum_values({5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 120, 180, 240, 900});

  add_option_function<unsigned>(
      app,
      "--cell_specific_koffset",
      [&sat_switch_config](unsigned value) {
        sat_switch_config.cell_specific_koffset = std::chrono::milliseconds(value);
      },
      "Cell-specific k-offset after switch [ms]. If not set, the serving cell value is used")
      ->range(1U, 1023U);

  add_option(
      app, "--k_mac", sat_switch_config.k_mac, "K_mac offset after switch. If not set, the serving cell value is used")
      ->range(1U, 512U);

  static ntn_polarization_t polarization;
  CLI::App*                 pol_subcmd =
      add_subcommand(app, "polarization", "Polarization after switch. If not set, the serving cell value is used");
  configure_cli11_ntn_polarization(*pol_subcmd, polarization);
  pol_subcmd->parse_complete_callback([&]() {
    if (app.get_subcommand("polarization")->count() != 0) {
      sat_switch_config.polarization = polarization;
    }
  });

  add_option(app,
             "--ta_report",
             sat_switch_config.ta_report,
             "Enable TA reporting after switch. If not set, the serving cell setting is used");

  add_option(app,
             "--use_state_vector",
             sat_switch_config.use_state_vector,
             "Whether to broadcast EphemerisInfo as ECEF state vectors (if true) or ECI Orbital parameters (if "
             "false) after switch");

  add_option(app,
             "--promote_to_serving",
             sat_switch_config.promote_to_serving,
             "Promote this sat-switch's target parameters to become the serving cell config at t_service")
      ->capture_default_str();

  add_option(app,
             "--promote_neighbors",
             sat_switch_config.promote_neighbors,
             "When promote_to_serving is enabled, keep the pre-switch neighbor cell list instead of clearing it")
      ->capture_default_str();
}

static void configure_cli11_ntn_args(CLI::App&                             app,
                                     du_high_unit_cell_ntn_config&         config,
                                     du_high_unit_ntn_serving_cell_config& serv_cell_ntn_config)
{
  add_option(app,
             "--cell_specific_koffset",
             serv_cell_ntn_config.cell_specific_koffset,
             "Cell-specific k-offset to be used for NTN [ms].")
      ->capture_default_str()
      ->range(1, 1023);

  add_option(
      app, "--ntn_ul_sync_validity_dur", serv_cell_ntn_config.ntn_ul_sync_validity_dur, "An UL sync validity duration")
      ->capture_default_str()
      ->enum_values({5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 120, 180, 240, 900});

  // Satellite reference (satellite_idx or inline ephemeris/gateway_location/ta_info).
  configure_cli11_ntn_satellite_args(app, serv_cell_ntn_config.sat_ref);

  // Distance from the serving cell reference location.
  add_option(
      app,
      "--distance_threshold",
      serv_cell_ntn_config.distance_threshold,
      "Distance from the serving cell reference location and is used in location-based measurement. Unit is meters.")
      ->capture_default_str()
      ->range(0, 3276250);

  // T-Service.
  add_ntn_timestamp_option(
      app,
      "--t_service",
      serv_cell_ntn_config.t_service,
      "Indicates end of service for the current cell, in ms unit of Unix time or as UTC time string "
      "(YYYY-MM-DDTHH:MM:SS[.mmm])")
      ->capture_default_str();

  // TA-report.
  add_option(app,
             "--ta_report",
             serv_cell_ntn_config.ta_report,
             " When this field is included in SIB19, it indicates reporting of timing advanced is enabled")
      ->capture_default_str();

  // TA reporting on T_TA variation (tar-Config).
  add_option(app,
             "--ta_report_offset_threshold",
             serv_cell_ntn_config.ta_report_offset_threshold,
             "Threshold of T_TA variation [ms] at which the UE sends a Timing Advance Report. Enables tar-Config.")
      ->capture_default_str()
      ->enum_values({0.5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0});
  add_option(app,
             "--ta_report_sr_enabled",
             serv_cell_ntn_config.ta_report_sr_enabled,
             "Whether a triggered Timing Advance Report may issue a Scheduling Request")
      ->capture_default_str();

  // Broadcast Ephemeris Info type in SIB19.
  add_option(app,
             "--use_state_vector",
             serv_cell_ntn_config.use_state_vector,
             "Whether to broadcast EphemerisInfo as ECEF state vectors (if true) or ECI Orbital parameters (if false)")
      ->capture_default_str();

  // Feeder link info.
  static feeder_link_info_t feeder_link_info;
  CLI::App*                 feeder_link_subcmd =
      add_subcommand(app, "feeder_link", "Feeder link parameters used to compensate Doppler shifts");
  configure_cli11_feeder_link(*feeder_link_subcmd, feeder_link_info);
  feeder_link_subcmd->parse_complete_callback([&]() {
    if (app.get_subcommand("feeder_link")->count() != 0) {
      serv_cell_ntn_config.feeder_link_info = feeder_link_info;
    }
  });

  // Cell reference location info.
  static geodetic_coordinates_t cell_reference_location;
  CLI::App*                     cell_reference_location_subcmd = add_subcommand(
      app, "reference_location", "Reference location of the serving cell provided as geodetic coordinates");
  configure_cli11_geodetic_coordinates(*cell_reference_location_subcmd, cell_reference_location, false);
  cell_reference_location_subcmd->parse_complete_callback([&]() {
    if (app.get_subcommand("reference_location")->count() != 0) {
      serv_cell_ntn_config.reference_location = cell_reference_location;
    }
  });

  // NTN antenna polarization.
  static ntn_polarization_t ntn_polarization;
  CLI::App*                 ntn_polarization_subcmd = add_subcommand(
      app,
      "polarization",
      "If present, it indicates polarization information for downlink/uplink transmission on service link.");
  configure_cli11_ntn_polarization(*ntn_polarization_subcmd, ntn_polarization);
  ntn_polarization_subcmd->parse_complete_callback([&]() {
    if (app.get_subcommand("polarization")->count() != 0) {
      serv_cell_ntn_config.polarization = ntn_polarization;
    }
  });

  // Moving reference location (SIB19, R18 extension).
  static geodetic_coordinates_t moving_ref_location;
  CLI::App*                     moving_ref_subcmd =
      add_subcommand(app, "moving_ref_location", "Moving reference location for NTN Earth-moving cell");
  configure_cli11_geodetic_coordinates(*moving_ref_subcmd, moving_ref_location, false);
  moving_ref_subcmd->parse_complete_callback([&]() {
    if (app.get_subcommand("moving_ref_location")->count() != 0) {
      serv_cell_ntn_config.moving_ref_location = moving_ref_location;
    }
  });

  // Satellite switch with resynchronization (SIB19, R18 extension).
  static du_high_unit_sat_switch_config sat_switch_config;
  CLI::App*                             sat_switch_subcmd =
      add_subcommand(app, "sat_switch_with_resync", "Satellite switch with resynchronization parameters");
  configure_cli11_sat_switch_with_resync(*sat_switch_subcmd, sat_switch_config);
  sat_switch_subcmd->parse_complete_callback([&]() {
    if (app.get_subcommand("sat_switch_with_resync")->count() != 0) {
      serv_cell_ntn_config.sat_switch_with_resync = sat_switch_config;
    }
  });

  // NTN neighbor cells.
  configure_cli11_ncells(app, config.ncells);
}

void ocudu::configure_cli11_cell_ntn_args(CLI::App& app, std::optional<du_high_unit_cell_ntn_config>& cell_ntn_params)
{
  static du_high_unit_cell_ntn_config         ntn_cfg;
  static du_high_unit_ntn_serving_cell_config serving_cfg;
  CLI::App* ntn_subcmd = add_subcommand(app, "ntn", "NTN configuration")->configurable();

  // An NTN serving cell always has a cell_specific_koffset; a TN-band cell that only reports NTN neighbor
  // cells does not, so this is used to decide whether \c serving should be populated.
  if (not cell_ntn_params.has_value()) {
    // Configure NTN options.
    configure_cli11_ntn_args(*ntn_subcmd, ntn_cfg, serving_cfg);
    auto ntn_verify_callback = [&]() {
      CLI::App* ntn_sub_cmd = app.get_subcommand("ntn");
      if (ntn_sub_cmd->count() != 0) {
        if (serving_cfg.cell_specific_koffset.count() != 0) {
          ntn_cfg.serving = serving_cfg;
        }
        cell_ntn_params = ntn_cfg;
      }
    };
    ntn_subcmd->parse_complete_callback(ntn_verify_callback);
  } else {
    // Seed the scratch serving-cell config from any value already loaded from YAML, so CLI options only override
    // the fields they explicitly set.
    if (cell_ntn_params->serving) {
      serving_cfg = *cell_ntn_params->serving;
    }
    configure_cli11_ntn_args(*ntn_subcmd, *cell_ntn_params, serving_cfg);
    ntn_subcmd->parse_complete_callback([&cell_ntn_params]() {
      if (serving_cfg.cell_specific_koffset.count() != 0) {
        cell_ntn_params->serving = serving_cfg;
      }
    });
  }
}
