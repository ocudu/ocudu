# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_subdirectory(consumers)

set(SOURCES
        flexible_o_du_metrics_builder.cpp
        flexible_o_du_metrics_consumers.cpp
        flexible_o_du_metrics_producer.cpp)

add_library(ocudu_flexible_o_du_unit_metrics_helpers STATIC ${SOURCES})
target_include_directories(ocudu_flexible_o_du_unit_metrics_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ocudu_flexible_o_du_unit_metrics_helpers PRIVATE ocudu_metrics_json_generators ocudu_flexible_o_du_split_helpers_metrics_consumers)
