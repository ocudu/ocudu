# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(e1ap_test_doubles e1ap_cu_cp_test_messages.cpp e1ap_cu_up_test_messages.cpp
            e1ap_test_message_validators.cpp)
set_target_properties(e1ap_test_doubles PROPERTIES UNITY_BUILD ON)
target_include_directories(e1ap_test_doubles PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(e1ap_test_doubles ocudu_support ocudulog ocudu_e1ap_common e1ap_asn1)
