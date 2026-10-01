# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        slot_point_extender_adaptor.cpp
        split6_o_du_application_unit_impl.cpp
        split6_o_du_factory.cpp
        split6_o_du_impl.cpp
        split6_o_du_unit_cli11_schema.cpp
        split6_o_du_unit_config_validator.cpp)

add_library(ocudu_split6_plugin STATIC split6_plugin_dummy.cpp)

add_library(ocudu_flexible_o_du_split_6 STATIC ${SOURCES})
target_include_directories(ocudu_flexible_o_du_split_6 PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ocudu_flexible_o_du_split_6
                                                    ocudu_o_du
                                                    ocudu_o_du_high_unit_helpers
                                                    ocudu_split6_plugin
                                                    ocudu_metrics_json_generators)
