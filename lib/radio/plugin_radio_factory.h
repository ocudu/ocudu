// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/radio/radio_factory.h"
#include <memory>
#include <string>

namespace ocudu {

std::unique_ptr<radio_factory> create_plugin_radio_factory(std::string driver_name);

} // namespace ocudu
