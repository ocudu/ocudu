// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/fapi/p7/builders/tx_precoding_and_beamforming_pdu_builder.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi;

TEST(tx_precoding_and_beamforming_pdu_builder, valid_basic_parameters_passes)
{
  tx_precoding_and_beamforming_pdu         pdu;
  tx_precoding_and_beamforming_pdu_builder builder(pdu);

  unsigned prg_size = 4;

  builder.set_prg_parameters(prg_size);

  ASSERT_EQ(prg_size, pdu.prg_size);
}

TEST(tx_precoding_and_beamforming_pdu_builder, add_prg_passes)
{
  tx_precoding_and_beamforming_pdu         pdu;
  tx_precoding_and_beamforming_pdu_builder builder(pdu);

  unsigned prg_size = 8;
  unsigned pm_index = 4;

  builder.set_prg_parameters(prg_size);
  builder.set_pmi(pm_index);

  ASSERT_EQ(pm_index, std::get<fapi::precoding_matrix_index>(pdu.prg.precoding));
}

TEST(tx_precoding_and_beamforming_pdu_builder, add_precoding_weights_passes)
{
  tx_precoding_and_beamforming_pdu         pdu;
  tx_precoding_and_beamforming_pdu_builder builder(pdu);

  unsigned prg_size   = 8;
  unsigned nof_layers = 2;
  unsigned nof_ports  = 8;

  builder.set_prg_parameters(prg_size);
  builder.set_precoding_weights(precoding_weight_matrix(nof_layers, nof_ports));

  ASSERT_EQ(prg_size, pdu.prg_size);
  ASSERT_TRUE(std::holds_alternative<fapi::prg_precoding_weights>(pdu.prg.precoding));
  ASSERT_EQ(nof_layers, std::get<fapi::prg_precoding_weights>(pdu.prg.precoding).get_nof_layers());
  ASSERT_EQ(nof_ports, std::get<fapi::prg_precoding_weights>(pdu.prg.precoding).get_nof_ports());
}

TEST(tx_precoding_and_beamforming_pdu_builder, add_beam_passes)
{
  tx_precoding_and_beamforming_pdu         pdu;
  tx_precoding_and_beamforming_pdu_builder builder(pdu);

  beam_identifier beam_id = to_beam_id(6);

  ASSERT_TRUE(pdu.prg.beams.empty());

  builder.set_beams({beam_id});

  ASSERT_EQ(precoding_beam_list({beam_id}), pdu.prg.beams);
}
