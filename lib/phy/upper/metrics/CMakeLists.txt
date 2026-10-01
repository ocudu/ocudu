# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        upper_phy_ldpc_metrics_collector.cpp
        upper_phy_pdsch_metrics_collector.cpp
        upper_phy_pusch_metrics_collector.cpp
        upper_phy_metrics_collector_impl.cpp)

add_library(ocudu_upper_phy_metrics STATIC ${SOURCES})
target_link_libraries(ocudu_upper_phy_metrics ocudu_support)
target_include_directories(ocudu_upper_phy_metrics PRIVATE ${CMAKE_SOURCE_DIR})
