// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/gtpu/extension_header/pdcp_pdu_number_packing.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocudu::gtpu;

namespace {

struct test_vector {
  uint32_t             sn;
  std::vector<uint8_t> packed_content;
};

std::vector<test_vector> valid_sn = {test_vector{.sn = 0x0000, .packed_content = {0x00, 0x00}},
                                     test_vector{.sn = 0x0801, .packed_content = {0x08, 0x01}},
                                     test_vector{.sn = 0x0180, .packed_content = {0x01, 0x80}},
                                     test_vector{.sn = 0x0fff, .packed_content = {0x0f, 0xff}}};

std::vector<test_vector> invalid_sn = {test_vector{.sn = 0x1000, .packed_content = {}},
                                       test_vector{.sn = 0x1001, .packed_content = {}},
                                       test_vector{.sn = 0x1fff, .packed_content = {}},
                                       test_vector{.sn = 0x2000, .packed_content = {}},
                                       test_vector{.sn = 0x4000, .packed_content = {}},
                                       test_vector{.sn = 0x8000, .packed_content = {}},
                                       test_vector{.sn = 0xf000, .packed_content = {}}};

std::vector<test_vector> invalid_content_value = {test_vector{.sn = {}, .packed_content = {0x10, 0x00}},
                                                  test_vector{.sn = {}, .packed_content = {0x10, 0x01}},
                                                  test_vector{.sn = {}, .packed_content = {0x1f, 0xff}},
                                                  test_vector{.sn = {}, .packed_content = {0x20, 0x00}},
                                                  test_vector{.sn = {}, .packed_content = {0x40, 0x00}},
                                                  test_vector{.sn = {}, .packed_content = {0x80, 0x00}},
                                                  test_vector{.sn = {}, .packed_content = {0xf0, 0x00}}};

std::vector<test_vector> invalid_content_length = {test_vector{.sn = {}, .packed_content = {0x00, 0x00, 0x00}},
                                                   test_vector{.sn = {}, .packed_content = {0x00}},
                                                   test_vector{.sn = {}, .packed_content = {}}};

ocudu::log_sink_spy& test_spy = []() -> ocudu::log_sink_spy& {
  if (!ocudulog::install_custom_sink(ocudu::log_sink_spy::name(),
                                     std::make_unique<ocudu::log_sink_spy>(ocudulog::get_default_log_formatter()))) {
    report_fatal_error("Unable to create logger spy");
  }
  auto* spy = static_cast<ocudu::log_sink_spy*>(ocudulog::find_sink(ocudu::log_sink_spy::name()));
  if (spy == nullptr) {
    report_fatal_error("Unable to create logger spy");
  }

  ocudulog::fetch_basic_logger("GTP-U", *spy, true);
  return *spy;
}();

/// Fixture class for PDCP PDU number packing tests.
class pdcp_pdu_number_packing_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // init test's logger.
    ocudulog::init();
    logger.set_level(ocudulog::basic_levels::debug);

    // reset log spy
    test_spy.reset_counters();

    // init GTP-U logger.
    ocudulog::fetch_basic_logger("GTP-U", false).set_level(ocudulog::basic_levels::debug);
    ocudulog::fetch_basic_logger("GTP-U", false).set_hex_dump_max_size(-1);

    logger.info("Creating PDCP PDU number packing object.");

    // Create packer object.
    packer = std::make_unique<pdcp_pdu_number_packing>(ocudulog::fetch_basic_logger("GTP-U", false));
  }

  void TearDown() override
  {
    // flush logger after each test
    ocudulog::flush();
  }

  ocudulog::basic_logger&                  logger = ocudulog::fetch_basic_logger("TEST", false);
  std::unique_ptr<pdcp_pdu_number_packing> packer;
};

TEST_F(pdcp_pdu_number_packing_test, create_new_entity)
{
  EXPECT_NE(packer, nullptr);

  // Check warnings and errors.
  EXPECT_EQ(test_spy.get_warning_counter(), 0);
  EXPECT_EQ(test_spy.get_error_counter(), 0);
}

TEST_F(pdcp_pdu_number_packing_test, valid_sn)
{
  for (auto& tv : valid_sn) {
    logger.info("Testing valid sn={} packed_content={}", tv.sn, tv.packed_content);

    // Test unpacking.
    byte_buffer packed_buf = byte_buffer::create(tv.packed_content).value();
    uint32_t    out_sn;
    EXPECT_TRUE(packer->unpack(out_sn, packed_buf));
    EXPECT_EQ(out_sn, tv.sn);

    // Test packing.
    byte_buffer out_buf;
    EXPECT_TRUE(packer->pack(out_buf, tv.sn));
    EXPECT_EQ(out_buf, packed_buf);
  }

  // Check warnings and errors.
  EXPECT_EQ(test_spy.get_warning_counter(), 0);
  EXPECT_EQ(test_spy.get_error_counter(), 0);
}

TEST_F(pdcp_pdu_number_packing_test, invalid_sn)
{
  for (auto& tv : invalid_sn) {
    logger.info("Testing invalid sn={} packed_content={}", tv.sn, tv.packed_content);

    // Test packing.
    byte_buffer packed_buf = byte_buffer::create(tv.packed_content).value();
    byte_buffer out_buf;
    EXPECT_FALSE(packer->pack(out_buf, tv.sn));
    EXPECT_EQ(out_buf, packed_buf);
  }

  // Check warnings and errors.
  EXPECT_EQ(test_spy.get_warning_counter(), 0);
  EXPECT_EQ(test_spy.get_error_counter(), invalid_sn.size());
}

TEST_F(pdcp_pdu_number_packing_test, invalid_content_value)
{
  for (auto& tv : invalid_content_value) {
    logger.info("Testing invalid packed_content={}", tv.packed_content);

    // Test unpacking.
    byte_buffer packed_buf = byte_buffer::create(tv.packed_content).value();
    uint32_t    out_sn     = 0;
    EXPECT_FALSE(packer->unpack(out_sn, packed_buf));
  }

  // Check warnings and errors.
  EXPECT_EQ(test_spy.get_warning_counter(), invalid_content_value.size());
  EXPECT_EQ(test_spy.get_error_counter(), 0);
}

TEST_F(pdcp_pdu_number_packing_test, invalid_content_length)
{
  for (auto& tv : invalid_content_length) {
    logger.info("Testing invalid packed_content={}", tv.packed_content);

    // Test unpacking.
    byte_buffer packed_buf = byte_buffer::create(tv.packed_content).value();
    uint32_t    out_sn     = 0;
    EXPECT_FALSE(packer->unpack(out_sn, packed_buf));
  }

  // Check warnings and errors.
  EXPECT_EQ(test_spy.get_warning_counter(), 0);
  EXPECT_EQ(test_spy.get_error_counter(), invalid_content_length.size());
}

} // namespace

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
