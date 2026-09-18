# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

add_library(security_test_doubles security_test_keys.cpp)
target_include_directories(security_test_doubles PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(security_test_doubles ocudu_support ocudulog ocudu_security)
