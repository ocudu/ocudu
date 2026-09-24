// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu_test_requirements.h"
#include "ocudu/adt/format.h"
#include "ocudu/fapi/p7/builders/dl_prs_pdu_builder.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi;

TEST(dl_prs_pdu_builder, valid_basic_parameters_passes)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-2");

  dl_prs_pdu         pdu;
  dl_prs_pdu_builder builder(pdu);

  subcarrier_spacing scs = subcarrier_spacing::kHz30;
  cyclic_prefix      cp  = cyclic_prefix::NORMAL;
  builder.set_bwp_parameters(scs, cp);

  ASSERT_EQ(scs, pdu.scs);
  ASSERT_EQ(cp, pdu.cp);
}

TEST(dl_prs_pdu_builder, valid_n_id_parameter_passes)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-2");

  dl_prs_pdu         pdu;
  dl_prs_pdu_builder builder(pdu);

  unsigned nid = 10;
  builder.set_n_id(nid);

  ASSERT_EQ(nid, pdu.nid_prs);
}

TEST(dl_prs_pdu_builder, valid_symbol_parameters_passes)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-2");

  dl_prs_pdu         pdu;
  dl_prs_pdu_builder builder(pdu);

  prs_num_symbols nof_symbols  = prs_num_symbols::six;
  unsigned        first_symbol = 2;
  builder.set_symbol_parameters(nof_symbols, first_symbol);

  ASSERT_EQ(nof_symbols, pdu.num_symbols);
  ASSERT_EQ(first_symbol, pdu.first_symbol);
}

TEST(dl_prs_pdu_builder, valid_rb_parameters_passes)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-2");

  dl_prs_pdu         pdu;
  dl_prs_pdu_builder builder(pdu);

  crb_interval crbs = {2, 10};
  builder.set_rb_parameters(crbs);

  ASSERT_EQ(crbs, pdu.crbs);
}

TEST(dl_prs_pdu_builder, valid_power_offset_parameter_passes)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-2");

  dl_prs_pdu         pdu;
  dl_prs_pdu_builder builder(pdu);

  std::optional<float> power = -10;
  builder.set_power_offset(power);

  ASSERT_EQ(power, pdu.prs_power_offset_db);
}

TEST(dl_prs_pdu_builder, valid_comb_parameters_passes)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-2");

  dl_prs_pdu         pdu;
  dl_prs_pdu_builder builder(pdu);

  prs_comb_size comb_size   = prs_comb_size::twelve;
  unsigned      comb_offset = 2;
  builder.set_comb_parameters(comb_size, comb_offset);

  ASSERT_EQ(comb_size, pdu.comb_size);
  ASSERT_EQ(comb_offset, pdu.comb_offset);
}
