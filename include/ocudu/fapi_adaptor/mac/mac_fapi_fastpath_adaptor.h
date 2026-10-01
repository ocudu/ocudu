// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {
namespace fapi_adaptor {

class mac_fapi_sector_fastpath_adaptor;

/// MAC-FAPI bidirectional fastpath adaptor interface.
class mac_fapi_fastpath_adaptor
{
public:
  virtual ~mac_fapi_fastpath_adaptor() = default;

  /// Returns the MAC-FAPI sector fastpath adaptor for the given cell id.
  virtual mac_fapi_sector_fastpath_adaptor& get_sector_adaptor(unsigned cell_id) = 0;

  /// Stops the MAC-FAPI adaptor.
  virtual void stop() = 0;
};

} // namespace fapi_adaptor
} // namespace ocudu
