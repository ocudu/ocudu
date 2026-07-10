# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        ntn_config_cli11_schema.cpp
        ntn_config_translators.cpp
        ntn_config_yaml_writer.cpp)

add_library(ocudu_ntn_app_helpers STATIC ${SOURCES})
target_include_directories(ocudu_ntn_app_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ocudu_ntn_app_helpers ocudu_app_helpers ocudu_yaml_util)
