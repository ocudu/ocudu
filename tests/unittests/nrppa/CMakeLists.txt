# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.



set(SOURCES
	nrppa_ue_context_test.cpp
	nrppa_du_context_test.cpp
	nrppa_meas_context_test.cpp
	nrppa_impl_test.cpp
	nrppa_asn1_converters_test.cpp
	)

add_executable(nrppa_test ${SOURCES})
target_include_directories(nrppa_test PRIVATE ${CMAKE_SOURCE_DIR})
set_target_properties(nrppa_test PROPERTIES UNITY_BUILD ON)
target_link_libraries(nrppa_test nrppa_test_doubles ocudu_nrppa ocudu_support nrppa_asn1 ocudu_ran ocudulog GTest::gtest ocudu_gtest_main)
gtest_discover_tests(nrppa_test PROPERTIES "LABELS;nrppa")
