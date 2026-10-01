# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


add_executable(radio_zmq_loopback_test radio_zmq_loopback_test.cpp)
target_link_libraries(radio_zmq_loopback_test ocudu_radio ocudu_support GTest::gtest GTest::gtest_main)
add_test(radio_zmq_loopback_test radio_zmq_loopback_test)

add_executable(radio_zmq_validator_test radio_zmq_validator_test.cpp)
target_link_libraries(radio_zmq_validator_test ocudu_radio ocudu_support GTest::gtest GTest::gtest_main)
add_test(radio_zmq_validator_test radio_zmq_validator_test)
