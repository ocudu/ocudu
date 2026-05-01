// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1ap/f1ap_ue_id_types.h"
#include "ocudu/f1ap/gateways/f1c_connection_client.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/rnti.h"
#include <functional>
#include <memory>
#include <unordered_map>

namespace ocudu::odu {

class f1ap_du;

/// Notifier used by the F1-c client test mode adapter to notify the test mode controller about F1AP events.
class f1c_test_mode_event_notifier
{
public:
  virtual ~f1c_test_mode_event_notifier() = default;

  /// Called from F1-c client when it detects the removal of a UE.
  virtual void on_ue_removed(rnti_t rnti) = 0;
};

/// F1C connection client decorator for DU test mode.
///
/// Intercepts outgoing F1AP PDUs (DU → CU direction): captures gnb_du_ue_f1ap_id for
/// test-mode UEs and injects mock CU responses (F1 Setup Response, DL RRC Message,
/// UE Context Release, F1 Removal Response) so the DU can operate without a real CU.
class f1c_du_test_mode_adapter : public f1c_connection_client
{
public:
  f1c_du_test_mode_adapter(f1c_connection_client&                        f1c_client_,
                           std::unique_ptr<f1c_test_mode_event_notifier> ev_notifier_,
                           ocudulog::basic_logger&                       logger_);

  std::unique_ptr<f1ap_message_notifier>
  handle_du_connection_request(std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier) override;

  bool release_ue(gnb_du_ue_f1ap_id_t du_ue_id);
  bool try_release_ue(rnti_t rnti);

private:
  struct ue_context {
    gnb_du_ue_f1ap_id_t                du_ue_id;
    std::optional<gnb_cu_ue_f1ap_id_t> cu_ue_id;
    rnti_t                             rnti;
  };

  void handle_tx_pdu(const f1ap_message& msg);
  void handle_tx_init(const f1ap_message& msg);
  void handle_tx_success(const f1ap_message& msg);
  void handle_tx_notifier_removed();

  f1c_connection_client&                        f1c_client;
  std::unique_ptr<f1c_test_mode_event_notifier> ev_notifier;
  ocudulog::basic_logger&                       logger;

  /// Registered notifier to send F1AP PDUs back to F1AP-DU.
  std::unique_ptr<f1ap_message_notifier> rx_notifier;
  std::unique_ptr<f1ap_message_notifier> tx_upstream;

  std::unordered_map<gnb_du_ue_f1ap_id_t, ue_context> du_ue_ids;

  unsigned next_cu_ue_id = 0;
};

} // namespace ocudu::odu
