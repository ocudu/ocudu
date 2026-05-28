// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../cu_cp_controller/node_connection_notifier.h"
#include "du_configuration_handler.h"
#include "ocudu/cu_cp/cu_cp_configuration.h"

namespace ocudu::ocucp {

struct du_processor_config_t {
  cu_cp_du_index_t                          du_index = cu_cp_du_index_t::invalid;
  const cu_cp_configuration&                cu_cp_cfg;
  ocudulog::basic_logger&                   logger         = ocudulog::fetch_basic_logger("CU-CP");
  du_connection_notifier*                   du_setup_notif = nullptr;
  std::unique_ptr<du_configuration_handler> du_cfg_hdlr;
};

} // namespace ocudu::ocucp
