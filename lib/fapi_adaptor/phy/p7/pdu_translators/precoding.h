// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/p7/messages/tx_precoding_and_beamforming_pdu.h"
#include "ocudu/fapi_adaptor/precoding_codebook_repository.h"
#include "ocudu/ran/beamforming/beam_identifier_helpers.h"

namespace ocudu {
namespace fapi_adaptor {

/// \brief Returns the precoding configuration of the given transmission precoding and beamforming PDU.
///
/// The PDU holds either the index of the precoding configuration in the repository, or the precoding weights. The
/// weights give the MIMO precoding matrix, and the beams select the antenna ports.
inline precoding_beamforming_composite get_precoding_config(const fapi::tx_precoding_and_beamforming_pdu& pdu,
                                                            const precoding_codebook_repository&          pm_repo)
{
  ocudu_assert(!std::holds_alternative<std::monostate>(pdu.prg.precoding), "The PRG precoding is not set.");

  if (const auto* index = std::get_if<fapi::precoding_matrix_index>(&pdu.prg.precoding)) {
    return pm_repo.get_precoding_config(*index);
  }

  precoding_weight_matrix weights{std::get<fapi::prg_precoding_weights>(pdu.prg.precoding)};
  precoding_beam_list     beams = get_default_beam_list(weights.get_nof_ports());

  return {std::move(weights), beams};
}

} // namespace fapi_adaptor
} // namespace ocudu
