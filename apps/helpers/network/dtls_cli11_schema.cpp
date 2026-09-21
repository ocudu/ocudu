// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "dtls_cli11_schema.h"
#include "dtls_appconfig.h"
#include "ocudu/support/cli11_utils.h"

using namespace ocudu;

void ocudu::configure_cli11_dtls_args(CLI::App& app, dtls_appconfig& config)
{
  add_option(app, "--mode", config.mode, "DTLS mode")
      ->transform(CLI::CheckedTransformer(std::map<std::string, dtls_appconfig_mode>{
          {"server", dtls_appconfig_mode::server}, {"client", dtls_appconfig_mode::client}}));
  add_option(app, "--cert_filename", config.cert_filename, "DTLS certificate filename (PEM format)");
  add_option(app, "--key_filename", config.key_filename, "DTLS key filename (PEM format)");
  add_option(app, "--ca_cert_filename", config.ca_cert_filename, "DTLS CA certificate filename (PEM format)");
  app.callback([&config]() { config.enabled = true; });
}
