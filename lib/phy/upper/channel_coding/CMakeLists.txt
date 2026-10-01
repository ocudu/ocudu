# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(CRC_CALCULATOR_SOURCES
        crc_calculator_generic_impl.cpp
        crc_calculator_lut_impl.cpp)
if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
    list(APPEND CRC_CALCULATOR_SOURCES crc_calculator_clmul_impl.cpp)
    set_source_files_properties(crc_calculator_clmul_impl.cpp PROPERTIES COMPILE_OPTIONS "-mpclmul;-msse4.1")
endif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
    # Add NEON CRC calculator source.
    list(APPEND CRC_CALCULATOR_SOURCES crc_calculator_neon_impl.cpp)

    # Check if polynomial multiplication is available with the current compiler flags.
    include(CheckCXXCompilerFlag)
    set(CHECK_ARM_CRYPTO_CODE "
        #include <arm_neon.h>
        #include <arm_neon.h>
        #include <stdio.h>

        int main() {
            uint64_t a = 0x12345678abcdef00;
            uint64_t b = 0xfedcba9876543210;
            uint64x2_t result = vreinterpretq_u64_p128(vmull_p64(a, b));

            printf(\"Result: 0x%016lx\", vgetq_lane_u64(result, 0));
            return 0;
        }")
    check_cxx_source_compiles("${CHECK_ARM_CRYPTO_CODE}" HAVE_ARM_CRYPTO)

    # If the compiler does not support the crypto features with the default compiler flags, try to append +crypto to -march.
    if (NOT HAVE_ARM_CRYPTO)
        message(STATUS "Current compile options do not support crypto features.")
        set(MARCH "native" CACHE STRING "Compiler -march flag. Default value is 'native' for aarch64.")
        message(STATUS "Current CPU value is ${MCPU}")
        message(STATUS "Current MARCH value is ${MARCH}")

        # Append +crypto to the required flags.
        set(CMAKE_REQUIRED_FLAGS "-march=${MARCH}+crypto")
        check_cxx_source_compiles("${CHECK_ARM_CRYPTO_CODE}" HAVE_PLUS_CRYPTO)

        # Print a fatal error if the +crypto appending is not supported.
        if (NOT HAVE_PLUS_CRYPTO)
            message(FATAL_ERROR "${CMAKE_CXX_COMPILER_ID} does not support +crypto feature.")
        endif (NOT HAVE_PLUS_CRYPTO)

        # Append the +crypto to the NEON CRC calculator source file only.
        set_source_files_properties(crc_calculator_neon_impl.cpp PROPERTIES COMPILE_OPTIONS "${CMAKE_REQUIRED_FLAGS}")
    endif (NOT HAVE_ARM_CRYPTO)
endif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")


add_library(ocudu_crc_calculator STATIC ${CRC_CALCULATOR_SOURCES})

add_subdirectory(polar)
add_subdirectory(ldpc)
add_subdirectory(short)

add_library(ocudu_channel_coding STATIC channel_coding_factories.cpp)

target_link_libraries(ocudu_channel_coding
        ocudu_crc_calculator
        ocudu_polar
        ocudu_ldpc
        ocudu_short_block)

add_to_exported_libs(ocudu_channel_coding
        ocudu_crc_calculator
        ocudu_polar
        ocudu_ldpc
        ocudu_short_block)
