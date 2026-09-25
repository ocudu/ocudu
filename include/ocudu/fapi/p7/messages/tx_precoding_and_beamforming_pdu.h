// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ran/beamforming/beam_identifier.h"
#include "ocudu/ran/precoding/precoding_codebook_type2_helpers.h"
#include <variant>

namespace ocudu {
namespace fapi {

/// Index of a precoding matrix in the precoding matrix table, as per SCF-222 v4.0 section 3.4.2.5.
using precoding_matrix_index = uint16_t;

/// \brief Precoding weights of a Physical Resource Group (PRG), as per SCF-222.10 Section 3.4.2.8.
///
/// The number of weights is the number of transmit antennas times the number of layers. The Type II codebook is
/// the only codebook that signals them, so the weights hold its two layers. A codebook of a later release that
/// signals the weights for more layers widens this type, and leaves the Type II one untouched.
using prg_precoding_weights = typeII_precoding_weight_matrix;

/// Precoding and beamforming PDU.
struct tx_precoding_and_beamforming_pdu {
  /// \brief Precoding of a Physical Resource Group (PRG).
  ///
  /// It holds the index of the precoding matrix in the precoding matrix table, or the precoding weights themselves. A
  /// \c std::monostate means that no precoding has been set, which tells an unset PRG apart from the index zero.
  ///
  /// [Implementation-defined] The precoding weights extend the SCF FAPI. The precoding matrix table can only hold the
  /// matrices that are enumerable at cell creation, which excludes the Type II codebook ones.
  using prg_precoding = std::variant<std::monostate, precoding_matrix_index, prg_precoding_weights>;

  /// Physical resource groups information.
  struct prgs_info {
    /// Precoding of the physical resource group.
    prg_precoding precoding;
    /// \brief Beam that carries each of the precoding matrix virtual ports.
    ///
    /// An empty list selects the antenna ports, that is, no beamforming.
    precoding_beam_list beams;
  };

  uint16_t prg_size;
  /// [Implementation-defined] Only a single PRG is used.
  prgs_info prg;
};

} // namespace fapi
} // namespace ocudu
