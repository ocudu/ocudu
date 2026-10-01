// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rohc_lib_factory.h"
#include "rohc_lib_compressor.h"
#include "rohc_lib_decompressor.h"

using namespace ocudu;
using namespace ocudu::rohc;

std::unique_ptr<rohc_compressor> rohc_lib_factory::create_rohc_compressor(const rohc_config& cfg) const
{
  return std::make_unique<rohc_lib_compressor>(cfg);
}

std::unique_ptr<rohc_decompressor> rohc_lib_factory::create_rohc_decompressor(const rohc_config& cfg) const
{
  return std::make_unique<rohc_lib_decompressor>(cfg);
}
