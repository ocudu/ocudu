// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "ocudu/phy/upper/signal_processors/ssb/dmrs_pbch_processor.h"
#include "ocudu/ran/ssb/ssb_properties.h"

namespace ocudu {

class dmrs_pbch_processor_impl : public dmrs_pbch_processor
{
private:
  /// PBCH DM-RS stride within a resource block.
  static constexpr unsigned stride = 4U;
  /// Number of PBCH DM-RS per resource block.
  static constexpr unsigned nof_dmrs_prb = NOF_SUBCARRIERS_PER_RB / stride;
  /// Number of PBCH DM-RS contained in a full SS/PBCH block OFDM symbol.
  static constexpr unsigned nof_dmrs_full_symbol = NOF_SSB_PRBS * nof_dmrs_prb;
  /// Number of PBCH DM-RS contained in one edge SS/PBCH block OFDM symbol.
  static constexpr unsigned nof_dmrs_edge_symbol = 4 * nof_dmrs_prb;
  /// Total number of resource elements for PBCH DM-RS in a SS/PBCH block.
  static constexpr unsigned NOF_RE = 2 * nof_dmrs_full_symbol + 2 * nof_dmrs_edge_symbol;

  std::unique_ptr<pseudo_random_generator> prg;

  /// \brief Computes the initial pseudo-random state
  /// \param [in] config provides the required parameters to calculate the value
  /// \return the initial pseudo-random state
  static unsigned c_init(const config_t& config);

  /// \brief Implements TS38.211 Section 7.4.1.4.1 Sequence generation
  /// \param [out] sequence provides the destination of the sequence generation
  /// \param [in] config provides the required fields to generate the signal
  void generation(std::array<cf_t, NOF_RE>& sequence, const config_t& config) const;

  /// \brief Implements TS38.211 Section 7.4.1.4.2 Mapping to physical resources
  /// \param [in] sequence provides the source of the sequence
  /// \param [out] grid provides the grid destination to map the signal
  /// \param [in] config provides the required fields to map the signal
  void mapping(const std::array<cf_t, NOF_RE>& sequence, resource_grid_writer& grid, const config_t& config) const;

public:
  dmrs_pbch_processor_impl(std::unique_ptr<pseudo_random_generator> pseudo_random_generator) :
    prg(std::move(pseudo_random_generator))
  {
    ocudu_assert(prg, "Invalid PRG.");
  }

  // See interface for documentation.
  void map(resource_grid_writer& grid, const config_t& config) override;
};

} // namespace ocudu
