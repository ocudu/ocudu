// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1u_appconfig_validator.h"
#include "f1u_appconfig.h"

using namespace ocudu;

bool ocudu::validate_f1u_sockets_appconfig(const f1u_sockets_appconfig& config)
{
  for (const f1u_socket_appconfig& f1u_socket : config.f1u_socket_cfg) {
    if (f1u_socket.sst.has_value() && not f1u_socket.five_qi.has_value()) {
      if (f1u_socket.sd.has_value()) {
        fmt::println(
            "F1-U socket has S-NSSAI configured, but no associated 5QI. Please, configure 5QI too. sst={} sd={:#x}",
            *f1u_socket.sst,
            *f1u_socket.sd);
      } else {
        fmt::println("F1-U socket has S-NSSAI configured, but no associated 5QI. Please, configure 5QI too. sst={}",
                     *f1u_socket.sst);
      }
      return false;
    }
    if (f1u_socket.five_qi.has_value() && not f1u_socket.sst.has_value()) {
      fmt::println("F1-U socket has 5QI configured, but no associated S-NSSAI. Please, configure the SST too and "
                   "(optionaly) the SD. {}",
                   *f1u_socket.five_qi);
      return false;
    }
  }
  return true;
}
