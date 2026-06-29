// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../message_builder_helpers.h"
#include "prs.h"
#include "ocudu/fapi_adaptor/precoding_matrix_table_generator.h"
#include "ocudu/phy/upper/signal_processors/prs/prs_generator_configuration.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi_adaptor;
using namespace unittest;

TEST(fapi_phy_dl_prs_pdu_adaptor, valid_pdu_pass)
{
  fapi::dl_prs_pdu                   fapi_pdu = build_valid_dl_prs_pdu();
  prs_generator_configuration        prs_config;
  slot_point                         slot(fapi_pdu.scs, 0, 1);
  auto                               pm_tools = generate_precoding_matrix_tables({pmi_codebook_one_port{}}, 0);
  const precoding_matrix_repository& pm_repo  = *std::get<std::unique_ptr<precoding_matrix_repository>>(pm_tools);

  convert_prs_fapi_to_phy(prs_config, fapi_pdu, slot, pm_repo);

  ASSERT_EQ(slot, prs_config.slot);
  ASSERT_EQ(fapi_pdu.cp, prs_config.cp);
  ASSERT_EQ(fapi_pdu.nid_prs, prs_config.n_id_prs);
  ASSERT_EQ(fapi_pdu.comb_size, prs_config.comb_size);
  ASSERT_EQ(fapi_pdu.comb_offset, prs_config.comb_offset);
  ASSERT_EQ(fapi_pdu.num_symbols, prs_config.duration);
  ASSERT_EQ(fapi_pdu.first_symbol, prs_config.start_symbol);
  ASSERT_EQ(fapi_pdu.crbs.start(), prs_config.prb_start);
  ASSERT_EQ(interval<uint16_t>::start_and_len(fapi_pdu.crbs.start(), fapi_pdu.crbs.length()), prs_config.freq_alloc);
}
