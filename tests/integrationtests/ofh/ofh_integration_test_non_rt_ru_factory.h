// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ru/ofh/ru_ofh_configuration.h"
#include "ocudu/ru/ru.h"
#include <memory>

namespace ocudu {
namespace ofh {
namespace test {

/// \brief Creates an Open Fronthaul Radio Unit that runs in non-realtime mode.
///
/// Mirrors \c create_ofh_ru(), except that the realtime timing worker is replaced by a timing manager that counts OTA
/// symbols, sleeping one symbol duration between consecutive notifications, instead of following the system clock.
std::unique_ptr<radio_unit> create_non_rt_ofh_ru(const ru_ofh_configuration& config,
                                                 ru_ofh_dependencies&&       dependencies);

} // namespace test
} // namespace ofh
} // namespace ocudu
