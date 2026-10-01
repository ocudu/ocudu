# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set(SOURCES_ZMQ
        radio_config_zmq_validator.cpp
        radio_factory_zmq_impl.cpp
        radio_session_zmq_impl.cpp
        radio_zmq_rx_channel.cpp
        radio_zmq_rx_stream.cpp
        radio_zmq_tx_channel.cpp
        radio_zmq_tx_stream.cpp)
add_library(ocudu_radio_zmq STATIC ${SOURCES_ZMQ})

target_link_libraries(ocudu_radio_zmq ${ZEROMQ_LIBRARIES} fmt ocudulog ocuduvec ocudu_support)

add_to_exported_libs(ocudu_radio_zmq)
