// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/fapi/fapi_power_unit.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi;

TEST(fapi_power_unit_test, fapi_power_unit_operations_passes)
{
  float value_one = 12.345;
  float value_two = -1.234;

  float dbfs_to_dbm_conversion_factor = 5.6;
  float db_to_dbfs_conversion_factor  = 3.4;

  fapi_power_unit power_unit_one(value_one, dbfs_to_dbm_conversion_factor, db_to_dbfs_conversion_factor);
  fapi_power_unit power_unit_two(value_two, dbfs_to_dbm_conversion_factor, db_to_dbfs_conversion_factor);

  ASSERT_EQ(value_one, power_unit_one.to_dB());
  ASSERT_EQ((power_unit_one + power_unit_two).value(), value_one + value_two);
  ASSERT_EQ((power_unit_one - power_unit_two).value(), value_one - value_two);
  ASSERT_EQ((power_unit_one * 2).value(), value_one * 2);
  ASSERT_EQ((power_unit_one / 2).value(), value_one / 2);
}

TEST(fapi_power_unit_test, conversion_from_dB_to_dBFS_passes)
{
  float value_one                     = 12.345;
  float dbfs_to_dbm_conversion_factor = 5.6;
  float db_to_dbfs_conversion_factor  = 3.4;
  float expected_dBFS                 = value_one + db_to_dbfs_conversion_factor;

  fapi_power_unit power_unit_one(value_one, dbfs_to_dbm_conversion_factor, db_to_dbfs_conversion_factor);

  EXPECT_EQ(power_unit_one.to_dBFS(), expected_dBFS);
}

TEST(fapi_power_unit_test, conversion_from_dB_to_dBm_passes)
{
  float value_one                     = 12.345;
  float dbfs_to_dbm_conversion_factor = 5.6;
  float db_to_dbfs_conversion_factor  = 3.4;
  float expected_dBm                  = (value_one + db_to_dbfs_conversion_factor) + dbfs_to_dbm_conversion_factor;

  fapi_power_unit power_unit_one(value_one, dbfs_to_dbm_conversion_factor, db_to_dbfs_conversion_factor);

  EXPECT_EQ(power_unit_one.to_dBm(), expected_dBm);
}
