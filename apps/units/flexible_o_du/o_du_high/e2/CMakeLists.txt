# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        o_du_high_e2_config_cli11_schema.cpp
        o_du_high_e2_config_translators.cpp
        o_du_high_e2_config_yaml_writer.cpp)

add_library(ocudu_o_du_high_app_unit_e2 STATIC ${SOURCES})
target_include_directories(ocudu_o_du_high_app_unit_e2 PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ocudu_o_du_high_app_unit_e2 ocudu_e2_app_helpers)
