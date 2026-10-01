# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(pdcp_test_doubles pdcp_pdu_generator.cpp)
set_target_properties(pdcp_test_doubles PROPERTIES UNITY_BUILD ON)
target_link_libraries(pdcp_test_doubles ocudu_support ocudulog)
