# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

add_executable(sched_config_test csi_helper_test.cpp pucch_resource_generator_test.cpp rlm_helper_test.cpp scheduler_cell_config_validator_test.cpp
              periodic_resource_sched_validator_test.cpp cg_resource_helpers_test.cpp)
target_link_libraries(sched_config_test sched_config sched_test_doubles ocudulog gtest ocudu_gtest_main)
gtest_discover_tests(sched_config_test)
