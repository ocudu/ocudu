# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.



set(SOURCES
        metrics_config_cli11_schema.cpp
        metrics_config_yaml_writer.cpp)

add_library(ocudu_metrics_app_service STATIC ${SOURCES})
target_include_directories(ocudu_metrics_app_service PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ocudu_metrics_app_service PRIVATE ocudu::yaml-cpp)
