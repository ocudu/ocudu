// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "test_rng_seed.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/error_handling.h"
#include <gtest/gtest.h>

namespace {

/// Setup a default log level for loggers based on a provided cmd line parameter "--log_level=", if present.
void setup_default_log_level(int argc, char** argv)
{
  // Parse logging level passed via command line arguments.
  ocudulog::basic_levels chosen_level = ocudulog::basic_levels::warning;

  for (int i = 1; i < argc; ++i) {
    std::string cmd_arg = argv[i];
    if (cmd_arg.find("--log_level") == 0) {
      // Log level specified in command line.
      size_t pos = cmd_arg.find("=");
      if (pos != std::string::npos and cmd_arg.size() > pos + 1) {
        cmd_arg               = cmd_arg.substr(pos + 1, cmd_arg.size());
        auto parsed_log_level = ocudulog::str_to_basic_level(cmd_arg);
        if (parsed_log_level.has_value()) {
          chosen_level = *parsed_log_level;
          i            = argc;
          break;
        }
      } else {
        for (; i < argc; ++i) {
          cmd_arg = argv[i];
          if (not cmd_arg.empty()) {
            auto parsed_log_level = ocudulog::str_to_basic_level(cmd_arg);
            if (parsed_log_level.has_value()) {
              chosen_level = *parsed_log_level;
              i            = argc;
              break;
            }
          }
        }
      }
    }
  }

  // Setup default log level.
  // TODO: Support setting a default log level in ocudulog instead of individual loggers.
  ocudulog::fetch_basic_logger("TEST").set_level(chosen_level);
  ocudulog::fetch_basic_logger("SCHED", true).set_level(chosen_level);
  ocudulog::fetch_basic_logger("MAC", true).set_level(chosen_level);
  ocudulog::fetch_basic_logger("RLC").set_level(chosen_level);
  ocudulog::fetch_basic_logger("DU-MNG").set_level(chosen_level);
  ocudulog::fetch_basic_logger("DU-F1").set_level(chosen_level);

  // Init ocudulog.
  ocudulog::init();
}

/// Gtest environment for setting up random seeds and log level.
class OCUDUTestEnvironment final : public ::testing::Environment
{
public:
  OCUDUTestEnvironment()
  {
    // Note: ::testing::UnitTest::GetInstance()->random_seed()  does not match the seed passed by --gtest_random_seed=
    uint32_t base_seed = ::testing::GTEST_FLAG(random_seed);
    if (base_seed == 0) {
      // The user didn't specify the seed via --gtest_random_seed=. We generate our own.
      base_seed = std::random_device{}();
    }
    ocudu::test_rng::init_base_seed(base_seed);
  }

  void TearDown() override
  {
    // Ensure logs are flushed.
    ocudulog::flush();
  }
};

class RandomGeneratorResetListener final : public ::testing::EmptyTestEventListener
{
private:
  void OnTestIterationStart(const testing::UnitTest& unit_test, int iteration) override
  {
    // On each test iteration increment, we update the seed.
    last_iteration = iteration;
    ocudu::test_rng::advance_iter_seed(iteration);
  }

  void OnTestStart(const testing::TestInfo& test_info) override { ocudu::test_rng::rewind_rng(); }

  void OnTestEnd(const testing::TestInfo& test_info) override {}

  void OnTestPartResult(const testing::TestPartResult& test_part_result) override
  {
    if (test_part_result.failed()) {
      ocudulog::flush();
      // Note: We cast to int32_t for printing, because that's the type that gtest_random_seed uses.
      const int iter_seed = static_cast<int32_t>(ocudu::test_rng::seed());
      fmt::print(stderr,
                 "\033[0;31m[  FAILED  ]\033[0m \033[0;33mOCUDU Random Seed: base_seed={}, iteration={}, "
                 "iter_seed={}.\033[0m\n",
                 static_cast<int32_t>(ocudu::test_rng::base_seed()),
                 last_iteration,
                 iter_seed);
      if (last_iteration > 0) {
        fmt::print(stderr,
                   "\033[0;31m[  FAILED  ]\033[0m \033[0;33mNote: To reproduce iter_seed={} at iteration=0, use "
                   "base_seed={}.\033[0m\n",
                   iter_seed,
                   static_cast<int32_t>(ocudu::test_rng::compute_base_seed_at_iter0(last_iteration)));
      }
    }
  }

  int last_iteration = 0;
};

} // namespace

int main(int argc, char** argv)
{
  // Parse --log_level= if it exists to set the default log level.
  setup_default_log_level(argc, argv);

  // Init Gtest.
  ::testing::InitGoogleTest(&argc, argv);

  // Set up global seed.
  ::testing::AddGlobalTestEnvironment(new OCUDUTestEnvironment{});

  // Setup test random listener.
  auto& listeners = ::testing::UnitTest::GetInstance()->listeners();
  listeners.Append(new RandomGeneratorResetListener{});

  return RUN_ALL_TESTS();
}
