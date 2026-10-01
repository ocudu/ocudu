# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_subdirectory(prach)
add_subdirectory(puxch)

add_library(ocudu_lower_phy_uplink_processor STATIC
        uplink_processor_factories.cpp
        uplink_processor_impl.cpp)
target_link_libraries(ocudu_lower_phy_uplink_processor
        ocudu_lower_phy_prach_processor
        ocudu_lower_phy_puxch_processor)