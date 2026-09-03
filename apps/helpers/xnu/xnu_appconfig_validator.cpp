// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "xnu_appconfig_validator.h"
#include "xnu_appconfig.h"

using namespace ocudu;

bool ocudu::validate_xnu_sockets_appconfig(const xnu_sockets_appconfig& config)
{
  for (const xnu_socket_appconfig& xnu_socket : config.xnu_socket_cfg) {
    if (xnu_socket.sst.has_value() && not xnu_socket.five_qi.has_value()) {
      if (xnu_socket.sd.has_value()) {
        fmt::println(
            "Xn-U socket has S-NSSAI configured, but no associated 5QI. Please, configure 5QI too. sst={} sd={:#x}",
            *xnu_socket.sst,
            *xnu_socket.sd);
      } else {
        fmt::println("Xn-U socket has S-NSSAI configured, but no associated 5QI. Please, configure 5QI too. sst={}",
                     *xnu_socket.sst);
      }
      return false;
    }
    if (xnu_socket.five_qi.has_value() && not xnu_socket.sst.has_value()) {
      fmt::println("Xn-U socket has 5QI configured, but no associated S-NSSAI. Please, configure the SST too and "
                   "(optionaly) the SD. {}",
                   *xnu_socket.five_qi);
      return false;
    }
  }
  return true;
}
