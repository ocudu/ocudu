// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "cu_cp_unit_logger_config.h"
#include "ocudu/ocudulog/ocudulog.h"

namespace ocudu {

/// Registers the CU-CP loggers in the logger service.
inline void register_cu_cp_loggers(const cu_cp_unit_logger_config& log_cfg)
{
  for (const auto& id : {"CU-CP", "CU-UEMNG"}) {
    auto& cu_cp_logger = ocudulog::fetch_basic_logger(id, false);
    cu_cp_logger.set_level(log_cfg.cu_level);
    cu_cp_logger.set_hex_dump_max_size(log_cfg.hex_max_size);
  }

  auto& pdcp_logger = ocudulog::fetch_basic_logger("PDCP", false);
  pdcp_logger.set_level(log_cfg.pdcp_level);
  pdcp_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& rrc_logger = ocudulog::fetch_basic_logger("RRC", false);
  rrc_logger.set_level(log_cfg.rrc_level);
  rrc_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& cu_e1ap_logger = ocudulog::fetch_basic_logger("CU-CP-E1", false);
  cu_e1ap_logger.set_level(log_cfg.e1ap_level);
  cu_e1ap_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& cu_f1ap_logger = ocudulog::fetch_basic_logger("CU-CP-F1", false);
  cu_f1ap_logger.set_level(log_cfg.f1ap_level);
  cu_f1ap_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& ngap_logger = ocudulog::fetch_basic_logger("NGAP", false);
  ngap_logger.set_level(log_cfg.ngap_level);
  ngap_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& xnap_logger = ocudulog::fetch_basic_logger("XNAP", false);
  xnap_logger.set_level(log_cfg.xnap_level);
  xnap_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& nrppa_logger = ocudulog::fetch_basic_logger("NRPPA", false);
  nrppa_logger.set_level(log_cfg.nrppa_level);
  nrppa_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& sec_logger = ocudulog::fetch_basic_logger("SEC", false);
  sec_logger.set_level(log_cfg.sec_level);
  sec_logger.set_hex_dump_max_size(log_cfg.hex_max_size);
}

} // namespace ocudu
