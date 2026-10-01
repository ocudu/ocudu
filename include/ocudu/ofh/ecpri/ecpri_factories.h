// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/ofh/ecpri/ecpri_packet_builder.h"
#include "ocudu/ofh/ecpri/ecpri_packet_decoder.h"
#include <memory>

namespace ocudu {
namespace ecpri {

/// Creates and returns an eCPRI packet builder.
std::unique_ptr<packet_builder> create_ecpri_packet_builder();

/// Creates and returns an eCPRI packet decoder utilizing payload size encoded in eCPRI header.
std::unique_ptr<packet_decoder> create_ecpri_packet_decoder_using_payload_size(ocudulog::basic_logger& logger,
                                                                               unsigned                sector);

/// Creates and returns an eCPRI packet decoder ignoring payload size encoded in eCPRI header.
std::unique_ptr<packet_decoder> create_ecpri_packet_decoder_ignoring_payload_size(ocudulog::basic_logger& logger,
                                                                                  unsigned                sector);

} // namespace ecpri
} // namespace ocudu
