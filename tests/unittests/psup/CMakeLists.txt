# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "psup")

add_executable(psup_packing_test psup_packing_test.cpp)
target_include_directories(psup_packing_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(psup_packing_test ocudu_psup ocudu_support ocudulog GTest::gtest GTest::gtest_main)
gtest_discover_tests(psup_packing_test)
