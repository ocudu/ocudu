// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>

namespace ocudu {

/// 3GPP TS 38.331, PhysCellId ::= INTEGER (0..1007).
using pci_t                 = uint16_t;
constexpr pci_t MIN_PCI     = 0;
constexpr pci_t MAX_PCI     = 1007;
constexpr pci_t INVALID_PCI = MAX_PCI + 1;

/// Returns true if the given struct is valid, otherwise false.
constexpr bool is_valid(pci_t pci)
{
  // PCI must be in [0..1007].
  return pci < INVALID_PCI;
}

/// \brief IE "PCI-Range" in TS 38.331. Specifies a range of Physical Cell Identifiers.
struct pci_range_t {
  pci_t start;
  enum class range_t {
    n1        = 1,
    n4        = 4,
    n8        = 8,
    n12       = 12,
    n16       = 16,
    n24       = 24,
    n32       = 32,
    n48       = 48,
    n64       = 64,
    n84       = 84,
    n96       = 96,
    n128      = 128,
    n168      = 168,
    n252      = 252,
    n504      = 504,
    n1008     = 1008,
    max_range = n1008
  };
  range_t size;
};

} // namespace ocudu
