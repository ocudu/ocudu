// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "helpers.h"
#include "pdcch.h"
#include "ocudu/fapi_adaptor/precoding_matrix_table_generator.h"
#include "ocudu/mac/mac_cell_result.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi_adaptor;
using namespace unittests;

TEST(mac_fapi_pdcch_pdu_conversor_test, mac_to_fapi_conversion_is_valid)
{
  unsigned                               nof_prbs            = 51;
  const mac_dl_sched_result_test_helper& result_test         = build_valid_mac_dl_sched_result();
  const mac_dl_sched_result&             result              = result_test.result;
  const dci_context_information          context_information = result.dl_res->dl_pdcchs.front().ctx;
  const dci_payload                      payload             = result.dl_pdcch_pdus.front();

  fapi::dl_pdcch_pdu         fapi_pdu;
  fapi::dl_pdcch_pdu_builder builder(fapi_pdu);
  auto                       pm_tools = generate_precoding_matrix_tables(pmi_codebook_one_port{}, 0);
  convert_pdcch_mac_to_fapi(builder, context_information, payload, *std::get<0>(pm_tools), nof_prbs);

  // BWP.
  ASSERT_EQ(context_information.bwp_cfg->cp, fapi_pdu.cp);
  ASSERT_EQ(context_information.bwp_cfg->scs, fapi_pdu.scs);
  ASSERT_EQ(context_information.coreset_cfg->coreset0_crbs(), fapi_pdu.coreset_bwp);

  // CORESET.
  ASSERT_EQ(context_information.coreset_cfg->duration(), fapi_pdu.symbols.length());
  ASSERT_EQ(context_information.coreset_cfg->get_precoder_granularity(), fapi_pdu.precoder_granularity);

  if (context_information.coreset_cfg->get_id() == to_coreset_id(0)) {
    ASSERT_TRUE(context_information.coreset_cfg->interleaved_mapping().has_value());
    ASSERT_TRUE(std::holds_alternative<fapi::dl_pdcch_pdu::mapping_coreset_0>(fapi_pdu.mapping));
    ASSERT_EQ(*context_information.coreset_cfg->interleaved_mapping(),
              std::get<fapi::dl_pdcch_pdu::mapping_coreset_0>(fapi_pdu.mapping).interleaved);
  } else if (context_information.coreset_cfg->interleaved_mapping().has_value()) {
    ASSERT_EQ(context_information.coreset_cfg->freq_domain_resources(), fapi_pdu.freq_domain_resource);
    ASSERT_TRUE(std::holds_alternative<fapi::dl_pdcch_pdu::mapping_interleaved>(fapi_pdu.mapping));
    ASSERT_EQ(*context_information.coreset_cfg->interleaved_mapping(),
              std::get<fapi::dl_pdcch_pdu::mapping_interleaved>(fapi_pdu.mapping).interleaved);
  } else {
    ASSERT_EQ(context_information.coreset_cfg->freq_domain_resources(), fapi_pdu.freq_domain_resource);
    ASSERT_TRUE(std::holds_alternative<fapi::dl_pdcch_pdu::mapping_non_interleaved>(fapi_pdu.mapping));
  }

  // DCIs.
  ASSERT_EQ(context_information.rnti, fapi_pdu.dl_dci.rnti);
  ASSERT_EQ(context_information.n_id_pdcch_data, fapi_pdu.dl_dci.nid_pdcch_data);
  ASSERT_EQ(context_information.n_rnti_pdcch_data, fapi_pdu.dl_dci.nrnti_pdcch_data);
  ASSERT_EQ(context_information.cces.ncce, fapi_pdu.dl_dci.cce_index);
  ASSERT_EQ(context_information.starting_symbol, fapi_pdu.symbols.start());
  ASSERT_EQ(context_information.cces.aggr_lvl, fapi_pdu.dl_dci.dci_aggregation_level);
  ASSERT_EQ(context_information.n_id_pdcch_dmrs, fapi_pdu.dl_dci.nid_pdcch_dmrs);
  ASSERT_EQ(payload, fapi_pdu.dl_dci.payload);
}
