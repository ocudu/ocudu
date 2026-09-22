# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        hal_cli11_schema.cpp
        hal_appconfig_yaml_writer.cpp)

add_library(ocudu_hal_app_helpers STATIC ${SOURCES})
target_include_directories(ocudu_hal_app_helpers PRIVATE ${CMAKE_SOURCE_DIR})
