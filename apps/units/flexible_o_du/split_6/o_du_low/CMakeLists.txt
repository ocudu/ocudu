# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_subdirectory(fapi_adaptor)
add_subdirectory(metrics)

set(SOURCES
            split6_flexible_o_du_low.cpp
            split6_flexible_o_du_low_session.cpp
            split6_flexible_o_du_low_session_factory.cpp
            split6_flexible_o_du_low_session_manager.cpp
            split6_o_du_low_application_unit_impl.cpp
            split6_o_du_low_metrics_collector_impl.cpp
            split6_o_du_low_unit_cli11_schema.cpp
            split6_o_du_low_unit_config_validator.cpp)

add_library(ocudu_split6_o_du_low_plugin STATIC split6_o_du_low_plugin_dummy.cpp)
target_include_directories(ocudu_split6_o_du_low_plugin PRIVATE ${CMAKE_SOURCE_DIR})

add_library(ocudu_flexible_o_du_low_split_6 STATIC ${SOURCES})
target_include_directories(ocudu_flexible_o_du_low_split_6 PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ocudu_flexible_o_du_low_split_6 PRIVATE
        ocudu_split6_flexible_o_du_low_unit_metrics_helpers
        ocudu_split6_flexible_o_du_low_unit_phy_fapi_p5_adaptor
        ocudu_o_du_low_unit_helpers
        ocudu_split_8_app_unit_helpers
        ocudu_split_7_2_app_unit_helpers
        ocudu_split6_o_du_low_plugin)
