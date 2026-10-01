# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        cu_up_f1u_metrics_producer.cpp
        cu_up_f1u_metrics_consumers.cpp
        cu_up_pdcp_metrics_producer.cpp
        cu_up_pdcp_metrics_consumers.cpp)

add_library(ocudu_cu_up_unit_metrics_helpers STATIC ${SOURCES})
target_include_directories(ocudu_cu_up_unit_metrics_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ocudu_cu_up_unit_metrics_helpers PRIVATE ocudu_cu_up_metrics_json_generators_helpers)
