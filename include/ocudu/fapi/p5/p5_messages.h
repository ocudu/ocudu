// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/cell_config.h"
#include "ocudu/fapi/common/error_code.h"

namespace ocudu {
namespace fapi {

/// Param request message.
struct param_request {};

/// Param response message.
struct param_response {
  error_code_id error_code;
};

/// Config request message.
struct config_request {
  cell_configuration cell_cfg;
};

/// Config response message.
struct config_response {
  error_code_id error_code;
};

/// Start request message.
struct start_request {};

/// Start response message.
struct start_response {};

/// Stop request message.
struct stop_request {};

/// Stop indication message.
struct stop_indication {};

} // namespace fapi
} // namespace ocudu
