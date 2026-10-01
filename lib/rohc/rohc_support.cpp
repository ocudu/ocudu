// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/rohc/rohc_support.h"

using namespace ocudu;
using namespace ocudu::rohc;

bool ocudu::rohc::rohc_supported()
{
#ifdef ENABLE_ROHC_LIB
  return true;
#else
  return false;
#endif
}
