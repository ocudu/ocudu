// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/units.h"

namespace ocudu {
namespace ecpri {

/// eCPRI protocol revision.
constexpr unsigned ECPRI_PROTOCOL_REVISION = 1U;

/// Size of the eCPRI common header.
constexpr units::bytes ECPRI_COMMON_HEADER_SIZE{4U};

/// Size of the header fields in a eCPRI IQ data packet.
constexpr units::bytes ECPRI_IQ_DATA_PACKET_FIELDS_SIZE{4U};

/// Size of the header fields in a eCPRI Real-Time control packet.
constexpr units::bytes ECPRI_REALTIME_CONTROL_PACKET_FIELDS_SIZE{4U};

} // namespace ecpri
} // namespace ocudu
