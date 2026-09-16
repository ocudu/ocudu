# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

set(SOURCES
        ra_scheduler.cpp
        ra_ue_repository.cpp
        si_scheduler.cpp
        sib1_scheduler.cpp
        ssb_scheduler.cpp
        prach_scheduler.cpp
        paging_scheduler.cpp
        csi_rs_scheduler.cpp
        prs_scheduler.cpp
        si_message_scheduler.cpp)

add_library(common_sched OBJECT ${SOURCES})
