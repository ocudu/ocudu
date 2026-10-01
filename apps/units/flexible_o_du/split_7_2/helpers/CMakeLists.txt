# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


if (DPDK_FOUND)
    add_definitions(-DDPDK_FOUND)
endif (DPDK_FOUND)

set(SOURCES
        ru_ofh_config_cli11_schema.cpp
        ru_ofh_factories.cpp
        ru_ofh_config_translator.cpp
        ru_ofh_config_validator.cpp
        ru_ofh_config_yaml_writer.cpp)

add_library(ocudu_split_7_2_app_unit_helpers STATIC ${SOURCES})
target_link_libraries(ocudu_split_7_2_app_unit_helpers PRIVATE ocudu_ru_ofh ocudu_cpu_affinities_helper ocudu_app_helpers)
target_include_directories(ocudu_split_7_2_app_unit_helpers PRIVATE ${CMAKE_SOURCE_DIR})
