# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
  mobility_manager_helpers.cpp
  mobility_manager_impl.cpp
  metrics/mobility_manager_metrics_aggregator.cpp
)

add_library(ocudu_cu_cp_mobility_manager STATIC ${SOURCES})
target_link_libraries(ocudu_cu_cp_mobility_manager ocudu_ran)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
