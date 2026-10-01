# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_subdirectory(p5)
add_subdirectory(p7)

add_library(ocudu_phy_fapi_fastpath_adaptor STATIC phy_fapi_fastpath_adaptor_factory_impl.cpp
    phy_fapi_fastpath_adaptor_impl.cpp
    phy_fapi_sector_fastpath_adaptor_impl.cpp)

target_link_libraries(ocudu_phy_fapi_fastpath_adaptor PRIVATE ocudu_phy_to_fapi_p7_sector_fastpath_adaptor
    ocudu_phy_to_fapi_p5_sector_fastpath_adaptor)
