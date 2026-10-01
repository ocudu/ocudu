# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        app_resource_usage.cpp
        app_resource_usage_config_cli11_schema.cpp
        app_resource_usage_config_yaml_writer.cpp
        metrics/app_resource_usage_metrics_consumer.cpp)

add_library(ocudu_resource_usage_app_service STATIC ${SOURCES})
target_include_directories(ocudu_resource_usage_app_service PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ocudu_resource_usage_app_service PRIVATE ocudu_metrics_app_helpers ocudu_metrics_json_generators ocudu::yaml-cpp)
