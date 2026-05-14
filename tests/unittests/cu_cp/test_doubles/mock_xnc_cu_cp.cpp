// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "mock_xnc_cu_cp.h"
#include "ocudu/adt/mutexed_mpmc_queue.h"
#include "ocudu/cu_cp/cu_cp_xnc_handler.h"
#include "ocudu/support/async/async_no_op_task.h"
#include "ocudu/xnap/xnap_message.h"

using namespace ocudu;
using namespace ocucp;

/// \brief Mock class for the interface between CU-CP and XNC CU-CP that accounts for the fact that the CU-CP may push
/// PDUs from different threads.
class synchronized_mock_xnc_cu_cp : public mock_xnc_cu_cp
{
public:
  explicit synchronized_mock_xnc_cu_cp() : rx_pdus(1024), pending_tx_pdus(16) {}

  async_task<bool> connect_to_peer(std::vector<transport_layer_address> peer_addrs) override
  {
    // Simulate SCTP_COMM_UP by creating the association (matching real gateway behavior).
    if (xnc_handler != nullptr && rx_pdu_notifier == nullptr) {
      auto xnc_sender = std::make_unique<xnap_tx_notifier>(*this);
      rx_pdu_notifier = xnc_handler->handle_new_xnc_cu_cp_connection(
          std::move(xnc_sender),
          sctp_association_info{.assoc_id = 1, .peer_addr = transport_layer_address::create_from_string("127.0.0.1")});
    }
    return launch_no_op_task(true);
  }

  void stop() override {}

  void attach_cu_cp(cu_cp_xnc_handler& xnc_handler_) override { xnc_handler = &xnc_handler_; }

  std::optional<uint16_t> get_listen_port() const override { return std::nullopt; }

  bool try_pop_rx_pdu(xnap_message& pdu) override { return rx_pdus.try_pop(pdu); }

  void push_tx_pdu(const xnap_message& pdu) override { rx_pdu_notifier->on_new_message(pdu); }

  void enqueue_next_tx_pdu(const xnap_message& pdu) override { pending_tx_pdus.push_blocking(pdu); }

private:
  using xnap_pdu_queue = concurrent_queue<xnap_message,
                                          concurrent_queue_policy::locking_mpmc,
                                          concurrent_queue_wait_policy::condition_variable>;

  class xnap_tx_notifier : public xnap_message_notifier
  {
  public:
    xnap_tx_notifier(synchronized_mock_xnc_cu_cp& parent_) : parent(parent_) {}
    ~xnap_tx_notifier() override = default;

    bool on_new_message(const xnap_message& pdu) override
    {
      // If a PDU response has been previously enqueued, we send it now.
      if (parent.rx_pdu_notifier != nullptr && !parent.pending_tx_pdus.empty()) {
        xnap_message tx_pdu;
        bool         discard = parent.pending_tx_pdus.try_pop(tx_pdu);
        (void)discard;
        parent.push_tx_pdu(tx_pdu);
      }

      bool success = parent.rx_pdus.push_blocking(pdu);
      report_error_if_not(success, "Queue is full");
      return true;
    }

  private:
    synchronized_mock_xnc_cu_cp& parent;
  };

  cu_cp_xnc_handler* xnc_handler = nullptr;

  xnap_pdu_queue rx_pdus;

  std::unique_ptr<xnap_message_notifier> rx_pdu_notifier;

  // Tx PDUs to send once the XNC CU-CP connection is set up.
  xnap_pdu_queue pending_tx_pdus;
};

std::unique_ptr<mock_xnc_cu_cp> ocudu::ocucp::create_mock_xnc_cu_cp()
{
  return std::make_unique<synchronized_mock_xnc_cu_cp>();
}
