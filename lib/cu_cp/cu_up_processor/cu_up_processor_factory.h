// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "cu_up_processor.h"
#include "cu_up_processor_config.h"
#include <memory>

namespace ocudu::ocucp {

/// Creates an instance of an CU-UP processor interface
std::unique_ptr<cu_up_processor> create_cu_up_processor(const cu_up_processor_config&       cfg,
                                                        const cu_up_processor_dependencies& dependencies);

} // namespace ocudu::ocucp
