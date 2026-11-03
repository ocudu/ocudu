// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "apps/services/remote_control/remote_command_dispatcher.h"
#include "ocudu/support/versioning/interface_versions.h"
#include "ocudu/support/versioning/version.h"
#include "fmt/format.h"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace ocudu;
using namespace app_services;

namespace {

/// Command that records its executions and replies with a fixed payload.
class dummy_remote_command : public remote_command
{
public:
  explicit dummy_remote_command(std::string name_) : name(std::move(name_)) {}

  std::string_view get_name() const override { return name; }

  std::string_view get_description() const override { return "Dummy command"; }

  expected<nlohmann::json, std::string> execute(const nlohmann::json& json) override
  {
    ++nof_executions;
    return nlohmann::json{{"status", "done"}};
  }

  std::string name;
  unsigned    nof_executions = 0;
};

class remote_command_dispatcher_test : public ::testing::Test
{
protected:
  remote_command_dispatcher_test()
  {
    auto cmd  = std::make_unique<dummy_remote_command>("dummy");
    dummy_cmd = cmd.get();
    dispatcher.add_command(std::move(cmd));
  }

  /// Sends the given request to the dispatcher and returns the decoded response.
  nlohmann::json send(const nlohmann::json& request)
  {
    return nlohmann::json::parse(dispatcher.handle_request(request.dump()));
  }

  /// Returns the major version of the remote control interface.
  static std::string supported_major()
  {
    std::string_view version = remote_control_interface_version;
    return std::string(version.substr(0, version.find('.')));
  }

  remote_command_dispatcher dispatcher;
  dummy_remote_command*     dummy_cmd = nullptr;
};

} // namespace

TEST_F(remote_command_dispatcher_test, request_without_version_is_dispatched)
{
  auto response = send({{"cmd", "dummy"}});

  EXPECT_EQ(dummy_cmd->nof_executions, 1);
  EXPECT_FALSE(response.contains("error"));
  EXPECT_EQ(response["cmd"], "dummy");
  EXPECT_EQ(response["result"]["status"], "done");
  EXPECT_TRUE(response.contains("timestamp"));
}

TEST_F(remote_command_dispatcher_test, request_with_supported_version_is_dispatched)
{
  auto response = send({{"cmd", "dummy"}, {"version", supported_major()}});
  EXPECT_FALSE(response.contains("error"));

  response = send({{"cmd", "dummy"}, {"version", remote_control_interface_version}});
  EXPECT_FALSE(response.contains("error"));

  EXPECT_EQ(dummy_cmd->nof_executions, 2);
}

TEST_F(remote_command_dispatcher_test, request_with_other_major_version_is_rejected)
{
  std::string other_major = std::to_string(std::stoul(supported_major()) + 1);

  auto response = send({{"cmd", "dummy"}, {"version", other_major}});

  EXPECT_EQ(dummy_cmd->nof_executions, 0);
  ASSERT_TRUE(response.contains("error"));
  EXPECT_EQ(response["cmd"], "dummy");
}

TEST_F(remote_command_dispatcher_test, request_with_newer_minor_version_is_rejected)
{
  std::string_view version     = remote_control_interface_version;
  unsigned         minor       = std::stoul(std::string(version.substr(version.find('.') + 1)));
  std::string      newer_minor = fmt::format("{}.{}", supported_major(), minor + 1);

  auto response = send({{"cmd", "dummy"}, {"version", newer_minor}});

  EXPECT_EQ(dummy_cmd->nof_executions, 0);
  EXPECT_TRUE(response.contains("error"));
}

TEST_F(remote_command_dispatcher_test, request_with_malformed_version_is_rejected)
{
  for (const nlohmann::json& version : {nlohmann::json("1.x"),
                                        nlohmann::json("x"),
                                        nlohmann::json(""),
                                        nlohmann::json("1."),
                                        nlohmann::json(".1"),
                                        nlohmann::json("1.0.0"),
                                        nlohmann::json(1)}) {
    auto response = send({{"cmd", "dummy"}, {"version", version}});
    EXPECT_TRUE(response.contains("error")) << "version=" << version.dump();
  }
  EXPECT_EQ(dummy_cmd->nof_executions, 0);
}

TEST_F(remote_command_dispatcher_test, version_command_reports_versions)
{
  auto response = send({{"cmd", "version"}});

  ASSERT_FALSE(response.contains("error"));
  const auto& result = response["result"];
  EXPECT_EQ(result["app_version"], get_version());
  EXPECT_TRUE(result.contains("build_hash"));
  EXPECT_EQ(result["remote_control_version"], remote_control_interface_version);
  EXPECT_EQ(result["metrics_version"], metrics_interface_version);
  EXPECT_FALSE(result.contains("commands"));
}

TEST_F(remote_command_dispatcher_test, help_command_lists_commands)
{
  auto response = send({{"cmd", "help"}});

  ASSERT_FALSE(response.contains("error"));

  // The command list is sorted by name and includes the commands the dispatcher always provides.
  const auto& commands = response["result"]["commands"];
  ASSERT_EQ(commands.size(), 4);
  EXPECT_EQ(commands[0]["name"], "dummy");
  EXPECT_EQ(commands[0]["description"], "Dummy command");
  EXPECT_EQ(commands[1]["name"], "help");
  EXPECT_EQ(commands[2]["name"], "quit");
  EXPECT_EQ(commands[3]["name"], "version");
}

TEST_F(remote_command_dispatcher_test, invalid_requests_are_rejected)
{
  EXPECT_TRUE(nlohmann::json::parse(dispatcher.handle_request("not json")).contains("error"));
  EXPECT_TRUE(send({{"foo", "bar"}}).contains("error"));
  EXPECT_TRUE(send({{"cmd", 1}}).contains("error"));

  auto response = send({{"cmd", "unknown"}});
  EXPECT_TRUE(response.contains("error"));
  EXPECT_EQ(response["cmd"], "unknown");
}
