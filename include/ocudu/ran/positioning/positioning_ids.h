// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>

namespace ocudu {

// LMF measurement ID.
enum class lmf_meas_id_t : uint32_t { min = 1, max = 65536 };

/// Convert integer to lmf_meas_id type.
inline lmf_meas_id_t uint_to_lmf_meas_id(uint32_t id)
{
  return static_cast<lmf_meas_id_t>(id);
}

// RAN measurement ID.
enum class ran_meas_id_t : uint32_t { min = 1, max = 65536 };

/// Convert integer to ran_meas_id type.
inline ran_meas_id_t uint_to_ran_meas_id(uint32_t id)
{
  return static_cast<ran_meas_id_t>(id);
}

// Transmission-Reception Point (TRP) ID.
enum class trp_id_t : uint32_t { min = 1, max = 65535 };

/// Convert integer to trp_id type.
inline trp_id_t uint_to_trp_id(uint32_t id)
{
  return static_cast<trp_id_t>(id);
}

} // namespace ocudu
