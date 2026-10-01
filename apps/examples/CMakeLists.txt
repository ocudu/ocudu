# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_subdirectory(ofh)
add_subdirectory(phy)
add_subdirectory(radio)
if (ENABLE_SCHEDTRACE)
    add_subdirectory(schedlog)
endif (ENABLE_SCHEDTRACE)
