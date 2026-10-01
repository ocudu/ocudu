// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/fapi/p7/builders/ul_prach_pdu_builder.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi;

TEST(ul_prach_pdu_builder, valid_preamble_parameters_passes)
{
  ul_prach_pdu::preambles_interval preambles = {12, 13};

  ul_prach_pdu         pdu;
  ul_prach_pdu_builder builder(pdu);

  builder.set_preamble_parameters(preambles);

  ASSERT_EQ(preambles, pdu.preambles);
}

TEST(ul_prach_pdu_builder, valid_prach_parameters_passes)
{
  uint8_t           num_prach_ocas = 4;
  uint16_t          num_cs         = 15;
  prach_format_type prach_format   = prach_format_type::one;

  ul_prach_pdu         pdu;
  ul_prach_pdu_builder builder(pdu);

  builder.set_prach_parameters(num_prach_ocas, num_cs, prach_format);

  ASSERT_EQ(num_prach_ocas, pdu.num_prach_ocas);
  ASSERT_EQ(num_cs, pdu.num_cs);
  ASSERT_EQ(prach_format, pdu.prach_format);
}

TEST(ul_prach_pdu_builder, valid_frequency_domain_parameters_passes)
{
  uint8_t index_fd_ra = 5;
  uint8_t num_fd_ra   = 23;

  ul_prach_pdu         pdu;
  ul_prach_pdu_builder builder(pdu);

  builder.set_frequency_domain_parameters(index_fd_ra, num_fd_ra);

  ASSERT_EQ(index_fd_ra, pdu.index_fd_ra);
  ASSERT_EQ(num_fd_ra, pdu.num_fd_ra);
}

TEST(ul_prach_pdu_builder, valid_time_domain_parameters_passes)
{
  uint8_t prach_start_symbol = 5;

  ul_prach_pdu         pdu;
  ul_prach_pdu_builder builder(pdu);

  builder.set_time_domain_parameters(prach_start_symbol);

  ASSERT_EQ(prach_start_symbol, pdu.prach_start_symbol);
}
