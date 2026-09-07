# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

# Directory where the FlatBuffers compiler drops the generated headers.
set(FBS_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/fbs)
add_subdirectory(fbs)

add_library(schedtrace STATIC
        cell_event_tracer.cpp
        scheduler_event_tracer.cpp
        event_converter.cpp
        event_trace_writer_registry.cpp)
target_link_libraries(schedtrace ocudu_ran ocudu_support schedtrace_fbs)

# Decode side of the tracer, used by the schedlog utility. Kept in its own library, as it depends on
# scheduler_logger to render the decoded events and scheduler_logger already depends on schedtrace.
add_library(trace_to_log STATIC trace_to_log.cpp)
target_link_libraries(trace_to_log PRIVATE schedtrace schedtrace_fbs scheduler_logger)

# This library provides the real cell_event_tracer implementation. Define OCUDU_HAS_SCHEDTRACE on
# scheduler_logger so that its own cell_event_tracer.cpp compiles out the stub one it falls back to.
target_compile_definitions(scheduler_logger PRIVATE -DOCUDU_HAS_SCHEDTRACE)
target_link_libraries(scheduler_logger schedtrace)
