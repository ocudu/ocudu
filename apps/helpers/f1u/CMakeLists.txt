# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES f1u_cli11_schema.cpp
            f1u_appconfig_validator.cpp
            f1u_config_yaml_writer.cpp
)

add_library(ocudu_f1u_app_helpers STATIC ${SOURCES})
target_include_directories(ocudu_f1u_app_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ocudu_f1u_app_helpers PRIVATE ocudu::yaml-cpp)
