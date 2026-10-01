# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_subdirectory(cu_up)
add_subdirectory(e2)

set(SOURCES
        o_cu_up_application_unit_impl.cpp
        o_cu_up_builder.cpp
        o_cu_up_unit_impl.cpp)

add_library(ocudu_o_cu_up_app_unit STATIC ${SOURCES})
target_include_directories(ocudu_o_cu_up_app_unit PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ocudu_o_cu_up_app_unit ocudu_cu_up_app_unit ocudu_o_cu_up_app_unit_e2 ocudu_o_cu_up)
