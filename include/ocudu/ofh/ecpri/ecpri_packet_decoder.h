// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ofh/ecpri/ecpri_packet_properties.h"
#include <variant>

namespace ocudu {
namespace ecpri {

/// eCPRI packet parameter.
struct packet_parameters {
  /// Common header.
  common_header header;
  /// eCPRI type parameters.
  std::variant<realtime_control_parameters, iq_data_parameters> type_params;
};

/// eCPRI packet decoder interface.
class packet_decoder
{
public:
  /// Default destructor.
  virtual ~packet_decoder() = default;

  /// \brief Decodes the given packet, filling the eCPRI params and returning a span to the eCPRI payload.
  ///
  /// If the decoder detects an unsupported value, it will return an empty span and the params value will be undefined.
  ///
  /// \param[in] packet eCPRI packet to decode.
  /// \param[out] params eCPRI decoded parameters.
  /// \return A span to the eCPRI payload on success, otherwise an empty span in case of error.
  virtual span<const uint8_t> decode(span<const uint8_t> packet, packet_parameters& params) = 0;
};

} // namespace ecpri
} // namespace ocudu
