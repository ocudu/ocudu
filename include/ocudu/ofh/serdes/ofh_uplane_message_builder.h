// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"
#include "ocudu/ofh/serdes/ofh_uplane_message_properties.h"
#include "ocudu/support/units.h"

namespace ocudu {
namespace ofh {

/// \brief Open Fronthaul User-Plane message builder interface.
///
/// Builds a User Plane message following the O-RAN Open Fronthaul specification.
class uplane_message_builder
{
public:
  /// Default destructor.
  virtual ~uplane_message_builder() = default;

  /// Returns the Open Fronthaul User-Plane header size in bytes.
  virtual units::bytes get_header_size(const ru_compression_params& params) const = 0;

  /// \brief Builds a User Plane message given the \c config parameters, places result in \c buffer.
  ///
  /// \param[out] buffer Buffer where the message will be built.
  /// \param[in] iq_data IQ samples.
  /// \param[in] config  User plane message parameters.
  /// \return Number of bytes serialized in the buffer.
  virtual unsigned
  build_message(span<uint8_t> buffer, span<const cbf16_t> iq_data, const uplane_message_params& params) = 0;
};

} // namespace ofh
} // namespace ocudu
