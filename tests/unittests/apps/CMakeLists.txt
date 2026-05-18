# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


add_subdirectory(units)

# Schema-agnostic YAML round-trip tests for application configurations. See yaml_roundtrip_test_helpers.h
# for what they verify.

set(YAML_ROUNDTRIP_LINKS
        ocudu_support
        ocudu_yaml_util
        GTest::gtest
        GTest::gtest_main
        ocudu::yaml-cpp)

set(YAML_ROUNDTRIP_INCLUDES ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/external)

# gNB
add_executable(yaml_roundtrip_gnb_test yaml_roundtrip_gnb_test.cpp)
target_include_directories(yaml_roundtrip_gnb_test PRIVATE ${YAML_ROUNDTRIP_INCLUDES})
target_compile_definitions(yaml_roundtrip_gnb_test PRIVATE CONFIGS_DIR="${CMAKE_SOURCE_DIR}/configs")
target_link_libraries(yaml_roundtrip_gnb_test PRIVATE
        gnb_base
        ocudu_o_cu_cp_app_unit
        ocudu_o_cu_up_app_unit
        ocudu_flexible_o_du_split_dynamic
        ${YAML_ROUNDTRIP_LINKS})
gtest_discover_tests(yaml_roundtrip_gnb_test)

# DU
add_executable(yaml_roundtrip_du_test yaml_roundtrip_du_test.cpp)
target_include_directories(yaml_roundtrip_du_test PRIVATE ${YAML_ROUNDTRIP_INCLUDES})
target_compile_definitions(yaml_roundtrip_du_test PRIVATE CONFIGS_DIR="${CMAKE_SOURCE_DIR}/configs")
target_link_libraries(yaml_roundtrip_du_test PRIVATE
        odu_base
        ocudu_flexible_o_du_split_dynamic
        ${YAML_ROUNDTRIP_LINKS})
gtest_discover_tests(yaml_roundtrip_du_test)

# CU. The cu app does not have a base static library, so we compile its appconfig sources directly into the test target.
add_executable(yaml_roundtrip_cu_test
        yaml_roundtrip_cu_test.cpp
        ${CMAKE_SOURCE_DIR}/apps/cu/cu_appconfig_cli11_schema.cpp
        ${CMAKE_SOURCE_DIR}/apps/cu/cu_appconfig_validator.cpp
        ${CMAKE_SOURCE_DIR}/apps/cu/cu_appconfig_yaml_writer.cpp)
target_include_directories(yaml_roundtrip_cu_test PRIVATE ${YAML_ROUNDTRIP_INCLUDES})
target_compile_definitions(yaml_roundtrip_cu_test PRIVATE CONFIGS_DIR="${CMAKE_SOURCE_DIR}/configs")
target_link_libraries(yaml_roundtrip_cu_test PRIVATE
        ocudu_app_helpers
        ocudu_app_services
        ocudu_e1_gateway
        ocudu_e2
        ocudu_f1c_gateway
        ocudu_xnc_gateway
        ocudu_f1u_cu_up_split_connector
        ocudu_ngap
        ocudu_o_cu_cp_app_unit
        ocudu_o_cu_up_app_unit
        ocudu_pcap
        ocudu_versioning
        ${YAML_ROUNDTRIP_LINKS})
gtest_discover_tests(yaml_roundtrip_cu_test)


# CU-CP.  Same approach as CU.
add_executable(yaml_roundtrip_cu_cp_test
        yaml_roundtrip_cu_cp_test.cpp
        ${CMAKE_SOURCE_DIR}/apps/cu_cp/cu_cp_appconfig_cli11_schema.cpp
        ${CMAKE_SOURCE_DIR}/apps/cu_cp/cu_cp_appconfig_yaml_writer.cpp)
target_include_directories(yaml_roundtrip_cu_cp_test PRIVATE ${YAML_ROUNDTRIP_INCLUDES})
target_compile_definitions(yaml_roundtrip_cu_cp_test PRIVATE CONFIGS_DIR="${CMAKE_SOURCE_DIR}/configs")
target_link_libraries(yaml_roundtrip_cu_cp_test PRIVATE
        ocudu_app_helpers
        ocudu_app_services
        ocudu_e1_gateway
        ocudu_e2
        ocudu_f1c_gateway
        ocudu_xnc_gateway
        ocudu_ngap
        ocudu_o_cu_cp_app_unit
        ocudu_pcap
        ocudu_versioning
        ${YAML_ROUNDTRIP_LINKS})
gtest_discover_tests(yaml_roundtrip_cu_cp_test)

# CU-UP. Same approach as CU.
add_executable(yaml_roundtrip_cu_up_test
        yaml_roundtrip_cu_up_test.cpp
        ${CMAKE_SOURCE_DIR}/apps/cu_up/cu_up_appconfig_cli11_schema.cpp
        ${CMAKE_SOURCE_DIR}/apps/cu_up/cu_up_appconfig_validator.cpp
        ${CMAKE_SOURCE_DIR}/apps/cu_up/cu_up_appconfig_yaml_writer.cpp)
target_include_directories(yaml_roundtrip_cu_up_test PRIVATE ${YAML_ROUNDTRIP_INCLUDES})
target_compile_definitions(yaml_roundtrip_cu_up_test PRIVATE CONFIGS_DIR="${CMAKE_SOURCE_DIR}/configs")
target_link_libraries(yaml_roundtrip_cu_up_test PRIVATE
        ocudu_app_helpers
        ocudu_app_services
        ocudu_e1_gateway
        ocudu_e2
        ocudu_f1u_cu_up_split_connector
        ocudu_o_cu_up_app_unit
        ocudu_pcap
        ocudu_versioning
        ${YAML_ROUNDTRIP_LINKS})
gtest_discover_tests(yaml_roundtrip_cu_up_test)
