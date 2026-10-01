// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "../support/prach_context_repository.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ofh/ofh_uplane_rx_symbol_notifier.h"

namespace ocudu {
namespace ofh {

/// Open Fronthaul User-Plane PRACH data flow notifier.
class uplane_prach_data_flow_notifier
{
public:
  uplane_prach_data_flow_notifier(ocudulog::basic_logger&                    logger_,
                                  std::shared_ptr<prach_context_repository>  prach_context_repo_,
                                  std::shared_ptr<uplane_rx_symbol_notifier> notifier_) :
    logger(logger_), prach_context_repo(std::move(prach_context_repo_)), notifier(std::move(notifier_))
  {
    ocudu_assert(prach_context_repo, "Invalid PRACH context repository");
    ocudu_assert(notifier, "Invalid notifier pointer");
  }

  /// Notifies the prach for the given slot.
  void notify_prach(slot_point slot);

private:
  ocudulog::basic_logger&                    logger;
  std::shared_ptr<prach_context_repository>  prach_context_repo;
  std::shared_ptr<uplane_rx_symbol_notifier> notifier;
};

} // namespace ofh
} // namespace ocudu
