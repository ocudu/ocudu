# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


configure_file(version_info.h.in ${CMAKE_CURRENT_BINARY_DIR}/version_info.h)
include_directories(${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
add_library(ocudu_versioning STATIC build_info.cpp version.cpp)
add_custom_target(gen_build_info COMMAND cmake -P ${CMAKE_BINARY_DIR}/build_info.cmake)
add_dependencies(ocudu_versioning gen_build_info)
