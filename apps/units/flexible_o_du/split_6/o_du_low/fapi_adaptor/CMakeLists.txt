# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        configuration_procedure.cpp
        phy_fapi_p5_sector_operation_request_adaptor.cpp)

add_library(ocudu_split6_flexible_o_du_low_unit_phy_fapi_p5_adaptor STATIC ${SOURCES})
target_include_directories(ocudu_split6_flexible_o_du_low_unit_phy_fapi_p5_adaptor PRIVATE ${CMAKE_SOURCE_DIR})
