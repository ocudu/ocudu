// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/pci.h"
#include <optional>

namespace ocudu::ocucp {

/// Info about one cell reported by a DU in the F1 Setup procedure.
struct du_reported_cell {
  /// Cell global id of the reported cell.
  nr_cell_global_id_t cgi;
  /// PCI of the reported cell, if provided.
  std::optional<pci_t> pci;
};

} // namespace ocudu::ocucp
