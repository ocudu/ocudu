# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

set_directory_properties(PROPERTIES LABELS "du_manager;du_high")

include_directories(../../..)

add_library(du_manager_test_helpers du_manager_test_helpers.cpp procedures/du_manager_procedure_test_helpers.cpp)
target_include_directories(du_manager_test_helpers PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(du_manager_test_helpers ocudu_du_manager ocudu_support ocudulog)

add_executable(du_bearer_test du_ue/du_bearer_test.cpp)
target_link_libraries(du_bearer_test du_manager_test_helpers ocudu_du_manager ocudu_support ocudulog gtest ocudu_gtest_main)
gtest_discover_tests(du_bearer_test)

add_executable(ue_manager_test du_ue/ue_manager_test.cpp)
target_link_libraries(ue_manager_test
        du_manager_test_helpers
        ocudu_du_manager
        ocudu_support
        ocudulog
        gtest
        ocudu_gtest_main)
target_include_directories(ue_manager_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(ue_manager_test)


add_executable(sib_test sib_test.cpp)
target_link_libraries(sib_test
        du_manager_test_helpers
        ocudu_du_manager
        ocudu_support
        ocudulog
        gtest
        ocudu_gtest_main)
target_include_directories(sib_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(sib_test)

add_executable(serving_cell_config_converter_test ../du_manager/serving_cell_config_converter_test.cpp)
target_link_libraries(serving_cell_config_converter_test ocudu_du_manager ocudu_support asn1_utils rrc_nr_asn1 ocudulog gtest ocudu_gtest_main)
gtest_discover_tests(serving_cell_config_converter_test)

add_executable(mac_cell_group_config_converter_test ../du_manager/mac_cell_group_config_converter_test.cpp)
target_link_libraries(mac_cell_group_config_converter_test ocudu_du_manager ocudu_support asn1_utils rrc_nr_asn1 ocudulog gtest ocudu_gtest_main)
gtest_discover_tests(mac_cell_group_config_converter_test)

add_executable(du_manager_ue_creation_test procedures/ue_creation_test.cpp)
target_include_directories(du_manager_ue_creation_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(du_manager_ue_creation_test du_manager_test_helpers gtest ocudu_gtest_main)
gtest_discover_tests(du_manager_ue_creation_test)

add_executable(du_ue_config_test
        procedures/ue_configuration_test.cpp
        procedures/ue_deletion_test.cpp
        procedures/du_ue_ric_configuration_procedure_test.cpp)
target_include_directories(du_ue_config_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(du_ue_config_test du_manager_test_helpers rrc_test_doubles pdcp_test_doubles gtest ocudu_gtest_main)
gtest_discover_tests(du_ue_config_test)

add_executable(du_manager_procedure_test
        procedures/du_param_config_procedure_test.cpp
        procedures/du_pws_broadcast_procedure_test.cpp
)
target_include_directories(du_manager_procedure_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(du_manager_procedure_test du_manager_test_helpers gtest ocudu_gtest_main)
gtest_discover_tests(du_manager_procedure_test)

add_executable(du_ran_resource_manager_test
        du_ran_resource_manager_test.cpp
        du_drx_resource_manager_test.cpp
        decode_ue_nr_cap_container_test.cpp
        du_meas_config_manager_test.cpp)
target_include_directories(du_ran_resource_manager_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(du_ran_resource_manager_test du_manager_test_helpers sched_test_doubles gtest ocudu_gtest_main)
gtest_discover_tests(du_ran_resource_manager_test)

add_executable(srs_resource_generator_test srs_resource_generator_test.cpp)
target_include_directories(srs_resource_generator_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srs_resource_generator_test du_manager_test_helpers gtest ocudu_gtest_main)
gtest_discover_tests(srs_resource_generator_test)

add_executable(du_periodic_srs_res_mng_test du_periodic_srs_res_mng_test.cpp)
target_include_directories(du_periodic_srs_res_mng_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(du_periodic_srs_res_mng_test du_manager_test_helpers sched_test_doubles gtest ocudu_gtest_main)
gtest_discover_tests(du_periodic_srs_res_mng_test)

add_executable(du_cg_type1_res_mng_test du_cg_type1_res_mng_test.cpp)
target_include_directories(du_cg_type1_res_mng_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(du_cg_type1_res_mng_test du_manager_test_helpers sched_test_doubles gtest ocudu_gtest_main)
gtest_discover_tests(du_cg_type1_res_mng_test)

add_executable(du_aperiodic_srs_res_mng_test du_aperiodic_srs_res_mng_test.cpp)
target_include_directories(du_aperiodic_srs_res_mng_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(du_aperiodic_srs_res_mng_test du_manager_test_helpers sched_test_doubles gtest ocudu_gtest_main)
gtest_discover_tests(du_aperiodic_srs_res_mng_test)

add_executable(cbs_encoder_test cbs/cbs_encoder_test.cpp)
target_include_directories(cbs_encoder_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cbs_encoder_test ocudu_cbs_encoder ocudu_support gtest ocudu_gtest_main)
gtest_discover_tests(cbs_encoder_test)

add_executable(sib19_ntn_test sib19_ntn_test.cpp)
target_include_directories(sib19_ntn_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(sib19_ntn_test du_manager_converters ocudu_du_manager ocudu_support ocudulog gtest gtest_main)
gtest_discover_tests(sib19_ntn_test)

add_executable(sib19_value_tag_test sib19_value_tag_test.cpp)
target_include_directories(sib19_value_tag_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(sib19_value_tag_test ocudu_du_high du_manager_converters ocudu_du_manager ocudu_support ocudulog gtest gtest_main)
gtest_discover_tests(sib19_value_tag_test)
