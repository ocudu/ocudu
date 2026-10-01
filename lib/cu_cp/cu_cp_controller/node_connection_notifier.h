// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/plmn_identity.h"
#include <set>

namespace ocudu::ocucp {

/// \brief Class responsible for notifying the CU-CP about DU node connections.
class du_connection_notifier
{
public:
  virtual ~du_connection_notifier() = default;

  /// \brief Asks the CU-CP which of the PLMNs of a DU have a connected AMF.
  /// \param[in] plmn_ids The PLMNs served by the cells of the DU.
  /// \return The subset of \c plmn_ids for which the CU-CP has a connected AMF.
  virtual std::set<plmn_identity> on_connected_plmns_required(const std::set<plmn_identity>& plmn_ids) = 0;
};

} // namespace ocudu::ocucp
