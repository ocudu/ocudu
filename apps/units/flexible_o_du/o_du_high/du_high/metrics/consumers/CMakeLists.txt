# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

set(SOURCES
        du_procedure_metrics_consumers.cpp
        mac_metrics_consumers.cpp
        scheduler_metrics_consumers.cpp)

add_library(ocudu_du_high_unit_metrics_consumers STATIC ${SOURCES})
target_include_directories(ocudu_du_high_unit_metrics_consumers PRIVATE ${CMAKE_SOURCE_DIR})
