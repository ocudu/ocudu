# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


FIND_PACKAGE(PkgConfig REQUIRED)

# Changing the requested prefix must invalidate discovery from an earlier configure.
if(NOT "${_UHD_LAST_DIR}" STREQUAL "$ENV{UHD_DIR}")
  unset(UHD_INCLUDE_DIRS CACHE)
  unset(UHD_LIBRARIES CACHE)
  unset(UHD_INCLUDE_DIRS)
  unset(UHD_LIBRARIES)
  unset(UHD_FOUND)
  unset(UHD_VERSION)
  unset(UHD_LIBRARY_DIR)
endif()
set(_UHD_LAST_DIR "$ENV{UHD_DIR}" CACHE INTERNAL "Last requested UHD prefix" FORCE)

IF(NOT UHD_FOUND)

if(NOT "$ENV{UHD_DIR}" STREQUAL "")
  # An explicit UHD prefix must not fall back to a system library with a different ABI.
  FIND_PATH(UHD_INCLUDE_DIRS NAMES uhd.h
            PATHS "$ENV{UHD_DIR}/include" NO_DEFAULT_PATH)
  FIND_LIBRARY(UHD_LIBRARIES NAMES uhd
               PATHS "$ENV{UHD_DIR}/lib" "$ENV{UHD_DIR}/lib64" NO_DEFAULT_PATH)
else()
  FIND_PATH(UHD_INCLUDE_DIRS NAMES uhd.h
            PATHS /usr/local/include /usr/include)
  FIND_LIBRARY(UHD_LIBRARIES NAMES uhd
               PATHS /usr/local/lib /usr/lib /usr/lib/x86_64-linux-gnu
                     /usr/local/lib64 /usr/local/lib32)
endif()

message(STATUS "UHD LIBRARIES " ${UHD_LIBRARIES})
message(STATUS "UHD INCLUDE DIRS " ${UHD_INCLUDE_DIRS})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(UHD DEFAULT_MSG UHD_LIBRARIES UHD_INCLUDE_DIRS)
MARK_AS_ADVANCED(UHD_LIBRARIES UHD_INCLUDE_DIRS)

include(CheckCXXSourceCompiles)

IF(UHD_FOUND)
  # UHD library directory
  get_filename_component(UHD_LIBRARY_DIR ${UHD_LIBRARIES} DIRECTORY)

  # UHD version, encoded in uhd/version.hpp as major * 1000000 + API * 10000 + ABI * 100 + patch
  if(EXISTS "${UHD_INCLUDE_DIRS}/uhd/version.hpp")
    file(STRINGS "${UHD_INCLUDE_DIRS}/uhd/version.hpp" _uhd_version_line REGEX "^#define UHD_VERSION [0-9]+$")
  endif()
  if(_uhd_version_line MATCHES "UHD_VERSION ([0-9]+)")
    math(EXPR _uhd_major "${CMAKE_MATCH_1} / 1000000")
    math(EXPR _uhd_api "(${CMAKE_MATCH_1} / 10000) % 100")
    math(EXPR _uhd_abi "(${CMAKE_MATCH_1} / 100) % 100")
    math(EXPR _uhd_patch "${CMAKE_MATCH_1} % 100")
    set(UHD_VERSION "${_uhd_major}.${_uhd_api}.${_uhd_abi}.${_uhd_patch}")
    message(STATUS "UHD VERSION " ${UHD_VERSION})
  endif()
  unset(_uhd_version_line)
  unset(_uhd_major)
  unset(_uhd_api)
  unset(_uhd_abi)
  unset(_uhd_patch)

  # Save current required variables
  set(_CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
  set(_CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES})
  set(_CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES})

  # Set required variables
  set(CMAKE_REQUIRED_INCLUDES ${UHD_INCLUDE_DIRS})
  set(CMAKE_REQUIRED_FLAGS "${CMAKE_CXX_FLAGS} -L${UHD_LIBRARY_DIR}")
  set(CMAKE_REQUIRED_LIBRARIES uhd boost_program_options boost_system)

  # Recover required variables
  set(CMAKE_REQUIRED_FLAGS ${_CMAKE_REQUIRED_FLAGS})
  set(CMAKE_REQUIRED_LIBRARIES ${_CMAKE_REQUIRED_LIBRARIES})
  set(CMAKE_REQUIRED_INCLUDES ${_CMAKE_REQUIRED_INCLUDES})
ENDIF(UHD_FOUND)

ENDIF(NOT UHD_FOUND)
