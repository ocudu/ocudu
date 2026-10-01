// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ofh/serdes/ofh_cplane_message_properties.h"

namespace ocudu {
namespace ofh {

/// \brief Open Fronthaul Control-Plane message builder interface.
///
/// Builds a Control-Plane message following the O-RAN Open Fronthaul specification.
class cplane_message_builder
{
public:
  /// Default destructor.
  virtual ~cplane_message_builder() = default;

  /// \brief Builds a DL/UL radio channel Control-Plane message into the given buffer using the given
  /// \c parameters.
  ///
  /// This Control-Plane message is used for most DL/UL radio channels.
  ///
  /// \param[out] buffer Buffer where the message will be built.
  /// \param[in] msg_params Control-Plane parameters.
  /// \return Number of bytes serialized in the buffer.
  virtual unsigned build_dl_ul_radio_channel_message(span<uint8_t>                          buffer,
                                                     const cplane_section_type1_parameters& msg_params) = 0;

  /// \brief Builds Control-Plane message aimed at idle or guard periods indication from O-DU to O-RU.
  ///
  /// \param[out] buffer    Buffer where the message will be built.
  /// \param[in] msg_params Control-Plane parameters.
  /// \return Number of bytes serialized in the buffer.
  virtual unsigned build_idle_guard_period_message(span<uint8_t>                          buffer,
                                                   const cplane_section_type0_parameters& msg_params) = 0;

  /// \brief Builds PRACH and mixed-numerology channel Control-Plane message into the given buffer using the given
  /// \c parameters.
  ///
  /// \param[out] buffer    Buffer where the message will be built.
  /// \param[in] msg_params Control-Plane parameters.
  /// \return Number of bytes serialized in the buffer.
  virtual unsigned build_prach_mixed_numerology_message(span<uint8_t>                          buffer,
                                                        const cplane_section_type3_parameters& msg_params) = 0;
};

} // namespace ofh
} // namespace ocudu
