# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        ofh.cpp)

add_library(ocudu_ru_metrics_json_generators_helpers STATIC ${SOURCES})
target_include_directories(ocudu_ru_metrics_json_generators_helpers PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/apps/helpers/metrics)
target_link_libraries(ocudu_ru_metrics_json_generators_helpers PRIVATE ocudu::yaml-cpp)
