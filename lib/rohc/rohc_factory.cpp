// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/rohc/rohc_factory.h"
#include "ocudu/rohc/rohc_compressor.h"
#include "ocudu/rohc/rohc_decompressor.h"
#ifdef ENABLE_ROHC_LIB
#include "rohc_lib/rohc_lib_factory.h"
#endif

using namespace ocudu;
using namespace ocudu::rohc;

class null_rohc_factory : public rohc_factory
{
public:
  null_rohc_factory()  = default;
  ~null_rohc_factory() = default;

  std::unique_ptr<rohc_compressor>   create_rohc_compressor(const rohc_config& cfg) const override { return nullptr; }
  std::unique_ptr<rohc_decompressor> create_rohc_decompressor(const rohc_config& cfg) const override { return nullptr; }
};

std::unique_ptr<rohc_factory> ocudu::rohc::create_rohc_factory()
{
#ifdef ENABLE_ROHC_LIB
  return std::make_unique<rohc_lib_factory>();
#else
  return std::make_unique<null_rohc_factory>();
#endif
}
