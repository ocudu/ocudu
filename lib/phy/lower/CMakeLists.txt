# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_subdirectory(modulation)
add_subdirectory(processors)

add_library(ocudu_lower_phy STATIC
        lower_phy_baseband_processor.cpp
        lower_phy_factory.cpp
        lower_phy_impl.cpp)
target_link_libraries(ocudu_lower_phy
        ocudu_dft
        ocudu_lower_phy_adaptors
        ocudu_lower_phy_downlink_processor
        ocudu_lower_phy_pdxch_processor
        ocudu_lower_phy_modulation
        ocudu_lower_phy_prach_processor
        ocudu_lower_phy_puxch_processor
        ocudu_lower_phy_uplink_processor)
