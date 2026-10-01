// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/f1ap/f1ap_message_handler.h"
#include "ocudu/f1ap/f1ap_message_notifier.h"
#include "ocudu/gateways/network_gateway.h"
#include "ocudu/support/error_handling.h"

namespace ocudu {

inline bool is_f1ap_pdu_packable(const asn1::f1ap::f1ap_pdu_c& pdu)
{
  byte_buffer   buffer;
  asn1::bit_ref bref(buffer);
  return pdu.pack(bref) == asn1::OCUDUASN_SUCCESS;
}

/// Reusable notifier class that a) stores the received PDU for test inspection and b)
/// calls the registered PDU handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.
class dummy_f1ap_pdu_notifier : public f1ap_message_notifier
{
public:
  explicit dummy_f1ap_pdu_notifier(f1ap_message_handler* handler_ = nullptr) : handler(handler_) {}
  void attach_handler(f1ap_message_handler* handler_) { handler = handler_; }
  void on_new_message(const f1ap_message& msg) override
  {
    logger.info("Received a PDU of type {}", msg.pdu.type().to_string());
    last_f1ap_msg = msg; // store msg

    if (handler != nullptr) {
      logger.info("Forwarding PDU");
      handler->handle_message(msg);
    }
  }

  f1ap_message last_f1ap_msg;

private:
  ocudulog::basic_logger& logger  = ocudulog::fetch_basic_logger("TEST");
  f1ap_message_handler*   handler = nullptr;
};

/// Reusable class implementing the notifier interface.
class f1ap_null_notifier : public f1ap_message_notifier
{
public:
  f1ap_message last_f1ap_msg;

  void on_new_message(const f1ap_message& msg) override
  {
    ocudulog::basic_logger& test_logger = ocudulog::fetch_basic_logger("TEST");
    test_logger.info("Received PDU");
    last_f1ap_msg = msg;
    if (not is_f1ap_pdu_packable(msg.pdu)) {
      report_fatal_error("Output F1AP message is not packable");
    }
  }
};

/// Dummy handler just printing the received PDU.
class dummy_f1ap_message_handler : public f1ap_message_handler
{
public:
  void handle_message(const f1ap_message& msg) override
  {
    last_msg = msg;
    logger.info("Received a PDU of type {}", msg.pdu.type().to_string());
    if (not is_f1ap_pdu_packable(msg.pdu)) {
      report_fatal_error("Output F1AP message is not packable");
    }
  }

  f1ap_message last_msg;

private:
  ocudulog::basic_logger& logger = ocudulog::fetch_basic_logger("TEST");
};

} // namespace ocudu
