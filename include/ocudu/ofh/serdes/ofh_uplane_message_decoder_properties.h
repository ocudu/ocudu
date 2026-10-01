// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/ofh/serdes/ofh_uplane_message_properties.h"
#include "ocudu/ran/resource_block.h"
#include <optional>

namespace ocudu {
namespace ofh {

/// Maximum number of supported sections.
constexpr unsigned MAX_NOF_SUPPORTED_SECTIONS = 1U;

/// Open Fronthaul User-Plane section parameters.
struct uplane_section_params {
  /// Section identifier.
  unsigned section_id;
  /// Resource block indicator.
  bool is_every_rb_used;
  /// Symbol number increment command.
  bool use_current_symbol_number;
  /// Start PRB.
  unsigned start_prb;
  /// Number of PRBs (though a value of 0 signals more than 255 PRBs in the OFH specification, this field always
  /// contains the real amount of PRBs).
  unsigned nof_prbs;
  /// User data compression header.
  ru_compression_params ud_comp_hdr;
  /// User data compression length.
  std::optional<unsigned> ud_comp_len;
  /// User data compression parameter.
  /// \note For simplicity, all the PRBs use the same compression parameters.
  std::optional<unsigned> ud_comp_param;
  /// IQ samples for the number of PRBs indicated by \c nof_prbs.
  static_vector<cbf16_t, MAX_NOF_SUBCARRIERS> iq_samples;
};

/// Open Fronthaul User-Plane message decoder results.
struct uplane_message_decoder_results {
  /// Maximum number of sections supported by this result.
  static constexpr unsigned RESULTS_MAX_NOF_SUPPORTED_SECTIONS = 2U;

  /// Open Fronthaul User-Plane message parameters.
  uplane_message_params params;
  /// User-Plane message sections.
  static_vector<uplane_section_params, RESULTS_MAX_NOF_SUPPORTED_SECTIONS> sections;
};

} // namespace ofh
} // namespace ocudu
