// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/ntn/ntn_math_helpers.h"
#include "ocudu/support/test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocudu_ntn;

TEST(func_fitting_test, fit_quadratic_func)
{
  {
    // y = 10 + 2x + 3x^2
    std::vector<double> x = {0, 1, 2, 3, 4, 5};
    std::vector<double> y = {10, 15, 26, 43, 66, 95};
    auto [c0, c1, c2]     = fit_quadratic(x, y, y[0]);
    ASSERT_EQ(c0, 10);
    ASSERT_EQ(c1, 2);
    ASSERT_EQ(c2, 3);
  }
  {
    // y = 3.33 + 2.5x + 0.1x^2
    std::vector<double> x = {0, 1, 2, 3, 4, 5};
    std::vector<double> y = {10, 15, 26, 43, 66, 95};
    auto [c0, c1, c2]     = fit_quadratic(x, y, y[0]);
    ASSERT_EQ(c0, 10);
    ASSERT_EQ(c1, 2);
    ASSERT_EQ(c2, 3);
  }
}

TEST(func_fitting_test, fit_linear_func)
{
  {
    // y = 0.5 + 3x
    std::vector<double> x = {0, 1, 2, 3, 4, 5};
    std::vector<double> y = {0.5, 3.5, 6.5, 9.5, 12.5, 15.5};
    auto [c0, c1, c2]     = fit_quadratic(x, y, y[0]);
    ASSERT_EQ(c0, 0.5);
    ASSERT_EQ(c1, 3);
    ASSERT_EQ(c2, 0);
  }
  {
    // y = 1.5 + 5.5x
    std::vector<double> x = {1, 2, 3, 4, 5};
    std::vector<double> y = {7.0, 12.5, 18.0, 23.5, 29};
    auto [c0, c1, c2]     = fit_quadratic(x, y, 1.5);
    ASSERT_EQ(c0, 1.5);
    ASSERT_EQ(c1, 5.5);
    ASSERT_EQ(c2, 0);
  }
}

TEST(func_fitting_test, fit_constant_func)
{
  {
    // y = 9.0
    std::vector<double> x = {0, 1, 2, 3, 4, 5};
    std::vector<double> y = {9, 9, 9, 9, 9, 9};
    auto [c0, c1, c2]     = fit_quadratic(x, y, y[0]);
    ASSERT_EQ(c0, 9);
    ASSERT_EQ(c1, 0);
    ASSERT_EQ(c2, 0);
  }
  {
    // y = 1.2345
    std::vector<double> x = {11, 12, 13, 14, 15, 16};
    std::vector<double> y = {1.2345, 1.2345, 1.2345, 1.2345, 1.2345, 1.2345};
    auto [c0, c1, c2]     = fit_quadratic(x, y, y[0]);
    ASSERT_EQ(c0, 1.2345);
    ASSERT_EQ(c1, 0);
    ASSERT_EQ(c2, 0);
  }
}
