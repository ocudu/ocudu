# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


add_executable(ocudulog_backend_throughput benchmarks/backend_throughput.cpp)
target_include_directories(ocudulog_backend_throughput PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ocudulog_backend_throughput ocudulog)

add_executable(ocudulog_frontend_latency benchmarks/frontend_latency.cpp)
target_include_directories(ocudulog_frontend_latency PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ocudulog_frontend_latency ocudulog)

add_executable(ocudulog_test ocudulog_test.cpp)
target_include_directories(ocudulog_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ocudulog_test ocudulog)
add_test(ocudulog_test ocudulog_test)

add_executable(log_channel_test log_channel_test.cpp)
target_include_directories(log_channel_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(log_channel_test ocudulog)
add_test(log_channel_test log_channel_test)

add_executable(log_backend_test log_backend_test.cpp)
target_include_directories(log_backend_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(log_backend_test ocudulog)
add_test(log_backend_test log_backend_test)

add_executable(logger_test logger_test.cpp)
target_include_directories(logger_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(logger_test ocudulog)
add_test(logger_test logger_test)

add_executable(any_test any_test.cpp)
target_include_directories(any_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(any_test ocudulog)
add_test(any_test any_test)

add_executable(file_sink_test file_sink_test.cpp)
target_include_directories(file_sink_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(file_sink_test ocudulog)
add_test(file_sink_test file_sink_test)

add_executable(syslog_sink_test syslog_sink_test.cpp)
target_include_directories(syslog_sink_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(syslog_sink_test ocudulog)

add_executable(file_utils_test file_utils_test.cpp)
target_include_directories(file_utils_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(file_utils_test ocudulog)
add_test(file_utils_test file_utils_test)

add_executable(tracer_test event_trace_test.cpp)
target_include_directories(tracer_test PRIVATE ${CMAKE_SOURCE_DIR})
add_definitions(-DENABLE_OCUDULOG_EVENT_TRACE)
target_link_libraries(tracer_test ocudulog)
add_test(tracer_test tracer_test)

add_executable(text_formatter_test text_formatter_test.cpp)
target_include_directories(text_formatter_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(text_formatter_test ocudulog)
add_test(text_formatter_test text_formatter_test)

add_executable(json_formatter_test json_formatter_test.cpp)
target_include_directories(json_formatter_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(json_formatter_test ocudulog)
add_test(json_formatter_test json_formatter_test)

add_executable(context_test context_test.cpp)
target_include_directories(context_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(context_test ocudulog)
add_test(context_test context_test)
