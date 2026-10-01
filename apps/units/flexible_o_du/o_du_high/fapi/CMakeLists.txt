# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        fapi_config_cli11_schema.cpp
        fapi_config_yaml_writer.cpp)

add_library(ocudu_fapi_app_unit STATIC ${SOURCES})
target_include_directories(ocudu_fapi_app_unit PRIVATE ${CMAKE_SOURCE_DIR})
