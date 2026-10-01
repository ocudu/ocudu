# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "phy")

add_subdirectory(precoding)

add_executable(dft_processor_test dft_processor_test.cpp)
target_include_directories(dft_processor_test PRIVATE ${CMAKE_SOURCE_DIR}/tests/unittests)
target_link_libraries(dft_processor_test ocudulog ocudu_dft GTest::gtest GTest::gtest_main)
add_test(dft_processor_test dft_processor_test)

if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
    add_executable(dft_processor_ci16_test dft_processor_ci16_test.cpp)
    target_include_directories(dft_processor_ci16_test PRIVATE ${CMAKE_SOURCE_DIR}/tests/unittests)
    target_link_libraries(dft_processor_ci16_test ocudulog ocudu_dft GTest::gtest GTest::gtest_main)
    add_test(dft_processor_ci16_test dft_processor_ci16_test)
endif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")

if ((ENABLE_MKL AND MKL_FOUND) OR (ENABLE_ARMPL AND ARMPL_FOUND) OR (ENABLE_FFTW AND FFTW3F_FOUND))
    target_compile_definitions(dft_processor_test PRIVATE HAVE_FFTW)
endif ((ENABLE_MKL AND MKL_FOUND) OR (ENABLE_ARMPL AND ARMPL_FOUND) OR (ENABLE_FFTW AND FFTW3F_FOUND))

if (ENABLE_FFTZ AND FFTZ_FOUND)
    target_compile_definitions(dft_processor_test PRIVATE HAVE_FFTZ)
endif (ENABLE_FFTZ AND FFTZ_FOUND)
