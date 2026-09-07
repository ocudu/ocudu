# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

add_executable(sched_event_tracer_test
               cell_event_tracer_test.cpp
               event_builder_pool_test.cpp
               event_converter_test.cpp
               event_trace_writer_registry_test.cpp
               sched_event_tracer_file_test.cpp)
target_link_libraries(sched_event_tracer_test schedtrace schedtrace_fbs scheduler_test_utils ocudu_support ocudulog gtest gtest_main)
gtest_discover_tests(sched_event_tracer_test)

add_executable(trace_to_log_test trace_to_log_test.cpp)
target_link_libraries(trace_to_log_test schedtrace schedtrace_fbs trace_to_log sched_config ocudu_support ocudulog gtest gtest_main)
gtest_discover_tests(trace_to_log_test)

add_subdirectory(roundtrip)
