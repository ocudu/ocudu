// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

/// \brief Receive buffer decoder callback interface.
///
/// This interface is used by shared channel decoders (namely PUSCH decoder) for guaranteeing the order of decoding.
class rx_buffer_decoder_callback
{
public:
  /// Default destructor.
  virtual ~rx_buffer_decoder_callback() = default;

  /// \brief Decode callback.
  ///
  /// This method is called from the last repetition.
  ///
  /// \param[in] codeblock_id Codeblock identifier within the transport block to decode.
  virtual void codeblock_decode(unsigned codeblock_id) = 0;
};

} // namespace ocudu
