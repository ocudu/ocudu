# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

set(SOURCES
        adapters/f1c_test_mode_adapter.cpp
        du_appconfig_cli11_schema.cpp
        du_appconfig_validators.cpp
        du_appconfig_translators.cpp
        du_appconfig_yaml_writer.cpp)


add_library(odu_base STATIC ${SOURCES})
target_link_libraries(odu_base PRIVATE
        ocudu_app_helpers
        ocudu_config_app_helpers
        ocudu_app_services
        ocudu_f1u_du_split_connector
        ocudu_cu_cp
        ocudu_e2
        e2ap_asn1
        ocudu_pcap
        ocudu_support
        ocudu_yaml_util
        ocudu_versioning
        ocudu_f1c_gateway)

if (DPDK_FOUND)
    add_definitions(-DDPDK_FOUND)
    target_link_libraries(odu_base PRIVATE hal_dpdk)
endif (DPDK_FOUND)

target_include_directories(odu_base PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/external)

# Default DU target with dynamic split.
add_executable(odu du.cpp)
add_backward(odu)
target_include_directories(odu PRIVATE ${CMAKE_SOURCE_DIR})
install(TARGETS odu RUNTIME)
target_link_libraries(odu PRIVATE odu_base ocudu_flexible_o_du_split_dynamic)
notify_binary_target(odu)


# Add the targets for the rest of the splits.
set(EXTENSIONS du_split_6 du_split_7_2 du_split_8)
set(LIBRARIES ocudu_flexible_o_du_split_6 ocudu_flexible_o_du_split_7_2 ocudu_flexible_o_du_split_8)

foreach (NAME LIB IN ZIP_LISTS EXTENSIONS LIBRARIES)
    # Prepend "o" to the target name to make it consistent with the dynamic one.
    set(TARGET "o${NAME}")
    add_executable(${TARGET} du.cpp)
    add_backward(${TARGET})
    target_include_directories(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR})
    target_link_libraries(${TARGET} PRIVATE odu_base ${LIB})
    set_target_properties(${TARGET} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../${NAME}"  # Path to the binary.
            RUNTIME_OUTPUT_NAME "odu"                                        # Binary name.
            EXCLUDE_FROM_ALL 1)                                                # Do not build by default.
    notify_binary_target(${TARGET} FILENAME "${CMAKE_INSTALL_BINDIR}/${TARGET}")
endforeach ()
