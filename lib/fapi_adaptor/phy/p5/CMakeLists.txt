# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        p5_requests_handler.cpp
        phy_fapi_p5_sector_fastpath_adaptor_impl.cpp)

add_library(ocudu_phy_to_fapi_p5_sector_fastpath_adaptor STATIC ${SOURCES})
target_link_libraries(ocudu_phy_to_fapi_p5_sector_fastpath_adaptor PRIVATE ocudu_support)
