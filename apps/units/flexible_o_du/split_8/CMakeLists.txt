# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.



add_subdirectory(helpers)

# Build the flexible du split 8 library when it is selected in the definition.
set(SOURCES
        split_8_o_du_application_unit_impl.cpp
        split_8_o_du_factory.cpp
        split_8_o_du_unit_cli11_schema.cpp
        split_8_o_du_unit_config_validator.cpp
        split_8_o_du_unit_config_yaml_writer.cpp)

add_library(ocudu_flexible_o_du_split_8 STATIC ${SOURCES})
target_include_directories(ocudu_flexible_o_du_split_8 PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ocudu_flexible_o_du_split_8
        ocudu_split_8_app_unit_helpers
        ocudu_flexible_o_du_helpers)
