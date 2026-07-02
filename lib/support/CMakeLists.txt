# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


add_subdirectory(versioning)
add_subdirectory(math)

add_library(ocudu_yaml_util STATIC config_yaml.cpp)
target_link_libraries(ocudu_yaml_util PRIVATE ocudu::yaml-cpp)

set(SOURCES
        executors/metrics/executor_metrics_backend.cpp
        executors/metrics/executor_metrics_channel.cpp
        executors/priority_task_queue.cpp
        executors/priority_task_worker.cpp
        executors/task_execution_manager.cpp
        executors/task_worker.cpp
        executors/task_worker_pool.cpp
        executors/unique_thread.cpp
        executors/execution_context_description.cpp
        executors/executor_decoration_factory.cpp
        network/io_broker_epoll.cpp
        network/io_broker_factory.cpp
        network/io_timer_source.cpp
        network/sockets.cpp
        network/transport_layer_address.cpp
        synchronization/futex_util.cpp
        tracing/resource_usage.cpp
        tracing/event_tracing.cpp
        backtrace.cpp
        bit_encoding.cpp
        byte_buffer.cpp
        byte_buffer_chain.cpp
        cpu_architecture_info.cpp
        resource_usage/perf_event_powercap_reader_impl.cpp
        resource_usage/powercap_energy_reader_impl.cpp
        resource_usage/rapl_msr_energy_reader_impl.cpp
        resource_usage/power_consumption.cpp
        resource_usage/resource_usage_utils.cpp
        rate_limiting/token_bucket.cpp
        rate_limiting/lockfree_token_bucket.cpp
        signal_handling.cpp
        sysinfo.cpp
        timers.cpp
)

add_library(ocudu_support STATIC ${SOURCES})
target_link_libraries(ocudu_support
        PRIVATE ${CMAKE_THREAD_LIBS_INIT} ${NUMA_LIBRARIES} ocudulog
        INTERFACE ocudu_support_math)
add_backward(ocudu_support)

if (Backward_FOUND AND BACKWARD_HAS_EXTERNAL_LIBRARIES)
    set_source_files_properties(backtrace.cpp APPEND PROPERTIES COMPILE_DEFINITIONS "HAVE_BACKWARD")
    set_source_files_properties(backtrace.cpp APPEND PROPERTIES INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/external)
endif (Backward_FOUND AND BACKWARD_HAS_EXTERNAL_LIBRARIES)

add_to_exported_libs(ocudu_support)
