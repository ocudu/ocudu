// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/rohc/rohc_factory.h"

namespace ocudu::rohc {

class rohc_lib_factory : public rohc_factory
{
public:
  rohc_lib_factory()  = default;
  ~rohc_lib_factory() = default;

  std::unique_ptr<rohc_compressor>   create_rohc_compressor(const rohc_config& cfg) const override;
  std::unique_ptr<rohc_decompressor> create_rohc_decompressor(const rohc_config& cfg) const override;
};

} // namespace ocudu::rohc
