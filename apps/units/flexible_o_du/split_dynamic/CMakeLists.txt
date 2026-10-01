# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        dynamic_o_du_application_unit_impl.cpp
        dynamic_o_du_factory.cpp
        dynamic_o_du_translators.cpp
        dynamic_o_du_unit_cli11_schema.cpp
        dynamic_o_du_unit_config_validator.cpp
        dynamic_o_du_unit_config_yaml_writer.cpp)

add_library(ocudu_flexible_o_du_split_dynamic STATIC ${SOURCES})
target_include_directories(ocudu_flexible_o_du_split_dynamic PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ocudu_flexible_o_du_split_dynamic
        ocudu_ru_dummy
        ocudu_split_8_app_unit_helpers
        ocudu_split_7_2_app_unit_helpers
        ocudu_flexible_o_du_helpers)
