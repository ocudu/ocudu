# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        split6_flexible_o_du_low_metrics_builder.cpp
        split6_flexible_o_du_low_metrics_consumers.cpp
        split6_flexible_o_du_low_metrics_producer.cpp)

add_library(ocudu_split6_flexible_o_du_low_unit_metrics_helpers STATIC ${SOURCES})
target_include_directories(ocudu_split6_flexible_o_du_low_unit_metrics_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ocudu_split6_flexible_o_du_low_unit_metrics_helpers PRIVATE ocudu_flexible_o_du_split_helpers_metrics_consumers ocudu_app_services)
