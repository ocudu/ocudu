// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/memory_pool/bounded_object_pool.h"

namespace ocudu {

class prach_buffer;

using prach_buffer_pool   = bounded_rc_object_pool<prach_buffer>;
using shared_prach_buffer = prach_buffer_pool::ptr;

} // namespace ocudu
