# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(ocudu_config_app_helpers STATIC config_yaml_schema.cpp)
target_include_directories(ocudu_config_app_helpers PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/external)
target_link_libraries(ocudu_config_app_helpers PRIVATE ocudu::yaml-cpp)
