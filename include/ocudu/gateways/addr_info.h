// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <netinet/in.h>

inline const char* ipproto_to_string(int protocol)
{
  switch (protocol) {
    case IPPROTO_SCTP:
      return "SCTP";
    case IPPROTO_UDP:
      return "UDP";
    case IPPROTO_IP:
      return "IP";
    case IPPROTO_IPV6:
      return "IPV6";
    case IPPROTO_ICMP:
      return "ICMP";
    case IPPROTO_TCP:
      return "TCP";
    default:
      break;
  }
  return "unknown protocol";
}
