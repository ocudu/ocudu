# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "phy")

add_subdirectory(modulation)
add_subdirectory(processors)

add_executable(lower_phy_test lower_phy_test.cpp)
target_link_libraries(lower_phy_test ocudu_lower_phy ocudulog GTest::gtest GTest::gtest_main)
add_test(lower_phy_test lower_phy_test)
