// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "prs.h"
#include "ocudu/scheduler/result/prs_info.h"

using namespace ocudu;
using namespace fapi_adaptor;

static void fill_precoding_and_beamforming(fapi::dl_prs_pdu_builder&        builder,
                                           const precoding_codebook_mapper& pm_mapper,
                                           unsigned                         cell_nof_prbs)
{
  fapi::tx_precoding_and_beamforming_pdu_builder pm_bf_builder = builder.get_tx_precoding_and_beamforming_pdu_builder();
  pm_bf_builder.set_prg_parameters(cell_nof_prbs);

  mac_prs_precoding_info info;
  pm_bf_builder.set_pmi(pm_mapper.map(info));

  // TODO: assign a beam ID to each resource and set it here.
}

void ocudu::fapi_adaptor::convert_prs_mac_to_fapi(fapi::dl_tti_request_builder&    builder,
                                                  const prs_info&                  prs_pdu,
                                                  const precoding_codebook_mapper& pm_mapper,
                                                  unsigned                         cell_nof_prbs)
{
  fapi::dl_prs_pdu_builder prs_builder = builder.add_prs_pdu();

  prs_builder.set_bwp_parameters(prs_pdu.scs, prs_pdu.cp)
      .set_n_id(prs_pdu.n_id_prs)
      .set_comb_parameters(prs_pdu.comb_size, prs_pdu.comb_offset)
      .set_symbol_parameters(prs_pdu.nof_symbols, prs_pdu.symbols.start())
      .set_rb_parameters(prs_pdu.crbs)
      .set_power_offset(static_cast<float>(prs_pdu.power_offset_db));

  fill_precoding_and_beamforming(prs_builder, pm_mapper, cell_nof_prbs);
}
