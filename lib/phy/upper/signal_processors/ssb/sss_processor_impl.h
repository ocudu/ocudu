// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/phy/upper/sequence_generators/sss_sequence_generator.h"
#include "ocudu/phy/upper/signal_processors/ssb/sss_processor.h"

namespace ocudu {
class sss_processor_impl : public sss_processor
{
private:
  /// First subcarrier in the SS/PBCH block.
  static constexpr unsigned ssb_k_begin = 56;
  /// Symbol index in the SSB where the SSS is mapped.
  static constexpr unsigned ssb_l = 2;
  /// SSS Sequence length in the SSB.
  static constexpr unsigned sequence_len = sss_sequence_generator::sequence_length;

  /// Internal SSS sequence generator.
  static const sss_sequence_generator sequence_generator;

  /// \brief Implements TS38.211 Section 7.4.2.3.2 Mapping to physical resources
  /// \param [in] sequence provides the source of the sequence
  /// \param [out] grid provides the destination of the sequence mapping
  /// \param [in] config provides the necessary parameters to generate the sequence
  void mapping(span<const cf_t> sequence, resource_grid_writer& grid, const config_t& config) const;

public:
  void map(resource_grid_writer& grid, const config_t& config) override;
};

} // namespace ocudu
