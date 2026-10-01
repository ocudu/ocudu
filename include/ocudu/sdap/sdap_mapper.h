// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

/// DRB to QoS flow mapping.
class sdap_mapper
{
public:
  virtual ~sdap_mapper() = default;

  virtual bool associate(int mapping_list_number, const void* mapping_list) = 0;

  virtual bool drb_release(int drb) = 0;
};

} // namespace ocudu
