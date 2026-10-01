# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


include_directories(../../..)

add_library(nrppa_test_doubles nrppa_test_message_validators.cpp nrppa_test_messages.cpp)
set_target_properties(nrppa_test_doubles PROPERTIES UNITY_BUILD ON)
target_link_libraries(nrppa_test_doubles ocudu_nrppa ocudu_support ocudulog nrppa_asn1)
