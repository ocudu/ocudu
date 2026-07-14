# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

# - Try to find OpenSSL library and check for SCTP DTLS support.
#
# Once done this will define
#  OpenSSL_FOUND        - System has OpenSSL library installed
#  OPENSSL_HAS_DTLS_SCTP - OpenSSL version supports DTLS for SCTP
# and add requirement for:
#  OpenSSL::SSL         - OpenSSL's SSL library
#  OpenSSL::Crypto      - OpenSSL's crypto library
#
# To use a custom installation directory, use -DOPENSSL_ROOT_DIR=<SSL_DIR>
# or -DOPENSSL_DIR=<dir> when running cmake.

set(OPENSSL_DIR "" CACHE PATH "Path to the OpenSSL installation")

if(OPENSSL_DIR)
    set(OPENSSL_ROOT_DIR "${OPENSSL_DIR}")
endif()

find_package(OpenSSL QUIET)

if(OpenSSL_FOUND)
    message(STATUS "Found OpenSSL ${OPENSSL_VERSION}")

    include(CheckCSourceCompiles)

    set(CMAKE_REQUIRED_LIBRARIES OpenSSL::SSL OpenSSL::Crypto)

    check_c_source_compiles("
    #include <openssl/ssl.h>

    int main(void)
    {
        SSL *ssl = 0;

    #ifdef OPENSSL_NO_SCTP
    #error SCTP support disabled
    #endif

        return 0;
    }
    " OPENSSL_HAS_DTLS_SCTP)

    if(OPENSSL_HAS_DTLS_SCTP)
        message(STATUS "OpenSSL found, with DTLS/SCTP support")
        message(STATUS "OpenSSL include dirs: \"${OPENSSL_INCLUDE_DIR}\"")
        message(STATUS "OpenSSL libraries dirs: \"${OPENSSL_LIBRARIES}\"")
    else()
        message(STATUS "OpenSSL found, but without DTLS/SCTP support")
    endif()

else()
    message(STATUS "OpenSSL not found")
endif()
