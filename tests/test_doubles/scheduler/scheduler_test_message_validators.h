// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Validators of scheduler result messages.
///
/// The validators in this file only perform checks that do not depend on any configuration or contextual information
/// of the scheduler or DU-high.

#pragma once

#include "ocudu/adt/span.h"

namespace ocudu {

struct dl_msg_alloc;
struct ul_sched_info;
struct coreset_configuration;

namespace test_helper {

/// \brief Determine if the PDSCH grant for a given UE has valid content.
bool is_valid_dl_msg_alloc(const dl_msg_alloc& grant, const std::optional<coreset_configuration>& coreset0);

/// \brief Determine if the PUSCH grant for a given UE has valid content.
bool is_valid_ul_sched_info(const ul_sched_info& grant);

/// \brief Determine if the UE PDSCH grants for a given slot are valid.
bool is_valid_dl_msg_alloc_list(span<const dl_msg_alloc> grants, const std::optional<coreset_configuration>& coreset0);

} // namespace test_helper
} // namespace ocudu
