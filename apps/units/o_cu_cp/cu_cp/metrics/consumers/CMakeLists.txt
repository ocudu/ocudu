# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        ngap_metrics_consumers.cpp
        rrc_metrics_consumers.cpp
)

add_library(ocudu_cu_cp_unit_metrics_consumers STATIC ${SOURCES})
target_include_directories(ocudu_cu_cp_unit_metrics_consumers PRIVATE ${CMAKE_SOURCE_DIR})
