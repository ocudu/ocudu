# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_subdirectory(pucch)
add_subdirectory(pusch)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_data)

# Only implement if hardware-acceleration for the PDSCH is enabled.
if (ENABLE_PDSCH_HWACC)
    add_executable(pdsch_encoder_hwacc_benchmark pdsch_encoder_hwacc_benchmark.cpp)
    set(PDSCH_ENCODER_HWACC_BENCHMARK_LIBRARIES ocudu_channel_processors ocudulog)
    if (DPDK_FOUND)
        set_source_files_properties(pdsch_encoder_hwacc_benchmark.cpp PROPERTIES COMPILE_DEFINITIONS "DPDK_FOUND")
        list(APPEND PDSCH_ENCODER_HWACC_BENCHMARK_LIBRARIES ocudu_hal_pdsch)
    endif (DPDK_FOUND)
    target_link_libraries(pdsch_encoder_hwacc_benchmark ${PDSCH_ENCODER_HWACC_BENCHMARK_LIBRARIES})
endif (ENABLE_PDSCH_HWACC)

add_executable(pdsch_processor_benchmark pdsch_processor_benchmark.cpp)
set(PDSCH_PROCESSOR_LIBRARIES ocudu_phy_support
        ocudu_pdsch_processor
        ocudu_channel_equalizer
        ocudu_channel_precoder
        ocudu_channel_processors
        ocudu_ran
        ocudulog
        ocuduvec)

if (ENABLE_PDSCH_HWACC)
    set_source_files_properties(pdsch_processor_benchmark.cpp PROPERTIES COMPILE_DEFINITIONS "HWACC_PDSCH_ENABLED")
    list(APPEND PDSCH_PROCESSOR_LIBRARIES ocudu_hal_pdsch)
endif (ENABLE_PDSCH_HWACC)

target_link_libraries(pdsch_processor_benchmark ${PDSCH_PROCESSOR_LIBRARIES})
add_test(pdsch_processor_benchmark_generic_qpsk pdsch_processor_benchmark -m silent -R 1 -B 1 -T 2 -P pdsch_scs15_5MHz_qpsk_min -t generic)
add_test(pdsch_processor_benchmark_generic_256qam pdsch_processor_benchmark -m silent -R 1 -B 1 -T 2 -P 2port_2layer_scs30_100MHz_256qam -t generic)
add_test(pdsch_processor_benchmark_flexible pdsch_processor_benchmark -m silent -R 1 -B 1 -T 2 -P pdsch_scs15_5MHz_qpsk_min -t flexible:4.0)
add_test(pdsch_processor_benchmark_lite pdsch_processor_benchmark -m silent -R 1 -B 1 -T 2 -P pdsch_scs15_5MHz_qpsk_min -t lite)

add_executable(prach_detector_benchmark prach_detector_benchmark.cpp)
target_link_libraries(prach_detector_benchmark
        ocudu_phy_support
        ocudu_prach_detector
        ocudu_dft
        ocudulog
        ocuduvec)
add_test(prach_detector_benchmark prach_detector_benchmark -R 1)

