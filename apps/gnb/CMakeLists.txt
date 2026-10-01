# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        gnb_appconfig_cli11_schema.cpp
        gnb_appconfig_validators.cpp
        gnb_appconfig_translators.cpp
        gnb_appconfig_yaml_writer.cpp)

add_library(gnb_base STATIC ${SOURCES})
target_include_directories(gnb_base PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(gnb_base PRIVATE
        ocudu_app_services
        ocudu_config_app_helpers
        ocudu_f1u_connector
        ocudu_ngap
        ngap_asn1
        ocudu_e2
        e2ap_asn1
        ocudu_gateway
        ocudu_pcap
        ocudu_support
        ocudu_yaml_util
        ocudu_versioning
        ocudu_f1c_gateway
        ocudu_xnc_gateway
        ocudu_e1_gateway
        ocudu_o_cu_cp_app_unit
        ocudu_o_cu_up_app_unit
)

if (DPDK_FOUND)
    add_definitions(-DDPDK_FOUND)
    target_link_libraries(gnb_base PRIVATE hal_dpdk)
endif (DPDK_FOUND)

target_include_directories(gnb_base PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/external)

# Default gNB target with dynamic split.
add_executable(gnb gnb.cpp)
add_backward(gnb)
target_include_directories(gnb PRIVATE ${CMAKE_SOURCE_DIR})
install(TARGETS gnb RUNTIME)
target_link_libraries(gnb PRIVATE gnb_base ocudu_flexible_o_du_split_dynamic)
notify_binary_target(gnb)

# Add the targets for the rest of the splits.
set(EXTENSIONS gnb_split_6 gnb_split_7_2 gnb_split_8)
set(LIBRARIES ocudu_flexible_o_du_split_6 ocudu_flexible_o_du_split_7_2 ocudu_flexible_o_du_split_8)

foreach (NAME LIB IN ZIP_LISTS EXTENSIONS LIBRARIES)
    add_executable(${NAME} gnb.cpp)
    add_backward(${NAME})
    target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR})
    target_link_libraries(${NAME} PRIVATE gnb_base ${LIB})
    set_target_properties(${NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../${NAME}"   # Path to the binary.
            RUNTIME_OUTPUT_NAME "gnb"                                           # Binary name.
            EXCLUDE_FROM_ALL 1)                                                 # Do not build by default.
    notify_binary_target(${NAME} FILENAME "${CMAKE_INSTALL_BINDIR}/${NAME}")
endforeach ()
