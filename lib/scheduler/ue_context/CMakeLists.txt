# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        logical_channel_system.cpp
        ta_management_system.cpp
        ue_channel_state_manager.cpp
        ue_link_adaptation_controller.cpp
        ue_drx_controller.cpp
        ue_cell_repository.cpp
        ue_repository.cpp
        ue.cpp
        ue_cell.cpp)

add_library(sched_ue_context STATIC ${SOURCES})
target_link_libraries(sched_ue_context ocudu_ran sched_support sched_cell)
