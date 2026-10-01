// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/common/error_indication.h"

namespace ocudu {
namespace fapi {

/// \brief Builds and returns an ERROR.indication message with the given parameters, as per SCF-222 v4.0 section 3.3.6.1
/// in table ERROR.indication message body
///
/// This builder is used to build any error code id but OUT_OF_SYNC error.
inline error_indication build_error_indication(slot_point slot, message_type_id msg_id, error_code_id error_id)
{
  error_indication msg;

  msg.slot       = slot;
  msg.message_id = msg_id;
  msg.error_code = error_id;

  return msg;
}

/// \brief Builds and returns an ERROR.indication message with the given parameters, as per SCF-222 v4.0 section 3.3.6.1
/// in table ERROR.indication message body
///
/// This builder is used to build only an OUT_OF_SYNC error code.
inline error_indication
build_out_of_sync_error_indication(slot_point slot, message_type_id msg_id, slot_point expected_slot)
{
  error_indication msg;

  msg.slot          = slot;
  msg.message_id    = msg_id;
  msg.error_code    = error_code_id::out_of_sync;
  msg.expected_slot = expected_slot;

  return msg;
}

/// \brief Builds and returns an ERROR.indication message with the given parameters, as per SCF-222 v4.0 section 3.3.6.1
/// in table ERROR.indication message body
///
/// This builder is used to build only an MSG_INVALID_SFN error code.
inline error_indication
build_invalid_sfn_error_indication(slot_point slot, message_type_id msg_id, slot_point expected_slot)
{
  error_indication msg;

  msg.slot          = slot;
  msg.message_id    = msg_id;
  msg.error_code    = error_code_id::msg_invalid_sfn;
  msg.expected_slot = expected_slot;

  return msg;
}

/// \brief Builds and returns an ERROR.indication message with the given parameters, as per SCF-222 v4.0 section 3.3.6.1
/// in table ERROR.indication message body
///
/// This builder is used to build only a MSG_SLOT_ERR error code.
inline error_indication build_msg_error_indication(slot_point slot, message_type_id msg_id)
{
  error_indication msg;

  msg.slot       = slot;
  msg.message_id = msg_id;
  msg.error_code = error_code_id::msg_slot_err;

  return msg;
}

/// \brief Builds and returns an ERROR.indication message with the given parameters, as per SCF-222 v4.0 section 3.3.6.1
/// in table ERROR.indication message body
///
/// This builder is used to build only a MSG_TX_ERR error code.
inline error_indication build_msg_tx_error_indication(slot_point slot)
{
  error_indication msg;

  msg.slot       = slot;
  msg.message_id = message_type_id::tx_data_request;
  msg.error_code = error_code_id::msg_tx_err;

  return msg;
}

/// \brief Builds and returns an ERROR.indication message with the given parameters, as per SCF-222 v4.0 section 3.3.6.1
/// in table ERROR.indication message body
///
/// This builder is used to build only a MSG_UL_DCI_ERR error code.
inline error_indication build_msg_ul_dci_error_indication(slot_point slot)
{
  error_indication msg;

  msg.slot       = slot;
  msg.message_id = message_type_id::ul_dci_request;
  msg.error_code = error_code_id::msg_ul_dci_err;

  return msg;
}

} // namespace fapi
} // namespace ocudu
