# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


include_directories(../../..)

add_library(rrc_test_doubles rrc_test_messages.cpp rrc_test_message_validators.cpp rrc_packed_test_messages.cpp)
set_target_properties(rrc_test_doubles PROPERTIES UNITY_BUILD ON)
target_link_libraries(rrc_test_doubles rrc_nr_asn1 ocudu_rrc ocudu_support ocudulog)
