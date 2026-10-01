# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set(SOURCES
        ofh_timing_manager_impl.cpp
        ofh_timing_metrics_collector_impl.cpp
        realtime_timing_worker.cpp)

add_library(ocudu_ofh_timing STATIC ${SOURCES})
target_link_libraries(ocudu_ofh_timing ocudulog ocudu_instrumentation)
