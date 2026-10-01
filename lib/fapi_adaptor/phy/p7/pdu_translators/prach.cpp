// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "prach.h"
#include "ocudu/adt/format.h"
#include "ocudu/phy/support/prach_buffer_context.h"
#include "ocudu/ran/prach/rach_config_common.h"

using namespace ocudu;
using namespace fapi_adaptor;

void ocudu::fapi_adaptor::convert_prach_fapi_to_phy(prach_buffer_context&       context,
                                                    const fapi::ul_prach_pdu&   fapi_pdu,
                                                    const rach_config_common&   prach_cfg,
                                                    const fapi::carrier_config& carrier_cfg,
                                                    span<const uint8_t>         ports,
                                                    slot_point                  slot,
                                                    unsigned                    sector_id)
{
  ocudu_assert(fapi_pdu.index_fd_ra == 0, "Only one FD occasion supported.");

  context.slot                 = slot;
  context.sector               = sector_id;
  context.format               = fapi_pdu.prach_format;
  context.nof_td_occasions     = fapi_pdu.num_prach_ocas;
  context.nof_fd_occasions     = fapi_pdu.num_fd_ra;
  context.start_symbol         = fapi_pdu.prach_start_symbol;
  context.start_preamble_index = fapi_pdu.preambles.start();
  context.nof_preamble_indices = fapi_pdu.preambles.length();

  context.pusch_scs       = slot.scs();
  context.restricted_set  = prach_cfg.restricted_set;
  context.nof_prb_ul_grid = carrier_cfg.ul_grid_size;

  context.rb_offset             = prach_cfg.rach_cfg_generic.msg1_frequency_start;
  context.root_sequence_index   = prach_cfg.prach_root_seq_index;
  context.zero_correlation_zone = prach_cfg.rach_cfg_generic.zero_correlation_zone_config;

  context.ports.assign(ports.begin(), ports.end());
}
