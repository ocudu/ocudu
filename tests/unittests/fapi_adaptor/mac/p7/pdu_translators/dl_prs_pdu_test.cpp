// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "helpers.h"
#include "prs.h"
#include "tests/ocudu_test_requirements.h"
#include "ocudu/fapi_adaptor/precoding_codebook_generator.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi_adaptor;
using namespace unittests;

TEST(mac_fapi_prs_pdu_conversor_test, valid_pdu_should_pass)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-2");

  static constexpr unsigned cell_nof_prbs = 51;

  prs_info pdu      = build_valid_prs_pdu();
  auto     pm_tools = generate_precoding_codebooks(pmi_codebook_one_port{}, antenna_topology::one_port, 0);

  fapi::dl_tti_request         msg;
  fapi::dl_tti_request_builder builder(msg);
  convert_prs_mac_to_fapi(builder, pdu, *pm_tools.first, cell_nof_prbs);

  ASSERT_EQ(1U, msg.pdus.size());
  const auto* fapi_pdu = std::get_if<fapi::dl_prs_pdu>(&msg.pdus.front().pdu);
  ASSERT_NE(fapi_pdu, nullptr);

  EXPECT_EQ(pdu.scs, fapi_pdu->scs);
  EXPECT_EQ(pdu.cp, fapi_pdu->cp);
  EXPECT_EQ(pdu.n_id_prs, fapi_pdu->nid_prs);
  EXPECT_EQ(pdu.comb_size, fapi_pdu->comb_size);
  EXPECT_EQ(pdu.comb_offset, fapi_pdu->comb_offset);
  EXPECT_EQ(pdu.nof_symbols, fapi_pdu->num_symbols);
  EXPECT_EQ(pdu.symbols.start(), fapi_pdu->first_symbol);
  EXPECT_EQ(pdu.crbs, fapi_pdu->crbs);
  ASSERT_TRUE(fapi_pdu->prs_power_offset_db.has_value());
  EXPECT_EQ(static_cast<float>(pdu.power_offset_db), *fapi_pdu->prs_power_offset_db);
  EXPECT_EQ(cell_nof_prbs, fapi_pdu->precoding_and_beamforming.prg_size);

  // The PRS is transmitted with omnidirectional precoding, and not beamformed.
  const auto* pmi = std::get_if<fapi::precoding_matrix_index>(&fapi_pdu->precoding_and_beamforming.prg.precoding);
  ASSERT_NE(pmi, nullptr);
  EXPECT_EQ(pm_tools.first->map(mac_prs_precoding_info{}), *pmi);
  EXPECT_TRUE(fapi_pdu->precoding_and_beamforming.prg.beams.empty());
}
