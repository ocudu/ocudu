# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

set(SOURCES_DIFI
        radio_difi_context_packet.cpp
        radio_difi_data_packet.cpp)

add_library(ocudu_radio_difi STATIC ${SOURCES_DIFI})

target_link_libraries(ocudu_radio_difi fmt ocudulog ocudu_support)

add_to_exported_libs(ocudu_radio_difi)
