# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        csi_rs.cpp
        pdcch.cpp
        pdsch.cpp
        prach.cpp
        prs.cpp
        pucch.cpp
        pusch.cpp
        srs.cpp
        ssb.cpp)

add_library(ocudu_mac_fapi_adaptors STATIC ${SOURCES})
target_link_libraries(ocudu_mac_fapi_adaptors ocudu_fapi_precoding_codebook_tools ocudu_fapi_uci_part2_tools ocudulog)
