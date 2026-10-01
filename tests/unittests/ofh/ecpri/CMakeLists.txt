# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set_directory_properties(PROPERTIES LABELS "ecpri")

add_executable(ecpri_packet_builder_impl_test ecpri_packet_builder_test.cpp)
target_link_libraries(ecpri_packet_builder_impl_test ocudu_ofh_ecpri ocudu_support ocudulog GTest::gtest GTest::gtest_main)
gtest_discover_tests(ecpri_packet_builder_impl_test)

add_executable(ecpri_packet_decoder_impl_test ecpri_packet_decoder_impl_test.cpp)
target_link_libraries(ecpri_packet_decoder_impl_test ocudu_ofh_ecpri ocudu_support ocudulog GTest::gtest GTest::gtest_main)
gtest_discover_tests(ecpri_packet_decoder_impl_test)
