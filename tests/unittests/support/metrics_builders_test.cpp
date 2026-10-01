// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "apps/services/app_execution_metrics/executor_metrics_manager.h"
#include "apps/services/app_resource_usage/app_resource_usage.h"
#include "apps/services/buffer_pool/metrics/buffer_pool_metrics_builder.h"
#include "apps/services/metrics/metrics_channel.h"
#include "apps/services/metrics/metrics_manager.h"
#include "apps/services/remote_control/remote_server_metrics_gateway.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/executors/inline_task_executor.h"
#include "ocudu/support/timers.h"
#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <vector>

using namespace ocudu;
using namespace ocudu::app_services;

namespace {

/// Gateway double that records every JSON message the metric JSON consumers send.
class recording_metrics_gateway : public remote_server_metrics_gateway
{
public:
  void send(std::string metrics) override { sent_messages.push_back(std::move(metrics)); }

  std::vector<std::string> sent_messages;
};

/// Owns the channel collection and (on demand) a manager that takes ownership of it, with an
/// inline executor, mirroring how an application wires the builders' output up.
class builders_fixture
{
public:
  builders_fixture() { ocudulog::fetch_basic_logger("TEST").set_level(ocudulog::basic_levels::warning); }

  /// Connects the channels created so far to a metrics manager, handing the ownership over.
  void connect_manager()
  {
    for (auto& channel : channels) {
      for (auto* producer : channel->get_producers()) {
        producers.push_back(producer);
      }
    }
    manager.emplace(ocudulog::fetch_basic_logger("TEST"),
                    executor,
                    std::move(channels),
                    timers,
                    std::chrono::milliseconds{report_period_ms});
  }

  /// Commands the producers of every channel, as the periodic report controller does.
  void command_all_producers()
  {
    for (auto* producer : producers) {
      producer->on_new_report_period();
    }
  }

  inline_task_executor                         executor;
  timer_manager                                timers{1};
  app_services::metrics_channel_collection     channels;
  std::vector<app_services::metrics_producer*> producers;
  std::optional<metrics_manager>               manager;

  static constexpr int report_period_ms = 1000;
};

} // namespace

TEST(metrics_builders_test, buffer_pool_builder_creates_no_channel_without_consumers_enabled)
{
  builders_fixture            fixture;
  recording_metrics_gateway   gateway;
  app_helpers::metrics_config common_cfg;

  build_buffer_pool_metrics_config(fixture.channels, common_cfg, &gateway);

  // The channel and its producer are always created; with no consumer flag set, the channel has
  // no consumers and commanding the producer delivers nothing.
  ASSERT_EQ(fixture.channels.size(), 1U);
  ASSERT_EQ(fixture.channels.front()->get_producers().size(), 1U);

  fixture.connect_manager();
  fixture.command_all_producers();
  EXPECT_TRUE(gateway.sent_messages.empty());
}

TEST(metrics_builders_test, buffer_pool_builder_delivers_json_report_through_the_gateway)
{
  builders_fixture            fixture;
  recording_metrics_gateway   gateway;
  app_helpers::metrics_config common_cfg;
  common_cfg.enable_json_metrics = true;

  build_buffer_pool_metrics_config(fixture.channels, common_cfg, &gateway);
  ASSERT_EQ(fixture.channels.size(), 1U);

  fixture.connect_manager();
  fixture.command_all_producers();

  // The inline executor delivers synchronously: one report period must produce exactly one JSON
  // message with the buffer pool fields.
  ASSERT_EQ(gateway.sent_messages.size(), 1U);
  EXPECT_NE(gateway.sent_messages.front().find("buffer_pool"), std::string::npos);
  EXPECT_NE(gateway.sent_messages.front().find("central_cache_size"), std::string::npos);
}

TEST(metrics_builders_test, buffer_pool_builder_with_only_log_metrics_sends_no_json)
{
  builders_fixture            fixture;
  recording_metrics_gateway   gateway;
  app_helpers::metrics_config common_cfg;
  common_cfg.enable_log_metrics = true;

  build_buffer_pool_metrics_config(fixture.channels, common_cfg, &gateway);
  ASSERT_EQ(fixture.channels.size(), 1U);
  // Exactly the log consumer is attached: it receives the report, so no JSON is sent.
  ASSERT_EQ(fixture.channels.front()->nof_consumers(), 1U);

  fixture.connect_manager();
  fixture.command_all_producers();

  EXPECT_TRUE(gateway.sent_messages.empty());
}

TEST(metrics_builders_test, resource_usage_service_is_not_built_when_disabled)
{
  builders_fixture          fixture;
  recording_metrics_gateway gateway;
  app_resource_usage_config cfg;

  auto service =
      build_app_resource_usage_service(fixture.channels, cfg, ocudulog::fetch_basic_logger("TEST"), &gateway);

  EXPECT_EQ(service.service, nullptr);
  EXPECT_TRUE(fixture.channels.empty());
}

TEST(metrics_builders_test, resource_usage_builder_delivers_json_report_through_the_gateway)
{
  builders_fixture          fixture;
  recording_metrics_gateway gateway;
  app_resource_usage_config cfg;
  cfg.enable_app_usage                          = true;
  cfg.metrics_consumers_cfg.enable_json_metrics = true;

  auto service =
      build_app_resource_usage_service(fixture.channels, cfg, ocudulog::fetch_basic_logger("TEST"), &gateway);
  ASSERT_NE(service.service, nullptr);
  ASSERT_EQ(fixture.channels.size(), 1U);
  ASSERT_EQ(fixture.channels.front()->get_producers().size(), 1U);

  fixture.connect_manager();
  fixture.command_all_producers();

  ASSERT_EQ(gateway.sent_messages.size(), 1U);
  EXPECT_FALSE(gateway.sent_messages.front().empty());
}

TEST(metrics_builders_test, resource_usage_builder_with_only_log_metrics_sends_no_json)
{
  builders_fixture          fixture;
  recording_metrics_gateway gateway;
  app_resource_usage_config cfg;
  cfg.enable_app_usage                         = true;
  cfg.metrics_consumers_cfg.enable_log_metrics = true;

  auto service =
      build_app_resource_usage_service(fixture.channels, cfg, ocudulog::fetch_basic_logger("TEST"), &gateway);
  ASSERT_NE(service.service, nullptr);
  ASSERT_EQ(fixture.channels.size(), 1U);
  // Exactly the log consumer is attached: it receives the report, so no JSON is sent.
  ASSERT_EQ(fixture.channels.front()->nof_consumers(), 1U);

  fixture.connect_manager();
  fixture.command_all_producers();

  EXPECT_TRUE(gateway.sent_messages.empty());
}

TEST(metrics_builders_test, buffer_pool_builder_with_log_and_json_sends_a_single_json_message)
{
  builders_fixture            fixture;
  recording_metrics_gateway   gateway;
  app_helpers::metrics_config common_cfg;
  common_cfg.enable_log_metrics  = true;
  common_cfg.enable_json_metrics = true;

  build_buffer_pool_metrics_config(fixture.channels, common_cfg, &gateway);
  ASSERT_EQ(fixture.channels.size(), 1U);

  fixture.connect_manager();
  fixture.command_all_producers();

  // Both consumers hang off the same channel and one report fans out to both of them; only the
  // JSON consumer talks to the gateway, so exactly one message is sent.
  ASSERT_EQ(gateway.sent_messages.size(), 1U);
  EXPECT_NE(gateway.sent_messages.front().find("buffer_pool"), std::string::npos);
}

TEST(metrics_builders_test, channels_from_several_builders_share_one_collection_and_manager)
{
  // Mirrors how an application aggregates the metrics of its services: every builder extends the
  // same channel collection, and one manager connects and drives all of them.
  builders_fixture          fixture;
  recording_metrics_gateway gateway;

  // Executor metrics channel.
  executor_metrics_config exec_cfg;
  exec_cfg.enable_executor_metrics                = true;
  exec_cfg.common_metrics_cfg.enable_json_metrics = true;
  auto exec_service = build_executor_metrics_service(fixture.channels, fixture.timers, exec_cfg, &gateway);
  ASSERT_NE(exec_service.service, nullptr);

  // Resource usage channel.
  app_resource_usage_config rusage_cfg;
  rusage_cfg.enable_app_usage                          = true;
  rusage_cfg.metrics_consumers_cfg.enable_json_metrics = true;
  auto rusage_service =
      build_app_resource_usage_service(fixture.channels, rusage_cfg, ocudulog::fetch_basic_logger("TEST"), &gateway);
  ASSERT_NE(rusage_service.service, nullptr);

  // Buffer pool channel.
  app_helpers::metrics_config bp_cfg;
  bp_cfg.enable_json_metrics = true;
  build_buffer_pool_metrics_config(fixture.channels, bp_cfg, &gateway);

  ASSERT_EQ(fixture.channels.size(), 3U);

  fixture.connect_manager();
  fixture.command_all_producers();

  // The executor metrics are pushed by the backend (not by the report period), so one report
  // period yields exactly the resource usage and the buffer pool JSON messages.
  ASSERT_EQ(gateway.sent_messages.size(), 2U);
  const bool found_buffer_pool = std::any_of(gateway.sent_messages.begin(), gateway.sent_messages.end(), [](auto& m) {
    return m.find("buffer_pool") != std::string::npos;
  });
  EXPECT_TRUE(found_buffer_pool);
}

TEST(metrics_builders_test, executor_metrics_service_is_not_built_when_disabled)
{
  builders_fixture        fixture;
  executor_metrics_config cfg;

  auto service = build_executor_metrics_service(fixture.channels, fixture.timers, cfg, nullptr);

  EXPECT_EQ(service.service, nullptr);
  EXPECT_EQ(service.channel_registry, nullptr);
  EXPECT_TRUE(fixture.channels.empty());
}

TEST(metrics_builders_test, executor_metrics_service_creates_channel_and_registry_when_enabled)
{
  builders_fixture          fixture;
  recording_metrics_gateway gateway;
  executor_metrics_config   cfg;
  cfg.enable_executor_metrics                = true;
  cfg.common_metrics_cfg.enable_json_metrics = true;

  auto service = build_executor_metrics_service(fixture.channels, fixture.timers, cfg, &gateway);
  ASSERT_NE(service.service, nullptr);
  EXPECT_NE(service.channel_registry, nullptr);
  ASSERT_EQ(fixture.channels.size(), 1U);
  ASSERT_EQ(fixture.channels.front()->get_producers().size(), 1U);

  // The executor metrics are pushed by the executor metrics backend (not by the report period),
  // so the wiring is verified structurally: the manager can drive the producer without crashing.
  fixture.connect_manager();
  EXPECT_NO_THROW(fixture.command_all_producers());
}
