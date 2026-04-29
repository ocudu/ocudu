# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


add_library(ocudu_radio STATIC
        plugin_radio_factory.cpp
        radio_factory.cpp)

set(OCUDU_RADIO_DEFINITIONS "")
set(OCUDU_RADIO_LIBRARIES "")

add_subdirectory(realtime_dummy)
list(APPEND OCUDU_RADIO_LIBRARIES ocudu_radio_realtime_dummy)

if (UHD_FOUND AND ENABLE_UHD)
    # The UHD library uses RTTI
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frtti")
    add_subdirectory(uhd)
    list(APPEND OCUDU_RADIO_DEFINITIONS -DENABLE_UHD)
    list(APPEND OCUDU_RADIO_LIBRARIES ocudu_radio_uhd)
endif (UHD_FOUND AND ENABLE_UHD)

if (ZEROMQ_FOUND AND ENABLE_ZEROMQ)
    add_subdirectory(zmq)
    list(APPEND OCUDU_RADIO_DEFINITIONS -DENABLE_ZMQ)
    list(APPEND OCUDU_RADIO_LIBRARIES ocudu_radio_zmq)
endif (ZEROMQ_FOUND AND ENABLE_ZEROMQ)

add_definitions(${OCUDU_RADIO_DEFINITIONS})
target_link_libraries(ocudu_radio ${OCUDU_RADIO_LIBRARIES} ocudulog dl)

add_to_exported_libs(ocudu_radio)
