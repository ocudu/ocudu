// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include <istream>

namespace ocudu::schedtrace {

/// Read a stream of size-prefixed CellEvent flatbuffers and convert it to scheduler logs.
bool trace_to_log(std::istream& input, ocudulog::basic_logger& logger);

} // namespace ocudu::schedtrace
