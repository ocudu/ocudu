// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "apps/services/remote_control/remote_server.h"
#include "apps/services/remote_control/remote_command.h"
#include "apps/services/remote_control/remote_command_dispatcher.h"
#include "apps/services/remote_control/remote_control_appconfig.h"
#include "nlohmann/json.hpp"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/synchronization/stop_event.h"
#include "ocudu/support/synchronization/sync_event.h"
#include "ocudu/support/versioning/interface_versions.h"
#ifndef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
#include "uWebSockets/App.h"
#ifndef __clang__
#pragma GCC diagnostic pop
#endif
#include "ocudu/adt/scope_exit.h"
#include "ocudu/support/executors/unique_thread.h"
#include <utility>

using namespace ocudu;
using namespace app_services;

namespace {

/// Remote server implementation.
class remote_server_impl : public remote_server,
                           public remote_server_operation_controller,
                           public remote_server_metrics_gateway

{
  /// WebSocket socket type alias.
  struct dummy_type {};
  using socket_type = uWS::WebSocket<false, true, dummy_type>;

  const std::string         bind_addr;
  const unsigned            port;
  const bool                enable_metrics_subscription;
  unique_thread             thread;
  std::atomic<uWS::App*>    server;
  std::atomic<uWS::Loop*>   server_loop;
  remote_command_dispatcher dispatcher;
  std::set<socket_type*>    metrics_subscribers;
  socket_type*              current_cmd_client = nullptr;
  bool                      rejecting_commands = false;
  stop_event_source         stop_control;

  /// Metrics subscription command.
  class metrics_subscribe_command : public remote_command
  {
    remote_server_impl* parent;

  public:
    explicit metrics_subscribe_command(remote_server_impl* parent_) : parent(parent_) {}

    // See interface for documentation.
    std::string_view get_name() const override { return "metrics_subscribe"; }

    // See interface for documentation.
    std::string_view get_description() const override { return "Subscribe to metrics notifications"; }

    // See interface for documentation.
    expected<nlohmann::json, std::string> execute(const nlohmann::json& json) override
    {
      parent->subscribe_metrics_client();
      // Report the version of the metrics the subscriber is going to receive.
      nlohmann::json result;
      result["metrics_version"] = metrics_interface_version;
      return result;
    }
  };

  /// Metrics unsubscription command.
  class metrics_unsubscribe_command : public remote_command
  {
    remote_server_impl* parent;

  public:
    explicit metrics_unsubscribe_command(remote_server_impl* parent_) : parent(parent_) {}

    // See interface for documentation.
    std::string_view get_name() const override { return "metrics_unsubscribe"; }

    // See interface for documentation.
    std::string_view get_description() const override { return "Unsubscribe to metrics notifications"; }

    // See interface for documentation.
    expected<nlohmann::json, std::string> execute(const nlohmann::json& json) override
    {
      parent->unsubscribe_metrics_client();
      return {};
    }
  };

public:
  remote_server_impl(const std::string& bind_addr_, unsigned port_, bool enable_metrics_subscription_) :
    bind_addr(bind_addr_), port(port_), enable_metrics_subscription(enable_metrics_subscription_)
  {
    if (enable_metrics_subscription) {
      // Add the metrics subscription commands.
      dispatcher.add_command(std::make_unique<metrics_subscribe_command>(this));
      dispatcher.add_command(std::make_unique<metrics_unsubscribe_command>(this));
    }
  }

  // See interface for documentation.
  ~remote_server_impl() override { stop(); }

  // See interface for documentation.
  remote_server_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  remote_server_metrics_gateway* get_metrics_gateway() override { return enable_metrics_subscription ? this : nullptr; }

  // See interface for documentation.
  void start() override
  {
    sync_event start_event;
    thread = unique_thread("ws_server", [this, token = start_event.get_token()]() mutable {
      uWS::App ws_server;
      ws_server
          .ws<dummy_type>(
              "/*",
              {.compression              = uWS::CompressOptions(uWS::DISABLED),
               .maxPayloadLength         = 16 * 1024,
               .idleTimeout              = 120,
               .maxBackpressure          = 16 * 1024 * 1024,
               .closeOnBackpressureLimit = false,
               .resetIdleTimeoutOnSend   = false,
               .sendPingsAutomatically   = true,
               .message =
                   [this](socket_type* ws, std::string_view message, uWS::OpCode opCode) {
                     // Only parse text based messages.
                     if (opCode != uWS::OpCode::TEXT) {
                       ws->send(build_remote_error_response("This WebSocket server only supports opcode TEXT messages"),
                                uWS::OpCode::TEXT,
                                false);
                       return;
                     }

                     current_cmd_client  = ws;
                     auto restore_client = make_scope_exit([this]() { current_cmd_client = nullptr; });

                     // Handle the incoming message and return back the response.
                     std::string response = handle_command(message);
                     ws->send(response, uWS::OpCode::TEXT, false);
                   },
               .close = [this](socket_type* ws, int, std::string_view) { metrics_subscribers.erase(ws); }})
          .listen(bind_addr, port, [this](auto* listen_socket) {
            if (listen_socket) {
              fmt::println("Remote control server listening on {}:{}", bind_addr, port);
            } else {
              fmt::println("Remote control server cannot listen on {}:{}", bind_addr, port);
            }
          });

      server      = &ws_server;
      server_loop = uWS::Loop::get();
      token.reset();
      ws_server.run();
    });

    start_event.wait();
  }

  // See interface for documentation.
  void stop() override
  {
    // Wait for completion.
    if (thread.running()) {
      // Make sure all pending tasks have been completed before closing the server.
      stop_control.stop();

      server_loop.load()->defer([this]() { server.load()->close(); });

      thread.join();
      server_loop = nullptr;
      server      = nullptr;
    }
  }

  // See interface for documentation.
  void reject_commands() override
  {
    if (!thread.running()) {
      return;
    }

    // Commands run in the server loop, so once this task completes no command is in progress.
    sync_event ev;
    server_loop.load()->defer([this, tk = ev.get_token()]() { rejecting_commands = true; });
    ev.wait();
  }

  // See interface for documentation.
  void send(std::string metrics) override
  {
    auto token = stop_control.get_token();
    if (OCUDU_UNLIKELY(token.is_stop_requested())) {
      return;
    }

    // Send the given metrics to all registered metrics subscribers.
    server_loop.load()->defer([this, metrics_text = std::move(metrics), tk = std::move(token)]() {
      for (auto* subscriber : metrics_subscribers) {
        subscriber->send(metrics_text, uWS::OpCode::TEXT, false);
      }
    });
  }

  // See interface for documentation.
  void add_commands(span<std::unique_ptr<remote_command>> remote_cmds) override
  {
    report_error_if_not(!thread.running(), "Remote server cannot add commands during operation");

    // Store the remote commands.
    for (auto& remote_cmd : remote_cmds) {
      dispatcher.add_command(std::move(remote_cmd));
    }
  }

private:
  /// Adds the client that invoked this method to the metrics subscription list.
  void subscribe_metrics_client()
  {
    ocudu_assert(current_cmd_client, "Invalid client");
    metrics_subscribers.emplace(current_cmd_client);
  }

  /// Removes the client that invoked this method from the metrics subscription list.
  void unsubscribe_metrics_client()
  {
    ocudu_assert(current_cmd_client, "Invalid client");
    metrics_subscribers.erase(current_cmd_client);
  }

  /// Handles the given command.
  std::string handle_command(std::string_view command)
  {
    if (rejecting_commands) {
      return build_remote_error_response("Remote server is not accepting commands");
    }

    return dispatcher.handle_request(command);
  }
};

} // namespace

std::unique_ptr<remote_server> ocudu::app_services::create_remote_server(const remote_control_appconfig& cfg)
{
  if (!cfg.enabled) {
    return nullptr;
  }

  return std::make_unique<remote_server_impl>(cfg.bind_addr, cfg.port, cfg.enable_metrics_subscription);
}
