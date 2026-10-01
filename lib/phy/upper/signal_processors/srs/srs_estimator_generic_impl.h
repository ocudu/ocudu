// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/complex_exponential_table.h"
#include "ocudu/phy/support/time_alignment_estimator/time_alignment_estimator.h"
#include "ocudu/phy/upper/sequence_generators/low_papr_sequence_generator.h"
#include "ocudu/phy/upper/signal_processors/srs/doa_estimator.h"
#include "ocudu/phy/upper/signal_processors/srs/srs_estimator.h"
#include <memory>

namespace ocudu {

/// Implements a generic Sounding Reference Signal estimator.
class srs_estimator_generic_impl : public srs_estimator
{
public:
  /// Collects the component dependencies.
  struct dependencies {
    /// Sequence generator.
    std::unique_ptr<low_papr_sequence_generator> sequence_generator;
    /// Time alignment estimator.
    std::unique_ptr<time_alignment_estimator> ta_estimator;
    /// Direction of arrival estimator.
    std::unique_ptr<doa_estimator> direction_estimator;
  };

  srs_estimator_generic_impl(dependencies deps_, unsigned max_nof_prb_) :
    cexp_table(cexp_table_size, cexp_table_amplitude), deps(std::move(deps_)), max_nof_prb(max_nof_prb_)
  {
    ocudu_assert(deps.sequence_generator, "Invalid sequence generator.");
    ocudu_assert(deps.ta_estimator, "Invalid TA estimator.");

    // We don't check for the DOA estimator since it's empty if DOA not configured.
  }

  srs_estimator_result estimate(const resource_grid_reader& grid, const srs_estimator_configuration& config) override;

private:
  /// \brief Maximum sequence length in subcarriers.
  ///
  /// It is given by the maximum value of \f$m_{SRS,0}\f$ in TS38.211 Table 6.4.1.4.3-1 and a comb size of 2.
  static constexpr unsigned max_seq_length = 272 * NOF_SUBCARRIERS_PER_RB / 2;
  /// Maximum number of resource elements that an OFDM symbol contains.
  static constexpr unsigned max_symbol_size = MAX_NOF_SUBCARRIERS;
  /// Complex exponential table size for time compensation.
  static constexpr unsigned cexp_table_size = 1024;
  /// Complex exponential table amplitude for time compensation.
  static constexpr float cexp_table_amplitude = 1.0F;
  /// Complex exponential table for time compensation.
  complex_exponential_table cexp_table;

  /// \brief Perform phase compensation to the mean least squared estimates.
  ///
  /// This function is equivalent to:
  /// \code
  ///  void compensate_phase_shift(span<cf_t> mean_lse, float phase_shift_subcarrier, float phase_shift_offset);
  ///    std::transform(mean_lse.begin(),
  ///                   mean_lse.end(),
  ///                   mean_lse.begin(),
  ///                   [phase_shift_subcarrier, phase_shift_offset, n = 0](cf_t in) mutable {
  ///                     return in *
  ///                            std::polar(1.0F, phase_shift_subcarrier * static_cast<float>(n++) +
  ///                            phase_shift_offset);
  ///                   });
  /// }
  /// \endcode
  ///
  /// \param[in,out] mean_lse               Mean least square estimates.
  /// \param[in]     phase_shift_subcarrier Phase shift increment for each subcarrier in radians.
  /// \param[in]     phase_shift_offset     Initial phase shift for the first subcarrier in radians.
  void compensate_phase_shift(span<cf_t> mean_lse, float phase_shift_subcarrier, float phase_shift_offset);

  /// Dependencies collection.
  dependencies deps;
  /// Maximum number of PRB.
  unsigned max_nof_prb;

  /// Phase correction temporary vector - table indices.
  std::array<unsigned, max_symbol_size> temp_phase;
  /// Phase correction temporary vector - complex exponential.
  std::array<cf_t, max_symbol_size> temp_cexp;
};

} // namespace ocudu
