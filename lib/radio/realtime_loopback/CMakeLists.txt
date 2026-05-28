# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

set(SOURCES_REALTIME_LOOPBACK
        loopback_buffer.cpp
        radio_config_realtime_loopback_validator.cpp
        radio_factory_realtime_loopback_impl.cpp
        radio_session_realtime_loopback_impl.cpp)

add_library(ocudu_radio_realtime_loopback STATIC ${SOURCES_REALTIME_LOOPBACK})
target_link_libraries(ocudu_radio_realtime_loopback fmt ocudulog ocudu_support)
add_to_exported_libs(ocudu_radio_realtime_loopback)
