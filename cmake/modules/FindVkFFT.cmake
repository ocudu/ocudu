# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


# - Try to find VkFFT - a header-only Fast Fourier Transform library
#
# VkFFT is not distributed by the common package managers, so it has to be installed
# manually. See docs/nvidia_cuda_build.md for the installation instructions, and
# point the build at the checkout with -DVKFFT_ROOT=<path> or the VKFFT_ROOT
# environment variable.
#
# Once done this will define
#  VkFFT_FOUND        - System has VkFFT
#  VkFFT_INCLUDE_DIRS - The VkFFT include directories
#  VkFFT_VERSION      - The VkFFT version, when the checkout reports one

set(VKFFT_ROOT "" CACHE PATH "Path to a VkFFT checkout containing vkFFT/vkFFT.h")

find_path(VkFFT_INCLUDE_DIR
            NAMES vkFFT/vkFFT.h
            HINTS ${VKFFT_ROOT} $ENV{VKFFT_ROOT}
            PATHS /usr/local/include
                  /usr/include)

# The VkFFT headers include each other through paths rooted at the vkFFT directory itself
# (for instance "vkFFT/vkFFT_Structs/vkFFT_Structs.h"), so that directory, and not the
# checkout containing it, is what a consumer has to compile against.
set(VkFFT_INCLUDE_DIRS ${VkFFT_INCLUDE_DIR}/vkFFT)

# The VkFFT sources carry no version macro, so the version is only known when the
# checkout ships a CMake project declaring one.
set(VkFFT_VERSION "unknown")
if (VkFFT_INCLUDE_DIR AND EXISTS "${VkFFT_INCLUDE_DIR}/CMakeLists.txt")
    file(STRINGS "${VkFFT_INCLUDE_DIR}/CMakeLists.txt" VkFFT_PROJECT_LINE
         REGEX "project[ \t]*\\(.*VERSION[ \t]+[0-9.]+")
    if (VkFFT_PROJECT_LINE)
        string(REGEX MATCH "VERSION[ \t]+([0-9.]+)" _vkfft_version_match "${VkFFT_PROJECT_LINE}")
        if (CMAKE_MATCH_1)
            set(VkFFT_VERSION "${CMAKE_MATCH_1}")
        endif()
    endif()
endif()

include(FindPackageHandleStandardArgs)
# Handle the QUIETLY and REQUIRED arguments and set VkFFT_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(VkFFT
                                  REQUIRED_VARS VkFFT_INCLUDE_DIR
                                  VERSION_VAR   VkFFT_VERSION)

mark_as_advanced(VkFFT_INCLUDE_DIR)
