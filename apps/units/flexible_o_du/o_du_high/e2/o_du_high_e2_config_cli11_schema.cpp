// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "o_du_high_e2_config_cli11_schema.h"
#include "apps/helpers/e2/e2_cli11_schema.h"
#include "o_du_high_e2_config.h"
#include "ocudu/support/cli11_utils.h"

using namespace ocudu;

static void configure_cli11_pcap_args(CLI::App& app, o_du_high_e2_pcap_config& pcap_params)
{
  add_option(app, "--e2ap_du_filename", pcap_params.filename, "E2AP PCAP file output path")->capture_default_str();
  add_option(app, "--e2ap_enable", pcap_params.enabled, "Enable E2AP packet capture")->always_capture_default();
}

void ocudu::configure_cli11_with_o_du_high_e2_config_schema(CLI::App& app, o_du_high_e2_config& config)
{
  // PCAP section.
  CLI::App* pcap_subcmd = add_subcommand(app, "pcap", "PCAP configuration")->configurable();
  configure_cli11_pcap_args(*pcap_subcmd, config.pcaps);

  // E2 section.
  configure_cli11_with_e2_config_schema(app, config.base_cfg, "--enable_du_e2", "Enable DU E2 agent");
}

void ocudu::autoderive_o_du_high_e2_parameters_after_parsing(o_du_high_e2_config& unit_cfg)
{
  // If O-DU E2 agent is disabled do not enable E2AP PCAP for it.
  unit_cfg.pcaps.enabled = unit_cfg.base_cfg.enable_unit_e2 && unit_cfg.pcaps.enabled;
}
