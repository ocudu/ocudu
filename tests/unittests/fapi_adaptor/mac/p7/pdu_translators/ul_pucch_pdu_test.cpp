// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "helpers.h"
#include "pucch.h"
#include "tests/ocudu_test_requirements.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi_adaptor;
using namespace unittests;

/// Converts the given MAC PUCCH PDU into a FAPI one.
static fapi::ul_pucch_pdu convert_pucch(const pucch_info& mac_pdu)
{
  fapi::ul_pucch_pdu         fapi_pdu;
  fapi::ul_pucch_pdu_builder builder(fapi_pdu);
  convert_pucch_mac_to_fapi(builder, mac_pdu);
  return fapi_pdu;
}

/// Checks the fields that are common to all the PUCCH formats.
static void validate_common_pucch_fields(const pucch_info& mac_pdu, const fapi::ul_pucch_pdu& fapi_pdu)
{
  EXPECT_EQ(mac_pdu.bwp_cfg->cp, fapi_pdu.cp);
  EXPECT_EQ(mac_pdu.bwp_cfg->scs, fapi_pdu.scs);
  EXPECT_EQ(mac_pdu.bwp_cfg->crbs, fapi_pdu.bwp);
  EXPECT_EQ(mac_pdu.crnti, fapi_pdu.rnti);
  EXPECT_EQ(mac_pdu.grant_prbs(), fapi_pdu.prbs);
  EXPECT_EQ(mac_pdu.res->syms, fapi_pdu.symbols);
  EXPECT_EQ(mac_pdu.res->second_hop_prb, fapi_pdu.second_hop_prb);
}

/// Checks the UCI bit lengths of a PUCCH format 2, 3 or 4 PDU.
template <typename FapiFormat>
static void validate_f2_f3_f4_uci_bits(const pucch_info& mac_pdu, const FapiFormat& fapi_format)
{
  EXPECT_EQ(mac_pdu.uci_bits.sr_bits, fapi_format.sr_bit_len);
  EXPECT_EQ(mac_pdu.uci_bits.harq_ack_nof_bits, fapi_format.bit_len_harq.value());
  EXPECT_EQ(mac_pdu.uci_bits.csi_part1_nof_bits, fapi_format.csi_part1_bit_length.value());
}

/// Checks the scrambling and hopping identifiers of a PUCCH format 3 or 4 PDU.
template <typename MacFormat, typename FapiFormat>
static void validate_f3_f4_identifiers(const MacFormat& mac_format, const FapiFormat& fapi_format)
{
  EXPECT_EQ(mac_format.n_id_0_scrambling, fapi_format.nid0_pucch_dmrs_scrambling);
  EXPECT_EQ(mac_format.n_id_scrambling, fapi_format.nid_pucch_scrambling);
  EXPECT_EQ(mac_format.n_id_hopping, fapi_format.nid_pucch_hopping);
}

TEST(mac_fapi_ul_pucch_format0_pdu_conversor_test, ul_pucch_format0_pdu_valid_should_pass)
{
  pucch_info_test_helper pdu_test = build_valid_pucch_format_0_pdu();
  const pucch_info&      mac_pdu  = pdu_test.info;

  fapi::ul_pucch_pdu fapi_pdu = convert_pucch(mac_pdu);

  const auto* format0 = std::get_if<fapi::ul_pucch_pdu_format_0>(&fapi_pdu.format);
  ASSERT_NE(format0, nullptr);
  validate_common_pucch_fields(mac_pdu, fapi_pdu);
  EXPECT_TRUE(fapi_pdu.second_hop_prb.has_value());

  EXPECT_EQ(std::get<pucch_resource::f0_config>(mac_pdu.res->format_params).initial_cyclic_shift,
            format0->initial_cyclic_shift);
  EXPECT_EQ(std::get<pucch_info::f0_config>(mac_pdu.format_params).n_id_hopping, format0->nid_pucch_hopping);

  // UCI bits.
  EXPECT_TRUE(format0->sr_present);
  EXPECT_EQ(mac_pdu.uci_bits.harq_ack_nof_bits, format0->bit_len_harq.value());
}

TEST(mac_fapi_ul_pucch_format1_pdu_conversor_test, ul_pucch_format1_pdu_valid_should_pass)
{
  pucch_info_test_helper pdu_test = build_valid_pucch_format_1_pdu();
  const pucch_info&      mac_pdu  = pdu_test.info;

  fapi::ul_pucch_pdu fapi_pdu = convert_pucch(mac_pdu);

  const auto* format1 = std::get_if<fapi::ul_pucch_pdu_format_1>(&fapi_pdu.format);
  ASSERT_NE(format1, nullptr);
  validate_common_pucch_fields(mac_pdu, fapi_pdu);

  const auto& res_f1 = std::get<pucch_resource::f1_config>(mac_pdu.res->format_params);
  EXPECT_EQ(res_f1.time_domain_occ, format1->time_domain_occ_index);
  EXPECT_EQ(res_f1.initial_cyclic_shift, format1->initial_cyclic_shift);
  EXPECT_EQ(std::get<pucch_info::f1_config>(mac_pdu.format_params).n_id_hopping, format1->nid_pucch_hopping);

  // UCI bits. The helper requests no SR.
  EXPECT_FALSE(format1->sr_present);
  EXPECT_EQ(mac_pdu.uci_bits.harq_ack_nof_bits, format1->bit_len_harq.value());
}

TEST(mac_fapi_ul_pucch_format2_pdu_conversor_test, ul_pucch_format2_pdu_valid_should_pass)
{
  pucch_info_test_helper pdu_test = build_valid_pucch_format_2_pdu();
  const pucch_info&      mac_pdu  = pdu_test.info;

  fapi::ul_pucch_pdu fapi_pdu = convert_pucch(mac_pdu);

  const auto* format2 = std::get_if<fapi::ul_pucch_pdu_format_2>(&fapi_pdu.format);
  ASSERT_NE(format2, nullptr);
  validate_common_pucch_fields(mac_pdu, fapi_pdu);

  const auto& f2 = std::get<pucch_info::f2_config>(mac_pdu.format_params);
  EXPECT_EQ(f2.n_id_0_scrambling, format2->nid0_pucch_dmrs_scrambling);
  EXPECT_EQ(f2.n_id_scrambling, format2->nid_pucch_scrambling);
  validate_f2_f3_f4_uci_bits(mac_pdu, *format2);
}

TEST(mac_fapi_ul_pucch_format3_pdu_conversor_test, ul_pucch_format3_pdu_valid_should_pass)
{
  pucch_info_test_helper pdu_test = build_valid_pucch_format_3_pdu();
  const pucch_info&      mac_pdu  = pdu_test.info;

  fapi::ul_pucch_pdu fapi_pdu = convert_pucch(mac_pdu);

  const auto* format3 = std::get_if<fapi::ul_pucch_pdu_format_3>(&fapi_pdu.format);
  ASSERT_NE(format3, nullptr);
  validate_common_pucch_fields(mac_pdu, fapi_pdu);

  const auto& res_f3 = std::get<pucch_resource::f3_config>(mac_pdu.res->format_params);
  EXPECT_EQ(res_f3.additional_dmrs, format3->add_dmrs_flag);
  EXPECT_EQ(res_f3.pi_2_bpsk, format3->pi2_bpsk);
  validate_f3_f4_identifiers(std::get<pucch_info::f3_config>(mac_pdu.format_params), *format3);
  validate_f2_f3_f4_uci_bits(mac_pdu, *format3);
}

TEST(mac_fapi_ul_pucch_format4_pdu_conversor_test, ul_pucch_format4_pdu_valid_should_pass)
{
  pucch_info_test_helper pdu_test = build_valid_pucch_format_4_pdu();
  const pucch_info&      mac_pdu  = pdu_test.info;

  fapi::ul_pucch_pdu fapi_pdu = convert_pucch(mac_pdu);

  const auto* format4 = std::get_if<fapi::ul_pucch_pdu_format_4>(&fapi_pdu.format);
  ASSERT_NE(format4, nullptr);
  validate_common_pucch_fields(mac_pdu, fapi_pdu);

  const auto& res_f4 = std::get<pucch_resource::f4_config>(mac_pdu.res->format_params);
  EXPECT_EQ(res_f4.additional_dmrs, format4->add_dmrs_flag);
  EXPECT_EQ(res_f4.pi_2_bpsk, format4->pi2_bpsk);
  EXPECT_EQ(static_cast<unsigned>(res_f4.occ_length), format4->pre_dft_occ_len);
  EXPECT_EQ(static_cast<unsigned>(res_f4.occ_index), format4->pre_dft_occ_idx);
  validate_f3_f4_identifiers(std::get<pucch_info::f4_config>(mac_pdu.format_params), *format4);
  validate_f2_f3_f4_uci_bits(mac_pdu, *format4);
}

TEST(mac_fapi_ul_pucch_pdu_conversor_test, pucch_without_second_hop_has_no_second_hop_prb)
{
  struct format_builder {
    const char* name;
    pucch_info_test_helper (*build)();
  };
  static constexpr format_builder builders[] = {
      format_builder{.name = "format 0", .build = build_valid_pucch_format_0_pdu},
      format_builder{.name = "format 1", .build = build_valid_pucch_format_1_pdu},
      format_builder{.name = "format 2", .build = build_valid_pucch_format_2_pdu},
      format_builder{.name = "format 3", .build = build_valid_pucch_format_3_pdu},
      format_builder{.name = "format 4", .build = build_valid_pucch_format_4_pdu}};

  for (const auto& format : builders) {
    SCOPED_TRACE(format.name);

    pucch_info_test_helper pdu_test = format.build();
    pdu_test.res->second_hop_prb.reset();

    fapi::ul_pucch_pdu fapi_pdu = convert_pucch(pdu_test.info);

    validate_common_pucch_fields(pdu_test.info, fapi_pdu);
    EXPECT_FALSE(fapi_pdu.second_hop_prb.has_value());
  }
}

namespace {

class mac_fapi_ul_pucch_multi_slot_fixture : public ::testing::TestWithParam<pucch_repetition_tx_slot>
{};

} // namespace

TEST_P(mac_fapi_ul_pucch_multi_slot_fixture, multi_slot_tx_indicator_matches_the_repetition_position)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-17-1");

  // Repetition state used to be a per-format field that was silently dropped at the FAPI boundary; verify that every
  // position reaches the FAPI PDU's multi_slot_tx_indicator from the common part of pucch_info.
  pucch_info_test_helper pdu_test = build_valid_pucch_format_1_pdu();
  if (GetParam() == pucch_repetition_tx_slot::no_multi_slot) {
    pdu_test.info.repetition.reset();
  } else {
    pdu_test.info.repetition = pucch_info::repetition_info{.anchor_slot = slot_point{}, .position = GetParam()};
  }

  fapi::ul_pucch_pdu fapi_pdu = convert_pucch(pdu_test.info);

  EXPECT_EQ(GetParam(), fapi_pdu.multi_slot_tx_indicator);
}

INSTANTIATE_TEST_SUITE_P(repetition_positions,
                         mac_fapi_ul_pucch_multi_slot_fixture,
                         ::testing::Values(pucch_repetition_tx_slot::no_multi_slot,
                                           pucch_repetition_tx_slot::starts,
                                           pucch_repetition_tx_slot::continues,
                                           pucch_repetition_tx_slot::ends));

TEST(mac_fapi_ul_pucch_format1_pdu_conversor_test, sr_is_present_only_when_an_sr_bit_is_requested)
{
  for (sr_nof_bits sr_bits : {sr_nof_bits::no_sr, sr_nof_bits::one}) {
    pucch_info_test_helper pdu_test = build_valid_pucch_format_1_pdu();
    pdu_test.info.uci_bits.sr_bits  = sr_bits;

    fapi::ul_pucch_pdu fapi_pdu = convert_pucch(pdu_test.info);

    const auto* format1 = std::get_if<fapi::ul_pucch_pdu_format_1>(&fapi_pdu.format);
    ASSERT_NE(format1, nullptr);
    EXPECT_EQ(sr_bits == sr_nof_bits::one, format1->sr_present);
  }
}

namespace {

class mac_fapi_ul_pucch_format2_sr_fixture : public ::testing::TestWithParam<sr_nof_bits>
{};

} // namespace

TEST_P(mac_fapi_ul_pucch_format2_sr_fixture, sr_bit_length_is_forwarded)
{
  pucch_info_test_helper pdu_test = build_valid_pucch_format_2_pdu();
  pdu_test.info.uci_bits.sr_bits  = GetParam();

  fapi::ul_pucch_pdu fapi_pdu = convert_pucch(pdu_test.info);

  const auto* format2 = std::get_if<fapi::ul_pucch_pdu_format_2>(&fapi_pdu.format);
  ASSERT_NE(format2, nullptr);
  EXPECT_EQ(GetParam(), format2->sr_bit_len);
}

INSTANTIATE_TEST_SUITE_P(
    sr_lengths,
    mac_fapi_ul_pucch_format2_sr_fixture,
    ::testing::Values(sr_nof_bits::no_sr, sr_nof_bits::one, sr_nof_bits::two, sr_nof_bits::three, sr_nof_bits::four));

TEST(mac_fapi_ul_pucch_format3_pdu_conversor_test, dmrs_cyclic_shift_is_always_zero)
{
  pucch_info_test_helper pdu_test = build_valid_pucch_format_3_pdu();

  fapi::ul_pucch_pdu fapi_pdu = convert_pucch(pdu_test.info);

  const auto* format3 = std::get_if<fapi::ul_pucch_pdu_format_3>(&fapi_pdu.format);
  ASSERT_NE(format3, nullptr);
  EXPECT_EQ(0U, format3->m0_pucch_dmrs_cyclic_shift);
}

namespace {

/// Parameters of a PUCCH format 4 OCC configuration and the DMRS cyclic shift expected for it.
struct format4_occ_param {
  pucch_f4_occ_len occ_length;
  pucch_f4_occ_idx occ_index;
  unsigned         expected_m0;
};

class mac_fapi_ul_pucch_format4_occ_fixture : public ::testing::TestWithParam<format4_occ_param>
{};

} // namespace

TEST_P(mac_fapi_ul_pucch_format4_occ_fixture, occ_configuration_selects_the_dmrs_cyclic_shift)
{
  pucch_info_test_helper pdu_test = build_valid_pucch_format_4_pdu();
  auto&                  res_f4   = std::get<pucch_resource::f4_config>(pdu_test.res->format_params);
  res_f4.occ_length               = GetParam().occ_length;
  res_f4.occ_index                = GetParam().occ_index;

  fapi::ul_pucch_pdu fapi_pdu = convert_pucch(pdu_test.info);

  const auto* format4 = std::get_if<fapi::ul_pucch_pdu_format_4>(&fapi_pdu.format);
  ASSERT_NE(format4, nullptr);
  EXPECT_EQ(static_cast<unsigned>(GetParam().occ_length), format4->pre_dft_occ_len);
  EXPECT_EQ(static_cast<unsigned>(GetParam().occ_index), format4->pre_dft_occ_idx);
  EXPECT_EQ(GetParam().expected_m0, format4->m0_pucch_dmrs_cyclic_shift);
}

INSTANTIATE_TEST_SUITE_P(
    occ_configurations,
    mac_fapi_ul_pucch_format4_occ_fixture,
    ::testing::Values(
        format4_occ_param{.occ_length = pucch_f4_occ_len::n2, .occ_index = pucch_f4_occ_idx::n0, .expected_m0 = 0},
        format4_occ_param{.occ_length = pucch_f4_occ_len::n2, .occ_index = pucch_f4_occ_idx::n1, .expected_m0 = 6},
        format4_occ_param{.occ_length = pucch_f4_occ_len::n4, .occ_index = pucch_f4_occ_idx::n0, .expected_m0 = 0},
        format4_occ_param{.occ_length = pucch_f4_occ_len::n4, .occ_index = pucch_f4_occ_idx::n1, .expected_m0 = 6},
        format4_occ_param{.occ_length = pucch_f4_occ_len::n4, .occ_index = pucch_f4_occ_idx::n2, .expected_m0 = 3},
        format4_occ_param{.occ_length = pucch_f4_occ_len::n4, .occ_index = pucch_f4_occ_idx::n3, .expected_m0 = 9}),
    [](const ::testing::TestParamInfo<format4_occ_param>& param_info) {
      return fmt::format("occ_len_{}_occ_idx_{}",
                         static_cast<unsigned>(param_info.param.occ_length),
                         static_cast<unsigned>(param_info.param.occ_index));
    });
