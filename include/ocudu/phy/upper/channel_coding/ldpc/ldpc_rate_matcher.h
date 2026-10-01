// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief LDPC rate matching.
#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/phy/upper/codeblock_metadata.h"

namespace ocudu {

class ldpc_encoder_buffer;

/// LDPC rate matching (i.e., bit selection and bit interleaving) interface.
class ldpc_rate_matcher
{
public:
  /// Default virtual destructor.
  virtual ~ldpc_rate_matcher() = default;

  /// \brief Carries out the rate matching of a codeblock.
  ///
  /// \param[out] output  Rate matched codeblock.
  /// \param[in]  input   Reference to an LDPC encoder buffer.
  /// \param[in]  cfg     Configuration parameters.
  /// \remark The sizes of \c input and \c output determine the behavior of the rate matching algorithm.
  virtual void rate_match(bit_buffer& output, const ldpc_encoder_buffer& input, const codeblock_metadata& cfg) = 0;
};

} // namespace ocudu
