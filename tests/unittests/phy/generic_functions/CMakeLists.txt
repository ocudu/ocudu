# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "phy")

add_subdirectory(precoding)

add_executable(dft_processor_test dft_processor_test.cpp)
target_include_directories(dft_processor_test PRIVATE ${CMAKE_SOURCE_DIR}/tests/test_doubles/utils)
target_link_libraries(dft_processor_test ocudulog ocudu_dft GTest::gtest GTest::gtest_main)
add_test(dft_processor_test dft_processor_test)

if ((ENABLE_MKL AND MKL_FOUND) OR (ENABLE_ARMPL AND ARMPL_FOUND) OR (ENABLE_FFTW AND FFTW3F_FOUND))
    target_compile_definitions(dft_processor_test PRIVATE HAVE_FFTW)
endif ((ENABLE_MKL AND MKL_FOUND) OR (ENABLE_ARMPL AND ARMPL_FOUND) OR (ENABLE_FFTW AND FFTW3F_FOUND))

if (ENABLE_FFTZ AND FFTZ_FOUND)
    target_compile_definitions(dft_processor_test PRIVATE HAVE_FFTZ)
endif (ENABLE_FFTZ AND FFTZ_FOUND)

if (ENABLE_HWY AND HWY_FOUND)
    target_compile_definitions(dft_processor_test PRIVATE HAVE_HWY)

    add_executable(dft_processor_hwy_test dft_processor_hwy_test.cpp)
    target_include_directories(dft_processor_hwy_test SYSTEM PRIVATE ${HWY_INCLUDE_DIRS})
    target_include_directories(dft_processor_hwy_test PRIVATE ${CMAKE_SOURCE_DIR}/lib/phy/generic_functions/dft_hwy)
    target_compile_definitions(dft_processor_hwy_test PRIVATE ${HWY_DEFINITIONS})
    target_link_libraries(dft_processor_hwy_test ocudulog ocudu_dft ${HWY_LIBRARIES} GTest::gtest GTest::gtest_main)
    add_test(dft_processor_hwy_test dft_processor_hwy_test)
endif (ENABLE_HWY AND HWY_FOUND)
