// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "apps/services/remote_control/remote_server.h"
#include "apps/services/remote_control/remote_command.h"
#include "apps/services/remote_control/remote_control_appconfig.h"
#include "nlohmann/json.hpp"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/synchronization/stop_event.h"
#include "ocudu/support/synchronization/sync_event.h"
#ifndef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
#include "uWebSockets/App.h"
#ifndef __clang__
#pragma GCC diagnostic pop
#endif
#include "apps/helpers/metrics/helpers.h"
#include "ocudu/adt/scope_exit.h"
#include "ocudu/support/executors/unique_thread.h"
#include <csignal>
#include <utility>

using namespace ocudu;
using namespace app_services;

/// Builds an error response encoded in JSON format.
static std::string build_error_response(std::string_view error, std::optional<std::string> cmd = std::nullopt)
{
  nlohmann::json response;
  response["error"] = error;
  if (cmd) {
    response["cmd"] = std::move(*cmd);
  }
  response["timestamp"] = app_helpers::get_time_stamp();
  return response.dump();
}

/// Builds a successful response encoded in JSON format, nesting a non-null payload under "result".
static std::string build_success_response(std::string_view cmd, const nlohmann::json& payload = {})
{
  nlohmann::json response;
  response["cmd"]       = cmd;
  response["timestamp"] = app_helpers::get_time_stamp();
  if (!payload.is_null()) {
    response["result"] = payload;
  }
  return response.dump();
}

namespace {

/// Command that manages closing the application.
class quit_remote_command : public remote_command
{
public:
  // See interface for documentation.
  std::string_view get_name() const override { return "quit"; }

  // See interface for documentation.
  std::string_view get_description() const override { return "Quit application"; }

  // See interface for documentation.
  expected<nlohmann::json, std::string> execute(const nlohmann::json& json) override
  {
    std::raise(SIGTERM);
    return {};
  }
};

/// Remote server implementation.
class remote_server_impl : public remote_server,
                           public remote_server_operation_controller,
                           public remote_server_metrics_gateway

{
  /// WebSocket socket type alias.
  struct dummy_type {};
  using socket_type = uWS::WebSocket<false, true, dummy_type>;

  const std::string                                                bind_addr;
  const unsigned                                                   port;
  const bool                                                       enable_metrics_subscription;
  unique_thread                                                    thread;
  std::atomic<uWS::App*>                                           server;
  std::atomic<uWS::Loop*>                                          server_loop;
  std::unordered_map<std::string, std::unique_ptr<remote_command>> commands;
  std::set<socket_type*>                                           metrics_subscribers;
  socket_type*                                                     current_cmd_client = nullptr;
  stop_event_source                                                stop_control;

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
      return {};
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
    // Add the quit command.
    {
      auto  quit_cmd = std::make_unique<quit_remote_command>();
      auto& cmd      = commands[std::string(quit_cmd->get_name())];
      cmd            = std::move(quit_cmd);
    }

    if (enable_metrics_subscription) {
      // Add the metrics subscription commands.
      {
        auto  sub_cmd = std::make_unique<metrics_subscribe_command>(this);
        auto& cmd     = commands[std::string(sub_cmd->get_name())];
        cmd           = std::move(sub_cmd);
      }
      {
        auto  unsub_cmd = std::make_unique<metrics_unsubscribe_command>(this);
        auto& cmd       = commands[std::string(unsub_cmd->get_name())];
        cmd             = std::move(unsub_cmd);
      }
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
          .ws<dummy_type>("/*",
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
                                   ws->send(
                                       build_error_response("This WebSocket server only supports opcode TEXT messages"),
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
      auto& cmd = commands[std::string(remote_cmd->get_name())];
      cmd       = std::move(remote_cmd);
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
    nlohmann::json req;

    try {
      req = nlohmann::json::parse(command);
    } catch (const nlohmann::json::parse_error& e) {
      return build_error_response(e.what());
    }

    auto cmd_key = req.find("cmd");
    if (cmd_key == req.end()) {
      return build_error_response("'cmd' object is missing and it is mandatory");
    }

    if (!cmd_key->is_string()) {
      return build_error_response("'cmd' object value type should be a string");
    }

    const auto& cmd_value = cmd_key.value().get_ref<const nlohmann::json::string_t&>();
    if (auto cmd = commands.find(cmd_value); cmd != commands.end()) {
      auto response_payload = cmd->second->execute(req);
      if (!response_payload) {
        return build_error_response(response_payload.error(), cmd_value);
      }

      return build_success_response(cmd_value, response_payload.value());
    }

    return build_error_response(fmt::format("Unknown command type: {}", cmd_value), cmd_value);
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
