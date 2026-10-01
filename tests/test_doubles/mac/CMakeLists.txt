# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(mac_test_doubles dummy_mac_result_notifier.cpp mac_test_messages.cpp)
target_include_directories(mac_test_doubles PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(mac_test_doubles ocudu_support ocudulog test_rng)
