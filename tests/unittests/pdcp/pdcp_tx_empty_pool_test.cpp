// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdcp_test_vectors.h"
#include "tests/unittests/pdcp/pdcp_tx_test_helpers.h"
#include <gtest/gtest.h>

using namespace ocudu;

constexpr uint32_t pool_size = 128;

/// Fixture class for PDCP TX tests
/// It requires TEST_P() and INSTANTIATE_TEST_SUITE_P() to create/spawn tests for each supported SN size
class pdcp_tx_empty_pool_test
  : public pdcp_tx_test_helper_default_crypto,
    public ::testing::Test,
    public ::testing::WithParamInterface<std::tuple<pdcp_sn_size, unsigned, rohc_test_params>>
{
protected:
  void SetUp() override
  {
    //  init test's logger
    ocudulog::init();
    logger.set_level(ocudulog::basic_levels::debug);

    // init RLC logger
    ocudulog::fetch_basic_logger("PDCP", false).set_level(ocudulog::basic_levels::debug);
    ocudulog::fetch_basic_logger("PDCP", false).set_hex_dump_max_size(100);
  }

  void TearDown() override
  {
    // flush logger after each test
    ocudulog::flush();
  }
};

/// Test empty pool handling
TEST_P(pdcp_tx_empty_pool_test, empty_pool)
{
  init(GetParam());
  uint32_t n_sdus          = pool_size + 1;
  auto     test_empty_pool = [this, n_sdus](uint32_t tx_next) {
    // Set state of PDCP entiy
    pdcp_tx_state st = {tx_next, tx_next, 0, tx_next, tx_next};
    pdcp_tx->set_state(st);
    pdcp_tx->configure_security(sec_cfg, security::integrity_enabled::off, security::ciphering_enabled::off);

    // Write first SDU
    for (uint32_t i = 0; i < n_sdus; i++) {
      auto sdu_buf = byte_buffer::create(sdu1);
      if (not sdu_buf.has_value()) {
        pdcp_tx->handle_sdu({});
        break;
      }
      pdcp_tx->handle_sdu(std::move(sdu_buf.value()));
    }
    // check nof max_count reached and max protocol failures.
    ASSERT_NE(test_frame.pdu_queue.size(), n_sdus);
    ASSERT_GE(test_frame.nof_protocol_failure, 1);
  };

  test_empty_pool(0);
}

///////////////////////////////////////////////////////////////////
// Finally, instantiate all testcases for each supported SN size //
///////////////////////////////////////////////////////////////////
static std::string
test_param_info_to_string(const ::testing::TestParamInfo<std::tuple<pdcp_sn_size, unsigned, rohc_test_params>>& info)
{
  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer),
                 "{}bit_{}",
                 pdcp_sn_size_to_uint(std::get<pdcp_sn_size>(info.param)),
                 std::get<rohc_test_params>(info.param).name);
  return fmt::to_string(buffer);
}

INSTANTIATE_TEST_SUITE_P(pdcp_tx_empty_pool_test_all_sn_sizes,
                         pdcp_tx_empty_pool_test,
                         ::testing::Combine(::testing::Values(pdcp_sn_size::size12bits, pdcp_sn_size::size18bits),
                                            ::testing::Values(1),
                                            ::testing::Values(cfg_rohc_disabled)),
                         test_param_info_to_string);

int main(int argc, char** argv)
{
  init_byte_buffer_segment_pool(pool_size);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
