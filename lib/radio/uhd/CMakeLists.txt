# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set(SOURCES_UHD
        radio_config_uhd_validator.cpp
        radio_uhd_impl.cpp
        radio_uhd_rx_stream.cpp
        radio_uhd_tx_stream.cpp)

add_library(ocudu_radio_uhd STATIC ${SOURCES_UHD})

target_link_libraries(ocudu_radio_uhd ${UHD_LIBRARIES} fmt ocudulog ocudu_support)

add_to_exported_libs(ocudu_radio_uhd)
