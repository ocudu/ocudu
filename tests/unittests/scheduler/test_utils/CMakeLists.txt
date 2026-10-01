# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(scheduler_test_utils
        config_generators.cpp
        indication_generators.cpp)
target_link_libraries(scheduler_test_utils ocudulog mac_configuration_helpers sched_config sched_test_doubles)

add_library(scheduler_test_suite
        scheduler_test_suite.cpp
        scheduler_output_test_helpers.cpp
        scheduler_test_simulator.cpp)
target_link_libraries(scheduler_test_suite ocudu_sched scheduler_test_utils gtest)
