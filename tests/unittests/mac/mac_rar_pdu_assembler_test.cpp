// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/mac/mac_dl/rar_pdu_assembler.h"
#include "mac_test_helpers.h"
#include "ocudu/adt/circular_array.h"
#include "ocudu/mac/ue_con_res_id.h"
#include "ocudu/support/bit_encoding.h"
#include "ocudu/support/test_utils.h"
#include <gtest/gtest.h>
#include <random>

using namespace ocudu;

std::random_device                      rd;
std::mt19937                            gen(rd());
std::uniform_int_distribution<unsigned> rnti_dist(to_value(rnti_t::MIN_CRNTI), to_value(rnti_t::MAX_CRNTI));
std::uniform_int_distribution<unsigned> rapid_dist(0, 63);
std::uniform_int_distribution<unsigned> mcs_dist(0, 15);
std::uniform_int_distribution<unsigned> time_res_dist(0, 15);
std::uniform_int_distribution<unsigned> freq_res_dist(0, 16383);
std::uniform_int_distribution<unsigned> ta_dist(0, 63);
std::uniform_int_distribution<unsigned> freq_hop_dist(0, 1);
std::uniform_int_distribution<uint8_t>  tpc_dist(0, 7);
std::uniform_int_distribution<unsigned> nof_ul_grants_per_rar(1, MAX_RAR_PDUS_PER_SLOT - 1);
std::uniform_int_distribution<unsigned> bi_dist(0, 15);

// Check TS 38.321 6.2.2 and 6.2.3.
static const unsigned RAR_PDU_SIZE = 8;

rar_ul_grant make_random_ul_grant()
{
  rar_ul_grant grant{};
  grant.freq_hop_flag            = freq_hop_dist(gen) > 0;
  grant.rapid                    = rapid_dist(gen);
  grant.temp_crnti               = to_rnti(rnti_dist(gen));
  grant.mcs                      = mcs_dist(gen);
  grant.freq_resource_assignment = freq_res_dist(gen);
  grant.time_resource_assignment = time_res_dist(gen);
  grant.tpc                      = tpc_dist(gen);
  grant.csi_req                  = freq_hop_dist(gen) > 0;
  grant.ta                       = ta_dist(gen);
  return grant;
}

rar_information make_random_rar_info(unsigned nof_ul_grants = 1, unsigned padding_bytes = 0)
{
  rar_information rar{};
  rar.pdsch_cfg.codewords.resize(1);
  rar.pdsch_cfg.codewords[0].tb_size_bytes = units::bytes{nof_ul_grants * RAR_PDU_SIZE + padding_bytes};
  rar.grants.resize(nof_ul_grants);
  for (unsigned i = 0; i < nof_ul_grants; ++i) {
    rar.grants[i] = make_random_ul_grant();
  }
  return rar;
}

/// \brief Checks whether it is the last subPDU of a MAC RAR PDU. The Extension field "E" should flag this information
/// according to TS 38.321 6.2.2.
static bool is_last_subpdu(span<const uint8_t> rar_pdu)
{
  return (rar_pdu[0] & (1U << 7U)) == 0;
}

/// \brief Checks whether the MAC RAR PDU contains a RAPID field as per TS 38.321 6.2.2.
static bool is_rapid_subpdu(span<const uint8_t> rar_pdu)
{
  return (rar_pdu[0] & (1U << 6U)) > 0;
}

/// Decode Backoff Indicator subPDU as per TS 38.321, Section 6.2.2. This subPDU has no payload.
static uint8_t decode_bi(span<const uint8_t> rar_pdu)
{
  return rar_pdu[0] & 0xfU;
}

/// Decode RAR UL PDU as per TS 38.321, Section 6.2.2 and 6.2.3.
rar_ul_grant decode_ul_grant(span<const uint8_t> rar_subpdu)
{
  byte_buffer  buf = byte_buffer::create(rar_subpdu).value();
  bit_decoder  dec(buf);
  rar_ul_grant ret{};

  dec.advance_bits(2);
  dec.unpack(ret.rapid, 6);
  dec.advance_bits(1);
  dec.unpack(ret.ta, 7 + 5);
  dec.unpack(ret.freq_hop_flag, 1);
  dec.unpack(ret.freq_resource_assignment, 14);
  dec.unpack(ret.time_resource_assignment, 4);
  uint8_t mcs;
  dec.unpack(mcs, 4);
  ret.mcs = mcs;
  dec.unpack(ret.tpc, 3);
  dec.unpack(ret.csi_req, 1);
  uint16_t rnti;
  dec.unpack(rnti, 16);
  ret.temp_crnti = to_rnti(rnti);

  TESTASSERT_EQ(RAR_PDU_SIZE, dec.nof_bytes());

  return ret;
}

bool operator==(const rar_ul_grant& lhs, const rar_ul_grant& rhs)
{
  return lhs.ta == rhs.ta and lhs.freq_hop_flag == rhs.freq_hop_flag and
         lhs.freq_resource_assignment == rhs.freq_resource_assignment and
         lhs.time_resource_assignment == rhs.time_resource_assignment and lhs.mcs == rhs.mcs and lhs.tpc == rhs.tpc and
         lhs.csi_req == rhs.csi_req and lhs.temp_crnti == rhs.temp_crnti;
}

/// Checks whether the MAC subPDU is a successRAR subPDU (T1=0, T2=1), as per TS 38.321 Figure 6.1.5a-3.
static bool is_successrar_subpdu(span<const uint8_t> rar_pdu)
{
  return (rar_pdu[0] & (1U << 6U)) == 0 and (rar_pdu[0] & (1U << 5U)) != 0;
}

// Check TS 38.321 6.1.5a and 6.2.3a.
static const unsigned SUCCESS_RAR_PDU_SIZE = 12;

/// Decoded content of a successRAR subPDU payload, as per TS 38.321, Figure 6.2.3a-2.
struct success_rar_content {
  ue_con_res_id_t con_res_id;
  uint8_t         tpc;
  uint8_t         harq_feedback_timing_indicator;
  uint8_t         pucch_resource_indicator;
  uint16_t        ta;
  rnti_t          crnti;
};

/// Decode successRAR subPDU (subheader + payload) as per TS 38.321, Figure 6.1.5a-3 and 6.2.3a-2.
success_rar_content decode_success_rar(span<const uint8_t> rar_subpdu)
{
  TESTASSERT_EQ(SUCCESS_RAR_PDU_SIZE, rar_subpdu.size());
  success_rar_content ret{};
  std::copy(rar_subpdu.begin() + 1, rar_subpdu.begin() + 1 + UE_CON_RES_ID_LEN, ret.con_res_id.begin());

  const uint8_t byte6                = rar_subpdu[7];
  ret.tpc                            = (byte6 >> 3U) & 0x3U;
  ret.harq_feedback_timing_indicator = byte6 & 0x7U;

  const uint8_t byte7          = rar_subpdu[8];
  ret.pucch_resource_indicator = (byte7 >> 4U) & 0xfU;
  ret.ta                       = static_cast<uint16_t>(((byte7 & 0xfU) << 8U) | rar_subpdu[9]);

  ret.crnti = to_rnti(static_cast<uint16_t>((rar_subpdu[10] << 8U) | rar_subpdu[11]));

  return ret;
}

/// Generates a random successRAR grant.
rar_ul_grant make_random_success_rar_grant()
{
  rar_ul_grant grant{};
  grant.rapid      = rapid_dist(gen);
  grant.temp_crnti = to_rnti(rnti_dist(gen));
  grant.ta         = ta_dist(gen);
  grant.tpc        = static_cast<int8_t>(tpc_dist(gen) & 0x3U);
  grant.type = rar_ul_grant::two_step_success_info{.harq_feedback_timing_indicator = static_cast<uint8_t>(gen() % 8),
                                                   .pucch_resource_indicator       = static_cast<uint8_t>(gen() % 16)};
  return grant;
}

/// Tests if the encoded RAR PDU matches the content in the original RAR.
void test_encoded_rar(const rar_information& original_rar, span<const uint8_t> rar_pdu)
{
  TESTASSERT(not rar_pdu.empty());
  TESTASSERT_EQ(RAR_PDU_SIZE * original_rar.grants.size(), rar_pdu.size());

  for (unsigned i = 0; i < original_rar.grants.size(); ++i) {
    span<const uint8_t> subpdu = rar_pdu.subspan(i * RAR_PDU_SIZE, RAR_PDU_SIZE);
    TESTASSERT_EQ(is_last_subpdu(subpdu), (i == original_rar.grants.size() - 1), "for index={}", i);
    TESTASSERT(is_rapid_subpdu(subpdu));

    rar_ul_grant grant2 = decode_ul_grant(subpdu);
    TESTASSERT(original_rar.grants[i] == grant2);
  }
}

TEST(rar_assembler_test, backoff_indicator_only)
{
  test_delimit_logger test_delim{"MAC assembler for Backoff Indicator only RAR"};

  rar_information rar_info{};
  rar_info.pdsch_cfg.codewords.resize(1);
  rar_info.pdsch_cfg.codewords[0].tb_size_bytes = units::bytes{1};
  rar_info.backoff_indicator                    = static_cast<uint8_t>(bi_dist(gen));

  static constexpr size_t  MAX_RAR_GRANT_SIZE = 64;
  ticking_ring_buffer_pool pdu_pool(MAX_DL_PDUS_PER_SLOT * MAX_GRANTS_PER_RAR * MAX_RAR_GRANT_SIZE, 1, 10240);
  test_helpers::dummy_mac_cell_rach_handler rach_handler;
  rar_pdu_assembler                         assembler(pdu_pool, rach_handler);
  span<const uint8_t>                       bytes = assembler.encode_rar_pdu(rar_info);

  ASSERT_EQ(1U, bytes.size());
  EXPECT_TRUE(is_last_subpdu(bytes));
  EXPECT_FALSE(is_rapid_subpdu(bytes));
  EXPECT_EQ(*rar_info.backoff_indicator, decode_bi(bytes));
}

TEST(rar_assembler_test, backoff_indicator_with_ul_grants)
{
  test_delimit_logger test_delim{"MAC assembler for Backoff Indicator plus UL grants"};

  rar_information rar_info                      = make_random_rar_info(nof_ul_grants_per_rar(gen));
  rar_info.backoff_indicator                    = static_cast<uint8_t>(bi_dist(gen));
  const unsigned nof_grants                     = rar_info.grants.size();
  rar_info.pdsch_cfg.codewords[0].tb_size_bytes = units::bytes{1U + nof_grants * RAR_PDU_SIZE};

  static constexpr size_t  MAX_RAR_GRANT_SIZE = 64;
  ticking_ring_buffer_pool pdu_pool(MAX_DL_PDUS_PER_SLOT * MAX_GRANTS_PER_RAR * MAX_RAR_GRANT_SIZE, 1, 10240);
  test_helpers::dummy_mac_cell_rach_handler rach_handler;
  rar_pdu_assembler                         assembler(pdu_pool, rach_handler);
  span<const uint8_t>                       bytes = assembler.encode_rar_pdu(rar_info);

  ASSERT_EQ(1U + RAR_PDU_SIZE * nof_grants, bytes.size());

  span<const uint8_t> bi_subpdu = bytes.subspan(0, 1);
  EXPECT_FALSE(is_last_subpdu(bi_subpdu));
  EXPECT_FALSE(is_rapid_subpdu(bi_subpdu));
  EXPECT_EQ(*rar_info.backoff_indicator, decode_bi(bi_subpdu));

  test_encoded_rar(rar_info, bytes.subspan(1, RAR_PDU_SIZE * nof_grants));
}

TEST(rar_assembler_test, multiple_random_ul_grants)
{
  static constexpr size_t MAX_RAR_GRANT_SIZE = 64;
  test_delimit_logger     test_delim{"MAC assembler for multiple UL grants"};

  rar_information          rar_info = make_random_rar_info(nof_ul_grants_per_rar(gen));
  ticking_ring_buffer_pool pdu_pool(MAX_DL_PDUS_PER_SLOT * MAX_GRANTS_PER_RAR * MAX_RAR_GRANT_SIZE, 1, 10240);

  test_helpers::dummy_mac_cell_rach_handler rach_handler;
  rar_pdu_assembler                         assembler(pdu_pool, rach_handler);
  span<const uint8_t>                       bytes = assembler.encode_rar_pdu(rar_info);

  test_encoded_rar(rar_info, bytes);
}

/// In this test, we verify that the RAR PDU assembler is able to store past RAR PDUs for a short period of time,
/// so that the output PDUs can be referenced by lower layers without risking dangling pointers.
TEST(rar_assembler_test, rar_assembler_maintains_old_results)
{
  static constexpr size_t MAX_RAR_GRANT_SIZE = 64;

  test_delimit_logger test_delim{"MAC assembler maintains previous results"};

  ticking_ring_buffer_pool pdu_pool(
      MAX_DL_PDUS_PER_SLOT * MAX_GRANTS_PER_RAR * MAX_RAR_GRANT_SIZE, NOF_SUBFRAMES_PER_FRAME, 10240);
  test_helpers::dummy_mac_cell_rach_handler rach_handler;
  rar_pdu_assembler                         assembler(pdu_pool, rach_handler);

  // The RAR assembler has to internally store previous slot results. This variable defines a reasonable slot duration
  // that the RAR assembler has to keep these results stored.
  static constexpr unsigned MEMORY_RESULT_IN_SLOTS = MAX_RAR_PDUS_PER_SLOT * NOF_SUBFRAMES_PER_FRAME;

  circular_array<span<const uint8_t>, MEMORY_RESULT_IN_SLOTS> previous_pdus;
  circular_array<rar_information, MEMORY_RESULT_IN_SLOTS>     previous_rars;

  unsigned nof_slots_tests = MEMORY_RESULT_IN_SLOTS * 64;
  for (unsigned i = 0; i < nof_slots_tests; ++i) {
    pdu_pool.tick(i);
    if (i >= MEMORY_RESULT_IN_SLOTS) {
      // Test old results to check if they are still valid.
      test_encoded_rar(previous_rars[i], previous_pdus[i]);
    }
    previous_rars[i] = make_random_rar_info(nof_ul_grants_per_rar(gen));
    previous_pdus[i] = assembler.encode_rar_pdu(previous_rars[i]);
  }
}

TEST(rar_assembler_test, success_rar_grant_zero_fills_unresolved_con_res_id)
{
  test_delimit_logger test_delim{"MAC assembler for successRAR grant with unresolved Contention Resolution Id"};

  rar_information rar_info{};
  rar_info.pdsch_cfg.codewords.resize(1);
  rar_info.pdsch_cfg.codewords[0].tb_size_bytes = units::bytes{SUCCESS_RAR_PDU_SIZE};
  rar_info.grants.resize(1);
  rar_info.grants[0] = make_random_success_rar_grant();

  static constexpr size_t  MAX_RAR_GRANT_SIZE = 64;
  ticking_ring_buffer_pool pdu_pool(MAX_DL_PDUS_PER_SLOT * MAX_GRANTS_PER_RAR * MAX_RAR_GRANT_SIZE, 1, 10240);
  // dummy_mac_cell_rach_handler::resolve_msga_con_res_id returns nullopt unless a Contention Resolution Id was
  // registered via add_msga_con_res_id.
  test_helpers::dummy_mac_cell_rach_handler rach_handler;
  rar_pdu_assembler                         assembler(pdu_pool, rach_handler);
  span<const uint8_t>                       bytes = assembler.encode_rar_pdu(rar_info);

  ASSERT_EQ(SUCCESS_RAR_PDU_SIZE, bytes.size());
  EXPECT_TRUE(is_last_subpdu(bytes));
  EXPECT_FALSE(is_rapid_subpdu(bytes));
  EXPECT_TRUE(is_successrar_subpdu(bytes));

  const rar_ul_grant&       grant   = rar_info.grants[0];
  const auto&               info    = std::get<rar_ul_grant::two_step_success_info>(grant.type);
  const success_rar_content decoded = decode_success_rar(bytes);

  EXPECT_EQ(decoded.con_res_id, ue_con_res_id_t{});
  EXPECT_EQ(decoded.tpc, static_cast<uint8_t>(grant.tpc) & 0x3U);
  EXPECT_EQ(decoded.harq_feedback_timing_indicator, info.harq_feedback_timing_indicator);
  EXPECT_EQ(decoded.pucch_resource_indicator, info.pucch_resource_indicator);
  EXPECT_EQ(decoded.ta, grant.ta);
  EXPECT_EQ(decoded.crnti, grant.temp_crnti);
}

TEST(rar_assembler_test, success_rar_grant_encodes_resolved_con_res_id)
{
  test_delimit_logger test_delim{"MAC assembler for successRAR grant with resolved Contention Resolution Id"};

  rar_information rar_info{};
  rar_info.pdsch_cfg.codewords.resize(1);
  rar_info.pdsch_cfg.codewords[0].tb_size_bytes = units::bytes{SUCCESS_RAR_PDU_SIZE};
  rar_info.grants.resize(1);
  rar_info.grants[0] = make_random_success_rar_grant();

  static constexpr size_t  MAX_RAR_GRANT_SIZE = 64;
  ticking_ring_buffer_pool pdu_pool(MAX_DL_PDUS_PER_SLOT * MAX_GRANTS_PER_RAR * MAX_RAR_GRANT_SIZE, 1, 10240);

  test_helpers::dummy_mac_cell_rach_handler rach_handler;
  rach_handler.con_res_id = ue_con_res_id_t{0x11, 0x22, 0x33, 0x44, 0x55, 0x66};

  rar_pdu_assembler   assembler(pdu_pool, rach_handler);
  span<const uint8_t> bytes = assembler.encode_rar_pdu(rar_info);

  const success_rar_content decoded = decode_success_rar(bytes);
  EXPECT_EQ(decoded.con_res_id, *rach_handler.con_res_id);
}

TEST(rar_assembler_test, mixed_fallback_and_success_rar_grants)
{
  test_delimit_logger test_delim{"MAC assembler for mixed fallbackRAR and successRAR grants"};

  rar_information rar_info{};
  rar_info.grants.resize(2);
  // fallbackRAR (four_step_info, the default grant type).
  rar_info.grants[0] = make_random_ul_grant();
  // successRAR.
  rar_info.grants[1] = make_random_success_rar_grant();
  rar_info.pdsch_cfg.codewords.resize(1);
  rar_info.pdsch_cfg.codewords[0].tb_size_bytes = units::bytes{RAR_PDU_SIZE + SUCCESS_RAR_PDU_SIZE};

  static constexpr size_t  MAX_RAR_GRANT_SIZE = 64;
  ticking_ring_buffer_pool pdu_pool(MAX_DL_PDUS_PER_SLOT * MAX_GRANTS_PER_RAR * MAX_RAR_GRANT_SIZE, 1, 10240);
  test_helpers::dummy_mac_cell_rach_handler rach_handler;
  rar_pdu_assembler                         assembler(pdu_pool, rach_handler);
  span<const uint8_t>                       bytes = assembler.encode_rar_pdu(rar_info);

  ASSERT_EQ(RAR_PDU_SIZE + SUCCESS_RAR_PDU_SIZE, bytes.size());

  span<const uint8_t> fallback_subpdu = bytes.subspan(0, RAR_PDU_SIZE);
  EXPECT_FALSE(is_last_subpdu(fallback_subpdu));
  EXPECT_TRUE(is_rapid_subpdu(fallback_subpdu));
  EXPECT_TRUE(rar_info.grants[0] == decode_ul_grant(fallback_subpdu));

  span<const uint8_t> success_subpdu = bytes.subspan(RAR_PDU_SIZE, SUCCESS_RAR_PDU_SIZE);
  EXPECT_TRUE(is_last_subpdu(success_subpdu));
  EXPECT_FALSE(is_rapid_subpdu(success_subpdu));
  EXPECT_TRUE(is_successrar_subpdu(success_subpdu));
}
