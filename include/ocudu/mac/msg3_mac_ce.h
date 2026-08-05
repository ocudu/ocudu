// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/logical_channel/phr_report.h"
#include <chrono>
#include <variant>
#include <vector>

namespace ocudu {

/// Decoded per-UE MAC CE carried in Msg3 whose processing is deferred until the UE context exists.
///
/// Only a Single Entry PHR fits in a Msg3, as the UE has no SCell yet. The microseconds alternative is a TA Report.
using msg3_mac_ce = std::variant<cell_ph_report, std::chrono::microseconds>;

/// Decoded Msg3 MAC CEs in the order in which they appeared in the PDU.
using msg3_mac_ce_list = std::vector<msg3_mac_ce>;

} // namespace ocudu
