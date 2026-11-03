// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "apps/services/remote_control/remote_command_dispatcher.h"
#include "apps/helpers/metrics/helpers.h"
#include "ocudu/support/ocudu_assert.h"
#include "ocudu/support/versioning/build_info.h"
#include "ocudu/support/versioning/interface_versions.h"
#include "ocudu/support/versioning/version.h"
#include "fmt/format.h"
#include <algorithm>
#include <charconv>
#include <csignal>

using namespace ocudu;
using namespace app_services;

std::string ocudu::app_services::build_remote_error_response(std::string_view error, std::optional<std::string> cmd)
{
  nlohmann::json response;
  response["error"] = error;
  if (cmd) {
    response["cmd"] = std::move(*cmd);
  }
  response["timestamp"] = app_helpers::get_time_stamp();
  return response.dump();
}

namespace {

/// Interface version split into its components.
struct interface_version {
  unsigned                major = 0;
  std::optional<unsigned> minor;
};

/// Command that reports the application and interface versions.
class version_remote_command : public remote_command
{
public:
  // See interface for documentation.
  std::string_view get_name() const override { return "version"; }

  // See interface for documentation.
  std::string_view get_description() const override { return "Report the application and interface versions"; }

  // See interface for documentation.
  expected<nlohmann::json, std::string> execute(const nlohmann::json& json) override
  {
    nlohmann::json result;
    result["app_version"]            = get_version();
    result["build_hash"]             = get_build_hash();
    result["remote_control_version"] = remote_control_interface_version;
    result["metrics_version"]        = metrics_interface_version;
    return result;
  }
};

/// Command that lists the available commands.
class help_remote_command : public remote_command
{
  const std::unordered_map<std::string, std::unique_ptr<remote_command>>& commands;

public:
  explicit help_remote_command(const std::unordered_map<std::string, std::unique_ptr<remote_command>>& commands_) :
    commands(commands_)
  {
  }

  // See interface for documentation.
  std::string_view get_name() const override { return "help"; }

  // See interface for documentation.
  std::string_view get_description() const override { return "List the available commands"; }

  // See interface for documentation.
  expected<nlohmann::json, std::string> execute(const nlohmann::json& json) override
  {
    // Sort the commands by name, the command map has no defined order.
    std::vector<const remote_command*> sorted_cmds;
    sorted_cmds.reserve(commands.size());
    for (const auto& [name, cmd] : commands) {
      sorted_cmds.push_back(cmd.get());
    }
    std::sort(sorted_cmds.begin(), sorted_cmds.end(), [](const remote_command* lhs, const remote_command* rhs) {
      return lhs->get_name() < rhs->get_name();
    });

    nlohmann::json  result;
    nlohmann::json& cmd_list = result["commands"];
    cmd_list                 = nlohmann::json::array();
    for (const auto* cmd : sorted_cmds) {
      cmd_list.push_back({{"name", cmd->get_name()}, {"description", cmd->get_description()}});
    }
    return result;
  }
};

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

} // namespace

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

/// Parses a version string with the format "MAJOR" or "MAJOR.MINOR".
static std::optional<interface_version> parse_interface_version(std::string_view str)
{
  auto parse_number = [](std::string_view number) -> std::optional<unsigned> {
    unsigned value = 0;
    auto [ptr, ec] = std::from_chars(number.data(), number.data() + number.size(), value);
    if (number.empty() || ec != std::errc() || ptr != number.data() + number.size()) {
      return std::nullopt;
    }
    return value;
  };

  interface_version version;
  auto              dot_pos = str.find('.');
  auto              major   = parse_number(str.substr(0, dot_pos));
  if (!major) {
    return std::nullopt;
  }
  version.major = *major;

  if (dot_pos != std::string_view::npos) {
    version.minor = parse_number(str.substr(dot_pos + 1));
    if (!version.minor) {
      return std::nullopt;
    }
  }
  return version;
}

/// Checks that the version requested by a client is supported by this server. The major versions must match, and a
/// requested minor version must not be newer than the one of the server.
static bool is_interface_version_supported(const interface_version& requested)
{
  auto supported = parse_interface_version(remote_control_interface_version);
  ocudu_assert(supported && supported->minor, "Invalid remote control interface version");

  if (requested.major != supported->major) {
    return false;
  }
  return !requested.minor || *requested.minor <= *supported->minor;
}

remote_command_dispatcher::remote_command_dispatcher()
{
  add_command(std::make_unique<version_remote_command>());
  add_command(std::make_unique<help_remote_command>(commands));
  add_command(std::make_unique<quit_remote_command>());
}

void remote_command_dispatcher::add_command(std::unique_ptr<remote_command> cmd)
{
  auto& entry = commands[std::string(cmd->get_name())];
  entry       = std::move(cmd);
}

std::string remote_command_dispatcher::handle_request(std::string_view request)
{
  nlohmann::json req;

  try {
    req = nlohmann::json::parse(request);
  } catch (const nlohmann::json::parse_error& e) {
    return build_remote_error_response(e.what());
  }

  auto cmd_key = req.find("cmd");
  if (cmd_key == req.end()) {
    return build_remote_error_response("'cmd' object is missing and it is mandatory");
  }

  if (!cmd_key->is_string()) {
    return build_remote_error_response("'cmd' object value type should be a string");
  }

  const auto& cmd_value = cmd_key.value().get_ref<const nlohmann::json::string_t&>();

  if (auto version_key = req.find("version"); version_key != req.end()) {
    if (!version_key->is_string()) {
      return build_remote_error_response("'version' object value type should be a string", cmd_value);
    }
    const auto& version_value = version_key.value().get_ref<const nlohmann::json::string_t&>();
    auto        version       = parse_interface_version(version_value);
    if (!version) {
      return build_remote_error_response(
          fmt::format("Invalid 'version' value '{}', expected 'MAJOR' or 'MAJOR.MINOR'", version_value), cmd_value);
    }
    if (!is_interface_version_supported(*version)) {
      return build_remote_error_response(fmt::format("Unsupported remote control interface version '{}', the server "
                                                     "implements version '{}'",
                                                     version_value,
                                                     remote_control_interface_version),
                                         cmd_value);
    }
  }

  if (auto cmd = commands.find(cmd_value); cmd != commands.end()) {
    auto response_payload = cmd->second->execute(req);
    if (!response_payload) {
      return build_remote_error_response(response_payload.error(), cmd_value);
    }

    return build_success_response(cmd_value, response_payload.value());
  }

  return build_remote_error_response(fmt::format("Unknown command type: {}", cmd_value), cmd_value);
}
