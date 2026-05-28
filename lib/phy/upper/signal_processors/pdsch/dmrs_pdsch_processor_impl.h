// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/phy/antenna_ports.h"
#include "ocudu/phy/support/re_buffer.h"
#include "ocudu/phy/support/resource_grid_mapper.h"
#include "ocudu/phy/support/resource_grid_writer.h"
#include "ocudu/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "ocudu/phy/upper/signal_processors/pdsch/dmrs_pdsch_processor.h"

namespace ocudu {

/// Defines a DMRS for PDSCH generic implementation.
class dmrs_pdsch_processor_impl : public dmrs_pdsch_processor
{
private:
  /// Defines the maximum number of DMRS per RB.
  static constexpr unsigned MAX_DRMS_PER_RB =
      std::max(get_nof_re_per_prb(dmrs_config_type::type1), get_nof_re_per_prb(dmrs_config_type::type2));

  /// Define the maximum number of DMRS per symbol.
  static constexpr unsigned MAX_DMRS_PER_SYMBOL = MAX_NOF_PRBS * MAX_DRMS_PER_RB;

  /// \brief Maximum number of OFDM symbols containing DM-RS on a PDSCH allocation
  ///
  /// It corresponds to a single symbol DM-RS with three additional positions, or to a double symbol DM-RS with one
  /// additional position, as per TS38.211 Section 7.4.1.1.2. and TS38.214 Section 5.6.1.2.
  static constexpr unsigned MAX_DMRS_SYMBOLS = 4;

  /// Pseudo-random sequence generator instance.
  std::unique_ptr<pseudo_random_generator> prg;
  std::unique_ptr<resource_grid_mapper>    mapper;

  /// \brief Implements TS38.211 Section 7.4.1.1.1 Sequence generation.
  ///
  /// This method generates the sequence described in TS38.211 Section 7.4.1.1.1, considering the only values required
  /// in TS38.211 Section 7.4.1.1.2.
  ///
  /// \param[out] sequence Provides the sequence destination.
  /// \param[in] symbol Denotes the symbol index.
  /// \param[in] config Provides the required parameters to calculate the sequences.
  void sequence_generation(span<cf_t> sequence, unsigned symbol, const config_t& config) const;

  /// \brief Applies the corresponding set of CDM codes to the DM-RS sequence.
  ///
  /// \param[out] sequence The DM-RS sequence after the application of CDM.
  /// \param[in] base_sequence The original DM-RS sequence.
  /// \param[in] dmrs_port DM-RS port number, determining the CDM code to apply.
  /// \param[in] symbol OFDM symbol corresponding to the input DM-RS sequence.
  /// \param[in] config PDSCH DM-RS configuration parameters.
  static void apply_cdm(span<cf_t>       sequence,
                        span<const cf_t> base_sequence,
                        unsigned         dmrs_port,
                        unsigned         symbol,
                        const config_t&  config);

  /// Temporal resource element storage.
  static_re_buffer<MAX_PORTS, MAX_DMRS_PER_SYMBOL * MAX_DMRS_SYMBOLS> temp_re;

public:
  dmrs_pdsch_processor_impl(std::unique_ptr<pseudo_random_generator> pseudo_random_generator_,
                            std::unique_ptr<resource_grid_mapper>    mapper_) :
    prg(std::move(pseudo_random_generator_)), mapper(std::move(mapper_))
  {
    ocudu_assert(prg, "Invalid PRG.");
    ocudu_assert(mapper, "Invalid mapper.");
  }

  // See interface for documentation.
  void map(resource_grid_writer& grid, const config_t& config) override;
};

} // namespace ocudu
