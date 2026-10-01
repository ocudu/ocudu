# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


# Find TRX header
find_package(TRX_DRIVER MODULE REQUIRED)

if (NOT (ZEROMQ_FOUND AND ENABLE_ZEROMQ))
    message(SEND_ERROR "ZeroMQ is required to build TRX driver.")
endif ()

if (NOT ENABLE_EXPORT)
    message(SEND_ERROR "Enable export is required. Append -DENABLE_EXPORT=TRUE to the cmake arguments.")
endif ()

# Add shared library.
add_library(trx_ocudu SHARED trx_ocudu.cpp)
target_link_libraries(trx_ocudu PRIVATE
        ocudu_radio
        ocudu_support
        ocudulog)
target_include_directories(trx_ocudu PRIVATE
        ${OCUDU_INCLUDE_DIR}
        ${TRX_DRIVER_DIR}
        "${OCUDU_SOURCE_DIR}/external/fmt/include/"
        "${OCUDU_SOURCE_DIR}/external/")

# Add dynamic library test.
add_executable(trx_ocudu_test trx_ocudu_test.cpp)
target_link_libraries(trx_ocudu_test dl)
target_include_directories(trx_ocudu_test PRIVATE ${TRX_DRIVER_DIR})
add_test(trx_ocudu_test trx_ocudu_test ./libtrx_ocudu.so)
target_link_options(trx_ocudu_test PRIVATE "SHELL:-Wl,--no-as-needed -lpthread -Wl,--as-needed")

# Make sure libtrx_ocudu is compiled when trx_ocudu_test is the only target.
add_dependencies(trx_ocudu_test trx_ocudu)
