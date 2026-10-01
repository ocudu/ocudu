# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_subdirectory(precoding)
add_subdirectory(transform_precoding)

# Initialise DFT sources and definitions for DFT libraries.
set(OCUDU_DFT_SOURCES dft_processor_generic_impl.cpp generic_functions_factories.cpp)
set(OCUDU_DFT_LIBRARIES "")
set(OCUDU_DFT_INCLUDE_DIRS "")
set(OCUDU_DFT_LIBRARY_DIRS "")
set(OCUDU_DFT_DEFINITIONS "")

if (ENABLE_MKL AND MKL_FOUND)
    # If MKL is enabled and found, then compile DFT FFTW API based and link it to MKL.
    list(APPEND OCUDU_DFT_LIBRARIES ${MKL_LIBRARIES})
    list(APPEND OCUDU_DFT_INCLUDE_DIRS ${MKL_INCLUDE_DIRS})
    list(APPEND OCUDU_DFT_LIBRARY_DIRS ${MKL_LIBRARY_DIRS})
elseif (ENABLE_ARMPL AND ARMPL_FOUND)
    # If ARMPL is enabled and found, then add FFTW source and libraries.
    list(APPEND OCUDU_DFT_LIBRARIES ${ARMPL_LIBRARIES})
    list(APPEND OCUDU_DFT_INCLUDE_DIRS ${ARMPL_INCLUDE_DIRS})
    list(APPEND OCUDU_DFT_LIBRARY_DIRS ${ARMPL_LIBRARY_DIRS})
elseif (ENABLE_FFTW AND FFTW3F_FOUND)
    # If FFTW is enabled and found, then add FFTW source and libraries.
    list(APPEND OCUDU_DFT_LIBRARIES ${FFTW3F_LIBRARIES})
    list(APPEND OCUDU_DFT_INCLUDE_DIRS ${FFTW3F_INCLUDE_DIRS})
    list(APPEND OCUDU_DFT_LIBRARY_DIRS ${FFTW3F_LIBRARY_DIRS})
endif (ENABLE_MKL AND MKL_FOUND)

if (ENABLE_FFTZ AND FFTZ_FOUND)
    # If FFTZ is enabled and found, then add FFTZ source and libraries.
    list(APPEND OCUDU_DFT_LIBRARIES ${FFTZ_LIBRARIES})
    list(APPEND OCUDU_DFT_INCLUDE_DIRS ${FFTZ_INCLUDE_DIRS})
    list(APPEND OCUDU_DFT_SOURCES dft_processor_fftz_impl.cpp)
    list(APPEND OCUDU_DFT_DEFINITIONS "HAVE_FFTZ")
endif (ENABLE_FFTZ AND FFTZ_FOUND)

if ((ENABLE_MKL AND MKL_FOUND) OR (ENABLE_ARMPL AND ARMPL_FOUND) OR (ENABLE_FFTW AND FFTW3F_FOUND))
    list(APPEND OCUDU_DFT_SOURCES dft_processor_fftw_impl.cpp)
    list(APPEND OCUDU_DFT_DEFINITIONS "HAVE_FFTW")
endif ((ENABLE_MKL AND MKL_FOUND) OR (ENABLE_ARMPL AND ARMPL_FOUND) OR (ENABLE_FFTW AND FFTW3F_FOUND))

if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
    list(APPEND OCUDU_DFT_SOURCES dft_processor_ci16_avx2.cpp)
    set_source_files_properties(dft_processor_ci16_avx2.cpp PROPERTIES COMPILE_OPTIONS "-mavx2;")
endif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")

add_library(ocudu_dft STATIC ${OCUDU_DFT_SOURCES})
target_include_directories(ocudu_dft PRIVATE ${OCUDU_DFT_INCLUDE_DIRS})
target_link_directories(ocudu_dft PRIVATE ${OCUDU_DFT_LIBRARY_DIRS})
target_link_libraries(ocudu_dft PRIVATE ${OCUDU_DFT_LIBRARIES} ocuduvec ocudulog)
target_compile_definitions(ocudu_dft PRIVATE ${OCUDU_DFT_DEFINITIONS})

add_to_exported_libs(ocudu_dft)
