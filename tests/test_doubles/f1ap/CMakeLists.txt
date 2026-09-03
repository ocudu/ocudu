# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(f1ap_test_doubles f1ap_test_message_validators.cpp f1ap_test_messages.cpp)
set_target_properties(f1ap_test_doubles PROPERTIES UNITY_BUILD ON)
target_include_directories(f1ap_test_doubles PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(f1ap_test_doubles ocudu_support ocudulog f1ap_asn1 ocudu_f1ap_common rrc_test_doubles pdcp_test_doubles test_rng)
