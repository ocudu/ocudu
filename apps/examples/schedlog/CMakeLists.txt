# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_executable(schedlog schedlog.cpp)
target_include_directories(schedlog PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(schedlog trace_to_log ocudulog ocudu_support)
