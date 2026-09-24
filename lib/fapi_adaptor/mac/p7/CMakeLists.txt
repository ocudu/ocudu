# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_subdirectory(pdu_translators)

add_library(ocudu_mac_to_fapi_p7_fastpath_translator STATIC mac_to_fapi_fastpath_translator.cpp)
target_link_libraries(ocudu_mac_to_fapi_p7_fastpath_translator PRIVATE ocudu_mac_fapi_adaptors
        ocudu_fapi_precoding_codebook_tools)


set(SOURCES
        fapi_to_mac_error_indication_fastpath_translator.cpp
        fapi_to_mac_indications_fastpath_translator.cpp
        fapi_to_mac_slot_indication_fastpath_translator.cpp
)

add_library(ocudu_fapi_to_mac_p7_fastpath_translator STATIC ${SOURCES})
target_link_libraries(ocudu_fapi_to_mac_p7_fastpath_translator PRIVATE ocuduvec ocudu_support)

add_library(ocudu_mac_to_fapi_p7_sector_fastpath_adaptor STATIC mac_fapi_p7_sector_fastpath_adaptor_impl.cpp
        mac_fapi_p7_sector_fastpath_adaptor_factory_impl.cpp)
target_link_libraries(ocudu_mac_to_fapi_p7_sector_fastpath_adaptor PRIVATE ocudu_mac_to_fapi_p7_fastpath_translator
        ocudu_fapi_to_mac_p7_fastpath_translator)
