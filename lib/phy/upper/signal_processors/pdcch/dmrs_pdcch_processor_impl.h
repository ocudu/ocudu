// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/re_buffer.h"
#include "ocudu/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "ocudu/phy/upper/signal_processors/pdcch/dmrs_pdcch_processor.h"
#include "ocudu/ran/pdcch/pdcch_constants.h"
#include <memory>

namespace ocudu {

/// Describes a generic implementation of a DMRS for PDCCH processor.
class dmrs_pdcch_processor_impl : public dmrs_pdcch_processor
{
private:
  /// DM-RS index increment in frequency domain.
  static constexpr unsigned STRIDE = 4;

  /// Number of DM-RS for PDCCH per active resource block.
  static constexpr unsigned NOF_DMRS_PER_RB = NOF_SUBCARRIERS_PER_RB / STRIDE;

  /// Maximum number of DM-RS for PDCCH that can be found in a symbol.
  static constexpr unsigned MAX_NOF_DMRS_PER_SYMBOL = pdcch_constants::MAX_NOF_RB_PDCCH * NOF_DMRS_PER_RB;

  /// Maximum number of DM-RS for PDCCH that can be found in a transmission.
  static constexpr unsigned MAX_NOF_DMRS = pdcch_constants::MAX_CORESET_DURATION * MAX_NOF_DMRS_PER_SYMBOL;

  /// Pseudo-random sequence generator instance.
  std::unique_ptr<pseudo_random_generator> prg;

  /// Resource grid mapper instance.
  std::unique_ptr<resource_grid_mapper> mapper;

  /// Temporary sequence storage.
  std::array<cf_t, MAX_NOF_DMRS_PER_SYMBOL> temp_sequence;

  /// \brief Computes the initial pseudo-random state.
  /// \param[in] symbol    Symbol index within the slot.
  /// \param[in] config    Required parameters.
  /// \return The initial pseudo-random state.
  static unsigned c_init(unsigned symbol, const config_t& config);

  /// \brief Sequence generation as per TS38.211 Section 7.4.1.3.1.
  ///
  /// This method generates the sequence described in TS38.211 Section 7.4.1.3.1, considering the only values required
  /// to fill the resource blocks according to TS38.211 Section 7.3.2.
  ///
  /// \param[out] sequence Sequence destination.
  /// \param[in] symbol    Symbol index within the slot.
  /// \param[in] config    Required parameters to calculate the sequences.
  void sequence_generation(span<cf_t> sequence, unsigned symbol, const config_t& config) const;

  /// \brief Mapping to physical resources as per TS38.211 Section 7.4.1.3.2.
  ///
  /// This method implements the signal mapping as described in TS38.211 Section 7.4.1.3.2.
  ///
  /// \param[out] grid   Resource grid writer interface.
  /// \param[in] d_pdcch PDCCH resource elements to map in the resource grid.
  /// \param[in] config  PDCCH modulator parameters.
  void mapping(resource_grid_writer& grid, const re_buffer_reader<>& d_pdcch, const config_t& config);

public:
  explicit dmrs_pdcch_processor_impl(std::unique_ptr<pseudo_random_generator> prg_,
                                     std::unique_ptr<resource_grid_mapper>    mapper_) :
    prg(std::move(prg_)), mapper(std::move(mapper_))
  {
    ocudu_assert(prg, "Invalid PRG.");
    ocudu_assert(mapper, "Invalid resource grid mapper.");
  }

  // See interface for documentation.
  void map(resource_grid_writer& grid, const config_t& config) override;
};

} // namespace ocudu
