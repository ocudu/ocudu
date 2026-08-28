// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/gtpu/gtpu_echo.h"
#include "ocudu/gtpu/gtpu_tunnel_common_tx.h"
#include "ocudu/pcap/dlt_pcap.h"

/// This factory header file depends on the GTP-U tunnel interfaces (see above include list). It is kept separate as
/// clients of the GTP-U tunnel interfaces do not need to call factory methods.
namespace ocudu {

struct gtpu_echo_creation_message {
  dlt_pcap&                                   gtpu_pcap;
  gtpu_tunnel_common_tx_upper_layer_notifier& tx_upper;
};

/// Creates an instance of a GTP-U entity.
std::unique_ptr<gtpu_echo> create_gtpu_echo(gtpu_echo_creation_message& msg);

} // namespace ocudu
