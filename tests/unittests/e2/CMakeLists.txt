# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "e2ap")

set(SOURCES
    common/e2ap_asn1_packer.cpp
)
add_library(e2_test_helpers STATIC ${SOURCES})
target_link_libraries(e2_test_helpers ocudu_e2 e2ap_asn1 ocudu_support)

add_executable(e2_asn1_packer_test common/e2_asn1_packer_test.cpp)
target_link_libraries(e2_asn1_packer_test
                      ocudu_support
                      ocudu_e2
                      ocudulog
                      e2ap_asn1
                      ocudu_ran
                      e2_test_helpers 		
                      GTest::gtest
                      GTest::gtest_main)
target_include_directories(e2_asn1_packer_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e2_asn1_packer_test)

add_executable(e2_setup_procedure_test e2_setup_procedure_test.cpp)
target_link_libraries(e2_setup_procedure_test ocudu_e2 ocudu_support ocudulog e2ap_asn1 e2_test_helpers GTest::gtest GTest::gtest_main
)
target_include_directories(e2_setup_procedure_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e2_setup_procedure_test)

add_executable(e2_setup_routine_test e2_setup_routine_test.cpp)
target_link_libraries(e2_setup_routine_test ocudu_e2 ocudu_support ocudulog e2ap_asn1 e2_test_helpers GTest::gtest GTest::gtest_main
)
target_include_directories(e2_setup_routine_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e2_setup_routine_test)

add_executable(ric_reconnection_routine_test ric_reconnection_routine_test.cpp)
target_link_libraries(ric_reconnection_routine_test ocudu_e2 ocudu_support ocudulog e2ap_asn1 e2_test_helpers GTest::gtest GTest::gtest_main
)
target_include_directories(ric_reconnection_routine_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(ric_reconnection_routine_test)

add_executable(ric_connection_loss_routine_test ric_connection_loss_routine_test.cpp)
target_link_libraries(ric_connection_loss_routine_test ocudu_e2 ocudu_support ocudulog e2ap_asn1 e2_test_helpers GTest::gtest GTest::gtest_main
)
target_include_directories(ric_connection_loss_routine_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(ric_connection_loss_routine_test)

add_executable(ric_connection_setup_routine_test ric_connection_setup_routine_test.cpp)
target_link_libraries(ric_connection_setup_routine_test ocudu_e2 ocudu_support ocudulog e2ap_asn1 e2_test_helpers GTest::gtest GTest::gtest_main
)
target_include_directories(ric_connection_setup_routine_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(ric_connection_setup_routine_test)

add_executable(e2_removal_procedure_test e2_removal_procedure_test.cpp)
target_link_libraries(e2_removal_procedure_test ocudu_e2 ocudu_support ocudulog e2ap_asn1 e2_test_helpers GTest::gtest GTest::gtest_main
)
target_include_directories(e2_removal_procedure_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e2_removal_procedure_test)

add_executable(e2_subscription_setup_test e2_subscription_setup_test.cpp)
target_link_libraries(e2_subscription_setup_test ocudu_e2 ocudu_support ocudulog e2ap_asn1 ocudu_ran e2_test_helpers GTest::gtest GTest::gtest_main
)
target_include_directories(e2_subscription_setup_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e2_subscription_setup_test)

add_executable(e2_ric_control_procedure_test e2_ric_control_procedure_test.cpp)
target_link_libraries(e2_ric_control_procedure_test ocudu_e2 ocudu_support ocudulog e2ap_asn1 ocudu_ran e2_test_helpers GTest::gtest GTest::gtest_main
)
target_include_directories(e2_ric_control_procedure_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e2_ric_control_procedure_test)

add_executable(e2ap_network_adapter_test e2ap_network_adapter_test.cpp dummy_ric.cpp)
target_link_libraries(e2ap_network_adapter_test ocudulog ocudu_e2 e2ap_asn1 ocudu_gateway ocudu_support e2_test_helpers GTest::gtest GTest::gtest_main
)
target_include_directories(e2ap_network_adapter_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e2ap_network_adapter_test PROPERTIES "LABELS;sctp")

add_executable(e2sm_kpm_meas_provider_test e2sm_kpm_meas_provider_test.cpp)
target_link_libraries(e2sm_kpm_meas_provider_test ocudulog ocudu_e2 e2ap_asn1 ocudu_pcap ocudu_gateway ocudu_support e2_test_helpers GTest::gtest GTest::gtest_main
)
target_include_directories(e2sm_kpm_meas_provider_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e2sm_kpm_meas_provider_test)

add_executable(e2sm_kpm_meas_provider_metrics_test e2sm_kpm_meas_provider_metrics_test.cpp)
target_link_libraries(e2sm_kpm_meas_provider_metrics_test ocudulog ocudu_e2 e2ap_asn1 ocudu_pcap ocudu_gateway ocudu_support e2_test_helpers GTest::gtest GTest::gtest_main
)
target_include_directories(e2sm_kpm_meas_provider_metrics_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e2sm_kpm_meas_provider_metrics_test)

add_executable(e2sm_kpm_test e2sm_kpm_test.cpp)
target_link_libraries(e2sm_kpm_test ocudulog ocudu_e2 e2ap_asn1 ocudu_pcap ocudu_gateway ocudu_support e2_test_helpers GTest::gtest GTest::gtest_main
)
target_include_directories(e2sm_kpm_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e2sm_kpm_test)

add_executable(e2_conn_upd_procedure_test e2_conn_upd_procedure_test.cpp)
target_link_libraries(e2_conn_upd_procedure_test ocudulog ocudu_e2 e2ap_asn1 ocudu_pcap ocudu_gateway ocudu_support e2_test_helpers GTest::gtest GTest::gtest_main
)
target_include_directories(e2_conn_upd_procedure_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e2_conn_upd_procedure_test)

add_executable(asn1_e2sm_ccc_test asn1_e2sm_ccc_test.cpp)
target_link_libraries(asn1_e2sm_ccc_test ocudulog ocudu_e2 e2ap_asn1 ocudu_pcap ocudu_support e2_test_helpers GTest::gtest GTest::gtest_main
)
target_include_directories(asn1_e2sm_ccc_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(asn1_e2sm_ccc_test)
