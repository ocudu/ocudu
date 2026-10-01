// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Channel precoder NEON implementation.

#pragma once

#include "channel_precoder_impl.h"

namespace ocudu {

/// Channel precoder implementation.
class channel_precoder_neon : public channel_precoder_impl
{
public:
  // See interface for documentation.
  void apply_precoding_port(span<cbf16_t>             port_re,
                            const re_buffer_reader<>& input_re,
                            span<const cf_t>          port_weights) const override;

  // See interface for documentation.
  void apply_layer_map_and_precoding(re_buffer_writer<cbf16_t>&     output,
                                     span<const ci8_t>              input,
                                     const precoding_weight_matrix& precoding) const override;
};

} // namespace ocudu
