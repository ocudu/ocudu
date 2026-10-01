// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"
#include "ocudu/ocuduvec/prod.h"
#include "ocudu/ocuduvec/sc_prod.h"
#include "ocudu/phy/phys_cell_id.h"
#include "ocudu/ran/pci.h"

namespace ocudu {

/// Secondary Synchronization Signal (SSS) sequence generator.
class sss_sequence_generator
{
public:
  /// Actual sequence length.
  static constexpr unsigned sequence_length = 127;

  /// Initializes the sequence generator with the base sequence.
  sss_sequence_generator()
  {
    // Initialize M sequence x0.
    std::array<unsigned, sequence_length + 7> x0;
    x0[6] = 0;
    x0[5] = 0;
    x0[4] = 0;
    x0[3] = 0;
    x0[2] = 0;
    x0[1] = 0;
    x0[0] = 1;

    // Generate M sequence x0.
    for (unsigned i = 0; i != sequence_length; ++i) {
      x0[i + 7] = (x0[i + 4] + x0[i]) % 2;
    }

    // Modulate M sequence.
    for (unsigned i = 0; i != sequence_length; ++i) {
      sequence0[i] = (1.0f - 2.0f * x0[i]);
    }

    // Initialize M sequence x1.
    std::array<unsigned, sequence_length + 7> x1;
    x1[6] = 0;
    x1[5] = 0;
    x1[4] = 0;
    x1[3] = 0;
    x1[2] = 0;
    x1[1] = 0;
    x1[0] = 1;

    // Generate M sequence x1.
    for (unsigned i = 0; i != sequence_length; ++i) {
      x1[i + 7] = (x1[i + 1] + x1[i]) % 2;
    }

    // Modulate M sequence.
    for (unsigned i = 0; i != sequence_length; ++i) {
      sequence1[i] = (1.0f - 2.0f * x1[i]);
    }
  }

  /// \brief Generates a SSS sequence according to TS38.211 Section 7.4.2.3.1.
  /// \param[out] out        Resultant complex sequence.
  /// \param[in]  pci        Physical Cell Identifier.
  /// \param[in]  amplitude  Amplitude of the output sequence.
  void generate(span<cf_t> out, pci_t pci, float amplitude) const
  {
    // Calculate generation parameters.
    unsigned m0 = 15 * (phys_cell_id::NID_1(pci) / 112) + 5 * phys_cell_id::NID_2(pci);
    unsigned m1 = phys_cell_id::NID_1(pci) % 112;

    // Apply d0 sequence with scaling.
    span<const cf_t> d0 = sequence0;
    ocuduvec::sc_prod(out.first(sequence_length - m0), d0.last(sequence_length - m0), amplitude);
    ocuduvec::sc_prod(out.last(m0), d0.first(m0), amplitude);

    // Apply d1 sequence.
    span<const cf_t> d1 = sequence1;
    ocuduvec::prod(out.first(sequence_length - m1), d1.last(sequence_length - m1), out.first(sequence_length - m1));
    ocuduvec::prod(out.last(m1), d1.first(m1), out.last(m1));
  }

private:
  std::array<cf_t, sequence_length> sequence0;
  std::array<cf_t, sequence_length> sequence1;
};

} // namespace ocudu
