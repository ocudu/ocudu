// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ngap/ngap.h"
#include "ocudu/ran/plmn_identity.h"

namespace ocudu {
namespace ocucp {

/// \brief Handler of the NG interface of the CU-CP.
///
/// This interface is used to forward NGAP messages or TNL connection updates to the CU-CP.
class cu_cp_ng_handler
{
public:
  virtual ~cu_cp_ng_handler() = default;

  /// \brief Get the NG message handler interface.
  /// \param[in] plmn The PLMN of the NGAP.
  /// \return A pointer to the NG message handler interface if it was found, nullptr otherwise.
  virtual ngap_message_handler* get_ngap_message_handler(const plmn_identity& plmn) = 0;

  /// \brief Get the state of the AMF connections.
  /// \return True if all AMFs are connected, false otherwise.
  virtual bool amfs_are_connected() = 0;
};

} // namespace ocucp
} // namespace ocudu
