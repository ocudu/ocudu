# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

FIND_PACKAGE(PkgConfig REQUIRED)

IF(NOT Sidekiq_FOUND)

    FIND_PATH(
            Sidekiq_INCLUDE_DIRS
            NAMES sidekiq_api.h
            HINTS $ENV{Sidekiq_DIR}/inc
            $ENV{Sidekiq_DIR}/sidekiq_core/inc
            PATHS /usr/local/include
            /usr/include
    )

    FIND_LIBRARY(
            Sidekiq_LIBRARY
            NAMES sidekiq__x86_64.gcc
            HINTS $ENV{Sidekiq_DIR}/lib
            PATHS /usr/local/lib
            /usr/lib
            /usr/lib/x86_64-linux-gnu
            /usr/local/lib64
            /usr/local/lib32
    )

    FIND_LIBRARY(
            Sidekiq_LIBRARY_GLIB
            NAMES libglib-2.0.a
            HINTS $ENV{Sidekiq_DIR}/lib/support/x86_64.gcc/usr/lib/epiq
            PATHS /usr/local/lib
            /usr/lib
            /usr/lib/epiq
            /usr/lib/x86_64-linux-gnu
            /usr/local/lib64
            /usr/local/lib32
    )

    FIND_LIBRARY(
            Sidekiq_LIBRARY_USB
            NAMES libusb-1.0.a
            HINTS $ENV{Sidekiq_DIR}/lib/support/x86_64.gcc/usr/lib/epiq
            PATHS /usr/local/lib
            /usr/lib
            /usr/lib/epiq
            /usr/lib/x86_64-linux-gnu
            /usr/local/lib64
            /usr/local/lib32
    )

    FIND_LIBRARY(
            Sidekiq_LIBRARY_TIRPC
            NAMES libtirpc.so.3
            PATHS /usr/local/lib
            /usr/lib64
            /usr/lib/epiq
            /usr/lib/x86_64-linux-gnu
            /usr/local/lib64
            /usr/local/lib32
    )

    set(Sidekiq_LIBRARIES ${Sidekiq_LIBRARY} ${Sidekiq_LIBRARY_GLIB} ${Sidekiq_LIBRARY_USB} ${Sidekiq_LIBRARY_TIRPC})

    if(Sidekiq_INCLUDE_DIRS)
        set(_sidekiq_api_h "${Sidekiq_INCLUDE_DIRS}/sidekiq_api.h")
        file(STRINGS "${_sidekiq_api_h}" _version_major REGEX "^#define[ \t]+LIBSIDEKIQ_VERSION_MAJOR[ \t]+[0-9]+")
        file(STRINGS "${_sidekiq_api_h}" _version_minor REGEX "^#define[ \t]+LIBSIDEKIQ_VERSION_MINOR[ \t]+[0-9]+")
        file(STRINGS "${_sidekiq_api_h}" _version_patch REGEX "^#define[ \t]+LIBSIDEKIQ_VERSION_PATCH[ \t]+[0-9]+")
        string(REGEX REPLACE "^#define[ \t]+LIBSIDEKIQ_VERSION_MAJOR[ \t]+([0-9]+)" "\\1" _major "${_version_major}")
        string(REGEX REPLACE "^#define[ \t]+LIBSIDEKIQ_VERSION_MINOR[ \t]+([0-9]+)" "\\1" _minor "${_version_minor}")
        string(REGEX REPLACE "^#define[ \t]+LIBSIDEKIQ_VERSION_PATCH[ \t]+([0-9]+)" "\\1" _patch "${_version_patch}")
        set(Sidekiq_VERSION "${_major}.${_minor}.${_patch}" CACHE STRING "Sidekiq version")
        unset(_sidekiq_api_h)
        unset(_version_major)
        unset(_version_minor)
        unset(_version_patch)
        unset(_major)
        unset(_minor)
        unset(_patch)
    endif()

    message(STATUS "Sidekiq LIBRARIES " ${Sidekiq_LIBRARIES})
    message(STATUS "Sidekiq INCLUDE DIRS " ${Sidekiq_INCLUDE_DIRS})
    message(STATUS "Sidekiq VERSION " ${Sidekiq_VERSION})

    INCLUDE(FindPackageHandleStandardArgs)
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(Sidekiq DEFAULT_MSG Sidekiq_LIBRARIES Sidekiq_INCLUDE_DIRS)
    MARK_AS_ADVANCED(Sidekiq_LIBRARIES Sidekiq_INCLUDE_DIRS)

ENDIF(NOT Sidekiq_FOUND)
