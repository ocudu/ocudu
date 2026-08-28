# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

set(SOURCES
        ue_cell_grid_allocator.cpp
        cell_group_event_manager.cpp
        ue_fallback_scheduler.cpp
        ue_scheduler_impl.cpp
        grant_params_selector.cpp
        intra_slice_scheduler.cpp
        triggered_ul_grant_scheduler.cpp
)

add_library(ue_sched OBJECT ${SOURCES})
