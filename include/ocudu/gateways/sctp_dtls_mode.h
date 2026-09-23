// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {

enum class dtls_mode { client, server };

inline const char* format_as(dtls_mode mode)
{
  static constexpr const char* options[] = {"client", "server"};
  return options[static_cast<unsigned>(mode)];
}
} // namespace ocudu
