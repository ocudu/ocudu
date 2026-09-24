# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_subdirectory(pdu_translators)

add_library(ocudu_fapi_to_phy_p7_fastpath_translator STATIC fapi_to_phy_fastpath_translator.cpp)
target_link_libraries(ocudu_fapi_to_phy_p7_fastpath_translator
        ocudu_fapi_phy_message_adaptors
        ocudu_fapi_precoding_codebook_tools
        ocudu_instrumentation)

set(SOURCES
        phy_to_fapi_error_event_fastpath_translator.cpp
        phy_to_fapi_results_event_fastpath_translator.cpp
        phy_to_fapi_time_event_fastpath_translator.cpp)

add_library(ocudu_phy_to_fapi_p7_fastpath_translator STATIC ${SOURCES})
target_link_libraries(ocudu_phy_to_fapi_p7_fastpath_translator PRIVATE ocudulog)

add_library(ocudu_phy_to_fapi_p7_sector_fastpath_adaptor STATIC phy_fapi_p7_sector_fastpath_adaptor_impl.cpp)
target_link_libraries(ocudu_phy_to_fapi_p7_sector_fastpath_adaptor PRIVATE ocudu_phy_to_fapi_p7_fastpath_translator
        ocudu_fapi_to_phy_p7_fastpath_translator)
