# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_subdirectory(metrics)

set(SOURCES
        cu_cp_unit_config_cli11_schema.cpp
        cu_cp_config_translators.cpp
        cu_cp_unit_config_validator.cpp
        cu_cp_unit_config_yaml_writer.cpp)

add_library(ocudu_cu_cp_app_unit STATIC ${SOURCES})
target_include_directories(ocudu_cu_cp_app_unit PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ocudu_cu_cp_app_unit PRIVATE ocudu_cu_cp_unit_metrics_helpers ocudu_app_helpers ocudu_ntn_app_helpers)
