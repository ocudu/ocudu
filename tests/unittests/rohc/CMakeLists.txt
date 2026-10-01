# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "rohc")

add_executable(rohc_config_test rohc_config_test.cpp)
target_link_libraries(rohc_config_test ocudu_rohc ocudu_support ocudulog GTest::gtest GTest::gtest_main)
gtest_discover_tests(rohc_config_test)

add_executable(rohc_factory_test rohc_factory_test.cpp)
target_link_libraries(rohc_factory_test ocudu_rohc ocudu_support ocudulog GTest::gtest GTest::gtest_main)
gtest_discover_tests(rohc_factory_test)

if(ROHC_FOUND)
  add_executable(rohc_lib_compressor_decompressor_test rohc_lib_compressor_decompressor_test.cpp)
  target_include_directories(rohc_lib_compressor_decompressor_test PRIVATE ${CMAKE_SOURCE_DIR})
  target_link_libraries(rohc_lib_compressor_decompressor_test ocudu_rohc ocudu_support ocudulog GTest::gtest GTest::gtest_main)
  gtest_discover_tests(rohc_lib_compressor_decompressor_test)
endif()
