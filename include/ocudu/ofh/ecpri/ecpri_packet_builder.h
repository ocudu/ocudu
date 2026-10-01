// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ofh/ecpri/ecpri_packet_properties.h"
#include "ocudu/support/units.h"

namespace ocudu {
namespace ecpri {

/// Describes the eCPRI packet builder.
class packet_builder
{
public:
  /// Default destructor.
  virtual ~packet_builder() = default;

  /// Returns the eCPRI header size in bytes for the given eCPRI message type.
  virtual units::bytes get_header_size(message_type msg_type) const = 0;

  /// Builds a control eCPRI packet using the given control parameters into the given \c buffer.
  ///
  /// This function expects that the buffer already contains the payload of the packet. The builder will write the
  /// header in the first \ref ecpri_packet_builder::get_header_size bytes of the buffer.
  ///
  /// \param[in] buffer Buffer where the packet will be built.
  /// \param[in] msg_params eCPRI message parameters.
  virtual void build_control_packet(span<uint8_t> buffer, const realtime_control_parameters& msg_params) = 0;

  /// Builds a data eCPRI packet using the given data parameters into the given \c buffer.
  ///
  /// This function expects that the buffer already contains the payload of the packet. The builder will write the
  /// header in the first \ref ecpri_packet_builder::get_header_size bytes of the buffer.
  ///
  /// \param[in] buffer Buffer where the packet will be built.
  /// \param[in] msg_params eCPRI message parameters.
  virtual void build_data_packet(span<uint8_t> buffer, const iq_data_parameters& msg_params) = 0;
};

} // namespace ecpri
} // namespace ocudu
