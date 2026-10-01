// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/common/error_indication_notifier.h"
#include "ocudu/fapi/p5/p5_responses_notifier.h"
#include "ocudu/mac/mac_cell_slot_handler.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/support/synchronization/stop_event.h"

namespace ocudu {

class task_executor;

namespace fapi_adaptor {

struct p5_transaction_outcome_manager;

/// \brief FAPI P5 responses handler.
///
/// This class is responsible for receiving, decoding and processing FAPI P5 response messages originating from an L1
/// instance. Its primary function is to interpret the contents of a P5 response and update the state and outcome of the
/// associated FAPI P5 transaction.
class p5_responses_handler : public fapi::p5_responses_notifier,
                             public fapi::error_indication_notifier,
                             public mac_cell_slot_handler
{
public:
  p5_responses_handler(ocudulog::basic_logger&         logger_,
                       p5_transaction_outcome_manager& transaction_manager_,
                       task_executor&                  fapi_ctrl_executor_) :
    logger(logger_), transaction_manager(transaction_manager_), fapi_ctrl_executor(fapi_ctrl_executor_)
  {
  }

  ~p5_responses_handler() override { stop_manager.stop(); }

  // See interface for documentation.
  void on_param_response(const fapi::param_response& msg) override;

  // See interface for documentation.
  void on_config_response(const fapi::config_response& msg) override;

  // See interface for documentation.
  void on_stop_indication(const fapi::stop_indication& msg) override;

  // See interface for documentation.
  void on_error_indication(const fapi::error_indication& msg) override;

  // See interface for documentation.
  void handle_slot_indication(const mac_cell_timing_context& context) override;

  // See interface for documentation.
  void handle_error_indication(slot_point sl_tx, error_event event) override
  {
    // Nothing to do in this handler.
  }

  // See interface for documentation.
  void handle_stop_indication() override
  {
    // Nothing to do in this handler.
  }

private:
  ocudulog::basic_logger&         logger;
  p5_transaction_outcome_manager& transaction_manager;
  task_executor&                  fapi_ctrl_executor;
  stop_event_source               stop_manager;
  std::atomic<bool>               slot_indication_processed{false};
};

} // namespace fapi_adaptor
} // namespace ocudu
