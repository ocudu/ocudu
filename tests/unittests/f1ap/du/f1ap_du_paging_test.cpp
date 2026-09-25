// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_du_test_helpers.h"
#include "test_doubles/f1ap/f1ap_test_messages.h"
#include "tests/ocudu_test_requirements.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/ran/pcch/paging_helper.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;

TEST(hashed_id_calculation_test, ts38304_annex_A_using_32bit_FCS_example)
{
  // TS 38.304, Annex A (informative) - Example of Hashed ID calculation using 32-bit FCS
  const uint64_t tmsi48 = 0x12341234U;

  // step a)
  constexpr static uint64_t Y1_expected = 0xc704dd7bU;
  constexpr static uint64_t denominator = 0x104c11db7U;
  constexpr static uint64_t numerator_a = 0xffffffff00000000U;
  ASSERT_EQ(paging_helper::detail::gf2_mod(numerator_a, denominator), Y1_expected);

  // step b)
  constexpr static uint64_t Y2_expected = 0x1d66f1a6U;
  constexpr static uint64_t numerator_b = tmsi48 << 32U;
  ASSERT_EQ(paging_helper::detail::gf2_mod(numerator_b, denominator), Y2_expected);

  // HASHED_ID = FCS = ones complement of (remainder Y1 XOR remainder Y2)
  constexpr static uint32_t Hashed_ID_expected = 0x259dd322U;
  ASSERT_EQ(paging_helper::detail::compute_hashed_ID(tmsi48), Hashed_ID_expected);
}

class f1ap_du_paging_test : public f1ap_du_test
{
protected:
  f1ap_du_paging_test()
  {
    OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BAS-16-1");

    // Test Preamble.
    run_f1_setup_procedure();

    this->f1c_gw.clear_tx_pdus();
  }
};

TEST_F(f1ap_du_paging_test, when_f1ap_paging_message_received_then_it_is_propagated_to_lower_layers)
{
  const auto   tmsi = test_rng::uniform_int<uint64_t>(0U, (static_cast<uint64_t>(1U) << 48U) - 1U);
  f1ap_message msg  = test_helpers::generate_f1ap_paging_message(tmsi);

  ASSERT_FALSE(this->paging_handler.last_paging_info.has_value());
  this->f1ap->handle_message(msg);
  ASSERT_TRUE(this->paging_handler.last_paging_info.has_value());
  auto& paging_info = this->paging_handler.last_paging_info.value();
  ASSERT_EQ(paging_info.ue_identity, tmsi % 1024U);
  ASSERT_EQ(paging_info.paging_type_indicator, paging_identity_type::cn_ue_paging_identity);
}

TEST_F(f1ap_du_paging_test, when_f1ap_paging_message_received_with_edrx_then_the_ue_hashed_id_is_computed)
{
  const auto   tmsi        = test_rng::uniform_int<uint64_t>(0U, (static_cast<uint64_t>(1U) << 48U) - 1U);
  f1ap_message msg         = test_helpers::generate_f1ap_paging_message(tmsi);
  auto&        asn1_paging = msg.pdu.init_msg().value.paging();
  asn1_paging->nr_paginge_drx_info_present                         = true;
  asn1_paging->nr_paginge_drx_info.nrpaging_e_drx_cycle_idle.value = asn1::f1ap::nr_paging_e_drx_cycle_idle_opts::hf2;
  asn1_paging->nr_paginge_drx_info.nrpaging_time_win_present       = true;
  asn1_paging->nr_paginge_drx_info.nrpaging_time_win.value         = asn1::f1ap::nr_paging_time_win_opts::s4;

  this->f1ap->handle_message(msg);
  ASSERT_TRUE(this->paging_handler.last_paging_info.has_value());
  auto& paging_info = this->paging_handler.last_paging_info.value();
  ASSERT_EQ(paging_info.ue_identity, tmsi % 4096U);
  ASSERT_TRUE(paging_info.edrx.has_value());
  ASSERT_EQ(paging_info.edrx->hashed_ue_identity, paging_helper::compute_UE_ID_H(tmsi));
  ASSERT_EQ(paging_info.edrx->cycle_idle.count(), NOF_SFNS * 2);
  ASSERT_EQ(paging_info.edrx->ptw_len, radio_frames{(int)std::round(4 * 1.28 * 100)});
}
