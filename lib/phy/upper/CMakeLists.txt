# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_subdirectory(channel_coding)
add_subdirectory(channel_modulation)
add_subdirectory(channel_processors)
add_subdirectory(equalization)
add_subdirectory(metrics)
add_subdirectory(sequence_generators)
add_subdirectory(signal_processors)

add_library(ocudu_downlink_processor
        downlink_processor_multi_executor_impl.cpp
        downlink_processor_pool_impl.cpp)
target_link_libraries(ocudu_downlink_processor
        ocudu_instrumentation
        ocudu_channel_processors)

add_library(ocudu_uplink_processor
        uplink_processor_impl.cpp
        uplink_processor_pool_impl.cpp
        uplink_request_processor_impl.cpp)
target_link_libraries(ocudu_uplink_processor ocudu_channel_processors)

add_library(ocudu_upper_phy
        upper_phy_error_handler_impl.cpp
        upper_phy_factories.cpp
        upper_phy_impl.cpp
        upper_phy_operation_controller_impl.cpp
        upper_phy_rx_results_notifier_wrapper.cpp
        upper_phy_rx_symbol_handler_impl.cpp)
target_link_libraries(ocudu_upper_phy
        ocudu_channel_coding
        ocudu_channel_equalizer
        ocudu_channel_precoder
        ocudu_channel_processors
        ocudu_downlink_processor
        ocudu_signal_processors
        ocudu_dft
        ocudu_instrumentation
        ocudu_phy_metrics
        ocudu_phy_support
        ocudu_transform_precoding
        ocudu_uplink_processor
        ocudu_upper_phy_metrics)

add_library(log_likelihood_ratio log_likelihood_ratio.cpp)
target_link_libraries(log_likelihood_ratio ocuduvec)

add_library(ocudu_upper_phy_support
        rx_buffer_pool_impl.cpp
)

add_to_exported_libs(log_likelihood_ratio ocudu_upper_phy_support)
