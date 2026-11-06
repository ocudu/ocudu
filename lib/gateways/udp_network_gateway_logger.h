// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/format/fmt_to_c_str.h"
#include "ocudu/support/format/prefixed_logger.h"
#include "fmt/format.h"
#include <string.h>

namespace ocudu {

class udp_gateway_log_prefix
{
public:
  udp_gateway_log_prefix(const std::string& if_name)
  {
    fmt::memory_buffer buffer;
    fmt::format_to(std::back_inserter(buffer), "if={}: ", if_name);
    prefix = ocudu::to_c_str(buffer);
  }
  const char* to_c_str() const { return prefix.c_str(); }

private:
  std::string prefix;
};

inline const char* format_as(const udp_gateway_log_prefix& o)
{
  return o.to_c_str();
}

using udp_gateway_logger = prefixed_logger<udp_gateway_log_prefix>;

} // namespace ocudu
