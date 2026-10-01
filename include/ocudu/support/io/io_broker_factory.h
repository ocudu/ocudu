// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/io/io_broker.h"
#include <memory>

namespace ocudu {

enum class io_broker_type { epoll, io_uring };

/// Creates an instance of an IO broker
std::unique_ptr<io_broker> create_io_broker(io_broker_type type, const io_broker_config& config = io_broker_config());

} // namespace ocudu
