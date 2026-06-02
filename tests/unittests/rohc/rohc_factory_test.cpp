// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/rohc/rohc_config.h"
#include "ocudu/rohc/rohc_compressor.h"
#include "ocudu/rohc/rohc_decompressor.h"
#include "ocudu/rohc/rohc_factory.h"
#include "ocudu/rohc/rohc_support.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocudu::rohc;

/// Fixture class for ROHC factory tests
class rohc_factory_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // init test's logger
    ocudulog::init();
    logger.set_level(ocudulog::basic_levels::debug);

    // init ROHC logger
    ocudulog::fetch_basic_logger("ROHC", false).set_level(ocudulog::basic_levels::debug);
    ocudulog::fetch_basic_logger("ROHC", false).set_hex_dump_max_size(-1);

    logger.info("ROHC factory test initialized.");
  }

  void TearDown() override
  {
    // flush logger after each test
    ocudulog::flush();
  }

  ocudulog::basic_logger& logger = ocudulog::fetch_basic_logger("TEST", false);
};

TEST_F(rohc_factory_test, create_rohc_compressor_when_supported)
{
  std::unique_ptr<rohc_factory> factory = create_rohc_factory();
  ASSERT_NE(factory, nullptr);
  std::unique_ptr<rohc_compressor> comp = factory->create_rohc_compressor(rohc_config{});
  if (rohc_supported()) {
    EXPECT_NE(comp, nullptr);
  } else {
    EXPECT_EQ(comp, nullptr);
  }
}

TEST_F(rohc_factory_test, create_rohc_decompressor_when_supported)
{
  std::unique_ptr<rohc_factory> factory = create_rohc_factory();
  ASSERT_NE(factory, nullptr);
  std::unique_ptr<rohc_decompressor> decomp = factory->create_rohc_decompressor(rohc_config{});
  if (rohc_supported()) {
    EXPECT_NE(decomp, nullptr);
  } else {
    EXPECT_EQ(decomp, nullptr);
  }
}

int main(int argc, char** argv)
{
  ocudulog::init();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
