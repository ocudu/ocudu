# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        lower_phy/lower_phy_sector.cpp
        lower_phy/lower_phy_sector_metrics_collector.cpp
        lower_phy/lower_phy_sector_timing_handler.cpp
        lower_phy/lower_phy_factory.cpp
        lower_phy/lower_phy_sector.cpp
        ru_controller_sdr_impl.cpp
        ru_factory_sdr_impl.cpp
        ru_lower_phy_downlink_handler_impl.cpp
        ru_lower_phy_uplink_request_handler_impl.cpp
        ru_metrics_collector_sdr_impl.cpp
        ru_radio_metrics_collector.cpp
        ru_sdr_executor_mapper.cpp
        ru_sdr_impl.cpp)

add_library(ocudu_ru_sdr STATIC ${SOURCES})
target_link_libraries(ocudu_ru_sdr ocudu_lower_phy ocudu_radio ocudulog)
