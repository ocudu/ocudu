# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        du_low_appconfig_cli11_schema.cpp
        du_low_appconfig_validators.cpp
        du_low_appconfig_translators.cpp
        du_low_appconfig_yaml_writer.cpp)


add_library(odu_low_base STATIC ${SOURCES})
target_link_libraries(odu_low_base PRIVATE
        ocudu_app_helpers
        ocudu_app_services
        ocudu_support
        ocudu_yaml_util
        ocudu_versioning)

target_include_directories(odu_low_base PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/external)

if (DPDK_FOUND)
    add_definitions(-DDPDK_FOUND)
    target_link_libraries(odu_low_base PRIVATE hal_dpdk)
endif (DPDK_FOUND)


add_executable(odu_low du_low.cpp)
add_backward(odu_low)
target_include_directories(odu_low PRIVATE ${CMAKE_SOURCE_DIR})
install(TARGETS odu_low RUNTIME)
target_link_libraries(odu_low PRIVATE odu_low_base ocudu_flexible_o_du_low_split_6)
notify_binary_target(odu_low)
