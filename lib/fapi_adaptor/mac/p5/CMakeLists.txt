# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        mac_fapi_p5_fastpath_cell_operation_controller_impl.cpp
        mac_fapi_p5_sector_fastpath_adaptor_impl.cpp
        mac_fapi_p5_start_cell_procedure.cpp
        mac_fapi_p5_stop_cell_procedure.cpp
        p5_responses_handler.cpp)

add_library(ocudu_mac_to_fapi_p5_sector_fastpath_adaptor STATIC ${SOURCES})
target_link_libraries(ocudu_mac_to_fapi_p5_sector_fastpath_adaptor PRIVATE ocudu_support)
