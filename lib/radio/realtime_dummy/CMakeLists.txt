# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

set(SOURCES_DUMMY_REALTIME
        radio_config_realtime_dummy_validator.cpp
        radio_factory_realtime_dummy_impl.cpp
        radio_session_realtime_dummy_impl.cpp)

add_library(ocudu_radio_realtime_dummy STATIC ${SOURCES_DUMMY_REALTIME})
target_link_libraries(ocudu_radio_realtime_dummy fmt ocudulog ocudu_support)
add_to_exported_libs(ocudu_radio_realtime_dummy)
