// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/phy/upper/sequence_generators/pss_sequence_generator.h"
#include "ocudu/phy/upper/signal_processors/ssb/pss_processor.h"

namespace ocudu {
class pss_processor_impl : public pss_processor
{
private:
  /// First subcarrier in the SS/PBCH block.
  static constexpr unsigned SSB_K_BEGIN = 56;
  /// Symbol index in the SSB where the PSS is mapped.
  static constexpr unsigned SSB_L = 0;
  /// Actual sequence length in the PSS.
  static constexpr unsigned SEQUENCE_LEN = pss_sequence_generator::sequence_length;

  /// Internal PSS sequence generator.
  static const pss_sequence_generator sequence_generator;

  /// \brief Implements TS38.211 Section 7.4.2.2.2 Mapping to physical resources
  /// \param [in] sequence provides the source of the sequence
  /// \param [out] grid provides the destination of the sequence mapping
  /// \param [in] config provides the necessary parameters to generate the sequence
  void
  mapping(const std::array<cf_t, SEQUENCE_LEN>& sequence, resource_grid_writer& grid, const config_t& config) const;

public:
  void map(resource_grid_writer& grid, const config_t& config) override;
};

} // namespace ocudu
