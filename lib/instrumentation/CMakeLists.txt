# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set(SOURCES
        traces/critical_traces.cpp
        traces/du_traces.cpp
        traces/ru_traces.cpp
        traces/up_traces.cpp
        traces/ofh_traces.cpp)

set(LIBS ocudu_support)

if(ENABLE_OCUDU_TRACY)
	list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/external/tracy/public/TracyClient.cpp)
	list(APPEND LIBS ${TRACY_LIBRARIES})
endif()

add_library(ocudu_instrumentation ${SOURCES})

if(ENABLE_OCUDU_TRACY)
  target_include_directories(ocudu_instrumentation PUBLIC ${CMAKE_SOURCE_DIR}/external/tracy/public/)
endif()

target_link_libraries(ocudu_instrumentation ${LIBS})

add_to_exported_libs(ocudu_instrumentation)
