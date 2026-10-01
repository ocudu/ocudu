// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/rohc/rohc_lib/rohc_lib_compressor.h"
#include "lib/rohc/rohc_lib/rohc_lib_decompressor.h"
#include "rohc_test_pdus.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocudu::rohc;

struct rohc_test_params {
  const char* name;
  rohc_config config;
};

static std::vector<rohc_test_params> rohc_test_set = {
    rohc_test_params{.name = "default_uncompressed", .config = rohc_config{}},
    rohc_test_params{.name = "default_v1_RTP_UDP_IP",
                     .config =
                         rohc_config{.profiles = rohc_profile_config().set_profile(rohc_profile::profile0x0001, true)}},
    rohc_test_params{.name = "default_v1_UDP_IP",
                     .config =
                         rohc_config{.profiles = rohc_profile_config().set_profile(rohc_profile::profile0x0002, true)}},
    rohc_test_params{.name = "default_v1_ESP_IP",
                     .config =
                         rohc_config{.profiles = rohc_profile_config().set_profile(rohc_profile::profile0x0003, true)}},
    rohc_test_params{.name = "default_v1_IP",
                     .config =
                         rohc_config{.profiles = rohc_profile_config().set_profile(rohc_profile::profile0x0004, true)}},
    rohc_test_params{.name = "default_v1_TCP_IP",
                     .config =
                         rohc_config{.profiles = rohc_profile_config().set_profile(rohc_profile::profile0x0006, true)}},
    rohc_test_params{.name   = "default_v1_RTP_UDP_IP_v1_UDP_IP_v1_IP",
                     .config = rohc_config{.profiles = rohc_profile_config()
                                                           .set_profile(rohc_profile::profile0x0001, true)
                                                           .set_profile(rohc_profile::profile0x0002, true)
                                                           .set_profile(rohc_profile::profile0x0004, true)}},
    rohc_test_params{.name   = "default_v1_RTP_UDP_IP_v1_UDP_IP_v1_IP_v1_TCP_IP",
                     .config = rohc_config{.profiles = rohc_profile_config()
                                                           .set_profile(rohc_profile::profile0x0001, true)
                                                           .set_profile(rohc_profile::profile0x0002, true)
                                                           .set_profile(rohc_profile::profile0x0004, true)
                                                           .set_profile(rohc_profile::profile0x0006, true)}},
    rohc_test_params{.name = "default_v2_RTP_UDP_IP",
                     .config =
                         rohc_config{.profiles = rohc_profile_config().set_profile(rohc_profile::profile0x0101, true)}},
    rohc_test_params{.name = "default_v2_UDP_IP",
                     .config =
                         rohc_config{.profiles = rohc_profile_config().set_profile(rohc_profile::profile0x0102, true)}},
    rohc_test_params{.name = "default_v2_ESP_IP",
                     .config =
                         rohc_config{.profiles = rohc_profile_config().set_profile(rohc_profile::profile0x0103, true)}},
    rohc_test_params{.name = "default_v2_IP",
                     .config =
                         rohc_config{.profiles = rohc_profile_config().set_profile(rohc_profile::profile0x0104, true)}},
    rohc_test_params{.name   = "default_v2_RTP_UDP_IP_v2_UDP_IP_v2_IP",
                     .config = rohc_config{.profiles = rohc_profile_config()
                                                           .set_profile(rohc_profile::profile0x0101, true)
                                                           .set_profile(rohc_profile::profile0x0102, true)
                                                           .set_profile(rohc_profile::profile0x0104, true)}},
    rohc_test_params{.name   = "default_v1_TCP_IP_v2_RTP_UDP_IP_v2_UDP_IP_v2_IP",
                     .config = rohc_config{.profiles = rohc_profile_config()
                                                           .set_profile(rohc_profile::profile0x0006, true)
                                                           .set_profile(rohc_profile::profile0x0101, true)
                                                           .set_profile(rohc_profile::profile0x0102, true)
                                                           .set_profile(rohc_profile::profile0x0104, true)}},
};

static std::vector<rohc_test_params> rohc_test_set_invalid = {
    rohc_test_params{.name   = "invalid_v1_RTP_UDP_IP_v2_RTP_UDP_IP",
                     .config = rohc_config{.profiles = rohc_profile_config()
                                                           .set_profile(rohc_profile::profile0x0001, true)
                                                           .set_profile(rohc_profile::profile0x0101, true)}},
    rohc_test_params{.name   = "invalid_v1_UDP_IP_v2_UDP_IP",
                     .config = rohc_config{.profiles = rohc_profile_config()
                                                           .set_profile(rohc_profile::profile0x0002, true)
                                                           .set_profile(rohc_profile::profile0x0102, true)}},
    rohc_test_params{.name   = "invalid_v1_ESP_IP_v2_ESP_IP",
                     .config = rohc_config{.profiles = rohc_profile_config()
                                                           .set_profile(rohc_profile::profile0x0003, true)
                                                           .set_profile(rohc_profile::profile0x0103, true)}},
    rohc_test_params{.name   = "invalid_v1_IP_v2_IP",
                     .config = rohc_config{.profiles = rohc_profile_config()
                                                           .set_profile(rohc_profile::profile0x0004, true)
                                                           .set_profile(rohc_profile::profile0x0104, true)}},
};

/// Dummy operator to avoid Valgrind warnings.
inline std::ostream& operator<<(std::ostream& os, const rohc_test_params& params)
{
  return os;
}

static ocudu::log_sink_spy& test_spy = []() -> ocudu::log_sink_spy& {
  if (!ocudulog::install_custom_sink(ocudu::log_sink_spy::name(),
                                     std::make_unique<ocudu::log_sink_spy>(ocudulog::get_default_log_formatter()))) {
    report_fatal_error("Unable to create logger spy");
  }
  auto* spy = static_cast<ocudu::log_sink_spy*>(ocudulog::find_sink(ocudu::log_sink_spy::name()));
  if (spy == nullptr) {
    report_fatal_error("Unable to create logger spy");
  }

  ocudulog::fetch_basic_logger("ROHC", *spy, true);
  return *spy;
}();

/// Fixture class for ROHC library compressor decompressor tests
class rohc_lib_test : public testing::TestWithParam<rohc_test_params>
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

    // reset log spy
    test_spy.reset_counters();
  }

  void TearDown() override
  {
    // flush logger after each test
    ocudulog::flush();
  }

  ocudulog::basic_logger& logger = ocudulog::fetch_basic_logger("TEST", false);
};

class rohc_lib_test_death : public rohc_lib_test
{};

TEST_P(rohc_lib_test, create_compressor)
{
  rohc_test_params                     param = GetParam();
  std::unique_ptr<rohc_lib_compressor> comp  = std::make_unique<rohc_lib_compressor>(param.config);
  EXPECT_NE(comp, nullptr);

  // No warnings or errors
  EXPECT_EQ(test_spy.get_warning_counter(), 0);
  EXPECT_EQ(test_spy.get_error_counter(), 0);
}

TEST_P(rohc_lib_test_death, create_compressor)
{
  (void)(::testing::GTEST_FLAG(death_test_style) = "threadsafe");
  rohc_test_params                     param = GetParam();
  std::unique_ptr<rohc_lib_compressor> comp;
  ASSERT_DEATH(comp = std::make_unique<rohc_lib_compressor>(param.config), R"(Failed to enable ROHC profile)");
}

TEST_P(rohc_lib_test, create_decompressor)
{
  rohc_test_params                       param  = GetParam();
  std::unique_ptr<rohc_lib_decompressor> decomp = std::make_unique<rohc_lib_decompressor>(param.config);
  EXPECT_NE(decomp, nullptr);

  // No warnings or errors
  EXPECT_EQ(test_spy.get_warning_counter(), 0);
  EXPECT_EQ(test_spy.get_error_counter(), 0);
}

TEST_P(rohc_lib_test_death, create_decompressor)
{
  (void)(::testing::GTEST_FLAG(death_test_style) = "threadsafe");
  rohc_test_params                       param = GetParam();
  std::unique_ptr<rohc_lib_decompressor> decomp;
  ASSERT_DEATH(decomp = std::make_unique<rohc_lib_decompressor>(param.config), R"(Failed to enable ROHC profile)");
}

TEST_P(rohc_lib_test, compress_decompress_ip)
{
  rohc_test_params                       param  = GetParam();
  std::unique_ptr<rohc_lib_compressor>   comp   = std::make_unique<rohc_lib_compressor>(param.config);
  std::unique_ptr<rohc_lib_decompressor> decomp = std::make_unique<rohc_lib_decompressor>(param.config);

  constexpr size_t                       n_pdu    = 5;
  std::array<byte_buffer, n_pdu>         original = {byte_buffer::create(ip_ping_req_1).value(),
                                                     byte_buffer::create(ip_ping_req_2).value(),
                                                     byte_buffer::create(ip_ping_req_3).value(),
                                                     byte_buffer::create(ip_ping_req_4).value(),
                                                     byte_buffer::create(ip_ping_req_5).value()};
  std::array<byte_buffer, n_pdu>         compressed;
  std::array<rohc_decromp_result, n_pdu> decompressed;

  uint32_t original_len   = 0;
  uint32_t compressed_len = 0;
  for (size_t i = 0; i < n_pdu; i++) {
    original_len += original[i].length();
    compressed[i] = comp->compress(std::move(original[i].deep_copy().value()));
    ASSERT_FALSE(compressed[i].empty());
    compressed_len += compressed[i].length();
  }

  if (param.config.profiles.is_profile_enabled(rohc_profile::profile0x0004) ||
      param.config.profiles.is_profile_enabled(rohc_profile::profile0x0104)) {
    EXPECT_LT(compressed_len, original_len);
  } else {
    EXPECT_GE(compressed_len, original_len);
  }

  for (size_t i = 0; i < n_pdu; i++) {
    decompressed[i] = decomp->decompress(std::move(compressed[i]));
    EXPECT_FALSE(decompressed[i].decomp_packet.empty());
    EXPECT_EQ(decompressed[i].decomp_packet, original[i]);
  }

  // No warnings or errors
  EXPECT_EQ(test_spy.get_warning_counter(), 0);
  EXPECT_EQ(test_spy.get_error_counter(), 0);
}

TEST_P(rohc_lib_test, compress_decompress_ip_with_feedback)
{
  rohc_test_params                       param  = GetParam();
  std::unique_ptr<rohc_lib_compressor>   comp   = std::make_unique<rohc_lib_compressor>(param.config);
  std::unique_ptr<rohc_lib_decompressor> decomp = std::make_unique<rohc_lib_decompressor>(param.config);

  constexpr size_t                       n_pdu    = 5;
  std::array<byte_buffer, n_pdu>         original = {byte_buffer::create(ip_ping_req_1).value(),
                                                     byte_buffer::create(ip_ping_req_2).value(),
                                                     byte_buffer::create(ip_ping_req_3).value(),
                                                     byte_buffer::create(ip_ping_req_4).value(),
                                                     byte_buffer::create(ip_ping_req_5).value()};
  std::array<byte_buffer, n_pdu>         compressed;
  std::array<rohc_decromp_result, n_pdu> decompressed;

  uint32_t original_len         = 0;
  uint32_t compressed_len       = 0;
  uint32_t nof_feedback_packets = 0;
  for (size_t i = 0; i < n_pdu; i++) {
    // compress
    original_len += original[i].length();
    compressed[i] = comp->compress(std::move(original[i].deep_copy().value()));
    ASSERT_FALSE(compressed[i].empty());
    compressed_len += compressed[i].length();

    // decompress and deliver feedback
    decompressed[i] = decomp->decompress(std::move(compressed[i]));
    EXPECT_FALSE(decompressed[i].decomp_packet.empty());
    EXPECT_EQ(decompressed[i].decomp_packet, original[i]);
    if (!decompressed[i].feedback_packet.empty()) {
      EXPECT_TRUE(comp->handle_feedback(std::move(decompressed[i].feedback_packet)));
      nof_feedback_packets++;
    }
  }
  EXPECT_GE(nof_feedback_packets, 1);

  if (param.config.profiles.is_profile_enabled(rohc_profile::profile0x0004) ||
      param.config.profiles.is_profile_enabled(rohc_profile::profile0x0104)) {
    EXPECT_LT(compressed_len, original_len);
  } else {
    EXPECT_GE(compressed_len, original_len);
  }

  // No warnings or errors
  EXPECT_EQ(test_spy.get_warning_counter(), 0);
  EXPECT_EQ(test_spy.get_error_counter(), 0);
}

TEST_P(rohc_lib_test, compress_decompress_udp)
{
  rohc_test_params                       param  = GetParam();
  std::unique_ptr<rohc_lib_compressor>   comp   = std::make_unique<rohc_lib_compressor>(param.config);
  std::unique_ptr<rohc_lib_decompressor> decomp = std::make_unique<rohc_lib_decompressor>(param.config);

  constexpr size_t                       n_pdu    = 5;
  std::array<byte_buffer, n_pdu>         original = {byte_buffer::create(udp_1).value(),
                                                     byte_buffer::create(udp_2).value(),
                                                     byte_buffer::create(udp_3).value(),
                                                     byte_buffer::create(udp_4).value(),
                                                     byte_buffer::create(udp_5).value()};
  std::array<byte_buffer, n_pdu>         compressed;
  std::array<rohc_decromp_result, n_pdu> decompressed;

  uint32_t original_len   = 0;
  uint32_t compressed_len = 0;
  for (size_t i = 0; i < n_pdu; i++) {
    original_len += original[i].length();
    compressed[i] = comp->compress(std::move(original[i].deep_copy().value()));
    ASSERT_FALSE(compressed[i].empty());
    compressed_len += compressed[i].length();
    logger.error("compressed[{}]_len={}", i, compressed[i].length());
  }

  if (param.config.profiles.is_profile_enabled(rohc_profile::profile0x0002) ||
      param.config.profiles.is_profile_enabled(rohc_profile::profile0x0004) ||
      param.config.profiles.is_profile_enabled(rohc_profile::profile0x0102) ||
      param.config.profiles.is_profile_enabled(rohc_profile::profile0x0104)) {
    EXPECT_LT(compressed_len, original_len);
  } else {
    EXPECT_GE(compressed_len, original_len);
  }

  uint32_t nof_feedback_packets = 0;
  for (size_t i = 0; i < n_pdu; i++) {
    decompressed[i] = decomp->decompress(std::move(compressed[i]));
    EXPECT_FALSE(decompressed[i].decomp_packet.empty());
    EXPECT_EQ(decompressed[i].decomp_packet, original[i]);
    if (!decompressed[i].feedback_packet.empty()) {
      nof_feedback_packets++;
      logger.error("i={} len={}", i, decompressed[i].feedback_packet.length());
    }
  }
  EXPECT_GE(nof_feedback_packets, 1);

  // No warnings or errors
  EXPECT_EQ(test_spy.get_warning_counter(), 0);
  EXPECT_EQ(test_spy.get_error_counter(), 0);
}

TEST_P(rohc_lib_test, compress_decompress_udp_with_feedback)
{
  rohc_test_params                       param  = GetParam();
  std::unique_ptr<rohc_lib_compressor>   comp   = std::make_unique<rohc_lib_compressor>(param.config);
  std::unique_ptr<rohc_lib_decompressor> decomp = std::make_unique<rohc_lib_decompressor>(param.config);

  constexpr size_t                       n_pdu    = 5;
  std::array<byte_buffer, n_pdu>         original = {byte_buffer::create(udp_1).value(),
                                                     byte_buffer::create(udp_2).value(),
                                                     byte_buffer::create(udp_3).value(),
                                                     byte_buffer::create(udp_4).value(),
                                                     byte_buffer::create(udp_5).value()};
  std::array<byte_buffer, n_pdu>         compressed;
  std::array<rohc_decromp_result, n_pdu> decompressed;

  uint32_t original_len         = 0;
  uint32_t compressed_len       = 0;
  uint32_t nof_feedback_packets = 0;
  for (size_t i = 0; i < n_pdu; i++) {
    // compress
    original_len += original[i].length();
    compressed[i] = comp->compress(std::move(original[i].deep_copy().value()));
    ASSERT_FALSE(compressed[i].empty());
    compressed_len += compressed[i].length();

    // decompress and deliver feedback
    decompressed[i] = decomp->decompress(std::move(compressed[i]));
    EXPECT_FALSE(decompressed[i].decomp_packet.empty());
    EXPECT_EQ(decompressed[i].decomp_packet, original[i]);
    if (!decompressed[i].feedback_packet.empty()) {
      EXPECT_TRUE(comp->handle_feedback(std::move(decompressed[i].feedback_packet)));
      nof_feedback_packets++;
    }
  }
  EXPECT_GE(nof_feedback_packets, 1);

  if (param.config.profiles.is_profile_enabled(rohc_profile::profile0x0002) ||
      param.config.profiles.is_profile_enabled(rohc_profile::profile0x0004) ||
      param.config.profiles.is_profile_enabled(rohc_profile::profile0x0102) ||
      param.config.profiles.is_profile_enabled(rohc_profile::profile0x0104)) {
    EXPECT_LT(compressed_len, original_len);
  } else {
    EXPECT_GE(compressed_len, original_len);
  }

  // No warnings or errors
  EXPECT_EQ(test_spy.get_warning_counter(), 0);
  EXPECT_EQ(test_spy.get_error_counter(), 0);
}

TEST_P(rohc_lib_test, compress_decompress_tcp)
{
  rohc_test_params                       param  = GetParam();
  std::unique_ptr<rohc_lib_compressor>   comp   = std::make_unique<rohc_lib_compressor>(param.config);
  std::unique_ptr<rohc_lib_decompressor> decomp = std::make_unique<rohc_lib_decompressor>(param.config);

  constexpr size_t                       n_pdu    = 5;
  std::array<byte_buffer, n_pdu>         original = {byte_buffer::create(tcp_1).value(),
                                                     byte_buffer::create(tcp_2).value(),
                                                     byte_buffer::create(tcp_3).value(),
                                                     byte_buffer::create(tcp_4).value(),
                                                     byte_buffer::create(tcp_5).value()};
  std::array<byte_buffer, n_pdu>         compressed;
  std::array<rohc_decromp_result, n_pdu> decompressed;

  uint32_t original_len   = 0;
  uint32_t compressed_len = 0;
  for (size_t i = 0; i < n_pdu; i++) {
    original_len += original[i].length();
    compressed[i] = comp->compress(std::move(original[i].deep_copy().value()));
    ASSERT_FALSE(compressed[i].empty());
    compressed_len += compressed[i].length();
  }

  if (param.config.profiles.is_profile_enabled(rohc_profile::profile0x0004) ||
      param.config.profiles.is_profile_enabled(rohc_profile::profile0x0006) ||
      param.config.profiles.is_profile_enabled(rohc_profile::profile0x0104)) {
    EXPECT_LT(compressed_len, original_len);
  } else {
    EXPECT_GE(compressed_len, original_len);
  }

  for (size_t i = 0; i < n_pdu; i++) {
    decompressed[i] = decomp->decompress(std::move(compressed[i]));
    EXPECT_FALSE(decompressed[i].decomp_packet.empty());
    EXPECT_EQ(decompressed[i].decomp_packet, original[i]);
  }

  // No warnings or errors
  EXPECT_EQ(test_spy.get_warning_counter(), 0);
  EXPECT_EQ(test_spy.get_error_counter(), 0);
}

TEST_P(rohc_lib_test, compress_decompress_tcp_with_feedback)
{
  rohc_test_params                       param  = GetParam();
  std::unique_ptr<rohc_lib_compressor>   comp   = std::make_unique<rohc_lib_compressor>(param.config);
  std::unique_ptr<rohc_lib_decompressor> decomp = std::make_unique<rohc_lib_decompressor>(param.config);

  constexpr size_t                       n_pdu    = 5;
  std::array<byte_buffer, n_pdu>         original = {byte_buffer::create(tcp_1).value(),
                                                     byte_buffer::create(tcp_2).value(),
                                                     byte_buffer::create(tcp_3).value(),
                                                     byte_buffer::create(tcp_4).value(),
                                                     byte_buffer::create(tcp_5).value()};
  std::array<byte_buffer, n_pdu>         compressed;
  std::array<rohc_decromp_result, n_pdu> decompressed;

  uint32_t original_len         = 0;
  uint32_t compressed_len       = 0;
  uint32_t nof_feedback_packets = 0;
  for (size_t i = 0; i < n_pdu; i++) {
    // compress
    original_len += original[i].length();
    compressed[i] = comp->compress(std::move(original[i].deep_copy().value()));
    ASSERT_FALSE(compressed[i].empty());
    compressed_len += compressed[i].length();

    // decompress and deliver feedback
    decompressed[i] = decomp->decompress(std::move(compressed[i]));
    EXPECT_FALSE(decompressed[i].decomp_packet.empty());
    EXPECT_EQ(decompressed[i].decomp_packet, original[i]);
    if (!decompressed[i].feedback_packet.empty()) {
      EXPECT_TRUE(comp->handle_feedback(std::move(decompressed[i].feedback_packet)));
      nof_feedback_packets++;
    }
  }
  EXPECT_GE(nof_feedback_packets, 1);

  if (param.config.profiles.is_profile_enabled(rohc_profile::profile0x0004) ||
      param.config.profiles.is_profile_enabled(rohc_profile::profile0x0006) ||
      param.config.profiles.is_profile_enabled(rohc_profile::profile0x0104)) {
    EXPECT_LT(compressed_len, original_len);
  } else {
    EXPECT_GE(compressed_len, original_len);
  }

  // No warnings or errors
  EXPECT_EQ(test_spy.get_warning_counter(), 0);
  EXPECT_EQ(test_spy.get_error_counter(), 0);
}

///////////////////////////////////////////////////////////////////
// Finally, instantiate all testcases for each supported SN size //
///////////////////////////////////////////////////////////////////
static std::string test_param_info_to_string(const ::testing::TestParamInfo<rohc_test_params>& info)
{
  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer), "{}", info.param.name);
  return fmt::to_string(buffer);
}

INSTANTIATE_TEST_SUITE_P(all_configs,
                         rohc_lib_test,
                         ::testing::ValuesIn(rohc_test_set.begin(), rohc_test_set.end()),
                         test_param_info_to_string);

INSTANTIATE_TEST_SUITE_P(all_configs,
                         rohc_lib_test_death,
                         ::testing::ValuesIn(rohc_test_set_invalid.begin(), rohc_test_set_invalid.end()),
                         test_param_info_to_string);

int main(int argc, char** argv)
{
  ocudulog::init();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
