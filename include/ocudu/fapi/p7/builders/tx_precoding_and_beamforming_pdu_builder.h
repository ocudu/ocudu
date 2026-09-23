// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/p7/messages/tx_precoding_and_beamforming_pdu.h"

namespace ocudu {
namespace fapi {

/// Helper class to fill the transmission precoding and beamforming parameters specified in SCF-222 v4.0
/// section 3.4.2.5.
class tx_precoding_and_beamforming_pdu_builder
{
  tx_precoding_and_beamforming_pdu& pdu;

public:
  explicit tx_precoding_and_beamforming_pdu_builder(tx_precoding_and_beamforming_pdu& pdu_) : pdu(pdu_) {}

  /// Sets the PRG parameters for the fields of the tranmission precoding and beamforming PDU.
  /// \note These parameters are specified in SCF-222 v4.0 section 3.4.2.5, in table Tx precoding and beamforming PDU.
  tx_precoding_and_beamforming_pdu_builder& set_prg_parameters(unsigned prg_size)
  {
    pdu.prg_size = prg_size;

    return *this;
  }

  /// \brief Sets the precoding matrix index in the transmission precoding and beamforming PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.5, in table Tx precoding and beamforming PDU.
  tx_precoding_and_beamforming_pdu_builder& set_pmi(unsigned pm_index)
  {
    pdu.prg.pm_index = pm_index;

    return *this;
  }

  /// \brief Sets the beam of each precoding matrix virtual port in the transmission precoding and beamforming PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.5, in table Tx precoding and beamforming PDU.
  tx_precoding_and_beamforming_pdu_builder& set_beams(const precoding_beam_list& beams)
  {
    pdu.prg.beams = beams;

    return *this;
  }
};

} // namespace fapi
} // namespace ocudu
