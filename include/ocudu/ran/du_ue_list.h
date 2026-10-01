// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/slotted_array.h"
#include "ocudu/ran/du_types.h"

namespace ocudu {

/// \brief Representation of a list of UEs indexed by DU UE Index.
template <typename T>
using du_ue_list = slotted_array<T, MAX_NOF_DU_UES>;

} // namespace ocudu
