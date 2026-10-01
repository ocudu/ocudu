// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"

namespace ocudu::rohc {

class rohc_compressor
{
public:
  virtual ~rohc_compressor()                                = default;
  virtual byte_buffer compress(byte_buffer packet)          = 0;
  virtual bool        handle_feedback(byte_buffer feedback) = 0;
};

} // namespace ocudu::rohc
