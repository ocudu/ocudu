// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/plmn_identity.h"
#include <set>

namespace ocudu::ocucp {

/// \brief Class responsible for notifying the CU-CP about DU node connections.
class du_connection_notifier
{
public:
  virtual ~du_connection_notifier() = default;

  /// \brief Notifies the CU-CP about a DU setup request.
  /// \return True if CU-CP accepts the request.
  virtual bool on_du_setup_request(cu_cp_du_index_t du_index, const std::set<plmn_identity>& plmn_ids) = 0;
};

} // namespace ocudu::ocucp
