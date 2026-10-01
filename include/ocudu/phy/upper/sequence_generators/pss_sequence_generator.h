// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"
#include "ocudu/ocuduvec/sc_prod.h"
#include "ocudu/phy/phys_cell_id.h"
#include "ocudu/ran/pci.h"

namespace ocudu {

/// Primary Synchronization Signal (PSS) sequence generator.
class pss_sequence_generator
{
public:
  /// Actual sequence length.
  static constexpr unsigned sequence_length = 127;

  /// Initializes the sequence generator with the base sequence.
  pss_sequence_generator()
  {
    // Initialize M sequence x.
    std::array<uint32_t, sequence_length + 7> x;
    x[6] = 1;
    x[5] = 1;
    x[4] = 1;
    x[3] = 0;
    x[2] = 1;
    x[1] = 1;
    x[0] = 0;

    // Generate M sequence x.
    for (uint32_t i = 0; i != sequence_length; ++i) {
      x[i + 7] = (x[i + 4] + x[i]) % 2;
    }

    // Modulate M sequence d.
    for (uint32_t i = 0; i != sequence_length; ++i) {
      sequence[i] = 1.0F - 2.0F * (float)x[i];
    }
  }

  /// \brief Generates a PSS sequence according to TS38.211 Section 7.4.2.2.1.
  /// \param[out] out        Resultant complex sequence.
  /// \param[in]  pci        Physical Cell Identifier.
  /// \param[in]  amplitude  Amplitude of the output sequence.
  void generate(span<cf_t> out, pci_t pci, float amplitude) const
  {
    ocudu_assert(out.size() == sequence_length,
                 "Output size (i.e., {}) must be equal to the sequence length (i.e., {}).",
                 out.size(),
                 sequence_length);
    span<const cf_t> sequence_view = sequence;
    unsigned         m             = M(phys_cell_id::NID_2(pci));

    // Copy sequence from offset to the end
    ocuduvec::sc_prod(out.first(sequence_length - m), sequence_view.last(sequence_length - m), amplitude);

    // Copy sequence from 0 to offset
    ocuduvec::sc_prod(out.last(m), sequence_view.first(m), amplitude);
  }

private:
  /// Calculates the parameter M.
  static inline unsigned M(unsigned N_id_2) { return ((43U * (N_id_2)) % sequence_length); }

  std::array<cf_t, sequence_length> sequence;
};

} // namespace ocudu
