// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_high_config.h"

namespace ocudu {

/// Registers the DU high loggers in the logger service.
inline void register_du_high_loggers(const du_high_unit_logger_config& log_cfg)
{
  for (const auto& id : {"DU", "DU-MNG", "UE-MNG"}) {
    auto& du_logger = ocudulog::fetch_basic_logger(id, false);
    du_logger.set_level(log_cfg.du_level);
    du_logger.set_hex_dump_max_size(log_cfg.hex_max_size);
  }

  for (const auto& id : {"MAC", "SCHED"}) {
    auto& mac_logger = ocudulog::fetch_basic_logger(id, true);
    mac_logger.set_level(log_cfg.mac_level);
    mac_logger.set_hex_dump_max_size(log_cfg.hex_max_size);
  }

  auto& rlc_logger = ocudulog::fetch_basic_logger("RLC", false);
  rlc_logger.set_level(log_cfg.rlc_level);
  rlc_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& du_f1ap_logger = ocudulog::fetch_basic_logger("DU-F1", false);
  du_f1ap_logger.set_level(log_cfg.f1ap_level);
  du_f1ap_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& f1u_logger = ocudulog::fetch_basic_logger("DU-F1-U", false);
  f1u_logger.set_level(log_cfg.f1u_level);
  f1u_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& gtpu_logger = ocudulog::fetch_basic_logger("GTPU", false);
  gtpu_logger.set_level(log_cfg.gtpu_level);
  gtpu_logger.set_hex_dump_max_size(log_cfg.hex_max_size);
}

} // namespace ocudu
