// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ofh/ecpri/ecpri_packet_builder.h"

namespace ocudu {
namespace ecpri {

/// eCPRI packet builder implementation.
class packet_builder_impl : public packet_builder
{
public:
  // See interface for documentation.
  units::bytes get_header_size(message_type msg_type) const override;

  // See interface for documentation.
  void build_control_packet(span<uint8_t> buffer, const realtime_control_parameters& msg_params) override;

  // See interface for documentation.
  void build_data_packet(span<uint8_t> buffer, const iq_data_parameters& msg_params) override;
};

} // namespace ecpri
} // namespace ocudu
