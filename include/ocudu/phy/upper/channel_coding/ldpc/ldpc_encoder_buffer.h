// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include <stdint.h>

namespace ocudu {

/// \brief LDPC encoder output buffer interface.
///
/// This interface allows the LDPC encoder to provide the encoded bits to the rate matcher without using an intermediate
/// buffer.
class ldpc_encoder_buffer
{
public:
  /// Default destructor.
  virtual ~ldpc_encoder_buffer() = default;

  /// Gets the encoded codeblock length.
  virtual unsigned get_codeblock_length() const = 0;

  /// \brief Reads the encoded codeblock data starting at bit index \c offset.
  ///
  /// The data size plus the offset shall not exceed the encoded codeblock length.
  ///
  /// \param[out] data   Read destination.
  /// \param[in]  offset Initial read position.
  virtual void write_codeblock(span<uint8_t> data, unsigned offset) const = 0;
};

} // namespace ocudu
