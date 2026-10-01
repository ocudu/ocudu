# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_subdirectory(du_high)
add_subdirectory(e2)
add_subdirectory(fapi)

add_library(ocudu_o_du_high_unit_helpers STATIC o_du_high_unit_factory.cpp)
target_include_directories(ocudu_o_du_high_unit_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ocudu_o_du_high_unit_helpers PRIVATE
        ocudu_du_high_unit_helpers
        ocudu_fapi_app_unit
        ocudu_o_du_high_app_unit_e2
        ocudu_o_du_high)
