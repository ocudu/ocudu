// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/gateways/sctp_dtls_mode.h"
#include <string>

namespace ocudu {

struct dtls_context_config {
  dtls_mode   mode;
  std::string session_id;
  std::string cert_filename;
  std::string key_filename;
};

} // namespace ocudu
