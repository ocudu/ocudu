# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_subdirectory(du_manager)
add_subdirectory(test_mode)

add_library(ocudu_du_high STATIC
        du_high_impl.cpp
        du_high_factory.cpp
        du_high_clock_controller_factory.cpp
        du_high_executor_mapper.cpp
        du_high_ntn_sib19_update_handler_impl.cpp
        du_high_ntn_sib19_update_handler_factory.cpp
        du_high_ntn_time_provider_impl.cpp
        du_high_ntn_time_provider_factory.cpp)
target_link_libraries(ocudu_du_high PUBLIC ocudu_du_high_adapters)

add_library(ocudu_o_du_high STATIC
        o_du_high_factory.cpp
        o_du_high_impl.cpp
        o_du_high_metrics_notifier_proxy.cpp)
target_link_libraries(ocudu_o_du_high PUBLIC ocudu_du_config_validators
        ocudu_du_high
        ocudu_mac_fapi_fastpath_adaptor
        ocudu_e2)
