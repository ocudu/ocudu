# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "sched|rtsan")

include_directories(../../..)

add_executable(scheduler_no_ues_benchmark scheduler_no_ues_benchmark.cpp)
target_link_libraries(scheduler_no_ues_benchmark ocudu_sched scheduler_test_utils ocudulog sched_config)
add_test(scheduler_no_ues_benchmark scheduler_no_ues_benchmark)

add_executable(scheduler_multi_ue_benchmark scheduler_multi_ue_benchmark.cpp)
target_link_libraries(scheduler_multi_ue_benchmark ocudu_sched sched_test_doubles sched_rrm ocudulog sched_config)
add_test(scheduler_multi_ue_benchmark scheduler_multi_ue_benchmark)
