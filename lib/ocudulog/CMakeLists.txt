# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set(SOURCES
        backend_worker.cpp
        ocudulog.cpp
        ocudulog_c.cpp
        event_trace.cpp)

include_directories(${PROJECT_SOURCE_DIR}/include/ocudu/ocudulog/bundled/)
include_directories(${PROJECT_SOURCE_DIR}/include/ocudu/ocudulog/formatters)

set(SOURCES
        ${SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/formatters/json_formatter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/formatters/text_formatter.cpp)


add_library(ocudulog STATIC ${SOURCES})
target_link_libraries(ocudulog fmt ${CMAKE_THREAD_LIBS_INIT})

add_to_exported_libs(ocudulog)
