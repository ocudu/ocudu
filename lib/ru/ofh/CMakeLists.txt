# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        ru_ofh_config_validator.cpp
        ru_ofh_controller_impl.cpp
        ru_ofh_downlink_plane_handler_proxy.cpp
        ru_ofh_error_handler_impl.cpp
        ru_ofh_executor_mapper.cpp
        ru_ofh_factory.cpp
        ru_ofh_impl.cpp
        ru_ofh_metrics_collector_impl.cpp
        ru_ofh_rx_symbol_handler_impl.cpp
        ru_ofh_timing_notifier_impl.cpp
        ru_ofh_uplink_plane_handler_proxy.cpp)

add_library(ocudu_ru_ofh STATIC ${SOURCES})
target_link_libraries(ocudu_ru_ofh ocudu_ofh ocudu_ofh_compression ocudu_ofh_ethernet ocudu_ofh_ecpri)
