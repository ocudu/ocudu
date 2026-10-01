# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_subdirectory(consumers)

set(SOURCES
        cu_cp_metrics_producer.cpp
        cu_cp_metrics_consumers.cpp)

add_library(ocudu_cu_cp_unit_metrics_helpers STATIC ${SOURCES})
target_include_directories(ocudu_cu_cp_unit_metrics_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ocudu_cu_cp_unit_metrics_helpers PRIVATE ocudu_cu_cp_unit_metrics_consumers ocudu_cu_cp_metrics_json_generators_helpers)
