# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "phy")

if (ZEROMQ_FOUND OR UHD_FOUND)

    add_executable(radio_ssb radio_ssb.cpp lower_phy_example_factory.cpp upper_phy_ssb_example.cpp)
    target_link_libraries(radio_ssb
            ocudu_radio
            ocudulog
            ocudu_support
            ocudu_phy_support
            ocudu_upper_phy_support
            ocudu_lower_phy
            ocudu_signal_processors
            ocudu_channel_processors
            ocudu_channel_precoder
            )
    # Add lower PHY sample loopback test with ZeroMQ only if ZeroMQ is available.
    if (ZEROMQ_FOUND AND BUILD_TESTING)
        add_test(radio_ssb_zmq_20MHz_n7 radio_ssb -P zmq_20MHz_n7 -D 10 -L 1 -v warning -du)
        add_test(radio_ssb_zmq_20MHz_n78 radio_ssb -P zmq_20MHz_n78 -D 10 -L 1 -v warning -du)
    endif (ZEROMQ_FOUND AND BUILD_TESTING)

endif (ZEROMQ_FOUND OR UHD_FOUND)
