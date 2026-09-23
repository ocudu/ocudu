// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "helpers.h"
#include "ocudu_test_requirements.h"
#include "prs.h"
#include "ocudu/fapi_adaptor/precoding_codebook_generator.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi_adaptor;
using namespace unittests;

TEST(mac_fapi_prs_pdu_conversor_test, valid_pdu_should_pass)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-2");

  const prs_info pdu = build_valid_prs_pdu();

  constexpr unsigned cell_nof_prbs = 51;
  auto               pm_tools      = generate_precoding_codebooks(pmi_codebook_one_port{}, 0);

  fapi::dl_tti_request         msg;
  fapi::dl_tti_request_builder builder(msg);
  convert_prs_mac_to_fapi(builder, pdu, *std::get<0>(pm_tools), cell_nof_prbs);

  ASSERT_EQ(msg.pdus.size(), 1U);
  const auto* fapi_pdu = std::get_if<fapi::dl_prs_pdu>(&msg.pdus.front().pdu);
  ASSERT_TRUE(fapi_pdu != nullptr);

  ASSERT_EQ(pdu.scs, fapi_pdu->scs);
  ASSERT_EQ(pdu.cp, fapi_pdu->cp);
  ASSERT_EQ(pdu.n_id_prs, fapi_pdu->nid_prs);
  ASSERT_EQ(pdu.comb_size, fapi_pdu->comb_size);
  ASSERT_EQ(pdu.comb_offset, fapi_pdu->comb_offset);
  ASSERT_EQ(pdu.nof_symbols, fapi_pdu->num_symbols);
  ASSERT_EQ(pdu.symbols.start(), fapi_pdu->first_symbol);
  ASSERT_EQ(pdu.crbs, fapi_pdu->crbs);
  ASSERT_TRUE(fapi_pdu->prs_power_offset_db.has_value());
  ASSERT_EQ(static_cast<float>(pdu.power_offset_db), fapi_pdu->prs_power_offset_db.value());
  ASSERT_EQ(cell_nof_prbs, fapi_pdu->precoding_and_beamforming.prg_size);
}
