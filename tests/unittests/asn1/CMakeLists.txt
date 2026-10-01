# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


include_directories(../../..)

set_directory_properties(PROPERTIES LABELS "asn1")

add_executable(asn1_utils_test asn1_utils_test.cpp)
target_link_libraries(asn1_utils_test asn1_utils ocudu_support GTest::gtest GTest::gtest_main)
add_test(asn1_utils_test asn1_utils_test)

add_executable(asn1_rrc_nr_test asn1_rrc_nr_test.cpp)
target_link_libraries(asn1_rrc_nr_test asn1_utils ocudu_support rrc_nr_asn1 GTest::gtest GTest::gtest_main)
gtest_discover_tests(asn1_rrc_nr_test)

add_executable(asn1_e1ap_test asn1_e1ap_test.cpp)
target_link_libraries(asn1_e1ap_test ocudu_pcap ocudu_support e1ap_asn1 ocudulog ocudu_pcap GTest::gtest GTest::gtest_main)
target_include_directories(asn1_e1ap_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(asn1_e1ap_test)

add_executable(asn1_e2ap_test asn1_e2ap_test.cpp asn1_e2sm_test.cpp)
target_link_libraries(asn1_e2ap_test e2ap_asn1 ocudulog ocudu_support ocudu_pcap GTest::gtest GTest::gtest_main)
target_include_directories(asn1_e2ap_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(asn1_e2ap_test)

add_executable(asn1_f1ap_test asn1_f1ap_test.cpp)
target_link_libraries(asn1_f1ap_test f1ap_asn1 ocudulog ocudu_support ocudu_pcap GTest::gtest GTest::gtest_main)
target_include_directories(asn1_f1ap_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(asn1_f1ap_test)

add_executable(asn1_ngap_test asn1_ngap_test.cpp)
target_link_libraries(asn1_ngap_test ngap_asn1 ocudulog ocudu_support ocudu_pcap GTest::gtest GTest::gtest_main)
gtest_discover_tests(asn1_ngap_test)

add_executable(asn1_cause_conversion_test asn1_cause_conversion_test.cpp)
target_link_libraries(asn1_cause_conversion_test ngap_asn1 f1ap_asn1 e1ap_asn1 ocudu_f1ap_common ocudulog ocudu_ran ocudu_support GTest::gtest GTest::gtest_main)
gtest_discover_tests(asn1_cause_conversion_test)
