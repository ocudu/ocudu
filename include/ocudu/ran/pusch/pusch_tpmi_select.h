// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/interval.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/pusch/pusch_constants.h"
#include "ocudu/ran/pusch/tx_scheme_configuration.h"
#include "ocudu/ran/srs/srs_channel_matrix.h"
#include "ocudu/support/ocudu_assert.h"

namespace ocudu {

/// PUSCH Transmit Precoding Matrix Indication (TPMI) information.
class pusch_tpmi_select_info
{
public:
  struct tpmi_info {
    /// Most suitable Transmit Precoding Matrix Indicator.
    unsigned tpmi;
    /// Average Signal-to-Interference-plus-Noise Ratio (SINR) in decibels.
    float avg_sinr_dB;
    /// SINR per layer in decibels.
    static_vector<float, pusch_constants::MAX_NOF_LAYERS> sinr_dB_layer;
  };

  /// Gets the maximum number of layers.
  unsigned get_max_nof_layers() const { return info.size(); }

  /// Determines if the information is valid.
  bool is_valid() const { return !info.empty(); }

  /// \brief Gets the TPMI information for a number of layers.
  ///
  /// \remark An assertion is triggered if the number of layers is out of range.
  const tpmi_info& get_tpmi_select(unsigned nof_layers) const
  {
    interval<unsigned, true> nof_layers_range(1, get_max_nof_layers());
    ocudu_assert(nof_layers_range.contains(nof_layers),
                 "The number of layers (i.e., {}) is out-of-range {}",
                 nof_layers,
                 nof_layers_range);

    return info[nof_layers - 1];
  }

  /// Constructs a PUSCH TPMI information from a span.
  pusch_tpmi_select_info(const span<tpmi_info>& info_) : info(info_.begin(), info_.end()) {}

  /// Constructs a PUSCH TPMI information from an initializer list.
  pusch_tpmi_select_info(const std::initializer_list<tpmi_info>& info_) : info(info_.begin(), info_.end()) {}

  /// Copy constructor.
  pusch_tpmi_select_info(const pusch_tpmi_select_info& other) noexcept : info(other.info.begin(), other.info.end())
  {
    // Do nothing.
  }

private:
  /// TPMI information for each number of layers.
  static_vector<tpmi_info, pusch_constants::MAX_NOF_LAYERS> info;
};

/// \brief Selects the Transmit Precoding Matrix Indicator (TPMI) for each possible number of layers supported by the
/// channel topology.
///
/// \param[in] channel         Channel coefficient matrix.
/// \param[in] noise_variance  Linear noise variance.
/// \param[in] max_rank        Maximum number of layers.
/// \param[in] codebook_subset Transmission scheme codebook subset.
/// \return The TPMI information given the channel coefficients and noise variance.
pusch_tpmi_select_info get_tpmi_select_info(const srs_channel_matrix& channel,
                                            float                     noise_variance,
                                            unsigned                  max_rank,
                                            tx_scheme_codebook_subset codebook_subset);

} // namespace ocudu
