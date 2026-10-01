// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../message_builder_helpers.h"
#include "prach.h"
#include "ocudu/adt/format.h"
#include "ocudu/phy/support/prach_buffer_context.h"
#include "ocudu/ran/prach/rach_config_common.h"
#include <gtest/gtest.h>
#include <numeric>

using namespace ocudu;
using namespace fapi_adaptor;
using namespace unittest;

TEST(FapiPhyUlPrachPduAdaptorTest, valid_pdu_pass)
{
  fapi::ul_prach_pdu fapi_pdu = build_valid_ul_prach_pdu();

  // As it's only one PRACH config TLV with one Fd occassion, modify the values from the PRACH FAPI PDU.
  fapi_pdu.index_fd_ra = 0;

  subcarrier_spacing scs_common = subcarrier_spacing::kHz15;

  rach_config_common prach;
  prach.rach_cfg_generic.prach_config_index           = 1;
  prach.rach_cfg_generic.ra_resp_window               = 2;
  prach.rach_cfg_generic.msg1_fdm                     = 1;
  prach.rach_cfg_generic.msg1_frequency_start         = 0;
  prach.rach_cfg_generic.zero_correlation_zone_config = 2;
  prach.rach_cfg_generic.preamble_rx_target_pw        = -100;
  prach.is_prach_root_seq_index_l839                  = true;
  prach.prach_root_seq_index                          = 0;
  prach.msg1_scs                                      = subcarrier_spacing::kHz15;
  prach.restricted_set                                = restricted_set_config::UNRESTRICTED;

  unsigned             sfn     = 1;
  unsigned             slot_id = 2;
  unsigned             sector  = 0;
  slot_point           slot(to_numerology_value(scs_common), sfn, slot_id);
  fapi::carrier_config carrier_cfg;
  carrier_cfg.ul_grid_size = 25;
  carrier_cfg.num_rx_ant   = 4;

  std::vector<uint8_t> prach_rx_ports(carrier_cfg.num_rx_ant);
  std::iota(prach_rx_ports.begin(), prach_rx_ports.end(), 0);

  prach_buffer_context context;
  convert_prach_fapi_to_phy(context, fapi_pdu, prach, carrier_cfg, prach_rx_ports, slot, sector);

  ASSERT_EQ(static_cast<unsigned>(fapi_pdu.prach_format), static_cast<unsigned>(context.format));
  ASSERT_EQ(fapi_pdu.prach_start_symbol, context.start_symbol);
  ASSERT_EQ(fapi_pdu.num_prach_ocas, context.nof_td_occasions);
  ASSERT_EQ(fapi_pdu.num_fd_ra, context.nof_fd_occasions);
  ASSERT_EQ(fapi_pdu.preambles.start(), context.start_preamble_index);
  ASSERT_EQ(fapi_pdu.preambles.length(), context.nof_preamble_indices);
  ASSERT_EQ(static_cast<unsigned>(prach.restricted_set), static_cast<unsigned>(context.restricted_set));
  ASSERT_EQ(static_cast<unsigned>(scs_common), static_cast<unsigned>(context.pusch_scs));
  ASSERT_EQ(prach.prach_root_seq_index, context.root_sequence_index);
  ASSERT_EQ(prach.rach_cfg_generic.msg1_frequency_start, context.rb_offset);
  ASSERT_EQ(prach.rach_cfg_generic.zero_correlation_zone_config, context.zero_correlation_zone);
  ASSERT_EQ(slot, context.slot);
  ASSERT_EQ(sector, context.sector);
  ASSERT_EQ(span<const uint8_t>(prach_rx_ports), span<const uint8_t>(context.ports));
  ASSERT_EQ(carrier_cfg.ul_grid_size, context.nof_prb_ul_grid);
}
