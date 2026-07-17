// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/cu_cp_types.h"

namespace ocudu::ocucp {

class du_processor_repository;
struct cu_cp_paging_message;

/// Paging message handler dependencies.
struct paging_message_handler_dependencies {
  du_processor_repository& dus;
  ocudulog::basic_logger&  logger;
};

/// Class responsible for handling incoming paging messages and forwarding them to the appropriate DUs.
class paging_message_handler
{
public:
  explicit paging_message_handler(const paging_message_handler_dependencies& dependencies);

  /// Handle Paging message sent by the core network and distribute across the served DU cells.
  void handle_paging_message(const cu_cp_paging_message& msg) const;

private:
  /// Handles the DU paging message.
  bool handle_du_paging_message(cu_cp_du_index_t du_index, const cu_cp_paging_message& msg) const;

  du_processor_repository& dus;
  ocudulog::basic_logger&  logger;
};

} // namespace ocudu::ocucp
