# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

set(SOURCES scheduler_result_logger.cpp cell_metrics_handler.cpp scheduler_event_logger.cpp cell_event_tracer.cpp)
add_library(scheduler_logger STATIC ${SOURCES})
target_link_libraries(scheduler_logger ocudu_sched ocudu_ran ocudulog)
