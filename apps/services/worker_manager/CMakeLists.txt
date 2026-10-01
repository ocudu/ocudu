# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        worker_manager.cpp
        worker_manager_cli11_schema.cpp
        worker_manager_appconfig_validator.cpp)

add_library(ocudu_cpu_affinities_helper STATIC cli11_cpu_affinities_parser_helper.cpp)

add_library(ocudu_worker_manager_app_service STATIC ${SOURCES})
target_include_directories(ocudu_worker_manager_app_service PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ocudu_worker_manager_app_service
        ocudu_cpu_affinities_helper
        ocudu_cu_up      # TODO: Temp
        ocudu_du_high    # TODO: Temp
        ocudu_cu_cp      # TODO: Temp
        ocudu_o_du_low   # TODO: Temp
        ocudu_ru_dummy   # TODO: Temp
        ocudu_ru_sdr     # TODO: Temp
        ocudu_ru_ofh)    # TODO: Temp
