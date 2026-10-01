// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Channel equalizer implementation for the Zero Forcing and the Minimum Mean Square Error methods.

#pragma once

#include "ocudu/phy/upper/equalization/channel_equalizer.h"
#include "ocudu/phy/upper/equalization/channel_equalizer_algorithm_type.h"

namespace ocudu {

/// Channel equalizer implementation for the Zero Forcing and the MMSE algorithms.
class channel_equalizer_generic_impl : public channel_equalizer
{
public:
  /// Maximum number of ports supported by the equalizer.
  static constexpr unsigned max_nof_ports = 8;

  /// Default constructor.
  explicit channel_equalizer_generic_impl(channel_equalizer_algorithm_type type_) : type(type_) {}

  // See interface for documentation.
  bool is_supported(unsigned nof_ports, unsigned nof_layers) override;

  // See interface for documentation.
  void equalize(span<cf_t>                       eq_symbols,
                span<float>                      eq_noise_vars,
                const re_buffer_reader<cbf16_t>& ch_symbols,
                const ch_est_list&               ch_estimates,
                span<const float>                noise_var_estimates,
                float                            tx_scaling) override;

private:
  /// Determines whether a combination of the algorithm type, number of layers, and number of ports is supported.
  static bool is_supported(channel_equalizer_algorithm_type algorithm, unsigned nof_ports, unsigned nof_layers);

  channel_equalizer_algorithm_type type;
};

} // namespace ocudu
