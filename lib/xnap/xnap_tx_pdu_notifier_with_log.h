// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "log_helpers.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/xnap/xnap_message.h"
#include "ocudu/xnap/xnap_message_notifier.h"

namespace ocudu::ocucp {

class xnap_tx_pdu_notifier_with_logging final : public xnap_message_notifier
{
public:
  xnap_tx_pdu_notifier_with_logging() : logger(ocudulog::fetch_basic_logger("XNAP", false)) {}

  ~xnap_tx_pdu_notifier_with_logging() override
  {
    if (decorated) {
      decorated.reset();
    }
  }

  void connect(std::unique_ptr<xnap_message_notifier> decorated_) { decorated = std::move(decorated_); }

  void disconnect()
  {
    if (is_connected()) {
      decorated.reset();
    }
  }

  bool is_connected() const { return decorated != nullptr; }

  [[nodiscard]] bool on_new_message(const xnap_message& msg) override
  {
    log_xnap_pdu(logger, logger.debug.enabled(), false, msg.pdu);

    if (decorated == nullptr) {
      return false;
    }
    return decorated->on_new_message(msg);
  }

private:
  ocudulog::basic_logger&                logger;
  std::unique_ptr<xnap_message_notifier> decorated;
};
} // namespace ocudu::ocucp
