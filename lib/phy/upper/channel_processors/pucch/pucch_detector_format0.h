// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// PUCCH Format 0 detector declaration.

#pragma once

#include "ocudu/phy/upper/channel_processors/pucch/pucch_detector.h"
#include "ocudu/phy/upper/pucch_helper.h"
#include "ocudu/phy/upper/sequence_generators/low_papr_sequence_collection.h"
#include "ocudu/ran/pucch/pucch_constants.h"

namespace ocudu {

/// PUCCH Format 0 detector.
class pucch_detector_format0
{
public:
  /// Creates a PUCCH Format 0 detector with its dependencies.
  pucch_detector_format0(std::unique_ptr<pseudo_random_generator>      pseudo_random_,
                         std::unique_ptr<low_papr_sequence_collection> low_papr_) :
    helper(std::move(pseudo_random_)), low_papr(std::move(low_papr_))
  {
    ocudu_assert(low_papr, "");
  }

  /// Detects a PUCCH Format 0 transmission. See \ref pucch_detector for more details.
  std::pair<pucch_uci_message, channel_state_information> detect(const resource_grid_reader&                  grid,
                                                                 const pucch_detector::format0_configuration& config);

private:
  /// Maximum number of RE used for PUCCH Format 0. Recall that PUCCH format 0 occupies a single RB.
  static constexpr unsigned max_nof_re = NOF_SUBCARRIERS_PER_RB * MAX_PORTS * pucch_constants::f0::MAX_NOF_SYMS;

  /// Temporary RE storage tensor dimensions.
  enum class dims : unsigned { re = 0, symbol = 1, rx_port = 2, nof_dims = 3 };

  /// PUCCH sequence helper.
  pucch_helper helper;
  /// Collection of low-PAPR sequences.
  std::unique_ptr<low_papr_sequence_collection> low_papr;

  /// Temporary storage of the resource elements.
  static_tensor<static_cast<unsigned>(dims::nof_dims), cf_t, max_nof_re, dims> temp_re;
};

} // namespace ocudu
