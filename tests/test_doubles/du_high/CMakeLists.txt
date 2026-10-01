# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

add_library(du_high_test_doubles rrm_policy_remote_command_helpers.cpp)
target_include_directories(du_high_test_doubles PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(du_high_test_doubles ocudu_support ocudu_du_high_unit_helpers)
