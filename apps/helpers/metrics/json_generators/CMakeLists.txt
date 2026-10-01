# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_subdirectory(cu_cp)
add_subdirectory(cu_up)
add_subdirectory(du_high)
add_subdirectory(ru)

set(SOURCES
        executors.cpp
        o_du_high.cpp
        o_du_low.cpp
        resource_usage.cpp)

add_library(ocudu_metrics_json_generators STATIC ${SOURCES})
target_include_directories(ocudu_metrics_json_generators PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ocudu_metrics_json_generators PRIVATE ocudu_cu_cp_metrics_json_generators_helpers
                                                             ocudu_cu_up_metrics_json_generators_helpers
                                                             ocudu_du_high_metrics_json_generators_helpers
                                                             ocudu_ru_metrics_json_generators_helpers)
