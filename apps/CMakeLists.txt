# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


if (NOT DISABLE_MBEDTLS AND NOT DISABLE_SCTP)
    add_subdirectory(cu)
    add_subdirectory(cu_cp)
    add_subdirectory(cu_up)
    add_subdirectory(du)
    add_subdirectory(du_low)
    add_subdirectory(gnb)
endif ()
add_subdirectory(examples)
add_subdirectory(helpers)
add_subdirectory(services)
add_subdirectory(units)
