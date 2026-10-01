// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/adt/format.h"
#include "ocudu/cu_cp/cu_cp_configuration.h"
#include "ocudu/cu_cp/cu_cp_configuration_helpers.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace config_helpers;

TEST(cu_cp_config_test, default_config_is_valid)
{
  ocudu::ocucp::cu_cp_configuration cfg = make_default_cu_cp_config();
  ASSERT_TRUE(is_valid_configuration(cfg));
}
