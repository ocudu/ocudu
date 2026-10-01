# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_subdirectory(consumers)

set(SOURCES
        du_high_rlc_metrics_producer.cpp
        du_high_rlc_metrics_consumers.cpp
        du_metrics_consumers.cpp
        du_metrics_producer.cpp)

add_library(ocudu_du_high_unit_metrics_helpers STATIC ${SOURCES})
target_include_directories(ocudu_du_high_unit_metrics_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ocudu_du_high_unit_metrics_helpers PRIVATE ocudu_du_high_unit_metrics_consumers ocudu_metrics_json_generators)
