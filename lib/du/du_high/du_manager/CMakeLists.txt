# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

add_subdirectory(cbs)
add_subdirectory(converters)
add_subdirectory(du_ue)
add_subdirectory(procedures)
add_subdirectory(ran_resource_management)

set(SOURCES
        du_cell_manager.cpp
        du_manager_factory.cpp
        du_manager_impl.cpp
        du_manager_resources_factory.cpp
        du_positioning_handler_factory.cpp
        metrics/du_metrics_aggregator_impl.cpp
        du_positioning_manager_impl.cpp
        du_manager_controller_impl.cpp
)

add_library(ocudu_du_manager STATIC ${SOURCES})
target_link_libraries(ocudu_du_manager
        du_ue
        du_resource_manager
        du_manager_converters
        ocudu_du_config_validators
        mac_configuration_helpers
        ocudu_rlc
        ocudu_f1u_du
        ocudu_gtpu)
