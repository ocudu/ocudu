// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ue_manager/ue_manager_impl.h"
#include "du_processor.h"
#include "du_processor_config.h"
#include <memory>

namespace ocudu::ocucp {

/// Creates an instance of an DU processor interface
std::unique_ptr<du_processor> create_du_processor(const du_processor_config& cfg,
                                                  du_processor_dependencies  dependencies);

} // namespace ocudu::ocucp
