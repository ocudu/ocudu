// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "xnc_connection_manager.h"
#include "ocudu/support/async/async_task_scheduler.h"
#include "ocudu/support/async/async_timer.h"
#include "ocudu/xnap/xnap_message.h"
#include "ocudu/xnap/xnap_message_notifier.h"

using namespace ocudu;
using namespace ocucp;

/// Context of a XNC connection which is shared between the xnc_connection_manager and the xnap_message_notifier.
class xnc_connection_manager::shared_xnc_connection_context
{
public:
  shared_xnc_connection_context(xnc_connection_manager& parent_) : parent(parent_) {}
  shared_xnc_connection_context(const shared_xnc_connection_context&)            = delete;
  shared_xnc_connection_context(shared_xnc_connection_context&&)                 = delete;
  shared_xnc_connection_context& operator=(const shared_xnc_connection_context&) = delete;
  shared_xnc_connection_context& operator=(shared_xnc_connection_context&&)      = delete;
  ~shared_xnc_connection_context() { disconnect(); }

  /// Assign a XNC repository index to the context. This is called when the SCTP assocation is created and
  /// attached to an XNAP handler.
  void connect_xnc(xnc_peer_index_t xnc_idx_)
  {
    xnc_idx     = xnc_idx_;
    msg_handler = parent.xnaps.find_xnap(xnc_idx);
  }

  /// Determines whether an XNAP message handlerrepository has been attached to this association.
  bool connected() const { return msg_handler != nullptr; }

  /// Deletes the associated XNC repository, if it exists.
  void disconnect()
  {
    if (not connected()) {
      // XNC was never allocated or was already removed.
      return;
    }

    // Notify the CU-CP that the connection is closed.
    parent.handle_xnc_gw_connection_closed(xnc_idx);

    xnc_idx     = xnc_peer_index_t::invalid;
    msg_handler = nullptr;
  }

  /// Handle XNAP message coming from the SCTP GW.
  void handle_message(const xnap_message& msg)
  {
    if (not connected()) {
      parent.logger.warning("Discarding Rx XNAP message. Cause: CU-CP XN-C connection has been closed");
      return;
    }

    // Forward message.
    msg_handler->handle_message(msg);
  }

private:
  xnc_connection_manager& parent;
  xnc_peer_index_t        xnc_idx     = xnc_peer_index_t::invalid;
  xnap_message_handler*   msg_handler = nullptr;
};

/// Notifier used to forward Rx XNAP messages from the XN-C GW to CU-CP in a thread safe manner.
class xnc_connection_manager::xnc_gw_to_cu_cp_pdu_adapter final : public xnap_message_notifier
{
public:
  xnc_gw_to_cu_cp_pdu_adapter(xnc_connection_manager& parent_, std::shared_ptr<shared_xnc_connection_context> ctxt_) :
    parent(parent_), ctxt(std::move(ctxt_))
  {
  }

  ~xnc_gw_to_cu_cp_pdu_adapter() override
  {
    // Defer destruction of context to CU-CP execution context.
    // Note: We make a copy of the shared_ptr of the context to extend its lifetime to when the defer callback actually
    // gets executed.
    // Note: We don't use move because the defer may fail.
    while (not parent.cu_cp_exec.defer([ctxt_cpy = ctxt]() { ctxt_cpy->disconnect(); })) {
      parent.logger.error("Failed to schedule XNC CU-CP removal task. Retrying...");
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  bool on_new_message(const xnap_message& msg) override
  {
    // Dispatch the XNAP Rx message handling to the CU-CP executor.
    while (not parent.cu_cp_exec.execute([this, msg]() { ctxt->handle_message(msg); })) {
      parent.logger.error("Failed to dispatch XNAP message to CU-CP. Retrying...");
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return true;
  }

private:
  xnc_connection_manager&                        parent;
  std::shared_ptr<shared_xnc_connection_context> ctxt;
};

xnc_connection_manager::xnc_connection_manager(xnap_repository&                            xnaps_,
                                               const std::vector<xnc_connection_gateway*>& xnc_gws_,
                                               timer_manager&                              timers_,
                                               task_executor&                              cu_cp_exec_,
                                               async_task_scheduler&                       common_task_sched_) :
  xnaps(xnaps_),
  xnc_gws(xnc_gws_),
  timers(timers_),
  cu_cp_exec(cu_cp_exec_),
  common_task_sched(common_task_sched_),
  logger(ocudulog::fetch_basic_logger("CU-CP"))
{
}

void xnc_connection_manager::register_peer_gateway(xnc_peer_index_t xnc_idx, xnc_connection_gateway* gateway)
{
  xnc_gateways[xnc_idx] = gateway;
}

void xnc_connection_manager::start(const xnap_configuration& xnap_cfg_)
{
  xnap_cfg = xnap_cfg_;

  if (xnap_cfg.no_connection_init) {
    logger.info("XNAP no_connection_init mode: skipping outbound connections, accepting inbound only");
    return;
  }

  auto xnaps_map = xnaps.get_xnaps();
  for (auto& xnap : xnaps_map) {
    auto  xnc_idx = xnap.first;
    auto* xnap_if = xnap.second;

    auto peer_addrs_opt = xnaps.get_peer_addrs(xnc_idx);
    if (!peer_addrs_opt.has_value() || peer_addrs_opt->empty()) {
      logger.warning("No peer address for XN-C peer {}", xnc_idx);
      continue;
    }
    auto peer_addrs = std::move(*peer_addrs_opt);

    auto gw_it = xnc_gateways.find(xnc_idx);
    if (gw_it == xnc_gateways.end() || gw_it->second == nullptr) {
      logger.warning("No XnAP gateway for XN-C peer {}", xnc_idx);
      continue;
    }
    xnc_connection_gateway* gateway = gw_it->second;

    xnaps.get_xnap_task_scheduler().handle_xnc_async_task(
        xnc_idx,
        launch_async([this, xnc_idx, xnap_if, peer_addrs = std::move(peer_addrs), gateway, connect_result = false](
                         coro_context<async_task<void>>& ctx) mutable {
          CORO_BEGIN(ctx);
          // Establish the SCTP association via the XnAP gateway assigned to this peer.
          CORO_AWAIT_VALUE(connect_result, gateway->connect_to_peer(peer_addrs));
          if (!connect_result) {
            logger.warning("Failed to connect to XN-C peer at {}. Scheduling reconnection in {}...",
                           peer_addrs.front(),
                           std::chrono::duration_cast<std::chrono::seconds>(xnap_cfg.reconnect_timer));
            reconnect_peer(xnc_idx, peer_addrs, gateway);
            CORO_EARLY_RETURN();
          }
          // Trigger XN Setup on the established association.
          CORO_AWAIT(xnap_if->handle_xn_setup_request_required());

          CORO_RETURN();
        }));
  }
}

void xnc_connection_manager::stop()
{
  // Note: Called from outside of the CU-CP execution context.
  stop_completed = false;
  stopped        = true;

  while (not cu_cp_exec.execute([this]() mutable {
    if (xnc_connections.empty()) {
      // No XNAPs connected. Notify completion.
      std::unique_lock<std::mutex> lock(stop_mutex);
      stop_completed = true;
      stop_cvar.notify_one();
      return;
    }

    // For each created XNAP connection context, launch the deletion routine.
    std::vector<xnc_peer_index_t> xnc_idxs;
    xnc_idxs.reserve(xnc_connections.size());
    for (const auto& [xnc_idx, ctxt] : xnc_connections) {
      xnc_idxs.push_back(xnc_idx);
    }
    for (xnc_peer_index_t xnc_idx : xnc_idxs) {
      // Disconnect CU-UP notifier.
      xnc_connections[xnc_idx]->disconnect();
    }
  })) {
    logger.debug("Failed to dispatch CU-CP CU-UP disconnection task. Retrying...");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Wait for XNAP stop to complete.
  {
    std::unique_lock<std::mutex> lock(stop_mutex);
    stop_cvar.wait(lock, [this] { return stop_completed; });
  }

  for (auto* gateway : xnc_gws) {
    if (gateway != nullptr) {
      gateway->stop();
    }
  }
}

std::unique_ptr<xnap_message_notifier>
xnc_connection_manager::handle_new_xnc_cu_cp_connection(std::unique_ptr<xnap_message_notifier> xnap_tx_pdu_notifier,
                                                        const sctp_association_info&           assoc_info)
{
  // Note: This function may be called from a different execution context than the CU-CP.
  if (stopped.load(std::memory_order_acquire)) {
    // CU-CP is in the process of being stopped.
    return nullptr;
  }

  // We create a "detached" notifier, that has no associated XNAP yet.
  auto shared_ctxt     = std::make_shared<shared_xnc_connection_context>(*this);
  auto rx_pdu_notifier = std::make_unique<xnc_gw_to_cu_cp_pdu_adapter>(*this, shared_ctxt);

  // Find XNAP neighbour. This needs to be done over the CU-CP execution context, so
  // we dispatch the task to find the correct XNAP and "attach" it to the notifier
  while (not cu_cp_exec.execute(
      [this, shared_ctxt, sender_notifier = std::move(xnap_tx_pdu_notifier), addr = assoc_info.peer_addr]() mutable {
        // Find XNAP based on address of peer.
        xnc_peer_index_t xnc_index = xnaps.find_xnap(addr);
        if (xnc_index == xnc_peer_index_t::invalid) {
          logger.warning("Rejecting new CU-CP connection. Cause: Failed to find XNAP for peer address {}", addr);
          return;
        }

        // Reject if there is already an active connection for this peer.
        if (xnc_connections.find(xnc_index) != xnc_connections.end()) {
          logger.warning("Rejecting new CU-CP connection. Cause: XN-C peer {} is already connected", xnc_index);
          return;
        }

        xnaps.connect_association(xnc_index, std::move(sender_notifier));

        // Register the XNAP peer in the shared XNAP connection context.
        shared_ctxt->connect_xnc(xnc_index);

        if (not xnc_connections.insert(std::make_pair(xnc_index, std::move(shared_ctxt))).second) {
          logger.error("Failed to store new CU-CP connection {}", fmt::underlying(xnc_index));
          return;
        }

        logger.debug("Added TNL association to XN-C {}", xnc_index);
      })) {
    logger.debug("Failed to dispatch CU-CP connection task. Retrying...");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  return rx_pdu_notifier;
}

void xnc_connection_manager::handle_xnc_gw_connection_closed(xnc_peer_index_t xnc_idx)
{
  // Note: Called from within CU-CP execution context.

  // Save peer addresses before removal so we can recreate the XNAP.
  std::optional<std::vector<transport_layer_address>> peer_addrs = xnaps.get_peer_addrs(xnc_idx);

  common_task_sched.schedule(
      launch_async([this, xnc_idx, peer_addrs = std::move(peer_addrs)](coro_context<async_task<void>>& ctx) {
        CORO_BEGIN(ctx);
        if (xnaps.find_xnap(xnc_idx) == nullptr) {
          // XN-C was already removed.
          CORO_EARLY_RETURN();
        }

        // Clear any pending reconnection tasks for this peer before removing it.
        xnaps.get_xnap_task_scheduler().clear_pending_tasks(xnc_idx);

        // Await for clean removal of the XNAP from the repository.
        CORO_AWAIT(xnaps.remove_xnap(xnc_idx));

        // Mark the connection as closed.
        xnc_connections.erase(xnc_idx);

        if (stopped) {
          // Do not recreate the XNAP instance on shutdown, return early instead.
          if (xnc_connections.empty()) {
            std::unique_lock<std::mutex> lock(stop_mutex);
            stop_completed = true;
            stop_cvar.notify_one();
          }
          CORO_EARLY_RETURN();
        }

        // Recreate a fresh XNAP instance so that the peer can reconnect.
        if (peer_addrs.has_value() && !peer_addrs->empty()) {
          if (xnaps.add_xnap(xnc_idx, peer_addrs.value(), xnap_cfg) == nullptr) {
            logger.error("Failed to recreate XNAP instance for peer address {}", peer_addrs->front());
          } else if (xnap_cfg.no_connection_init) {
            logger.info(
                "XN-C peer {} disconnected. Recreated XNAP, waiting for inbound reconnection (no_connection_init mode)",
                xnc_idx);
          } else {
            logger.info("XN-C peer {} disconnected. Recreated XNAP, scheduling reconnection in {}...",
                        xnc_idx,
                        std::chrono::duration_cast<std::chrono::seconds>(xnap_cfg.reconnect_timer));
            auto gw_it = xnc_gateways.find(xnc_idx);
            if (gw_it != xnc_gateways.end() && gw_it->second != nullptr) {
              // Schedule outbound reconnection attempt.
              reconnect_peer(xnc_idx, peer_addrs.value(), gw_it->second);
            }
          }
        }

        CORO_RETURN();
      }));
}

void xnc_connection_manager::reconnect_peer(xnc_peer_index_t                            xnc_idx,
                                            const std::vector<transport_layer_address>& peer_addrs,
                                            xnc_connection_gateway*                     xnc_gw)
{
  // Schedule on the per-peer task scheduler so the reconnect coroutine does not block the common task queue.
  xnaps.get_xnap_task_scheduler().handle_xnc_async_task(
      xnc_idx,
      launch_async([this,
                    xnc_idx,
                    peer_addrs,
                    xnc_gw,
                    retry_timer    = unique_timer{timer_factory{timers, cu_cp_exec}.create_timer()},
                    connect_result = false,
                    xnap_if = static_cast<xnap_interface*>(nullptr)](coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);

        logger.info("XN-C peer {}: Scheduling reconnection in {}...", xnc_idx, xnap_cfg.reconnect_timer);
        CORO_AWAIT(async_wait_for(retry_timer, xnap_cfg.reconnect_timer));

        // Skip if shutting down or peer was already reconnected (e.g. by an inbound connection).
        if (stopped or xnc_connections.count(xnc_idx) > 0) {
          CORO_EARLY_RETURN();
        }

        // Establish SCTP association via the XnAP gateway assigned to this peer.
        CORO_AWAIT_VALUE(connect_result, xnc_gw->connect_to_peer(peer_addrs));
        if (!connect_result) {
          CORO_EARLY_RETURN();
        }

        // Trigger XN Setup on the re-established association.
        xnap_if = xnaps.find_xnap(xnc_idx);
        if (xnap_if != nullptr) {
          CORO_AWAIT(xnap_if->handle_xn_setup_request_required());
        }

        CORO_RETURN();
      }));
}
