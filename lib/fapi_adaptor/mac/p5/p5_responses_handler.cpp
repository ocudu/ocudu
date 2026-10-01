// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "p5_responses_handler.h"
#include "p5_transaction_outcome_manager.h"
#include "ocudu/fapi/common/error_indication.h"
#include "ocudu/fapi/p5/p5_messages.h"

using namespace ocudu;
using namespace fapi_adaptor;

void p5_responses_handler::on_param_response(const fapi::param_response& msg)
{
  auto token = stop_manager.get_token();
  if (OCUDU_UNLIKELY(token.is_stop_requested())) {
    return;
  }

  if (!fapi_ctrl_executor.defer([this, error_code = msg.error_code, tk = std::move(token)]() {
        // Set the transaction result.
        transaction_manager.param_response_outcome.set(error_code == fapi::error_code_id::msg_ok);
      })) {
    logger.warning("FAPI control executor: failed to enqueue task to set the PARAM.response awaitable");
  }
}

void p5_responses_handler::on_config_response(const fapi::config_response& msg)
{
  // Reset the SLOT.indication processed flag.
  slot_indication_processed.store(false, std::memory_order_relaxed);

  auto token = stop_manager.get_token();
  if (OCUDU_UNLIKELY(token.is_stop_requested())) {
    return;
  }

  if (!fapi_ctrl_executor.defer([this, error_code = msg.error_code, tk = std::move(token)]() {
        // Set the transaction result.
        transaction_manager.config_response_outcome.set(error_code == fapi::error_code_id::msg_ok);
      })) {
    logger.warning("FAPI control executor: failed to enqueue task to set the CONFIG.response awaitable");
  }
}

void p5_responses_handler::on_stop_indication(const fapi::stop_indication& msg)
{
  auto token = stop_manager.get_token();
  if (OCUDU_UNLIKELY(token.is_stop_requested())) {
    return;
  }

  if (!fapi_ctrl_executor.defer([this, tk = std::move(token)]() {
        // Receiving the STOP.indication message means the stop procedure finished successfully. In case of error, it
        // gets reported by an ERROR.indication.
        transaction_manager.stop_response_outcome.set(true);
      })) {
    logger.warning("FAPI control executor: failed to enqueue task to set the STOP.indication awaitable");
  }
}

void p5_responses_handler::handle_slot_indication(const mac_cell_timing_context& context)
{
  // Set the outcome of the transaction using the first SLOT.indication. Ignore after that first SLOT.indication. This
  // avoids to set the outcome of the transaction for every SLOT.indication.
  if (OCUDU_LIKELY(slot_indication_processed.load(std::memory_order_relaxed))) {
    return;
  }

  auto token = stop_manager.get_token();
  if (OCUDU_UNLIKELY(token.is_stop_requested())) {
    return;
  }

  if (!fapi_ctrl_executor.defer([this, tk = std::move(token)]() {
        transaction_manager.start_outcome.set(true);
        // Mark the SLOT.indication processed flag.
        slot_indication_processed.store(true, std::memory_order_relaxed);
      })) {
    logger.warning("FAPI control executor: failed to enqueue task to set the START.request awaitable");
  }
}

void p5_responses_handler::on_error_indication(const fapi::error_indication& msg)
{
  auto token = stop_manager.get_token();
  if (OCUDU_UNLIKELY(token.is_stop_requested())) {
    return;
  }

  if (!fapi_ctrl_executor.defer([this, msg_id = msg.message_id, tk = std::move(token)]() {
        switch (msg_id) {
          case fapi::message_type_id::param_request:
            logger.warning("Dropped ERROR.indication message as PARAM.request does not trigger an ERROR.indication");
            return;
          case fapi::message_type_id::config_request:
            logger.warning("Dropped ERROR.indication message as CONFIG.request does not trigger an ERROR.indication");
            return;
          case fapi::message_type_id::start_request:
            transaction_manager.start_outcome.set(false);
            return;
          case fapi::message_type_id::stop_request:
            transaction_manager.stop_response_outcome.set(false);
            return;
          default:
            break;
        }
      })) {
    logger.warning("FAPI control executor: failed to enqueue task to set the start/stop coroutines awaitable");
  }
}
